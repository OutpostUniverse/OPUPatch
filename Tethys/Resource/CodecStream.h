
#pragma once

#include "Tethys/Resource/StreamIO.h"

namespace Tethys {

BEGIN_PACKED

/// Adaptive Huffman Tree used in LZH streams.
class AdaptiveHuffmanTree : public OP2Class<AdaptiveHuffmanTree> {
public:
   AdaptiveHuffmanTree() { InternalCtor<0x4ABD30>(); }
  ~AdaptiveHuffmanTree() { Thunk<0x4ABD70>();        }

  void Init(StreamIO* pInternalStream)            { return Thunk<0x4ABDA0, &$::Init>(pInternalStream);               }
  void WriteCodeToOutput(int code)                { return Thunk<0x4ABE80, &$::WriteCodeToOutput>(code);             }
  void WriteFixedHuffmanCodeToOutput(int code)    { return Thunk<0x4ABED0, &$::WriteFixedHuffmanCodeToOutput>(code); }
  int  GetNextCode()                              { return Thunk<0x4ABF10, &$::GetNextCode>();                       }
  int  GetOffsetToMatchStart()                    { return Thunk<0x4ABF60, &$::GetOffsetToMatchStart>();             }
  void FlushWriteBuffer()                         { return Thunk<0x4ABFC0, &$::FlushWriteBuffer>();                  }
  int  GetNextBit()                               { return Thunk<0x4AC000, &$::GetNextBit>();                        }
  int  GetNext8Bits()                             { return Thunk<0x4AC070, &$::GetNext8Bits>();                      }
  void Write(uint8 numBits, uint16 wordBitBuffer) { return Thunk<0x4AC0E0, &$::Write>(numBits, wordBitBuffer);       }
  void F1()                                       { return Thunk<0x4AC170, &$::F1>();                                }
  void UpdateTree(int code)                       { return Thunk<0x4AC2C0, &$::UpdateTree>(code);                    }

public:
  // ** TODO Verify packing/alignment
  StreamIO* pStream_;
  uint16*   pCount;
  uint16*   field_08;
  uint16*   pTree_;

  uint16 readWordBitBuffer_;   ///< Read
  uint8  numBitsAvailable_;    ///< Read

  uint16 writeWordBitBuffer_;  ///< Write
  uint8  numBitsPending_;      ///< Write
};
static_assert(sizeof(AdaptiveHuffmanTree) == 0x16, "Incorrect AdaptiveHuffmanTree size.");

END_PACKED

/// LZH decode input wrapper stream.
class LZHRStream : public StreamIO {
  using $ = LZHRStream;
public:
  LZHRStream()                         { InternalCtor<0x433E60>();                          }
  LZHRStream(StreamIO* pWrappedStream) { InternalCtor<0x433F30, StreamIO*>(pWrappedStream); }

  void*  Destroy(ibool freeMem = 0)              override { return Thunk<0x433EC0, &$::Destroy>(freeMem);     }
  size_t Tell()                                  override { return Thunk<0x4343E0, &$::Tell>();               }
  ibool  Seek(size_t position)                   override { return Thunk<0x434170, &$::Seek>(position);       }
  int    F1(int a)                               override { return Thunk<0x434130, &$::F1>(a);                }
  ibool  Flush()                                 override { return Thunk<0x4343D0, &$::Flush>();              }
  ibool  Write(size_t size, const void* pBuffer) override { return Thunk<0x4343F0, &$::Write>(size, pBuffer); }
  ibool  Read(size_t size, void* pBuffer)        override { return Thunk<0x434280, &$::Read>(size, pBuffer);  }
  void   Close()                                 override { return Thunk<0x434110, &$::Close>();              }

  DEFINE_VTBL_GETTER(0x4D0410);

