#pragma once

#include <cstddef>

#include <bitset>
#include <chrono>
#include <memory>
#include <stdexcept>

#include <iostream>

#include <etcpal/cpp/synchronized.h>
#include <etcpal/cpp/rwlock.h>

namespace etcpal
{

#if (__cplusplus >= 201703L)
using MemoryResource = std::pmr::memory_resource;
template <typename T>
using PolymorphicAllocator = std::pmr::polymorphic_allocator<T>;
using DefaultAllocator     = PolymorphicAllocator<std::byte>;
#else   // #if (__cplusplus >= 201703L)
class MemoryResource
{
public:
  [[nodiscard]] void* allocate(std::size_t bytes, std::size_t alignment = alignof(std::max_align_t))
  {
    return do_allocate(bytes, alignment);
  }

  void deallocate(void* p, std::size_t bytes, std::size_t alignment = alignof(std::max_align_t))
  {
    do_deallocate(p, bytes, alignment);
  }

  [[nodiscard]] bool is_equal(const MemoryResource& rhs) const noexcept { return do_is_equal(rhs); }

protected:
  [[nodiscard]] virtual void* do_allocate(std::size_t bytes, std::size_t alignment)            = 0;
  [[nodiscard]] virtual void  do_deallocate(void* p, std::size_t bytes, std::size_t alignment) = 0;
  [[nodiscard]] virtual bool  do_is_equal(const MemoryResource& rhs) const noexcept            = 0;
};

template <typename T>
class PolymorphicAllocator
{
public:
  using value_type = T;

  PolymorphicAllocator() noexcept = default;
  template <typename U>
  PolymorphicAllocator(const PolymorphicAllocator<U>& rhs) noexcept : resource_{rhs.resource()}
  {
  }
  PolymorphicAllocator(MemoryResource* res) noexcept : resource_{res} {}

  [[nodiscard]] auto allocate(std::size_t n) -> T*
  {
    if (!resource_)
    {
      return std::allocator<T>{}.allocate(n);
    }

    return reinterpret_cast<T*>(resource_->allocate(sizeof(T) * n, alignof(T)));
  }

  void deallocate(T* p, std::size_t n)
  {
    if (!resource_)
    {
      std::allocator<T>{}.deallocate(p, n);
      return;
    }

    resource_->deallocate(p, sizeof(T) * n, alignof(T));
  }

  [[nodiscard]] auto resource() const noexcept -> MemoryResource* { return resource_; }

  [[nodiscard]] friend bool operator==(const PolymorphicAllocator& lhs, const PolymorphicAllocator& rhs) noexcept
  {
    return (lhs.resource_ && rhs.resource_) || lhs.resource_->is_equal(*rhs.resource_);
  }
  [[nodiscard]] friend bool operator!=(const PolymorphicAllocator& lhs, const PolymorphicAllocator& rhs) noexcept
  {
    return !(lhs == rhs);
  }

private:
  MemoryResource* resource_ = nullptr;
};

using DefaultAllocator = PolymorphicAllocator<unsigned char>;
#endif  // #if (__cplusplus >= 201703L)

template <std::size_t Size, std::size_t BlockSize = sizeof(std::max_align_t), bool Debug = false>
class BlockMemory : public MemoryResource
{
public:
  BlockMemory()                                          = default;
  BlockMemory(const BlockMemory& rhs)                    = delete;
  BlockMemory(BlockMemory&& rhs)                         = delete;
  ~BlockMemory() noexcept                                = default;
  auto operator=(const BlockMemory& rhs) -> BlockMemory& = delete;
  auto operator=(BlockMemory&& rhs) -> BlockMemory&      = delete;

protected:
  [[nodiscard]] void* do_allocate(std::size_t bytes, std::size_t alignment) override
  {
    const auto blocks = to_blocks(bytes);
    first_free_block_ = find_first_free_block(first_free_block_);
    const auto free   = num_free_blocks(first_free_block_, blocks);
    if (free == blocks)
    {
      auto* const p     = allocate_blocks(first_free_block_, blocks);
      first_free_block_ = find_first_free_block(first_free_block_ + blocks);
      return p;
    }

    for (auto i = first_free_block_ + free, free_blocks = num_free_blocks(i, blocks); i < num_blocks();
         free_blocks = num_free_blocks(i += free_blocks + 1, blocks))
    {
      if (free_blocks == blocks)
      {
        return allocate_blocks(i, blocks);
      }
    }

    throw std::bad_alloc{};
  }

