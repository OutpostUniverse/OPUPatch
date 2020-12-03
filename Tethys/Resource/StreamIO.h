
#pragma once

#include "Tethys/Common/Memory.h"

namespace Tethys {

BEGIN_PACKED

/// Stream input/output base classa.
class StreamIO : public OP2Class<StreamIO> {
public:
  virtual int GetStatus() const { return Thunk<0x4077C0, &$::GetStatus>(); }

  virtual void* Destroy(ibool freeMem = 0) { return Thunk<0x4AB340, &$::Destroy>(freeMem); }

  virtual size_t Tell()                { return Thunk<0x4AB3B0, &$::Tell>();         }
  virtual ibool  Seek(size_t position) { return Thunk<0x4AB3C0, &$::Seek>(position); }

  virtual int F1(int a) { return Thunk<0x4AB370, &$::F1>(a); }

  virtual ibool Flush()                                 { return Thunk<0x4AB3D0, &$::Flush>();              }
  virtual ibool Write(size_t size, const void* pBuffer) { return Thunk<0x4AB3E0, &$::Write>(size, pBuffer); }
  virtual ibool Read(size_t size, void* pBuffer)        { return Thunk<0x4AB3F0, &$::Read>(size, pBuffer);  }

  virtual void Close() { return Thunk<0x4AB3A0, &$::Close>(); }

#define OP2_STREAMIO_VTBL($)  $(GetStatus)  $(Destroy)  $(Tell)  $(Seek)  $(F1)  $(Flush)  $(Write)  $(Read)  $(Close)
  DEFINE_VTBL_TYPE(OP2_STREAMIO_VTBL)  // ** TODO vtbl address

public:
  size_t position_;
  int    status_;
};


/// Basic file read stream.
class FileRStream : public StreamIO {
  using $ = FileRStream;
public:
  FileRStream()                      { InternalCtor<0x4AB400>();                       }
  FileRStream(HANDLE      hFile)     { InternalCtor<0x4AB460, HANDLE>(hFile);          }
  FileRStream(const char* pFilename) { InternalCtor<0x4AB4B0, const char*>(pFilename); }

  void*  Destroy(ibool freeMem = 0)              override { return Thunk<0x4AB440, &$::Destroy>(freeMem);     }
  size_t Tell()                                  override { return Thunk<0x4AB640, &$::Tell>();               }
  ibool  Seek(size_t position)                   override { return Thunk<0x4AB670, &$::Seek>(position);       }
  int    F1(int a)                               override { return Thunk<0x4AB510, &$::F1>(a);                }
  ibool  Flush()                                 override { return Thunk<0x4AB730, &$::Flush>();              }
  ibool  Write(size_t size, const void* pBuffer) override { return Thunk<0x4AB7C0, &$::Write>(size, pBuffer); }
  ibool  Read(size_t size, void* pBuffer)        override { return Thunk<0x4AB7D0, &$::Read>(size, pBuffer);  }
  void   Close()                                 override { return Thunk<0x4AB590, &$::Close>();              }

  DEFINE_VTBL_GETTER(0x4D7524);

  void SetOpenFileHandle(HANDLE hFile) { return Thunk<0x4AB550, &$::SetOpenFileHandle>(hFile); }
  int  OpenFile(const char* pFilename) { return Thunk<0x4AB5E0, &$::OpenFile>(pFilename);      }

  int GetFileSize() { return Thunk<0x4AB700, &$::GetFileSize>(); }

  int TranslateLastError() { return Thunk<0x4AB740, &$::TranslateLastError>(); }

  int ReadNextChunk(size_t position) { return Thunk<0x4AB8E0, &$::ReadNextChunk>(position); }

public:
  uint8  buffer_[2048];
  size_t positionOfLastReadChunkStart_;
  size_t positionOfLastReadChunkEnd_;
  size_t streamPosition_;
  HANDLE hFile_;
  ibool  ownsFile_;                      ///< Will close the OS file handle on Close
};
static_assert(sizeof(FileRStream) == 0x820, "Incorrect FileRStream size.");

/// Basic file write stream.
class FileWStream : public StreamIO {
  using $ = FileWStream;
public:
  FileWStream()                      { InternalCtor<0x4AB990>();                       }
  FileWStream(HANDLE      hFile)     { InternalCtor<0x4AB9E0, HANDLE>(hFile);          }
  FileWStream(const char* pFilename) { InternalCtor<0x4ABA20, const char*>(pFilename); }

  void*  Destroy(ibool freeMem = 0)              override { return Thunk<0x4AB9C0, &$::Destroy>(freeMem);     }
  size_t Tell()                                  override { return Thunk<0x4ABB90, &$::Tell>();               }
  ibool  Seek(size_t position)                   override { return Thunk<0x4ABBB0, &$::Seek>(position);       }
  int    F1(int a)                               override { return Thunk<0x4ABA70, &$::F1>(a);                }
  ibool  Flush()                                 override { return Thunk<0x4ABC30, &$::Flush>();              }
  ibool  Write(size_t size, const void* pBuffer) override { return Thunk<0x4ABCC0, &$::Write>(size, pBuffer); }
  ibool  Read(size_t size, void* pBuffer)        override { return Thunk<0x4ABD20, &$::Read>(size, pBuffer);  }
  void   Close()                                 override { return Thunk<0x4ABAF0, &$::Close>();              }