  void Init()                                   { return Thunk<0x433FE0, &$::Init>();                         }
  void AttachToStream(StreamIO* pWrappedStream) { return Thunk<0x434080, &$::AttachToStream>(pWrappedStream); }
  void DetachStream()                           { return Thunk<0x4340F0, &$::DetachStream>();                 }

public:
  StreamIO*            pWrappedStream_;
  AdaptiveHuffmanTree* pHuffmanTree_;
  uint8*               ringBuffer_;      ///< Ring buffer size = 4096+59
  int                  matchIndex_;
  int                  numCharsInRun_;
  int                  numCharsCopied_;
  int                  writeIndex_;
  size_t               streamPosition_;
};
static_assert(sizeof(LZHRStream) == 0x2C, "Incorrect LZHRStream size.");

/// LZH encode output wrapper stream.
class LZHWStream : public StreamIO {
  using $ = LZHWStream;
public:
  LZHWStream()                         { InternalCtor<0x4B0090>();                          }
  LZHWStream(StreamIO* pWrappedStream) { InternalCtor<0x4B0090, StreamIO*>(pWrappedStream); }

  void*  Destroy(ibool freeMem = 0)              override { return Thunk<0x4B00B0, &$::Destroy>(freeMem);     }
  size_t Tell()                                  override { return Thunk<0x4B07D0, &$::Tell>();               }
  ibool  Seek(size_t position)                   override { return Thunk<0x4B07E0, &$::Seek>(position);       }
  int    F1(int a)                               override { return Thunk<0x4B0270, &$::F1>(a);                }
  ibool  Flush()                                 override { return Thunk<0x4B06A0, &$::Flush>();              }
  ibool  Write(size_t size, const void* pBuffer) override { return Thunk<0x4B0540, &$::Write>(size, pBuffer); }
  ibool  Read(size_t size, void* pBuffer)        override { return Thunk<0x4B0800, &$::Read>(size, pBuffer);  }
  void   Close()                                 override { return Thunk<0x4B0260, &$::Close>();              }

  DEFINE_VTBL_GETTER(0x4D766C);

  void Init()                                   { return Thunk<0x4B0160, &$::Init>();            }
  void AttachToStream(StreamIO* pWrappedStream) { return Thunk<0x4B01D0, &$::AttachToStream>(pWrappedStream); }
  void DetachStream()                           { return Thunk<0x4B0240, &$::DetachStream>();                 }
  void FindLongestRun(int a)                    { return Thunk<0x4B02B0, &$::FindLongestRun>(a);              }
  void F2(int a)                                { return Thunk<0x4B0450, &$::F2>(a);                          }

public:
  StreamIO*            pWrappedStream_;
  AdaptiveHuffmanTree* pHuffmanTree_;
  int                  field_14;          ///< int ? index
  int                  writeIndex_;       ///< Maybe?
  int                  field_1C;
  int                  field_20;
  int                  field_24;
  int                  field_28;
  int                  runOffset_;
  int                  runLength_;
  size_t               streamPosition_;
  uint8*               ringBuffer_;       ///< uint8[4096 + 59]*
  uint8*               field_3C;          ///< uint8[8192 + 2]*
  uint8*               field_40;          ///< uint8[8192 + 512 + 2]*
  uint8*               field_44;          ///< uint8[8192 + 2]*
};
static_assert(sizeof(LZHWStream) == 0x48, "Incorrect LZHWStream size.");


/// LZ decode input wrapper stream.
class LZRStream : public StreamIO {
  using $ = LZRStream;
public:
  LZRStream()                         { InternalCtor<0x434400>();                          }
  LZRStream(StreamIO* pWrappedStream) { InternalCtor<0x434480, StreamIO*>(pWrappedStream); }

  void*  Destroy(ibool freeMem = 0)              override { return Thunk<0x434430, &$::Destroy>(freeMem);     }
  size_t Tell()                                  override { return Thunk<0x434970, &$::Tell>();               }
  ibool  Seek(size_t position)                   override { return Thunk<0x434630, &$::Seek>(position);       }
  int    F1(int a)                               override { return Thunk<0x4345F0, &$::F1>(a);                }
  ibool  Flush()                                 override { return Thunk<0x434960, &$::Flush>();              }
  ibool  Write(size_t size, const void* pBuffer) override { return Thunk<0x434980, &$::Write>(size, pBuffer); }
  ibool  Read(size_t size, void* pBuffer)        override { return Thunk<0x434800, &$::Read>(size, pBuffer);  }
  void   Close()                                 override { return Thunk<0x4345D0, &$::Close>();              }

