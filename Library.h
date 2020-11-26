
#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

// RAII class wrapping a DLL.
class Library {
public:
   Library(const char* pModuleName, bool load = true)
     : hModule_(load ? LoadLibraryA(pModuleName) : GetModuleHandleA(pModuleName)) { }
  ~Library() {
    if (hModule_ != NULL) {
      FreeLibrary(hModule_);
      hModule_ = NULL;
    }
  }

  Library(const Library&)            = delete;
  Library& operator=(const Library&) = delete;

  bool IsLoaded() const { return (hModule_ != NULL); }

  template <typename Fn>  Fn*  GetFunction(const char* pName) const
    { return IsLoaded() ? reinterpret_cast<Fn*>(GetProcAddress(hModule_, pName)) : nullptr; }
  template <typename Fn>  bool GetFunction(const char* pName, Fn** pPfnOut) const
    { return (pPfnOut != nullptr) && (*pPfnOut = GetFunction(pName)); }

  HMODULE GetHandle() const { return hModule_; }
  operator HMODULE()  const { return hModule_; }

private:
  HMODULE hModule_;
};
