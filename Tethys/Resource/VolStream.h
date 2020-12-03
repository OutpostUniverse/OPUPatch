
#pragma once

#include "Tethys/Resource/CodecStream.h"

namespace Tethys {

enum class CompressionCode : uint8 {
  Uncompressed = 0,
  RLE,
  LZ,
  LZH,
};

BEGIN_PACKED

struct VolIndexEntry {
  uint32          fileNameOffset;   ///< Offset in string table
  uint32          dataOffset;       ///< Offset to the data in the Vol file
  uint32          dataLength;       ///< Length of the internal file data
  CompressionCode compressionCode;  ///< Compression used on file data
  uint8           bUsed;            ///< Indicates index entry is valid (Not quite bool? Tested against 1)
                                    ///  [Unused entries must only occur at the end of the table (binary searched)]
};

struct VBlkHeader {
  uint32 tag;             ///< 'VBLK'
  uint32 size      : 31;  ///< Size, including header
  uint32 alignment :  1;  ///< 0 = 16 bit, 1 = 32 bit
};


/// Base Vol VBLK read/write stream.
class BaseVBlkRWStream : public StreamIO {
public:
  BaseVBlkRWStream()                  { InternalCtor<0x406E20>(); }
  BaseVBlkRWStream(InternalCtorChain) {                           }

  void*  Destroy(ibool freeMem = 0)              override { return Thunk<0x406E50, &$::Destroy>(freeMem);     }
  size_t Tell()                                  override { return Thunk<0x406EB0, &$::Tell>();               }
  ibool  Seek(size_t position)                   override { return Thunk<0x406ED0, &$::Seek>(position);       }
  int    F1(int a)                               override { return Thunk<0x406E90, &$::F1>(a);                }
  ibool  Flush()                                 override { return Thunk<0x406F60, &$::Flush>();              }
  ibool  Write(size_t size, const void* pBuffer) override { return Thunk<0x406FA0, &$::Write>(size, pBuffer); }
  ibool  Read(size_t size, void* pBuffer)        override { return Thunk<0x407080, &$::Read>(size, pBuffer);  }
  void   Close()                                 override { return Thunk<0x4AB3A0, &$::Close>();              }

  DEFINE_VTBL_GETTER(0x4CF548);

public:
  StreamIO*  pContainerStream_;  ///< Vol file image
  VBlkHeader header_;
  size_t     positionOffset_;    ///< Offset within vol file  [Current stream position]
  size_t     startOffset_;       ///< Offset within vol file  [Offset of header, data is at +8]
};
static_assert(sizeof(BaseVBlkRWStream) == 0x20, "Incorrect BaseVBlkRWStream size.");

/// Used to read Vol header sections  ['VOL ', 'volh', 'vols', 'voli']
class HeaderVBlkRWStream : public BaseVBlkRWStream {
  using $ = HeaderVBlkRWStream;
public:
  HeaderVBlkRWStream() : BaseVBlkRWStream(UseInternalCtorChain) { InternalCtor<0x407470>(); }

  void* Destroy(ibool freeMem = 0) override { return Thunk<0x4074A0, &$::Destroy>(freeMem);     }
  void  Close()                    override { return Thunk<0x407720, &$::Close>();              }

  DEFINE_VTBL_GETTER(0x4CF598);

  int OpenRead(StreamIO* pContainerStream, uint32 tag)
    { return Thunk<0x407620, int(StreamIO*, uint32)>(pContainerStream, tag); }
  int OpenRead(StreamIO* pContainerStream) { return Thunk<0x4076B0, int(StreamIO*)>(pContainerStream); }
};
static_assert(sizeof(HeaderVBlkRWStream) == 0x20, "Incorrect HeaderVBlkRWStream size.");

/// Vol VBLK read/write stream.
class VBlkRWStream : public BaseVBlkRWStream {
  using $ = VBlkRWStream;
public:
  VBlkRWStream() : BaseVBlkRWStream(UseInternalCtorChain) { InternalCtor<0x407150>(); }

  void* Destroy(ibool freeMem = 0) override { return Thunk<0x407180, &$::Destroy>(freeMem); }
  void  Close()                    override { return Thunk<0x407370, &$::Close>();          }

  DEFINE_VTBL_GETTER(0x4CF570);