  void do_deallocate(void* p, std::size_t bytes, std::size_t alignment) override
  {
    const auto start_index = to_index(p);
    first_free_block_      = std::min(static_cast<int>(start_index), first_free_block_);
    mark_free(start_index);
    for (auto i = start_index + 1; i < num_blocks() && is_allocated(i) && !is_start_block(i); ++i)
    {
      mark_free(i);
    }
  }

  [[nodiscard]] bool do_is_equal(const MemoryResource& rhs) const noexcept override
  {
    return this == std::addressof(rhs);
  }

  void                      mark_start_block(int block) noexcept { live_.set(2 * block + 1); }
  void                      mark_allocated(int block) noexcept { live_.set(2 * block); }
  void                      mark_free(int block) noexcept { live_.reset(2 * block).reset(2 * block + 1); }
  [[nodiscard]] static auto to_blocks(std::size_t bytes) noexcept
  {
    return (bytes / BlockSize) + ((bytes % BlockSize == 0) ? 0 : 1);
  }

  [[nodiscard]] auto to_index(void* p) const noexcept
  {
    return (reinterpret_cast<unsigned char*>(p) - buffer_.data()) / BlockSize;
  }

  [[nodiscard]] auto num_blocks() const noexcept { return static_cast<int>(live_.size() / 2); }
  [[nodiscard]] bool is_allocated(int block) const noexcept { return live_[2 * block]; }
  [[nodiscard]] bool is_start_block(int block) const noexcept { return live_[2 * block + 1]; }
  [[nodiscard]] auto num_free_blocks(int start, int max) const noexcept
  {
    const auto limit = std::min(start + max, num_blocks());
    for (auto i = start; i < limit; ++i)
    {
      if (is_allocated(i))
      {
        return i - start;
      }
    }

    return limit - start;
  }

  [[nodiscard]] void* allocate_blocks(int start, int blocks) noexcept
  {
    mark_start_block(start);
    for (auto i = start; i < start + blocks; ++i)
    {
      mark_allocated(i);
    }

    return buffer_.data() + (start * BlockSize);
  }

  [[nodiscard]] auto find_first_free_block(int start) const noexcept
  {
    for (auto i = start; i < num_blocks(); ++i)
    {
      if (!is_allocated(i))
      {
        return i;
      }
    }

    return num_blocks();
  }

private:
  static_assert(Size >= BlockSize, "memory buffer must be large enough to contain at least one block");
  static_assert(BlockSize % alignof(std::max_align_t) == 0,
                "memory block size must be a multiple of the maximum alignment");

  alignas(std::max_align_t) std::array<unsigned char, Size> buffer_;
  int                               first_free_block_ = 0;
  std::bitset<2 * Size / BlockSize> live_             = {};
};

template <std::size_t Size, std::size_t BlockSize>
class BlockMemory<Size, BlockSize, true> : public BlockMemory<Size, BlockSize, false>
{
public:
  ~BlockMemory() noexcept
  {
    std::cout << "\n"
                 "Allocator Statistics for Block Memory Allocator\n"
                 "===============================================\n"
                 "block size               - "
              << BlockSize
              << " bytes\n"
                 "blocks                   - "
              << BlockMemory<Size, BlockSize, false>::to_blocks(Size)
              << "\n"
                 "                         - "
              << Size / 1024.0
              << " kiB\n"
                 "memory overhead          - "
              << (sizeof(BlockMemory<Size, BlockSize, false>) - Size) / 1024.0 << " kiB ("
              << (sizeof(BlockMemory<Size, BlockSize, false>) - Size) * 100.0 / Size
              << "%)\n"
                 "smallest allocation      - "
              << smallest_alloc_
              << " bytes\n"
                 "largest allocation       - "
              << largest_alloc_ / 1024.0
              << " kiB\n"
                 "                         - "
              << BlockMemory<Size, BlockSize, false>::to_blocks(largest_alloc_)
              << " blocks\n"
                 "single-block allocations - "
              << single_block_allocs_ << " (" << (single_block_allocs_ * 100.0) / total_allocs_
              << "%)\n"
                 "total allocations        - "
              << total_allocs_
              << " allocations\n"
                 "                         - "
              << total_allocd_blocks_
              << " blocks\n"
                 "                         - "
              << total_allocd_blocks_ * BlockSize / 1024.0
              << " kiB\n"
                 "high water mark          - "
              << high_water_mark_ << " blocks ("
              << high_water_mark_ * 100.0 / BlockMemory<Size, BlockSize, false>::to_blocks(Size)
              << "%)\n"
                 "                         - "
              << high_water_mark_ * BlockSize / 1024.0
              << " kiB\n"
                 "wastage                  - "
              << wastage_ / 1024.0 << " kiB (" << (wastage_ * 100.0) / (total_allocd_blocks_ * BlockSize)
              << "%)\n"
                 "total time in allocator  - "
              << std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(time_in_allocator_).count()
              << "ms\n";
    if (allocated_blocks_ != 0)
    {
      throw std::logic_error{"allocation count is not zero"};
    }
    for (auto i = 0; i < BlockMemory<Size, BlockSize, false>::num_blocks(); ++i)
    {
      if (BlockMemory<Size, BlockSize, false>::is_allocated(i))
      {
        throw std::logic_error{"memory block leaked"};
      }
    }
  }

protected:
  [[nodiscard]] void* do_allocate(std::size_t bytes, std::size_t alignment) override
  {
    const auto  blocks = BlockMemory<Size, BlockSize, false>::to_blocks(bytes);
    const auto  start  = std::chrono::high_resolution_clock::now();
    auto* const p      = BlockMemory<Size, BlockSize, false>::do_allocate(bytes, alignment);
    time_in_allocator_ += std::chrono::high_resolution_clock::now() - start;
    allocated_blocks_ += blocks;
    single_block_allocs_ += (blocks == 1) ? 1 : 0;
    wastage_ += blocks * BlockSize - bytes;
    ++total_allocs_;
    total_allocd_blocks_ += blocks;
    smallest_alloc_  = std::min(bytes, smallest_alloc_);
    largest_alloc_   = std::max(bytes, largest_alloc_);
    high_water_mark_ = std::max(allocated_blocks_, high_water_mark_);

    return p;
  }

