#pragma once

#include <cstddef>
#include <cstdlib>

#include <algorithm>
#include <array>
#include <bitset>
#include <chrono>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <vector>

#include <etcpal/cpp/synchronized.h>
#include <etcpal/cpp/rwlock.h>

namespace etcpal
{

#if (__cplusplus >= 201703L)
using namespace std::pmr;
template <typename T = std::byte>
using polymorphic_allocator = std::pmr::polymorphic_allocator<T>;
#else  // #if (__cplusplus >= 201703L)
class memory_resource
{
public:
  virtual ~memory_resource() noexcept = default;

  [[nodiscard]] void* allocate(std::size_t bytes, std::size_t alignment = alignof(std::max_align_t))
  {
    return do_allocate(bytes, alignment);
  }

  void deallocate(void* p, std::size_t bytes, std::size_t alignment = alignof(std::max_align_t))
  {
    do_deallocate(p, bytes, alignment);
  }

  [[nodiscard]] bool is_equal(const memory_resource& rhs) const noexcept { return do_is_equal(rhs); }

protected:
  [[nodiscard]] virtual void* do_allocate(std::size_t bytes, std::size_t alignment)            = 0;
  virtual void                do_deallocate(void* p, std::size_t bytes, std::size_t alignment) = 0;
  [[nodiscard]] virtual bool  do_is_equal(const memory_resource& rhs) const noexcept           = 0;
};

namespace detail
{

[[nodiscard]] inline auto default_resource_ptr() noexcept -> memory_resource*&
{
  struct MallocResource : public memory_resource
  {
  protected:
    [[nodiscard]] void* do_allocate(std::size_t bytes, std::size_t alignment) override { return ::malloc(bytes); }
    void                do_deallocate(void* p, std::size_t bytes, std::size_t alignment) override { ::free(p); }
    [[nodiscard]] bool  do_is_equal(const memory_resource& rhs) const noexcept override
    {
      return this == std::addressof(rhs);
    }
  };

  static MallocResource   malloc_resource{};
  static memory_resource* resource = std::addressof(malloc_resource);

  return resource;
}

}  // namespace detail

[[nodiscard]] inline auto set_default_resource(memory_resource* resource) noexcept -> memory_resource*
{
  return detail::default_resource_ptr() = resource;
}

[[nodiscard]] inline auto get_default_resource() noexcept -> memory_resource*
{
  return detail::default_resource_ptr();
}
template <typename T = unsigned char>
class polymorphic_allocator
{
public:
  using value_type = T;

  polymorphic_allocator() noexcept = default;
  template <typename U>
  polymorphic_allocator(const polymorphic_allocator<U>& rhs) noexcept : resource_{rhs.resource()}
  {
  }
  polymorphic_allocator(memory_resource* res) noexcept : resource_{res} {}

  void               deallocate(T* p, std::size_t n) { resource_->deallocate(p, sizeof(T) * n, alignof(T)); }
  [[nodiscard]] auto allocate(std::size_t n) -> T*
  {
    return reinterpret_cast<T*>(resource_->allocate(sizeof(T) * n, alignof(T)));
  }

  [[nodiscard]] auto resource() const noexcept -> memory_resource* { return resource_; }

  [[nodiscard]] friend bool operator==(const polymorphic_allocator& lhs, const polymorphic_allocator& rhs) noexcept
  {
    return (lhs.resource_ && rhs.resource_) || lhs.resource_->is_equal(*rhs.resource_);
  }
  [[nodiscard]] friend bool operator!=(const polymorphic_allocator& lhs, const polymorphic_allocator& rhs) noexcept
  {
    return !(lhs == rhs);
  }

private:
  memory_resource* resource_ = get_default_resource();
};

[[nodiscard]] inline auto null_memory_resource() noexcept -> memory_resource*
{
  struct NullResource : public memory_resource
  {
  protected:
    [[nodiscard]] void* do_allocate(std::size_t bytes, std::size_t alignment) override { throw std::bad_alloc{}; }
    void                do_deallocate(void* p, std::size_t bytes, std::size_t alignment) override {}
    [[nodiscard]] bool  do_is_equal(const memory_resource& rhs) const noexcept override
    {
      return this == std::addressof(rhs);
    }
  };

  static NullResource resource{};

  return std::addressof(resource);
}

#endif  // #if (__cplusplus >= 201703L)

namespace detail
{

template <std::size_t Size, bool Debug>
class MonotonicBufferStatistics
{
public:
  static constexpr void report_deallocation() noexcept {}
  static constexpr void report_allocation(const unsigned char* prev_end,
                                          std::size_t          bytes,
                                          const unsigned char* new_end) noexcept
  {
  }