  DEFINE_VTBL_GETTER(0x4D754C);

  void SetOpenFileHandle(HANDLE hFile) { return Thunk<0x4ABAB0, &$::SetOpenFileHandle>(hFile); }
  int  OpenFile(const char* pFilename) { return Thunk<0x4ABB30, &$::OpenFile>(pFilename);      }

  int GetFileSize() { return Thunk<0x4ABC00, &$::GetFileSize>(); }

  int TranslateLastError() { return Thunk<0x4ABC40, &$::TranslateLastError>(); }

public:
  HANDLE hFile_;
  ibool  ownsFile_;  ///< Will close the OS file handle on Close
  uint8  field_14;   ///< Unused
};
static_assert(sizeof(FileWStream) == 0x15, "Incorrect FileWStream size.");

/// Basic file read/write stream.  @ref FileRStream is more efficient for reading large files.
class FileRWStream : public StreamIO {
  using $ = FileRWStream;
public:
  FileRWStream()                      { InternalCtor<0x4B0CE0>();                       }
  FileRWStream(HANDLE      hFile)     { InternalCtor<0x4B0D30, HANDLE>(hFile);          }
  FileRWStream(const char* pFilename) { InternalCtor<0x4B0D70, const char*>(pFilename); }
  //~FileRWStream();  // 0x4B0DA0

  void*  Destroy(ibool freeMem = 0)              override { return Thunk<0x4B0D10, &$::Destroy>(freeMem);     }
  size_t Tell()                                  override { return Thunk<0x4B0EE0, &$::Tell>();               }
  ibool  Seek(size_t position)                   override { return Thunk<0x4B0F00, &$::Seek>(position);       }
  int    F1(int a)                               override { return Thunk<0x4B0DC0, &$::F1>(a);                }
  ibool  Flush()                                 override { return Thunk<0x4B0F80, &$::Flush>();              }
  ibool  Write(size_t size, const void* pBuffer) override { return Thunk<0x4B1010, &$::Write>(size, pBuffer); }
  ibool  Read(size_t size, void* pBuffer)        override { return Thunk<0x4B1070, &$::Read>(size, pBuffer);  }
  void   Close()                                 override { return Thunk<0x4B0E40, &$::Close>();              }

  DEFINE_VTBL_GETTER(0x4D76BC);

  void SetOpenFileHandle(HANDLE hFile) { return Thunk<0x4B0E00, &$::SetOpenFileHandle>(hFile); }
  int  OpenFile(const char* pFilename) { return Thunk<0x4B0E80, &$::OpenFile>(pFilename);      }

  int GetFileSize() { return Thunk<0x4B0F50, &$::GetFileSize>(); }

  int TranslateLastError() { return Thunk<0x4B0F90, &$::TranslateLastError>(); }

public:
  HANDLE hFile_;
  ibool  ownsFile_;  ///< Will close the OS file handle on Close
};
static_assert(sizeof(FileRWStream) == 0x14, "Incorrect FileRWStream size.");


/// Basic memory read/write stream.
class MemRWStream : public StreamIO {
  using $ = MemRWStream;
public:
  MemRWStream()                            { InternalCtor<0x4AFE10>();               }
  MemRWStream(size_t size, void* pAddress) { InternalCtor<0x4AFE50>(size, pAddress); }

  void*  Destroy(ibool freeMem = 0)              override { return Thunk<0x4AFE30, &$::Destroy>(freeMem);     }
  size_t Tell()                                  override { return Thunk<0x4AFF40, &$::Tell>();               }
  ibool  Seek(size_t position)                   override { return Thunk<0x4AFF50, &$::Seek>(position);       }
  int    F1(int a)                               override { return Thunk<0x4AFF00, &$::F1>(a);                }
  ibool  Flush()                                 override { return Thunk<0x4AFFA0, &$::Flush>();              }
  ibool  Write(size_t size, const void* pBuffer) override { return Thunk<0x4AFFB0, &$::Write>(size, pBuffer); }
  ibool  Read(size_t size, void* pBuffer)        override { return Thunk<0x4B0020, &$::Read>(size, pBuffer);  }
  void   Close()                                 override { return Thunk<0x4AFEA0, &$::Close>();              }

  DEFINE_VTBL_GETTER(0x4D7644);

  void SetBuffer(size_t size, void* pAddress) { return Thunk<0x4AFE80, &$::SetBuffer>(size, pAddress); }
  int  F2(int a)                              { return Thunk<0x4AFEC0, &$::F2>(a);                     }

public:
  uint8* begin_;
  uint8* end_;
  uint8* currentPos_;
};
static_assert(sizeof(MemRWStream) == 0x18, "Incorrect MemRWStream size.");

END_PACKED

} // Tethys