  void do_deallocate(void* p, std::size_t bytes, std::size_t alignment) override
  {
    const auto start_index = BlockMemory<Size, BlockSize, false>::to_index(p);
    const auto blocks      = BlockMemory<Size, BlockSize, false>::to_blocks(bytes);
    if (start_index < 0 || start_index >= BlockMemory<Size, BlockSize, false>::num_blocks())
    {
      throw std::logic_error{"attempting to deallocate memory from outside this resource"};
    }
    if (start_index + blocks > BlockMemory<Size, BlockSize, false>::num_blocks())
    {
      throw std::logic_error{"deallocation size would reach outside this resource"};
    }
    if (!BlockMemory<Size, BlockSize, false>::is_start_block(start_index))
    {
      throw std::logic_error{"deallocating pointer to a memory block that's not a starting block of a chunk"};
    }

    allocated_blocks_ -= count_blocks_to_deallocate(start_index, blocks);

    const auto start = std::chrono::high_resolution_clock::now();
    BlockMemory<Size, BlockSize, false>::do_deallocate(p, bytes, alignment);
    time_in_allocator_ += std::chrono::high_resolution_clock::now() - start;
  }

  [[nodiscard]] auto count_blocks_to_deallocate(int start, int calculated_blocks) const noexcept
  {
    auto count = 1;
    for (auto i = start + 1; BlockMemory<Size, BlockSize, false>::is_allocated(i) &&
                             !BlockMemory<Size, BlockSize, false>::is_start_block(i) &&
                             i < BlockMemory<Size, BlockSize, false>::num_blocks();
         ++i, ++count)
    {
      if (i >= start + calculated_blocks)
      {
        std::cerr << "    mismatched allocation size\n";
      }
      if (!BlockMemory<Size, BlockSize, false>::is_allocated(i))
      {
        throw std::logic_error{"deallocating non-allocated memory"};
      }
    }

    return count;
  }

private:
  std::size_t                                  smallest_alloc_      = static_cast<std::size_t>(-1);
  std::size_t                                  largest_alloc_       = 0;
  std::size_t                                  wastage_             = 0;
  std::chrono::high_resolution_clock::duration time_in_allocator_   = {};
  int                                          allocated_blocks_    = 0;
  int                                          high_water_mark_     = 0;
  int                                          total_allocs_        = 0;
  int                                          total_allocd_blocks_ = 0;
  int                                          single_block_allocs_ = 0;
};

template <std::size_t Size,
          typename Lock         = RwLock,
          std::size_t BlockSize = sizeof(std::max_align_t),
          bool        Debug     = false>
class SyncBlockMemory : public MemoryResource
{
protected:
  [[nodiscard]] void* do_allocate(std::size_t bytes, std::size_t alignment) override
  {
    return memory_.lock()->allocate(bytes, alignment);
  }

