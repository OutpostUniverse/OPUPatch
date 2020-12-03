
#pragma once

#include "Tethys/Common/Memory.h"

namespace Tethys {

struct Packet;

struct TrafficCounters {
  int timeOfLastReset;
  int numPacketsSent;
  int numBytesSent;
  int numPacketsReceived;
  int numBytesReceived;
};

class NetTransportLayer : public OP2Class<NetTransportLayer> {
public:
  // ** TODO
  virtual void* Destroy(ibool freeMem = 0)                                       = 0; // virtual ~NetTransportLayer()
  virtual int   GetHostPlayerNetID()                                             = 0;
  virtual void  ShutDownInvite()                                                 = 0;
  virtual int   ReplicatePlayersList()                                           = 0;
  virtual int   GetOpponentNetIDList(int* pNetIDList, int maxNumID)              = 0;
  virtual void  RemovePlayer(int playerNetID)                                    = 0;
  virtual int   Send(const Packet& packet)                                       = 0;
  virtual int   Receive(Packet* pPacket)                                         = 0;
  virtual int   IsHost()                                                         = 0;
  virtual int   IsValidPlayer()                                                  = 0;
  virtual int   Func10()                                                         = 0;  // ** Return 1
  virtual int   GetAddressString(int playerNetID, char* pBuffer, int bufferSize) = 0;
  virtual int   ResetTrafficCounters()                                           = 0;
  virtual int   GetTrafficCounts(TrafficCounters* pTrafficCounters)              = 0;

public:
  int playerNetID_;
};

} // Tethys
