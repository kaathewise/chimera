#ifndef SIMPLETOUCH_MEMORY_SDRAM_ALLOC_H_
#define SIMPLETOUCH_MEMORY_SDRAM_ALLOC_H_

#include <stddef.h>

#include <atomic>
#include <new>
#include <utility>

namespace simpletouch {

/**
 * @brief
 * Provides an interface for monotonic, pseudo-dynamic allocation of memory
 * into Daisy SDRAM.
 *
 * @details
 * By default statically reserves 32MB of storage in SDRAM for runtime
 * allocation. Due to Daisy's STM32 lacking an MMU, please note that "dynamic"
 * allocations made this way will never actually be reclaimed during program
 * lifetime.
 *
 * @warning
 * This only works when built on Daisy platform and does not (yet?)
 * handle allocations beyond the max available memory with anything more
 * than a breakpoint.
 *
 */
class SDRAM {
 public:
  // 32 MiB (half of SDRAM) by default
  static constexpr size_t kPoolSize = 32 * 1024 * 1024;

  template <typename T, class... Args>
  static T* allocate(Args&&... args) {
    auto ptr = instance().allocate_raw(sizeof(T), alignof(T));
    return new (ptr) T(std::forward<Args>(args)...);
  }

  // NOTE: UNTESTED
  template <typename T>
  static T* allocate_buf(size_t size) {
    auto ptr = instance().allocate_raw(size * sizeof(T), alignof(T));
    return static_cast<T*>(ptr);
  }

 private:
  SDRAM() = default;
  ~SDRAM() = default;

  static SDRAM& instance() {
    static SDRAM sdram;
    return sdram;
  }

  void* allocate_raw(size_t size, size_t alignment);

  std::atomic_size_t pool_pos_ = {0};
};

}  // namespace simpletouch

#endif  // SIMPLETOUCH_MEMORY_SDRAM_ALLOC_H_