  void do_deallocate(void* p, std::size_t bytes, std::size_t alignment) override
  {
    memory_.lock()->deallocate(p, bytes, alignment);
  }

  [[nodiscard]] bool do_is_equal(const MemoryResource& rhs) const noexcept override
  {
    return this == std::addressof(rhs);
  }

private:
  Synchronized<BlockMemory<Size, BlockSize, Debug>, Lock> memory_{};
};

template <std::size_t Size, typename Lock, std::size_t BlockSize>
class SyncBlockMemory<Size, Lock, BlockSize, true> : public MemoryResource
{
public:
  ~SyncBlockMemory() noexcept
  {
    std::cout << "\n"
                 "Allocator Statistics for Synchronized Block Memory Allocator\n"
                 "============================================================\n"
                 "time on allocator synchronization - "
              << std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(time_in_allocator_).count()
              << "ms\n";
  }

protected:
  [[nodiscard]] void* do_allocate(std::size_t bytes, std::size_t alignment) override
  {
    const auto start = std::chrono::high_resolution_clock::now();
    auto       alloc = memory_.lock();
    time_in_allocator_ += std::chrono::high_resolution_clock::now() - start;

    return alloc->allocate(bytes, alignment);
  }

  void do_deallocate(void* p, std::size_t bytes, std::size_t alignment) override
  {
    const auto start = std::chrono::high_resolution_clock::now();
    auto       alloc = memory_.lock();
    time_in_allocator_ += std::chrono::high_resolution_clock::now() - start;

    alloc->deallocate(p, bytes, alignment);
  }

  [[nodiscard]] bool do_is_equal(const MemoryResource& rhs) const noexcept override
  {
    return this == std::addressof(rhs);
  }

private:
  Synchronized<BlockMemory<Size, BlockSize, true>, Lock> memory_{};
  std::chrono::high_resolution_clock::duration           time_in_allocator_ = {};
};
template <typename T, typename Allocator>
struct DeleteUsingAlloc
{
  Allocator allocator = {};

  DeleteUsingAlloc(const Allocator& alloc) : allocator{alloc} {}
  template <typename U, typename Alloc, typename = std::enable_if_t<std::is_base_of<T, U>::value>>
  DeleteUsingAlloc(const DeleteUsingAlloc<U, Alloc>& rhs) : allocator{rhs.allocator} {};
  template <typename U, typename Alloc, typename = std::enable_if_t<std::is_base_of<T, U>::value>>
  DeleteUsingAlloc(DeleteUsingAlloc<U, Alloc>&& rhs) : allocator{std::move(rhs.allocator)} {};

  DeleteUsingAlloc()                                = default;
  DeleteUsingAlloc(const DeleteUsingAlloc& rhs)     = default;
  DeleteUsingAlloc(DeleteUsingAlloc&& rhs) noexcept = default;
  ~DeleteUsingAlloc() noexcept                      = default;
  auto operator=(const DeleteUsingAlloc& rhs) -> DeleteUsingAlloc&
  {
    allocator.~Allocator();
    new (std::addressof(allocator)) Allocator(rhs.allocator);
    return *this;
  }
  auto operator=(DeleteUsingAlloc&& rhs) noexcept -> DeleteUsingAlloc&
  {
    allocator.~Allocator();
    new (std::addressof(allocator)) Allocator(std::move(rhs.allocator));
    return *this;
  }

  template <typename U>
  void operator()(U* ptr) const noexcept
  {
    ptr->~U();
    typename std::allocator_traits<Allocator>::template rebind_alloc<U>{allocator}.deallocate(ptr, 1);
  }
};

template <typename T, typename Allocator>
struct DeleteUsingAlloc<T[], Allocator>
{
};

template <typename T, typename Allocator, typename... Args, std::enable_if_t<!std::is_array<T>::value>* = nullptr>
[[nodiscard]] auto allocate_unique(const Allocator& allocator, Args&&... args)
{
  auto alloc = typename std::allocator_traits<Allocator>::template rebind_alloc<T>{allocator};
  auto ptr   = alloc.allocate(1);
  try
  {
    new (ptr) T(std::forward<Args>(args)...);
  }
  catch (...)
  {
    alloc.deallocate(ptr, 1);
    throw;
  }

  return std::unique_ptr<T, DeleteUsingAlloc<T, Allocator>>{ptr, DeleteUsingAlloc<T, Allocator>{alloc}};
}

}  // namespace etcpal