  [[nodiscard]] static constexpr auto report_allocator_entry() noexcept
  {
    return finally([] {});
  }
};

template <std::size_t Size>
class MonotonicBufferStatistics<Size, true>
{
public:
  ~MonotonicBufferStatistics() noexcept
  {
    std::cout << "\n"
                 "+++++++++++++++++++++++++++++++++++++++++\n"
                 "Allocator Statistics for Monotonic Buffer\n"
                 "=========================================\n"
                 "total allocations       - "
              << total_allocs_
              << " allocations\n"
                 "                        - "
              << total_bytes_allocd_
              << " bytes\n"
                 "wastage                 - "
              << (Size - total_bytes_allocd_ + padding_bytes_) << " bytes ("
              << (Size - total_bytes_allocd_ + padding_bytes_) * 100.0 / Size
              << "%)\n"
                 "                        - "
              << Size - total_bytes_allocd_
              << " bytes left over\n"
                 "                        - "
              << padding_bytes_
              << " bytes padding\n"
                 "total time in allocator - "
              << std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(time_in_alloc_).count()
              << "ms\n"
                 "-----------------------------------------\n";
    if (current_allocs_ != 0)
    {
      throw std::logic_error{"allocation count is not zero"};
    }
  }

  void report_deallocation() noexcept { --current_allocs_; }
  void report_allocation(const unsigned char* prev_end, std::size_t bytes, const unsigned char* new_end) noexcept
  {
    ++current_allocs_;
    ++total_allocs_;
    total_bytes_allocd_ += new_end - prev_end;
    padding_bytes_ += new_end - prev_end - bytes;
  }

  [[nodiscard]] auto report_allocator_entry() noexcept
  {
    return finally([&, start = std::chrono::high_resolution_clock::now()] {
      time_in_alloc_ += std::chrono::high_resolution_clock::now() - start;
    });
  }

private:
  int                                          current_allocs_     = 0;
  int                                          total_allocs_       = 0;
  std::size_t                                  total_bytes_allocd_ = 0;
  std::size_t                                  padding_bytes_      = 0;
  std::chrono::high_resolution_clock::duration time_in_alloc_      = {};
};

}  // namespace detail

template <std::size_t Size = std::numeric_limits<std::uint8_t>::max(), bool Debug = false>
class MonotonicBuffer : public memory_resource, private detail::MonotonicBufferStatistics<Size, Debug>
{
private:
  using Stats = detail::MonotonicBufferStatistics<Size, Debug>;

public:
  MonotonicBuffer()                                              = default;
  MonotonicBuffer(const MonotonicBuffer& rhs)                    = delete;
  MonotonicBuffer(MonotonicBuffer&& rhs)                         = delete;
  ~MonotonicBuffer() noexcept override                           = default;
  auto operator=(const MonotonicBuffer& rhs) -> MonotonicBuffer& = delete;
  auto operator=(MonotonicBuffer&& rhs) -> MonotonicBuffer&      = delete;

  [[nodiscard]] auto  free_bytes() const noexcept { return Size - allocd_bytes_; }
  [[nodiscard]] bool  owns(void* p) const noexcept { return (p >= buffer_.data()) && (p < buffer_.data() + Size); }
  [[nodiscard]] bool  empty() const noexcept { return num_allocs_ == 0; }
  [[nodiscard]] void* try_allocate(std::size_t bytes, std::size_t alignment) noexcept
  {
    const auto timer = Stats::report_allocator_entry();
    return try_alloc_impl(bytes, alignment);
  }

protected:
  [[nodiscard]] void* do_allocate(std::size_t bytes, std::size_t alignment) override
  {
    const auto timer = Stats::report_allocator_entry();
    if (auto* const p = try_alloc_impl(bytes, alignment))
    {
      return p;
    }

    throw std::bad_alloc{};
  }

