
#pragma once

#include "Tethys/Common/Memory.h"
#include "Tethys/Network/Packet.h"

namespace Tethys {

class NetTransportLayer;

struct PlayerGurInfo {
  int playerNetID;
  int b1[7];
  int timeOfLastReceivedPacket;
  int b2;
  int b3;
  int numResentPackets;
};

BEGIN_PACKED
class NetBuffer {
public:
  NetBuffer* pPrev_;
  NetBuffer* pNext_;
  int        b1[5];
  Packet     packet_;
};
END_PACKED

/// Guaranteed network packet send/receive manager.
class GurManager : public OP2Class<GurManager> {
public:
  GurManager() { InternalCtor<0x42D710>(); }

  int Init(NetTransportLayer* pNetTransportLayer) { return Thunk<0x42D760, &$::Init>(pNetTransportLayer); }

public:
  int        field_04;
  NetBuffer* pCurBuffer_;
  int        field_0C;
  NetBuffer* pHead_;
  NetBuffer* pTail_;
  NetBuffer  netBuffer_[37];

  NetTransportLayer* pNetTransportLayer_;
  int                field_52C8;
  int                field_52CC;
  int                numPlayers_;
  PlayerGurInfo      playerInfo_[5];

  int numRetries_;
  int timeOut_;
  int timeOutD4_;

  int field_53D0;
  int field_53D4;
  int field_53D8;
  int field_53DC;
};

} // Tethys