  int OpenWrite(StreamIO* pContainerStream, uint32 tag, int size, ibool align32)
    { return Thunk<0x4071E0, &$::OpenWrite>(pContainerStream, tag, size, align32); }
  int OpenRead(StreamIO* pContainerStream, uint32 tag)
    { return Thunk<0x407270, int(StreamIO*, uint32)>(pContainerStream, tag); }
  int OpenRead(StreamIO* pContainerStream) { return Thunk<0x407300, int(StreamIO*)>(pContainerStream); }
};
static_assert(sizeof(VBlkRWStream) == 0x20, "Incorrect VBlkRWStream size.");


/// Base Vol file stream abstract class.
class BaseVolFileStream : public StreamIO {
  using $ = BaseVolFileStream;
public:
  BaseVolFileStream()                  { InternalCtor<0x49ED60>(); }
  BaseVolFileStream(InternalCtorChain) {                           }

  int   GetStatus()          const override { return Thunk<0x4A1390, &$::GetStatus>();      }
  void* Destroy(ibool freeMem = 0) override { return Thunk<0x49EDC0, &$::Destroy>(freeMem); }
  void  Close()                    override = 0;

  virtual void OpenStreamByID(size_t identifier) = 0;  ///< [Pure Call] ** maybe?
  virtual void OpenStream(const char* pFilename) = 0;  ///< [Pure Call] ** maybe?
  virtual void GetDataLengthByID(size_t identifier) { return Thunk<0x4A1440, &$::GetDataLengthByID>(identifier); }
  virtual void GetDataLength(const char* pFilename) { return Thunk<0x4A13B0, &$::GetDataLength>(pFilename);      }

#define OP2_BASEVOLSTREAM_VTBL($)  $(OpenStreamByID)  $(OpenStream)  $(GetDataLengthByID)  $(GetDataLength)
  DEFINE_VTBL_TYPE(OP2_BASEVOLSTREAM_VTBL, 0x4D6AB0);

  int  F2(const char* pFilename)  { return Thunk<0x49EEE0, &$::F2>(pFilename);   }
  void F3()                       { return Thunk<0x49F030, &$::F3>();            }
  int  F4(size_t fileSize)        { return Thunk<0x49F0B0, &$::F4>(fileSize);    }
  void F5()                       { return Thunk<0x49F1C0, &$::F5>();            }
  int  F6(size_t identifer)       { return Thunk<0x49F200, &$::F6>(identifer);   }
  int  F7(const char* pFilename)  { return Thunk<0x49F250, &$::F7>(pFilename);   }
  int  F8(const char* pFilename)  { return Thunk<0x49F2D0, &$::F8>(pFilename);   }
  int  F9(size_t identifier)      { return Thunk<0x49F370, &$::F9>(identifier);  }
  int  F10(const char* pFilename) { return Thunk<0x49F400, &$::F10>(pFilename);  }
  int  F11(size_t identifier)     { return Thunk<0x49F490, &$::F11>(identifier); }

public:
  int            numIndexEntries_;
  VolIndexEntry* pIndexTable_;            ///< VolIndexEntry[]*
  void*          pFileNameBuffer_;        ///< "vols" section
  void*          pMappedAddress_;         ///< Memory mapped file
  HANDLE         hFileMapping_;
  HANDLE         hFile_;
  size_t         fileSize_;               ///< Vol file size or -1
  StreamIO*      pOpenedInternalStream_;
  MemRWStream    memRWStream_;            ///< Input Stream
  VBlkRWStream   vBlkRWStream_;           ///< Case 0: Uncompressed
  RLERStream     rleRStream_;             ///< Case 1: RLE
  LZHRStream     lzhRStream_;             ///< Case 3: LZH
  LZRStream      lzRStream_;              ///< Case 2: LZ
  ibool          setFileSize_;            ///< Inits fileSize on open
};
static_assert(sizeof(BaseVolFileStream) == 0x15C, "Incorrect BaseVolFileStream size.");

/// Vol file read input stream.
class VolFileRStream : public BaseVolFileStream {
  using $ = VolFileRStream;
public:
  VolFileRStream() : BaseVolFileStream(UseInternalCtorChain) { InternalCtor<0x4A0960>(); }

  void*  Destroy(ibool freeMem = 0)              override { return Thunk<0x4A0990, &$::Destroy>(freeMem);              }
  size_t Tell()                                  override { return Thunk<0x4A1320, &$::Tell>();                        }
  ibool  Seek(size_t position)                   override { return Thunk<0x4A1330, &$::Seek>(position);                }
  int    F1(int a)                               override { return Thunk<0x4A1300, &$::F1>(a);                         }
  ibool  Write(size_t size, const void* pBuffer) override { return Thunk<0x4A1350, &$::Write>(size, pBuffer);          }
  ibool  Read(size_t size, void* pBuffer)        override { return Thunk<0x4A1360, &$::Read>(size, pBuffer);           }
  void   Close()                                 override { return Thunk<0x4A12E0, &$::Close>();                       }
  void   OpenStreamByID(size_t identifier)       override { return Thunk<0x4A18D0, &$::OpenStreamByID>(identifier);    }
  void   OpenStream(const char* pFilename)       override { return Thunk<0x4A1830, &$::OpenStream>(pFilename);         }
  void   GetDataLengthByID(size_t identifier)    override { return Thunk<0x4A16D0, &$::GetDataLengthByID>(identifier); }
  void   GetDataLength(const char* pFilename)    override { return Thunk<0x4A1590, &$::GetDataLength>(pFilename);      }

