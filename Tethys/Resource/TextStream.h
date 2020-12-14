
#pragma once

#include "Tethys/Common/Memory.h"

namespace Tethys {

class StreamIO;

/// Internal class used to parse text files such as techtrees.  Sheet text files use @ref SheetParser instead.
class TextStream : public OP2Class<TextStream> {
public:
   TextStream() { InternalCtor<0x48EFF0>(); }
  ~TextStream() { Thunk<0x48F000>();        }

  ibool Open(const char* pFilename) { return Thunk<0x48F020, &$::Open>(pFilename); }
  void  Close()                     { return Thunk<0x48F050, &$::Close>();         }

  void  ResetToLastToken()                        { return Thunk<0x48F070, &$::ResetToLastToken>();              }
  ibool SkipRestOfLine()                          { return Thunk<0x48F090, &$::SkipRestOfLine>();                }
  ibool ReadInt(int* pValue)                      { return Thunk<0x48F0E0, &$::ReadInt>(pValue);                 }
  ibool ReadToken(char*  pToken,  int bufferSize) { return Thunk<0x48F210, &$::ReadToken>(pToken, bufferSize);   }
  ibool ReadString(char* pString, int bufferSize) { return Thunk<0x48F330, &$::ReadString>(pString, bufferSize); }
  ibool FindLineOfLastReadToken()                 { return Thunk<0x48F430, &$::FindLineOfLastReadToken>();       }

public:
  StreamIO* pStream_;
  int       position_; 
};


/// Internal class used to parse Sheet text files.
class SheetParser : public OP2Class<SheetParser> {
public:
   SheetParser() { InternalCtor<0x411470>(); }
  ~SheetParser() { Thunk<0x411260>();        }

  StreamIO* Open(const char* pFilename) { return Thunk<0x411290, &$::Open>(pFilename); }
  void      Close()                     { return Thunk<0x4112E0, &$::Close>();         }

  ibool F1()           { return Thunk<0x411310, &$::F1>();           }
  ibool GetNextField() { return Thunk<0x4113E0, &$::GetNextField>(); }

public:
  // ** TODO verify this is correct
  StreamIO* pStream_;
  ibool     opened_;
  int       field_08;
  int       curField_;
  uint8*    pEOL_;
  uint8     delimiter_;
  int       numFields_;
  uint8     buffer[512];
  uint8*    pBuffer_;
};
static_assert(sizeof(SheetParser) == 0x220, "Incorrect SheetParser size.");

} // Tethys