  DEFINE_VTBL_GETTER(0x4D0438);

  void Init()                                   { return Thunk<0x434500, &$::Init>();                         }
  void AttachToStream(StreamIO* pWrappedStream) { return Thunk<0x434560, &$::AttachToStream>(pWrappedStream); }
  void DetachStream()                           { return Thunk<0x4345B0, &$::DetachStream>();                 }
  int  GetBits(int numBitsToRead)               { return Thunk<0x434730, &$::GetBits>(numBitsToRead);         }

public:
  // ** TODO verify these are correct
  StreamIO* pWrappedStream_;
  size_t    streamPosition_;
  uint8*    pBuffer_;           ///< Buffer size = 4096
  int       writeIndex_;
  uint8     currentByte_;
  uint8     field_1D;
  uint16    currentBitMask_;    ///< Initially 0x80
  int       numCharsRepeated_;  ///< Repeat progress
  ibool     repeating_;         ///< Buffer filled during repeat
  int       repeatIndex_;
  int       repeatedRunLength_;
};
static_assert(sizeof(LZRStream) == 0x30, "Incorrect LZRStream size.");

/// LZ encode output wrapper stream.
class LZWStream : public StreamIO {
  using $ = LZWStream;
public:
  LZWStream()                         { InternalCtor<0x434990>();                          }
  LZWStream(StreamIO* pWrappedStream) { InternalCtor<0x434A40, StreamIO*>(pWrappedStream); }

  void*  Destroy(ibool freeMem = 0)              override { return Thunk<0x4349E0, &$::Destroy>(freeMem);     }
  size_t Tell()                                  override { return Thunk<0x4352B0, &$::Tell>();               }
  ibool  Seek(size_t position)                   override { return Thunk<0x4352C0, &$::Seek>(position);       }
  int    F1(int a)                               override { return Thunk<0x434BB0, &$::F1>(a);                }
  ibool  Flush()                                 override { return Thunk<0x435240, &$::Flush>();              }
  ibool  Write(size_t size, const void* pBuffer) override { return Thunk<0x434BF0, &$::Write>(size, pBuffer); }
  ibool  Read(size_t size, void* pBuffer)        override { return Thunk<0x4352E0, &$::Read>(size, pBuffer);  }
  void   Close()                                 override { return Thunk<0x434B90, &$::Close>();              }

  DEFINE_VTBL_GETTER(0x4D0460);

  void Init()                                   { return Thunk<0x434B00, &$::Init>();                         }
  void AttachToStream(StreamIO* pWrappedStream) { return Thunk<0x434B40, &$::AttachToStream>(pWrappedStream); }
  void DetachStream()                           { return Thunk<0x434B70, &$::DetachStream>();                 }
  void F2(int a)                                { return Thunk<0x434E70, &$::F2>(a);                          }
  void WriteBits(int bitBuffer, int numBits)    { return Thunk<0x434EB0, &$::WriteBits>(bitBuffer, numBits);  }
  void F3(int a, int b)                         { return Thunk<0x434F80, &$::F3>(a, b);                       }
  void F4(int a, int b)                         { return Thunk<0x434FD0, &$::F4>(a, b);                       }
  void F5(int a)                                { return Thunk<0x435050, &$::F5>(a);                          }
  void F6(int a)                                { return Thunk<0x435090, &$::F6>(a);                          }
  int  F7(int a)                                { return Thunk<0x435160, &$::F7>(a);                          }

public:
  StreamIO* pWrappedStream_;
  size_t    streamPosition_;
  uint8*    pBuffer_;         ///< Buffer size = 4096
  uint8*    field_18;         ///< uint8[4096*12 + 12]* buffer
  int       field_1C;
  int       field_20;
  uint8     field_24;
  uint8     field_25;         ///< Padding?
  short     field_26;
};
static_assert(sizeof(LZWStream) == 0x28, "Incorrect LZWStream size.");


/// RLE decode input wrapper stream.
class RLERStream : public StreamIO {
  using $ = RLERStream;
public:
  RLERStream()                         { InternalCtor<0x4AFAA0>();                          }
  RLERStream(StreamIO* pWrappedStream) { InternalCtor<0x4AFAE0, StreamIO*>(pWrappedStream); }