  DEFINE_VTBL_GETTER(0x4D6B20);

  int OpenVolFile(const char* pVolFile)   { return Thunk<0x4A0AE0, &$::OpenVolFile>(pVolFile);   }
  int F2()                                { return Thunk<0x4A0E90, &$::F2>();                    }
  int F7OpenStream(VolIndexEntry* pFile)  { return Thunk<0x4A0F20, &$::F7OpenStream>(pFile);     }
  int F6OpenStream(const char* pFilename) { return Thunk<0x4A0FD0, &$::F6OpenStream>(pFilename); }
  int F3(int a)                           { return Thunk<0x4A10C0, &$::F3>(a);                   }
  int F4(int a)                           { return Thunk<0x4A1120, &$::F4>(a);                   }
  int F5(int a)                           { return Thunk<0x4A11E0, &$::F5>(a);                   }

public:
  uint8*       field_15C;
  FileRStream  fileStream_;
};
static_assert(sizeof(VolFileRStream) == 0x980, "Incorrect VolFileRStream size.");

/// Vol file write output stream.
class VolFileWStream : public BaseVolFileStream {
  using $ = VolFileWStream;
public:
  VolFileWStream() : BaseVolFileStream(UseInternalCtorChain) { InternalCtor<0x49F4F0>(); }

  void*  Destroy(ibool freeMem = 0)              override { return Thunk<0x49F550, &$::Destroy>(freeMem);           }
  size_t Tell()                                  override { return Thunk<0x4A08C0, &$::Tell>();                     }
  ibool  Seek(size_t position)                   override { return Thunk<0x4A08D0, &$::Seek>(position);             }
  int    F1(int a)                               override { return Thunk<0x4A08A0, &$::F1>(a);                      }
  ibool  Flush()                                 override { return Thunk<0x4A08F0, &$::Flush>();                    }
  ibool  Write(size_t size, const void* pBuffer) override { return Thunk<0x4A0900, &$::Write>(size, pBuffer);       }
  ibool  Read(size_t size, void* pBuffer)        override { return Thunk<0x4A0930, &$::Read>(size, pBuffer);        }
  void   Close()                                 override { return Thunk<0x4A07C0, &$::Close>();                    }
  void   OpenStreamByID(size_t identifier)       override { return Thunk<0x4A1520, &$::OpenStreamByID>(identifier); }
  void   OpenStream(const char* pFilename)       override { return Thunk<0x4A14A0, &$::OpenStream>(pFilename);      }

  DEFINE_VTBL_GETTER(0x4D6AE8);

  int  F2(int a, int b, int c)         { return Thunk<0x49F600, &$::F2>(a, b, c);      }
  int  F3(int a, int b, int c)         { return Thunk<0x49F930, &$::F3>(a, b, c);      }
  int  F4(int a)                       { return Thunk<0x49FAF0, &$::F4>(a);            }
  int  F5()                            { return Thunk<0x49FDF0, &$::F5>();             }
  void F6(int a)                       { return Thunk<0x4A0030, &$::F6>(a);            }
  void F7(int a)                       { return Thunk<0x4A00D0, &$::F7>(a);            }
  int  F8()                            { return Thunk<0x4A0160, &$::F8>();             }
  void F9()                            { return Thunk<0x4A01B0, &$::F9>();             }
  int  F10(int a, int b, int c)        { return Thunk<0x4A01D0, &$::F10>(a, b, c);     }
  int  F11(int a, int b, int c)        { return Thunk<0x4A03A0, &$::F11>(a, b, c);     }
  int  F12(int a, int b, int c)        { return Thunk<0x4A0620, &$::F12>(a, b, c);     }
  int  F13(VolIndexEntry* pIndexEntry) { return Thunk<0x4A06F0, &$::F13>(pIndexEntry); }

public:
  int          field_15C;      ///< Unused? [Base class member?]
  int          field_160;
  int          field_164;
  int          field_168;
  FileRWStream fileRWStream_;
  RLEWStream   rleWStream_;
  LZHWStream   lzhWStream_;
  LZWStream    lzWStream_;     ///< +0x260
  // ** TODO more member variables...?
};

END_PACKED

} // Tethys
