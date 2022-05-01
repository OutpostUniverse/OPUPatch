
#pragma once

#include "Tethys/Common/Types.h"
#include "Tethys/Common/Memory.h"
#include "Tethys/Game/TApp.h"

#include "Version.h"

#if defined(_WIN32)
# include <intrin.h>
#elif defined(__GNUC__)
# include <cpuid.h>
#endif

namespace Tethys { class StreamIO; }


inline HINSTANCE g_hInst = nullptr;  ///< HINSTANCE of this DLL.  DllMain must set this during DLL_PROCESS_ATTACH!


constexpr Tethys::GameVersion OP2Version = { OP2_MAJOR_VERSION, OP2_MINOR_VERSION, OP2_STEPPING_VERSION };


/// Returns true if the virtual machine hypervisor bit of cpuid is set.
inline bool IsVirtualMachine() {
  uint32_t out[4] = { };
#if defined(_WIN32)
  __cpuid(reinterpret_cast<int*>(&out[0]), 0x1);
#elif defined(__GNUC__)
  __get_cpuid(0x1, &out[0], &out[1], &out[2], &out[3]);
#endif
  return (out[2] & (1u << 31)) != 0;
}


/// Thread-local storage type compatible with Windows XP, which does not allow implicit TLS in runtime-loaded DLLs.
template <typename T>
class ThreadLocal {
  static_assert(sizeof(T) <= sizeof(void*), "T is too large for ThreadLocal.");
public:
   ThreadLocal() : handle(TlsAlloc()) { }
   ThreadLocal(const ThreadLocal& src) : ThreadLocal() { IsValid() && src.IsValid() && Set(src.Get()); }
   ThreadLocal(ThreadLocal&& src) : handle(src.handle) { src.handle = TLS_OUT_OF_INDEXES; }
  ~ThreadLocal() { if (IsValid()) { TlsFree(handle);  handle = TLS_OUT_OF_INDEXES; } }

  bool IsValid() const { return (handle != TLS_OUT_OF_INDEXES); }

  T    Get() const         { return T(TlsGetValue(handle));                }
  bool Set(const T& value) { return   TlsSetValue(handle, (void*)(value)); }

  operator T() const { return Get(); }

  template <typename T2>  bool operator==(const ThreadLocal<T2>& other) const { return (Get() == other.Get()); }
  template <typename T2>  bool operator!=(const ThreadLocal<T2>& other) const { return (Get() != other.Get()); }

  template <typename T2>  ThreadLocal& operator=(const   T2& value) { Set(value);           return *this; }
  template <typename T2>  ThreadLocal& operator+=(const  T2& value) { Set(Get() +  value);  return *this; }
  template <typename T2>  ThreadLocal& operator-=(const  T2& value) { Set(Get() -  value);  return *this; }
  template <typename T2>  ThreadLocal& operator*=(const  T2& value) { Set(Get() *  value);  return *this; }
  template <typename T2>  ThreadLocal& operator/=(const  T2& value) { Set(Get() /  value);  return *this; }
  template <typename T2>  ThreadLocal& operator&=(const  T2& value) { Set(Get() &  value);  return *this; }
  template <typename T2>  ThreadLocal& operator|=(const  T2& value) { Set(Get() |  value);  return *this; }
  template <typename T2>  ThreadLocal& operator^=(const  T2& value) { Set(Get() ^  value);  return *this; }
  template <typename T2>  ThreadLocal& operator<<=(const T2& value) { Set(Get() << value);  return *this; }
  template <typename T2>  ThreadLocal& operator>>=(const T2& value) { Set(Get() >> value);  return *this; }

  template <typename U = T, typename = decltype(std::declval<U>() + 1)>
  ThreadLocal& operator++() { Set(Get() + 1);  return *this; }
  template <typename U = T, typename = decltype(std::declval<U>() - 1)>
  ThreadLocal& operator--() { Set(Get() - 1);  return *this; }
  template <typename U = T, typename = decltype(std::declval<U>() + 1)>
  T operator++(int) { const T old = Get();  Set(old + 1);  return old; }
  template <typename U = T, typename = decltype(std::declval<U>() - 1)>
  T operator--(int) { const T old = Get();  Set(old - 1);  return old; }

private:
  DWORD handle;
};


/// Printfs a debug message.  You need to include stdio.h and windows.h to use this.
#define DEBUG_MSG(format, ...)  \
  { char dbgStr[1024]; snprintf(dbgStr, sizeof(dbgStr), format "\n", __VA_ARGS__); OutputDebugStringA(dbgStr); } (void)0

#if _DEBUG || ENABLE_DEBUG_MSG
# define DEBUG_ONLY_MSG(format, ...)  DEBUG_MSG(format, __VA_ARGS__)
#else
# define DEBUG_ONLY_MSG(format, ...)
#endif
