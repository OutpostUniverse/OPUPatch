
#pragma once

#include "Tethys/Common/Memory.h"

namespace Tethys {

class MemoryMappedFile : OP2Class<MemoryMappedFile> {
public:
   MemoryMappedFile() { InternalCtor<0x484530>(); }
  ~MemoryMappedFile() { Thunk<0x484540>();        }

  ibool OpenFile(const char* pFilename, ibool writeAccess = false)
    { return Thunk<0x484590, &$::OpenFile>(pFilename, writeAccess); }

  // ** TODO more member functions

public:
  // ** TODO member variables?
  HFILE  hFile_;
  int    field_04;
  void*  pMappedAddress_;
  int    field_0C;
  HANDLE hFileMapping_;
};

} // Tethys
