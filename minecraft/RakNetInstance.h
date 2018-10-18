#pragma once

#include "NetworkIdentifier.h"
#include "RakNet/statistics.h"
#include "RakNet/types.h"
#include <functional>
#include <string>
#include <unordered_set>
#include <vector>

namespace Social {
struct GameConnectionInfo;
}

struct RakPeerHelper {
  struct IPSupportInterface {
    IPSupportInterface();
    virtual ~IPSupportInterface();
  };

  int result;               // 0
  int IPv4ConnectionIndex;  // 4
  int IPv6ConnectionIndex;  // 8
  short unk12, unk14;       // 12, 14
  IPSupportInterface *ifce; // 16

  RakPeerHelper(IPSupportInterface const &);

  bool isIPv4Supported() const;
  unsigned short getIPv4BoundPort() const;
  int getIPv4ConnectionIndex() const;
  bool isIPv6Supported() const;
  unsigned short getIPv6BoundPort() const;
  int getIPv6ConnectionIndex() const;
  int getConnectionIndex(Social::GameConnectionInfo const &) const;
  int getResult() const;
  int peerSetup(RakNet::RakPeerInterface *, int, unsigned short, unsigned short);

  void LogIPSupport();
};

namespace Json {
struct Value;
}

namespace Social {
enum struct ConnectionType : short { Undefined = -1, Local = 0, IPv4 = 1, IPv6 = 2, NAT = 3, UPNP = 4, UnknownIP = 5 };
struct ThirdPartyInfo {
  std::unordered_set<std::string> data; // 0
  std::string CreatorId;                // 56
  std::string CreatorName;              // 88
  bool xblRequired;                     // 120

  ThirdPartyInfo();
  ThirdPartyInfo(ThirdPartyInfo const &);

  ThirdPartyInfo &operator=(ThirdPartyInfo const &);

  std::string &getCreatorId() const;
  std::string &getCreatorName() const;

  bool isValid() const;
  bool isEligible(std::string const &) const;
  bool isXblRequired() const;

  ~ThirdPartyInfo();
};
static_assert(offsetof(ThirdPartyInfo, CreatorId) == 56);
static_assert(offsetof(ThirdPartyInfo, xblRequired) == 120);
struct GameConnectionInfo {
  alignas(8) ConnectionType type;     // 0
  std::string HostIpAddress;          // 8
  std::string UnresolvedUrl;          // 40
  alignas(8) unsigned short HostPort; // 72
  std::string RakNetGUID;             // 80
  ThirdPartyInfo thirdPartyInfo;      // 104

  GameConnectionInfo();
  GameConnectionInfo(GameConnectionInfo const &);
  GameConnectionInfo(ConnectionType, std::string const &, std::string const &);
  GameConnectionInfo(RakNet::SystemAddress, ThirdPartyInfo const &);

  GameConnectionInfo &operator=(GameConnectionInfo const &);

  static GameConnectionInfo fromJson(Json::Value const &);
  static GameConnectionInfo fromStringized(Json::Value const &);

  ConnectionType getType();
  char const *typeAsString() const;
  std::string &getHostIpAddress() const;
  void setUnresolvedUrl(std::string const &);
  unsigned int getPort() const;
  ThirdPartyInfo getThirdPartyInfo();

  Json::Value getStringized();
  void toJson(Json::Value const &);

  ~GameConnectionInfo();
};
} // namespace Social

static_assert(offsetof(Social::GameConnectionInfo, HostIpAddress) == 8);
static_assert(offsetof(Social::GameConnectionInfo, RakNetGUID) == 80);

struct Connector {
  struct NatPunchInfo {
    char filler_before[8];
    RakNet::SystemAddress address;
    int filler_after[4];
  }; // 160
  struct ConnectionStateListener;
  Connector();
  virtual ~Connector();
  virtual void onAppResumed()                                           = 0;
  virtual void onAppSuspend()                                           = 0;
  virtual std::vector<std::string> getLocalIps() const                  = 0;
  virtual std::string getLocalIp()                                      = 0;
  virtual unsigned short getPort() const                                = 0;
  virtual std::vector<RakNet::SystemAddress> getRefinedLocalIps() const = 0;
  virtual Social::GameConnectionInfo *getConnectedGameInfo() const      = 0;
  virtual bool setupNatPunch(bool)                                      = 0;
  virtual NatPunchInfo getNatPunchInfo() const                          = 0;
  virtual void startNatPunchingClient(Social::GameConnectionInfo)       = 0;
  virtual void addConnectionStateListener(ConnectionStateListener *)    = 0;
  virtual void removeConnectionStateListener(ConnectionStateListener *) = 0;
  virtual bool isIPv4Supported() const                                  = 0;
  virtual bool isIPv6Supported() const                                  = 0;
  virtual unsigned short getIPv4Port() const                            = 0;
  virtual unsigned short getIPv6Port() const                            = 0;
  virtual RakNet::RakNetGUID getGUID() const                            = 0;
};

static_assert(sizeof(Connector::NatPunchInfo) == 160);

struct NetworkStatus {
  char filler[100];
};

struct NetworkPeer {
  enum struct Reliability : int {};

  virtual ~NetworkPeer();
  virtual bool sendPacket(std::string, NetworkPeer::Reliability, int) = 0;
  virtual bool receivePacket(std::string &)                           = 0;
  virtual NetworkStatus getNetworkStatus()                            = 0;
};

struct NetworkIdentifier;