  void*  Destroy(ibool freeMem = 0)              override { return Thunk<0x4AFAC0, &$::Destroy>(freeMem);     }
  size_t Tell()                                  override { return Thunk<0x4AFBD0, &$::Tell>();               }
  ibool  Seek(size_t position)                   override { return Thunk<0x4AFBF0, &$::Seek>(position);       }
  int    F1(int a)                               override { return Thunk<0x4AFB90, &$::F1>(a);                }
  ibool  Flush()                                 override { return Thunk<0x4AFC90, &$::Flush>();              }
  ibool  Write(size_t size, const void* pBuffer) override { return Thunk<0x4AFCA0, &$::Write>(size, pBuffer); }
  ibool  Read(size_t size, void* pBuffer)        override { return Thunk<0x4AFCB0, &$::Read>(size, pBuffer);  }
  void   Close()                                 override { return Thunk<0x4AFB80, &$::Close>();              }

  DEFINE_VTBL_GETTER(0x4D761C);

  void Init()                                  { return Thunk<0x4AFB10, &$::Init>();                        }
  void AttachToStream(StreamIO* wrappedStream) { return Thunk<0x4AFB30, &$::AttachToStream>(wrappedStream); }
  void DetachStream()                          { return Thunk<0x4AFB60, &$::DetachStream>();                }

public:
  StreamIO* wrappedStream_;
  uint8     charToRepeat_; 
  uint8     numTimesToRepeat_; 
  uint8     chunkSize_;         ///< Raw data copy size
  uint8     field_13[0x7B];
  size_t    streamPosition_;
  uint16    field_96;           ///< Padding?
};
static_assert(sizeof(RLERStream) == 0x98, "Incorrect RLERStream size.");

/// RLE encode output wrapper stream.
class RLEWStream : public StreamIO {
  using $ = RLEWStream;
public:
  RLEWStream()                         { InternalCtor<0x4B0810>();                          }
  RLEWStream(StreamIO* pWrappedStream) { InternalCtor<0x4B0850, StreamIO*>(pWrappedStream); }

  void*  Destroy(ibool freeMem = 0)              override { return Thunk<0x4B0830, &$::Destroy>(freeMem);     }
  size_t Tell()                                  override { return Thunk<0x4B0960, &$::Tell>();               }
  ibool  Seek(size_t position)                   override { return Thunk<0x4B0980, &$::Seek>(position);       }
  int    F1(int a)                               override { return Thunk<0x4B0920, &$::F1>(a);                }
  ibool  Flush()                                 override { return Thunk<0x4B09B0, &$::Flush>();              }
  ibool  Write(size_t size, const void* pBuffer) override { return Thunk<0x4B0B00, &$::Write>(size, pBuffer); }
  ibool  Read(size_t size, void* pBuffer)        override { return Thunk<0x4B0CD0, &$::Read>(size, pBuffer);  }
  void   Close()                                 override { return Thunk<0x4B0910, &$::Close>();              }
  
  DEFINE_VTBL_GETTER(0x4D7694);

  void Init()                                   { return Thunk<0x4B08A0, &$::Init>();                         }
  void AttachToStream(StreamIO* pWrappedStream) { return Thunk<0x4B08C0, &$::AttachToStream>(pWrappedStream); }
  void DetachStream()                           { return Thunk<0x4B08F0, &$::DetachStream>();                 }

public:
  StreamIO* pWrappedStream_;
  uint8     charToRepeat_;      ///< Maybe?
  uint8     numTimesToRepeat_;  ///< Maybe?
  uint8     chunkSize_;         ///< Maybe?
  uint8     field_13[0x7B];
  size_t    streamPosition_;
  uint16    field_96;           ///< Padding?
};
static_assert(sizeof(RLEWStream) == 0x98, "Incorrect RLEWStream size.");

} // Tethys
