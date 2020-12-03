
#pragma once

#include "Tethys/Common/Memory.h"

namespace Tethys {

class CConfig : public OP2Class<CConfig> {
public:
   CConfig(const char* pFilename) { InternalCtor<0x410750, const char*>(pFilename); }
  ~CConfig()                      { Thunk<0x410820>();                              }

  int   CopyIniFile(const char* pExistingFile) { return Thunk<0x410C40, &$::CopyIniFile>(pExistingFile); }
  ibool FileExists()                           { return Thunk<0x410C10, &$::FileExists>();               }
  void  Flush()                                { return Thunk<0x410C00>();                               }

  int GetInt(const char* pKey, int defaultVal) { return Thunk<0x4109B0, int(const char*, int)>(pKey, defaultVal); }
  int GetInt(const char* pSection, const char* pKey, int defaultVal)
    { return Thunk<0x410870, int(const char*, const char*, int)>(pSection, pKey, defaultVal); }
    
  int GetString(const char* pKey, char* pOut, uint32 bufferSize, const char* pDefaultVal = "")
    { return Thunk<0x410B30, int(const char*, char*, uint32, const char*)>(pKey, pOut, bufferSize, pDefaultVal); }
  int GetString(const char* pSection, const char* pKey, char* pOut, uint32 bufferSize, const char* pDefaultVal = "") {
    return Thunk<0x4108B0, int(const char*, const char*, char*, uint32, const char*)>(
      pSection, pKey, pOut, bufferSize, pDefaultVal);
  }

  int SetInt(const char* pKey, int value) { return Thunk<0x4108E0, int(const char*, int)>(pKey, value); }
  int SetInt(const char* pSection, const char* pKey, int value)
    { return Thunk<0x410830, int(const char*, const char*, int)>(pSection, pKey, value); }
    
  int SetString(const char* pKey, const char* pValue)
    { return Thunk<0x410A70, int(const char*, const char*)>(pKey, pValue); }
  int SetString(const char* pSection, const char* pKey, const char* pValue)
    { return Thunk<0x410890, int(const char*, const char*, const char*)>(pSection, pKey, pValue); }

  static CConfig* GetInstance() { return OP2Mem<0x547090, CConfig*>(); }

public:
  char iniPath_[MAX_PATH];
};
static_assert(sizeof(CConfig) == 260, "Incorrect CConfig size.");

inline CConfig& g_configFile = *CConfig::GetInstance();

} // Tethys