struct RakNetInstance : Connector {
  struct ConnectionCallbacks {
    ConnectionCallbacks();
    virtual ~ConnectionCallbacks();
  };
  struct NatConnectionInfo {
    RakNet::SystemAddress address;
    unsigned v0, v1;
    bool b0;
    void operator=(NatConnectionInfo const &);
  };
  struct PingCallbackData {
    std::string tag;
    std::function<void(unsigned)> callback;
    PingCallbackData(PingCallbackData &&);
    PingCallbackData &operator=(PingCallbackData &&);
    ~PingCallbackData();
  };
  struct RakNetNetworkPeer : NetworkPeer {
    RakNet::RakPeerInterface *peer; // 8
    NetworkIdentifier id;           // 16
    std::string s0;                 // 168
    std::vector<std::string> pkts;  // 200
    unsigned u0, u1, u2;            // 224, 228, 232
    virtual ~RakNetNetworkPeer();
    virtual bool sendPacket(std::string, NetworkPeer::Reliability, int);
    virtual bool receivePacket(std::string &);
    virtual NetworkStatus getNetworkStatus();

    static unsigned getReliability(NetworkPeer::Reliability);

    RakNetNetworkPeer(RakNet::RakPeerInterface &, NetworkIdentifier const &);
    NetworkIdentifier *getId() const;
    void newData(std::string);
    bool update();
  };
  static_assert(sizeof(RakNetNetworkPeer) == 240);

  enum struct NATState : int {};
  ConnectionCallbacks *callbacks;                                        // 8
  NetworkIdentifier nid;                                                 // 16
  Social::GameConnectionInfo connInfo1;                                  // 168
  bool connected;                                                        // 408
  RakNet::RakPeerInterface *peerIfce;                                    // 416
  NetworkIdentifier localId;                                             // 424
  Connector::NatPunchInfo natInfo;                                       // 576
  NATState natState;                                                     // 736
  std::vector<NatConnectionInfo> natConnectionInfos;                     // 744
  RakPeerHelper helper;                                                  // 768
  RakPeerHelper::IPSupportInterface *ipSupportIfce;                      // 792
  bool natReady;                                                         // 800
  bool serverMode;                                                       // 801
  bool b802, b803;                                                       // 802, 803
  Social::GameConnectionInfo connInfo2;                                  // 808
  std::vector<std::unique_ptr<RakNetInstance::RakNetNetworkPeer>> peers; // 1048
  std::vector<Connector::ConnectionStateListener *> connStateListner;    // 1072
  char c1096;                                                            // 1096
  unsigned short u1098, u1100;                                           // 1098, 1100
  unsigned u1104;                                                        // 1104
  std::string LocalIP;                                                   // 1112
  std::vector<PingCallbackData> pingCbs;                                 // 1144

  RakNetInstance(ConnectionCallbacks &, RakPeerHelper::IPSupportInterface &);
  void _changeNatState(NATState, int, std::string const &);
  RakNetNetworkPeer *_createPeer(NetworkIdentifier const &);
  RakNetNetworkPeer *_getPeer(NetworkIdentifier const &);
  void _openNatConnection(RakNet::SystemAddress const &);
  void _pintNatService(bool);
  void _removePeer(NetworkIdentifier const &);
  bool _startupIfNeeded(unsigned short, unsigned short, int);
  char const *_stateToString(NATState) const;
  void _storeLocalIP();

  virtual void onAppResumed();
  virtual void onAppSuspend();
  virtual std::vector<std::string> getLocalIps() const;
  virtual std::string getLocalIp();
  virtual unsigned short getPort() const;
  virtual std::vector<RakNet::SystemAddress> getRefinedLocalIps() const;
  virtual Social::GameConnectionInfo *getConnectedGameInfo() const;
  virtual bool setupNatPunch(bool);
  virtual NatPunchInfo getNatPunchInfo() const;
  virtual void startNatPunchingClient(Social::GameConnectionInfo);
  virtual void addConnectionStateListener(ConnectionStateListener *);
  virtual void removeConnectionStateListener(ConnectionStateListener *);
  virtual bool isIPv4Supported() const;
  virtual bool isIPv6Supported() const;
  virtual unsigned short getIPv4Port() const;
  virtual unsigned short getIPv6Port() const;
  virtual RakNet::RakNetGUID getGUID() const;

  void connect(Social::GameConnectionInfo, Social::GameConnectionInfo);
  void disconnect();
  RakNet::RakPeerInterface *getPeer();
  RakNet::RakPeerInterface const *getPeer() const;
  void getPingTimeForConnection(std::string const &, int, std::function<void(unsigned)>);
  
  std::vector<RakNet::SystemAddress> getRefinedLocalIps();
  bool getStatistics(RakNet::RakNetStatistics *);
  bool host(int, int, int);
  bool isMyLocalId(NetworkIdentifier const &);
  bool isServer() const;
  bool natPongReceived(RakNet::SystemAddress const &);
  bool runEvents();
  void tick();
};

static_assert(offsetof(RakNetInstance::RakNetNetworkPeer, pkts) == 200);
static_assert(offsetof(RakNetInstance, connInfo1) == 168);
static_assert(offsetof(RakNetInstance, localId) == 424);
static_assert(offsetof(RakNetInstance, natInfo) == 576);
static_assert(offsetof(RakNetInstance, natConnectionInfos) == 744);
static_assert(offsetof(RakNetInstance, ipSupportIfce) == 792);
static_assert(offsetof(RakNetInstance, connInfo2) == 808);
static_assert(offsetof(RakNetInstance, peers) == 1048);
static_assert(offsetof(RakNetInstance, pingCbs) == 1144);
static_assert(sizeof(RakNetInstance) == 1168);