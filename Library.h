
#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <type_traits>

// RAII class wrapping a DLL.
class Library {
  template <typename T>  using FnToPfn = std::conditional_t<std::is_function_v<T>, T*, T>;

public:
   Library(const char* pModuleName, bool load = true)
     : hModule_((load && pModuleName) ? LoadLibraryA(pModuleName) : GetModuleHandleA(pModuleName)), hasHandle_(load) { }
  ~Library() {
    if (hasHandle_ && (hModule_ != NULL)) {
      FreeLibrary(hModule_);
      hModule_ = NULL;
    }
  }

  Library(const Library&)            = delete;
  Library& operator=(const Library&) = delete;

  bool IsLoaded() const { return (hModule_ != NULL); }

  template <typename T>  FnToPfn<T>  Get(const char* pName) const
    { return IsLoaded() ? reinterpret_cast<FnToPfn<T>>(GetProcAddress(hModule_, pName)) : nullptr; }
  template <typename T>  bool Get(const char* pName, FnToPfn<T>* pPfnOut) const
    { return (pPfnOut != nullptr) && (*pPfnOut = Get(pName)); }

  HMODULE GetHandle() const { return hModule_; }
  operator HMODULE()  const { return hModule_; }

private:
  HMODULE hModule_;
  bool    hasHandle_;
};
