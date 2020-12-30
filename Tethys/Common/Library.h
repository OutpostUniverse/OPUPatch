
#pragma once

#include "Tethys/Common/Memory.h"
#include <type_traits>

namespace Tethys::TethysUtil {

/// RAII class wrapping a DLL.
class Library {
  template <typename T>  using FnToPfn = TethysImpl::FnToPfn<T>;
public:
   Library(const char* pModuleName, bool load = true)
     : hModule_((load && pModuleName) ? LoadLibraryA(pModuleName) : GetModuleHandleA(pModuleName)), ownHandle_(load) { }
  ~Library() {
    if (ownHandle_ && (hModule_ != NULL)) {
      FreeLibrary(hModule_);
      hModule_ = NULL;
    }
  }

  Library(const Library&)            = delete;
  Library& operator=(const Library&) = delete;

  bool IsLoaded() const { return (hModule_ != NULL); }  ///< Is module loaded?

  /// Gets an export by name.
  template <typename T = void*>
  auto Get(const char* pName) const { return IsLoaded() ? FnToPfn<T>(GetProcAddress(hModule_, pName)) : nullptr; }

  /// Gets an export by ordinal.
  template <typename T = void*>  auto Get(uint16 ordinal) const { return Get((char*)(static_cast<uintptr>(ordinal))); }

  /// Gets an export via an output parameter and returns true if it exists.
  template <typename T = void*, typename NameOrOrdinal = const char*>
  bool Get(NameOrOrdinal pName, FnToPfn<T>* pPfnOut) const { return (pPfnOut != nullptr) && (*pPfnOut = Get(pName)); }

  /// Gets an export by name, with built-in init-once.  @note Each template instance is expected to be unique.
  template <auto Pfn, typename NameOrOrdinal = const char*>
  auto Get(NameOrOrdinal pName) const { static auto pfn = Get<decltype(Pfn)>(pName);  return pfn; }

  HMODULE GetHandle() const { return hModule_; }  ///< Gets the OS module handle.
  operator HMODULE()  const { return hModule_; }  ///< Implicitly convers to the OS module handle.

private:
  HMODULE hModule_;
  bool    ownHandle_;
};

} // Tethys::TethysUtil
