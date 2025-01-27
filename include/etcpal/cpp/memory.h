#pragma once

#include <memory>

namespace etcpal
{

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

  void operator()(T* ptr) const noexcept
  {
    ptr->~T();
    typename std::allocator_traits<Allocator>::template rebind_alloc<T>{allocator}.deallocate(ptr, 1);
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
