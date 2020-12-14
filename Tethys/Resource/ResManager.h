
#pragma once

#include "Tethys/Common/Memory.h"
#include "Tethys/Resource/StreamIO.h"

namespace Tethys {

class ResManager : public OP2Class<ResManager> {
public:
  int  Init()             { return Thunk<0x470F70, &$::Init>();             }
  int  InitCDDir()        { return Thunk<0x4719D0, &$::InitCDDir>();        }
  void InitInstalledDir() { return Thunk<0x471850, &$::InitInstalledDir>(); }
  void ShutDown()         { return Thunk<0x471140, &$::ShutDown>();         }

  ibool GetFilePath(const char* pFilename, char* pPath) { return Thunk<0x471590, &$::GetFilePath>(pFilename, pPath); }

  /// OpenStream (and ChecksumStream) will first call GetFilePath, then search in VOLs.
  StreamIO* OpenStream(const char*     pFilename)           { return Thunk<0x471170, &$::OpenStream>(pFilename);       }
  StreamIO* CreateStream(const char*   pFilename)           { return Thunk<0x471B60, &$::CreateStream>(pFilename);     }
  void      ReleaseStream(StreamIO* pStream)                { return Thunk<0x4713D0, &$::ReleaseStream>(pStream);      }
  void*     LockStream(const char* pFilename, size_t* pLen) { return Thunk<0x471430, &$::LockStream>(pFilename, pLen); }
  void      UnlockStream(void* a)                           { return Thunk<0x471490, &$::UnlockStream>(a);             }
  uint32    ChecksumStream(const char* pFilename)           { return Thunk<0x4712A0, &$::ChecksumStream>(pFilename);   }

  int  FindCDRoot(char*      pPath) { return Thunk<0x4714A0, &$::FindCDRoot>(pPath);      }
  void GetCDDir(char*        pPath) { return Thunk<0x471AA0, &$::GetCDDir>(pPath);        }
  void GetInstalledDir(char* pPath) { return Thunk<0x471A70, &$::GetInstalledDir>(pPath); }

  void VerifyCD() { return Thunk<0x471900, &$::VerifyCD>(); }

  static ResManager* GetInstance() { return OP2Mem<0x56C028, ResManager*>(); }

  /// @note This does not check VOLs.
  bool FileExists(const char* pFilename) { char path[MAX_PATH];  return GetFilePath(pFilename, &path[0]); }

public:
  char installedDir_[MAX_PATH];
  char cdDir_[MAX_PATH];
};

inline ResManager& g_resManager = *ResManager::GetInstance();

} // Tethys
