
#pragma once

#include "Tethys/Common/Types.h"
#include "Tethys/Game/GameStartInfo.h"

namespace Tethys {

BEGIN_PACKED


/// Packet header
struct PacketHeader {
  int    srcPlayerNetID;
  int    dstPlayerNetID;
  uint8  sizeOfPayload;
  uint8  type;
  uint32 checksum;
};

// --------------------------------------------- Transport Layer messages ----------------------------------------------

enum class TransportLayerCommand : int {
  JoinRequest           = 0,
  JoinGranted           = 1,
  JoinRefused           = 2,
  StartGame             = 3,
  SetPlayersList        = 4,
  SetPlayersListFailed  = 5,
  UpdateStatus          = 6,

  // Custom message types
  HostedGameSearchQuery  = 7,
  HostedGameSearchReply  = 8,
  GameServerPoke         = 9,
  JoinHelpRequest        = 10,
  RequestExternalAddress = 11,
  EchoExternalAddress    = 12,
};

enum class PeerStatus : uint16 {
  EmptySlot = 0,
  Joining,           ///< Joining Player Cleaned from list
  Normal,
  ReplicateSuccess,  ///< Successfully replicated players list
  ReplicateFailure   ///< Failed to replicate players list
};


// -------------------------------------------------- Payload formats --------------------------------------------------

// [Nested structure]
struct NetPeerInfo {
  int        ip;
  uint16     port;
  PeerStatus status;
  int        playerNetID;
};

struct TransportLayerHeader {
  TransportLayerCommand commandType;
};

struct JoinRequest : public TransportLayerHeader {
  GUID sessionIdentifier;
  int  returnPortNum;      ///< [47800-47807]
  char password[12];
};

struct JoinReply : public TransportLayerHeader {
  GUID sessionIdentifier;
  int  newPlayerNetID;
};

struct JoinReturned : public TransportLayerHeader {
  int newPlayerNetID;
  int unused1;
  int unused2;
  int unused3;
};

struct PlayersList : public TransportLayerHeader {
  int         numPlayers;
  NetPeerInfo netPeerInfo[6];
};

struct StatusUpdate : public TransportLayerHeader {
  PeerStatus newStatus;
};


// ----------------------------------------------- Custom Packet formats -----------------------------------------------

// [Nested structure]
struct CreateGameInfo {
  StartupFlags startupFlags;
  char         gameCreatorName[15];
};

struct HostedGameSearchQuery : public TransportLayerHeader {
  GUID   gameIdentifier;
  uint32 timeStamp;
  char   password[12];
};

struct HostedGameSearchReply : public TransportLayerHeader {
  GUID           gameIdentifier;
  uint32         timeStamp;
  GUID           sessionIdentifier;
  CreateGameInfo createGameInfo;
  sockaddr_in    hostAddress;
};


enum class PokeStatusCode : int {
  GameHosted = 0,
  GameStarted,
  GameCancelled,
  GameDropped,
};

struct GameServerPoke : public TransportLayerHeader {
  PokeStatusCode statusCode;
  int            randValue;
};

struct JoinHelpRequest : public TransportLayerHeader {
  GUID        sessionIdentifier;
  int         returnPortNum;      ///< [47800-47807]
  char        password[12];
  sockaddr_in clientAddr;
};

struct RequestExternalAddress : public TransportLayerHeader {
  uint16 internalPort;
};

struct EchoExternalAddress : public TransportLayerHeader {
  uint16      replyPort;
  sockaddr_in addr;
};

union TransportLayerMessage {
  // Header only
  TransportLayerHeader tlHeader;

  // Standard messages
  JoinRequest  joinRequest;
  JoinReply    joinReply;
  JoinReturned joinReturned;
  PlayersList  playersList;
  StatusUpdate statusUpdate;

  // Custom messages
  HostedGameSearchQuery  searchQuery;
  HostedGameSearchReply  searchReply;
  GameServerPoke         gameServerPoke;
  JoinHelpRequest        joinHelpRequest;
  RequestExternalAddress requestExternalAddress;
  EchoExternalAddress    echoExternalAddress;
};


// --------------------------------------------------- Game Messages ---------------------------------------------------

struct GameMessageHeader {};


union GameMessage {
  // Header only
  GameMessageHeader gmHeader;

  // Standard messages
};


// ------------------------------------------------------ Packet -------------------------------------------------------
class Packet : public OP2Class<Packet> {
public:
  int Checksum() const { return Thunk<0x490F10, &Packet::Checksum>(); }

public:
  PacketHeader            header;
  union {
    uint8                 payloadData[0x212];
    TransportLayerMessage tlMessage;
    GameMessage           gameMessage;
  };
};

END_PACKED

} // Tethys