  void do_deallocate(void* p, std::size_t bytes, std::size_t alignment) override
  {
    const auto timer = Stats::report_allocator_entry();
    Stats::report_deallocation();
    --num_allocs_;
  }

  [[nodiscard]] bool do_is_equal(const memory_resource& rhs) const noexcept override
  {
    return this == std::addressof(rhs);
  }

private:
  [[nodiscard]] void* try_alloc_impl(std::size_t bytes, std::size_t alignment) noexcept
  {
    auto* const init_end = buffer_.data() + allocd_bytes_;
    auto*       curr     = static_cast<void*>(init_end);
    auto        free     = free_bytes();
    auto* const p        = std::align(alignment, bytes, curr, free);
    if (!p)
    {
      return p;
    }

    ++num_allocs_;
    allocd_bytes_ = static_cast<unsigned char*>(p) + bytes - buffer_.data();
    Stats::report_allocation(init_end, bytes, buffer_.data() + allocd_bytes_);

    return p;
  }

  alignas(std::max_align_t) std::array<unsigned char, Size> buffer_;
  std::size_t allocd_bytes_ = 0;
  std::size_t num_allocs_   = 0;
};

template <std::size_t Size, std::size_t BlockSize = sizeof(std::max_align_t), bool Debug = false>
class BlockMemory : public memory_resource
{
public:
  BlockMemory()                                          = default;
  BlockMemory(const BlockMemory& rhs)                    = delete;
  BlockMemory(BlockMemory&& rhs)                         = delete;
  ~BlockMemory() noexcept override                       = default;
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

