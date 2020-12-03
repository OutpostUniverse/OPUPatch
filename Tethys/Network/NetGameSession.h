
#pragma once

#include "Tethys/Common/Memory.h"

namespace Tethys {

/// Net game session base class.  Used in populating the "pick game session" list.
class NetGameSession : public OP2Class<NetGameSession> {
public:
  NetGameSession() : field_04(0), field_08(0), pHostName_(nullptr) { }

  virtual void Destroy(ibool freeMem = 0) { return Thunk<0x412A40, &$::Destroy>(freeMem); }
  virtual int  GetPingTime()              { return Thunk<0x412A20, &$::GetPingTime>();    }
  virtual int  GetHostAddressString(int bufferSize, char* pOut)
    { return Thunk<0x412A30, &$::GetHostAddressString>(bufferSize, pOut); }

#define OP2_NETGAMESESSION_VTBL($)  $(Destroy)  $(GetPingTime)  $(GetHostAddressString)
  DEFINE_VTBL_TYPE(OP2_NETGAMESESSION_VTBL, 0x4CFAB8);

public:
  int   field_04;
  int   field_08;
  char* pHostName_;
  int   field_10;     // ** TODO gameParameters?
  int   field_14;     // ** TODO gameParameters?
  int   field_18;     // ** TODO gameParameters?
  int   field_1C;     // ** TODO gameParameters?
  int   maxPlayers_;
};

/// TCP/IP game session class.
class TCPGameSession : public NetGameSession {
public:
  TCPGameSession() : NetGameSession(), ping_(0), pingDivisor_(0) { }

  void Destroy(ibool freeMem = 0) override { return Thunk<0x491C80, &$::Destroy>(freeMem); }
  int  GetPingTime()              override { return Thunk<0x491C30, &$::GetPingTime>();    }
  int  GetHostAddressString(int bufferSize, char* pOut) override
    { return Thunk<0x491C40, &$::GetHostAddressString>(bufferSize, pOut); }

  DEFINE_VTBL_GETTER(0x4D6518);

public:
  int  field_24;            // ** TODO Not sure if this field belongs here or the parent
  int  hostAddress_;
  GUID sessionIdentifier_;
  int  ping_;
  int  pingDivisor_;
};
static_assert(sizeof(TCPGameSession) == 0x44, "Incorrect TCPGameSession size.");

// ** TODO other *GameSession subclasses

} // Tethys
