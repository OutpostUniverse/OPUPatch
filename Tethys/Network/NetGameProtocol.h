
#pragma once

#include "Tethys/Common/Memory.h"

namespace Tethys {

class NetGameSession;
class NetGameProtocol;
class NetTransportLayer;

struct ProtocolControlMapping {
  int              controlID;
  NetGameProtocol* pNetGameProtocol;
};

class NetGameProtocol : public OP2Class<NetGameProtocol> {
public:
  virtual ibool       IsEnabled()       = 0;
  virtual ibool       DoStart()         = 0;
  virtual ibool       Func2()           = 0;  ///< Return 1
  virtual const char* GetProtocolName() = 0;

  /// [0]  TCP
  /// [1]  SIGS
  /// [2]  Modem
  /// [3]  IPX
  static auto& GetProtocolNameLut() { return OP2Mem<NetGameProtocol*(&)[4]>(0x4D6450); }

  /// [0]  controlID = 3, protocol = TCP
  /// [1]  controlID = 5, protocol = IPX
  /// [2]  controlID = 4, protocol = Modem
  /// [3]  controlID = 7, protocol = Serial
  /// [4]  controlID = 6, protocol = SIGS
  static auto& GetProtocolControlLut() { return OP2Mem<ProtocolControlMapping(&)[5]>(0x4DF380); }
};

class NetGameSearchProtocol : public OP2Class<NetGameSearchProtocol> {
public:
  virtual void* Destroy(ibool freeMem = 0) { return Thunk<0x412520, &$::Destroy>(freeMem); }

  virtual int Func1() { return Thunk<0x4122D0, &$::Func1>(); }
  virtual int Func2() { return Thunk<0x4122E0, &$::Func2>(); }

  virtual int GetLocalAddressString(char* pOut, int size)
    { return Thunk<0x4122F0, &$::GetLocalAddressString>(pOut, size); }

  virtual void SearchForGames(NetGameSession** ppOut, const char* pPassword, const GUID& gameID) = 0;

  virtual NetTransportLayer* SetupHostedGame(
    const GUID& gameID, const char* pPassword, int maxPlayers, void* pGameParams) = 0;
  virtual NetTransportLayer* RequestJoinGame(const NetGameSession& session)       = 0;

#define OP2_NETGAMESEARCHPROTOCOL_VTBL($)  \
  $(Destroy)  $(Func1)  $(Func2)  $(GetLocalAddressString)  $(SearchForGames)  $(SetupHostedGame)  $(RequestJoinGame)
  DEFINE_VTBL_TYPE(OP2_NETGAMESEARCHPROTOCOL_VTBL, 0x4CFA60);
};

class TCPGameSearchProtocol : public NetGameSearchProtocol {
public:
  DEFINE_VTBL_GETTER(0x4D64F8);

public:
  int              field_04;           // ** TODO Does this belong in the parent?
  int              specifiedIP_;
  HANDLE           hBroadcastThread_;
  int              field_10;
  int              field_14;
  int              field_1C;
  NetGameSession*  pSession_;
};

} // Tethys
