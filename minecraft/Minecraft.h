#pragma once

#include <chrono>
#include <memory>
#include <unordered_map>
#include <vector>

struct GameCallbacks;
struct SkinInfoFactory;
struct IMinecraftEventing;
struct ResourcePackManager;
struct StructureManager;
struct Whitelist;
struct PermissionsMap;
struct NetworkStatistics;
struct PrivateKeyManager;
struct FilePathManager;
struct MinecraftCommands;
struct GameSession;
struct Timer;
struct NetworkHandler;
struct PacketSender;
struct DedicatedServer;
namespace Documentation {
struct Writer {
  std::string content;
  Writer();
};
} // namespace Documentation
struct IMinecraftApp;
struct ContentTierManager;
struct NetworkIdentifier;
struct Level;
struct NetEventCallback;
struct RakNetServerLocator;
struct Player;
namespace mce {
struct UUID;
}
struct Scheduler;
struct TextFilteringProcessor;

struct Minecraft {
  GameCallbacks *callbacks;                                 // 0
  SkinInfoFactory *skinInfoFactory;                         // 8
  IMinecraftEventing *minecraftEventing;                    // 16
  std::unique_ptr<ResourcePackManager> resourcePackManager; // 24
  std::unique_ptr<StructureManager> structureManager;       // 32
  Whitelist *whitelist;                                     // 40
  PermissionsMap *permMap;                                  // 48
  std::unique_ptr<NetworkStatistics> networkStats;          // 56
  std::unique_ptr<PrivateKeyManager> privateKeyManager;     // 64
  std::string s0;                                           // 72
  FilePathManager *filePathManager;                         // 104
  uint64_t unk112, unk120, unk128;                          // 112, 120, 128
  std::chrono::seconds clock;                               // 136
  std::unique_ptr<MinecraftCommands> commands;              // 144
  std::unique_ptr<GameSession> session;                     // 152
  Timer *timer0, *timer1;                                   // 160, 168
  NetworkHandler *network;                                  // 176
  PacketSender *sender;                                     // 184
  DedicatedServer *server;                                  // 192
  unsigned char subId;                                      // 200
  Documentation::Writer writer;                             // 208

  Minecraft(IMinecraftApp &, GameCallbacks &, SkinInfoFactory &, Whitelist &, PermissionsMap const &, FilePathManager *, std::chrono::duration<long, std::ratio<1l, 1l>>, IMinecraftEventing &,
            NetworkHandler &, PacketSender &, unsigned char, Timer &, Timer &, ContentTierManager const &, bool);

  void activeWhitelist();
  void clearThreadCallbacks();
  void clientReset();
  void disconnectClient(NetworkIdentifier const &, std::string const &);
  unsigned char getClientSubId() const;
  MinecraftCommands *getCommands();
  IMinecraftEventing *getEventing() const;
  float getLastTimestep();
  Level *getLevel();
  NetEventCallback *getNetEventCallback();
  NetworkHandler *getNetworkHandler();
  NetworkHandler *getNetworkHandler() const;
  NetworkStatistics *getNetworkStatistics();
  ResourcePackManager *getResourceLoader();
  RakNetServerLocator *getServerLocator();
  RakNetServerLocator *getServerLocator() const;
  NetworkHandler *getServerNetworkHandler();
  bool getSimPaused() const;
  StructureManager *getStructureManager();
  Timer *getTimer();
  bool hasCommands();
  void hostMultiplayer(std::string const &, std::unique_ptr<Level>, Player *, mce::UUID const &, std::unique_ptr<NetEventCallback>, int, bool, bool, std::vector<std::string> const &, std::string, int,
                       int, int, int, std::unordered_map<PackIdVersion, std::string> const &, Scheduler &, TextFilteringProcessor *);
  void init(bool);
  void initAsDedicatedServer();
  void initCommands();
  bool isInitialized() const;
  bool isLeaveGameDone() const;
  bool isModded();
  bool isOnlineClient();
  void joinWorldInProgress(std::unique_ptr<NetEventCallback>);
  void onClientCreatedLevel(std::unique_ptr<Level>);
  void resetGameSession();
  void setGameModeReal(GameType);
  void setSimTimePause(bool);
  void setSimTimeScale(float);
  void setupServerCommands(std::string const&, std::string const&);
  void startClientGame(std::unique_ptr<NetEventCallback>);
  void startLeaveGame(bool);
  void tickRealtime(int,int);
  void tickSimtime(int,int);
  bool update(bool);
  void updateScreens();
  bool usesNonLocalConnection(NetworkIdentifier const&);
  ~Minecraft();
};

static_assert(offsetof(Minecraft, filePathManager) == 104);
static_assert(offsetof(Minecraft, clock) == 136);
static_assert(offsetof(Minecraft, subId) == 200);
static_assert(offsetof(Minecraft, writer) == 208);