  [[nodiscard]] bool do_is_equal(const memory_resource& rhs) const noexcept override
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
  ~BlockMemory() noexcept override
  {
    std::cout << "\n"
                 "+++++++++++++++++++++++++++++++++++++++++++++++\n"
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
                 "                         - "
              << BlockMemory<Size, BlockSize, false>::to_blocks(smallest_alloc_)
              << " blocks\n"
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
              << "ms\n"
                 "-----------------------------------------------\n";
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
class SyncBlockMemory : public memory_resource
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

  [[nodiscard]] bool do_is_equal(const memory_resource& rhs) const noexcept override
  {
    return this == std::addressof(rhs);
  }

private:
  Synchronized<BlockMemory<Size, BlockSize, Debug>, Lock> memory_{};
};

template <std::size_t Size, typename Lock, std::size_t BlockSize>
class SyncBlockMemory<Size, Lock, BlockSize, true> : public memory_resource
{
public:
  ~SyncBlockMemory() noexcept override
  {
    std::cout << "\n"
                 "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n"
                 "Allocator Statistics for Synchronized Block Memory Allocator\n"
                 "============================================================\n"
                 "time on allocator synchronization - "
              << std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(time_in_allocator_).count()
              << "ms\n"
                 "------------------------------------------------------------\n";
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

  [[nodiscard]] bool do_is_equal(const memory_resource& rhs) const noexcept override
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

namespace detail
{

template <std::size_t SmallSize, bool Debug>
class DualLevelBlockPoolStatistics
{
public:
  static constexpr void               report_new_small_buffer() noexcept {}
  static constexpr void               report_small_deallocation(std::size_t wastage) noexcept {}
  static constexpr void               report_small_allocation(std::size_t bytes, std::size_t padding) noexcept {}
  static constexpr void               report_large_allocation(std::size_t bytes) noexcept {}
  [[nodiscard]] static constexpr auto report_lock() noexcept { return true; }
  static constexpr void               report_locked(bool arg) noexcept {}
  [[nodiscard]] static constexpr auto report_allocator_entry() noexcept
  {
    return finally([] {});
  }
};

template <std::size_t SmallSize>
class DualLevelBlockPoolStatistics<SmallSize, true>
{
public:
  static constexpr auto small_size = SmallSize;

  ~DualLevelBlockPoolStatistics() noexcept
  {
    std::cout << "\n"
                 "++++++++++++++++++++++++++++++++++++++++++++++\n"
                 "Allocator Statistics for Dual Level Block Pool\n"
                 "==============================================\n"
                 "blocks                       - "
              << total_block_bytes_ / 1024.0 << " kiB ("
              << total_block_bytes_ * 100.0 / (total_small_buffer_bytes_ + total_block_bytes_)
              << "%)\n"
                 "small buffers                - "
              << total_small_buffers_ << " x " << small_size / 1024.0
              << " kiB\n"
                 "                             - "
              << total_small_buffers_ * small_size / 1024.0 << " kiB ("
              << total_small_buffer_bytes_ * 100.0 / (total_small_buffer_bytes_ + total_block_bytes_)
              << "%)\n"
                 "small buffer high water mark - "
              << max_small_buffers_
              << " buffers\n"
                 "                             - "
              << max_small_buffers_ * small_size / 1024.0
              << " kiB\n"
                 "small buffer wastage         - "
              << small_buffer_wastage_ / 1024.0 << " kiB ("
              << small_buffer_wastage_ * 100.0 / (total_small_buffers_ * small_size)
              << "%)\n"
                 "                             - "
              << small_buffer_padding_
              << " bytes padding\n"
                 "time spent synchronizing     - "
              << std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(time_spent_locking_).count()
              << "ms (" << time_spent_locking_.count() * 100.0 / total_time_.count()
              << "%)\n"
                 "time spent in allocator      - "
              << std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(total_time_).count()
              << "ms\n"
                 "----------------------------------------------\n";
  }

  void report_new_small_buffer() noexcept
  {
    ++total_small_buffers_;
    ++curr_small_buffers_;
    max_small_buffers_ = std::max(curr_small_buffers_, max_small_buffers_);
  }

  void report_small_deallocation(std::size_t wastage) noexcept
  {
    small_buffer_wastage_ += wastage;
    --curr_small_buffers_;
  }

  void report_large_allocation(std::size_t bytes) noexcept { total_block_bytes_ += bytes; }
  void report_small_allocation(std::size_t bytes, std::size_t padding) noexcept
  {
    total_small_buffer_bytes_ += bytes;
    small_buffer_wastage_ += padding;
    small_buffer_padding_ += padding;
  }

  [[nodiscard]] auto report_lock() const noexcept { return std::chrono::high_resolution_clock::now(); }
  void               report_locked(const std::chrono::high_resolution_clock::time_point& start) noexcept
  {
    time_spent_locking_ += std::chrono::high_resolution_clock::now() - start;
  }

  [[nodiscard]] auto report_allocator_entry() noexcept
  {
    return finally([&, start = std::chrono::high_resolution_clock::now()] {
      total_time_ += std::chrono::high_resolution_clock::now() - start;
    });
  }

private:
  int                                          total_small_buffers_      = 0;
  int                                          curr_small_buffers_       = 0;
  int                                          max_small_buffers_        = 0;
  std::size_t                                  small_buffer_wastage_     = 0;
  std::size_t                                  small_buffer_padding_     = 0;
  std::size_t                                  total_block_bytes_        = 0;
  std::size_t                                  total_small_buffer_bytes_ = 0;
  std::chrono::high_resolution_clock::duration time_spent_locking_       = {};
  std::chrono::high_resolution_clock::duration total_time_               = {};
};

[[nodiscard]] constexpr auto make_max_aligned(std::size_t size) noexcept
{
  return (size % alignof(std::max_align_t) == 0) ? size
                                                 : ((size / alignof(std::max_align_t)) + 1) * alignof(std::max_align_t);
}

struct DummyLock
{
  static constexpr void lock() noexcept {}
  static constexpr void unlock() noexcept {}
  static constexpr bool try_lock() noexcept { return true; }
};

}  // namespace detail

template <std::size_t Size,
          std::size_t SmallBlockSize =
              detail::make_max_aligned(std::max<std::size_t>(Size >> 10, std::numeric_limits<unsigned char>::max())),
          typename Lock = detail::DummyLock,
          std::size_t BlockSize =
              detail::make_max_aligned(sizeof(Synchronized<MonotonicBuffer<SmallBlockSize>, Lock>) >> 5),
          bool Debug = false>
class BasicDualLevelBlockPool : public memory_resource, detail::DualLevelBlockPoolStatistics<SmallBlockSize, Debug>
{
public:
  using LockType = Lock;

  static constexpr auto capacity   = Size;
  static constexpr auto small_size = SmallBlockSize;
  static constexpr auto block_size = BlockSize;
  static constexpr bool debug      = Debug;

private:
  using Stats = detail::DualLevelBlockPoolStatistics<small_size, debug>;

public:
  BasicDualLevelBlockPool() { small_pools_.lock()->resize(0); }
  BasicDualLevelBlockPool(const BasicDualLevelBlockPool& rhs) = delete;
  BasicDualLevelBlockPool(BasicDualLevelBlockPool&& rhs)      = delete;

  ~BasicDualLevelBlockPool() noexcept override
  {
    if (debug)
    {
      const auto small_pools = small_pools_.clock();
      for (const auto& pool : *small_pools)
      {
        Stats::report_small_deallocation(pool->lock()->free_bytes());
      }
    }
  }

  auto operator=(const BasicDualLevelBlockPool& rhs) -> BasicDualLevelBlockPool& = delete;
  auto operator=(BasicDualLevelBlockPool&& rhs) -> BasicDualLevelBlockPool&      = delete;

private:
  using SmallBuffer     = MonotonicBuffer<small_size>;
  using SyncSmallBuffer = Synchronized<SmallBuffer, LockType>;
  using SmallBufferPtr  = std::unique_ptr<SyncSmallBuffer, DeleteUsingAlloc<SyncSmallBuffer, polymorphic_allocator<>>>;

protected:
  [[nodiscard]] void* do_allocate(std::size_t bytes, std::size_t alignment) override
  {
    const auto timer = Stats::report_allocator_entry();
    if (bytes <= small_size)
    {
      return allocate_from_small_buffers(bytes, alignment);
    }

    Stats::report_large_allocation(bytes);

    return pool_.allocate(bytes, alignment);
  }

  void do_deallocate(void* p, std::size_t bytes, std::size_t alignment) override
  {
    const auto timer = Stats::report_allocator_entry();
    if (try_deallocate_from_small_buffers(p, bytes, alignment))
    {
      return;
    }

    pool_.deallocate(p, bytes, alignment);
  }

  [[nodiscard]] bool do_is_equal(const memory_resource& rhs) const noexcept override
  {
    return this == std::addressof(rhs);
  }

  [[nodiscard]] void* try_allocate_from_existing_small_buffers(std::size_t bytes, std::size_t alignment) noexcept
  {
    const auto pool_lock_start = Stats::report_lock();
    auto       small_pools     = small_pools_.clock();
    Stats::report_locked(pool_lock_start);
    for (const auto& small_pool_ptr : *small_pools)
    {
      const auto buff_lock_start = Stats::report_lock();
      auto       pool            = small_pool_ptr->try_lock();
      Stats::report_locked(buff_lock_start);
      if (!pool)
      {
        continue;
      }

      const auto prev_free = (*pool)->free_bytes();
      if (auto* const p = (*pool)->try_allocate(bytes, alignment))
      {
        Stats::report_small_allocation(bytes, prev_free - (*pool)->free_bytes() - bytes);
        return p;
      }
    }

    return nullptr;
  }

  [[nodiscard]] void* allocate_from_small_buffers(std::size_t bytes, std::size_t alignment)
  {
    if (auto* const p = try_allocate_from_existing_small_buffers(bytes, alignment))
    {
      return p;
    }

    const auto start = Stats::report_lock();
    auto       pools = small_pools_.lock();
    Stats::report_locked(start);
    pools->erase(std::partition(pools->begin(), pools->end(),
                                [&](const SmallBufferPtr& ptr) {
                                  const auto check_lock_start = Stats::report_lock();
                                  const auto pool             = ptr->lock();
                                  Stats::report_locked(check_lock_start);
                                  if (pool->empty() && (pool->free_bytes() < block_size))
                                  {
                                    Stats::report_small_deallocation(pool->free_bytes());
                                    return false;
                                  }

                                  return true;
                                }),
                 pools->end());

    Stats::report_new_small_buffer();
    pools->push_back(allocate_unique<SyncSmallBuffer>(polymorphic_allocator<>{std::addressof(pool_)}));

    const auto new_lock_start = Stats::report_lock();
    auto       new_pool       = pools->back()->lock();
    Stats::report_locked(new_lock_start);
    Stats::report_small_allocation(bytes, 0);

    return new_pool->allocate(bytes, alignment);
  }

  [[nodiscard]] bool try_deallocate_from_small_buffers(void* p, std::size_t bytes, std::size_t alignment)
  {
    const auto pool_lock_start = Stats::report_lock();
    auto       small_pools     = small_pools_.clock();
    Stats::report_locked(pool_lock_start);
    for (auto& pool_ptr : *small_pools)
    {
      const auto buffer_lock_start = Stats::report_lock();
      auto       pool              = pool_ptr->lock();
      Stats::report_locked(buffer_lock_start);
      if (pool->owns(p))
      {
        pool->deallocate(p, bytes, alignment);
        return true;
      }
    }

    return false;
  }

private:
  SyncBlockMemory<capacity, LockType, block_size, debug>                       pool_{};
  Synchronized<std::vector<SmallBufferPtr, polymorphic_allocator<>>, LockType> small_pools_{
      small_size / sizeof(SmallBufferPtr), polymorphic_allocator<>{std::addressof(pool_)}};
};

template <std::size_t Size,
          std::size_t SmallBlockSize = BasicDualLevelBlockPool<Size>::small_size,
          std::size_t BlockSize      = BasicDualLevelBlockPool<Size, SmallBlockSize>::block_size,
          bool        Debug          = false>
using DualLevelBlockPool = BasicDualLevelBlockPool<Size, SmallBlockSize, detail::DummyLock, BlockSize, Debug>;
template <std::size_t Size,
          std::size_t SmallBlockSize = DualLevelBlockPool<Size>::small_size,
          std::size_t BlockSize      = DualLevelBlockPool<Size, SmallBlockSize>::block_size,
          typename Lock              = RwLock,
          bool Debug                 = false>
using SyncDualLevelBlockPool = BasicDualLevelBlockPool<Size, SmallBlockSize, Lock, BlockSize, Debug>;
template <std::size_t Size,
          std::size_t SmallBlockSize = DualLevelBlockPool<Size>::small_size,
          std::size_t BlockSize      = DualLevelBlockPool<Size, SmallBlockSize>::block_size>
using DebugDualLevelBlockPool = DualLevelBlockPool<Size, SmallBlockSize, BlockSize, true>;
template <std::size_t Size,
          std::size_t SmallBlockSize = SyncDualLevelBlockPool<Size>::small_size,
          std::size_t BlockSize      = SyncDualLevelBlockPool<Size, SmallBlockSize>::block_size,
          typename Lock              = typename SyncDualLevelBlockPool<Size, BlockSize>::LockType>
using DebugSyncDualLevelBlockPool = SyncDualLevelBlockPool<Size, SmallBlockSize, BlockSize, Lock, true>;

class MemoryPerformanceRecorder : public memory_resource
{
public:
  MemoryPerformanceRecorder(memory_resource& upstream) noexcept : upstream_{std::addressof(upstream)} {}
  MemoryPerformanceRecorder(const MemoryPerformanceRecorder& rhs) = delete;
  MemoryPerformanceRecorder(MemoryPerformanceRecorder&& rhs)      = delete;

  ~MemoryPerformanceRecorder() noexcept
  {
    std::cout << "\n"
                 "++++++++++++++++++++++++++++++++++++++++++++++++\n"
                 "Allocator Statistics for Generic Memory Resource\n"
                 "================================================\n"
                 "time spent in allocator - "
              << std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(time_spent_).count() << "ms\n";
  }

  auto operator=(const MemoryPerformanceRecorder& rhs) -> MemoryPerformanceRecorder& = delete;
  auto operator=(MemoryPerformanceRecorder&& rhs) -> MemoryPerformanceRecorder&      = delete;

protected:
  [[nodiscard]] void* do_allocate(std::size_t bytes, std::size_t alignment) override
  {
    const auto  start = std::chrono::high_resolution_clock::now();
    auto* const p     = upstream_->allocate(bytes, alignment);
    time_spent_ += std::chrono::high_resolution_clock::now() - start;

    return p;
  }

  void do_deallocate(void* p, std::size_t bytes, std::size_t alignment) override
  {
    const auto start = std::chrono::high_resolution_clock::now();
    upstream_->deallocate(p, bytes, alignment);
    time_spent_ += std::chrono::high_resolution_clock::now() - start;
  }

  [[nodiscard]] bool do_is_equal(const memory_resource& rhs) const noexcept override
  {
    return this == std::addressof(rhs);
  }

private:
  memory_resource*                             upstream_;
  std::chrono::high_resolution_clock::duration time_spent_ = {};
};

}  // namespace etcpal
