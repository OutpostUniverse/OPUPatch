
#pragma once

#include "Tethys/Common/Memory.h"

namespace Tethys {

class MemoryMappedFile : OP2Class<MemoryMappedFile> {
public:
   MemoryMappedFile() : hObject_(NULL) { InternalCtor<0x484530>(); }
  ~MemoryMappedFile() {
    Thunk<0x484540>();
    if ((hObject_ != NULL) && (hObject_ != INVALID_HANDLE_VALUE)) {
      CloseHandle(hObject_);
    }
    if ((hFile_ != NULL) && (hFile_ != INVALID_HANDLE_VALUE)) {
      CloseHandle(hFile_);
    }
  }

  ibool OpenFile(const char* pFilename, ibool writeAccess = false)
    { return Thunk<0x484590, &$::OpenFile>(pFilename, writeAccess); }

  ibool Flush() { return Thunk<0x4847A0, &$::Flush>(); }

  HANDLE DuplicateFileMapping() { return Thunk<0x4847C0, &$::DuplicateFileMapping>(); }

  // ** TODO more member functions
  // ** TODO 0x4846B0
  // ** TODO 0x484800
  // ** TODO 0x484910
  // ** TODO 0x4849B0

public:
  // ** TODO member variables?
  HANDLE hFile_;
  size_t size_;
  void*  pMappedAddress_;
  HANDLE hObject_;
  HANDLE hFileMapping_;
  int    field_14;
};

} // Tethys
