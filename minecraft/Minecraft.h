#pragma once

#include "Documentation.h"
#include "types.h"
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
struct IMinecraftApp;
struct ContentTierManager;
struct NetworkIdentifier;
struct Level;
struct NetEventCallback;
struct RakNetServerLocator;
struct Player;
struct Scheduler;
struct TextFilteringProcessor;

namespace entt {
template <typename T> struct Registry;
}

struct EntityContext;

struct EntityRegistry {
  entt::Registry<EntityId> *reg;
  template <typename T0, typename T1, typename T2> struct View {
    EntityRegistry *reg;
    View(EntityRegistry &);
    template <typename F> void each(F f);
  };
  bool isValidEntity(EntityContext const &);
  template <typename T> void removeComponentFromAllEntities();
  template <typename T0, typename T1, typename T2, typename F> void viewEach(F);
};

struct IEntityRegistryOwner {
  virtual ~IEntityRegistryOwner();
  virtual EntityRegistry &getEntityRegistry() = 0;
};

struct Minecraft : IEntityRegistryOwner {
  GameCallbacks *callbacks;                                 // 8
  SkinInfoFactory *skinInfoFactory;                         // 16
  IMinecraftEventing *minecraftEventing;                    // 24
  std::unique_ptr<ResourcePackManager> resourcePackManager; // 32
  std::unique_ptr<StructureManager> structureManager;       // 40
  Whitelist *whitelist;                                     // 48
  PermissionsMap *permMap;                                  // 56
  std::unique_ptr<NetworkStatistics> networkStats;          // 64
  std::unique_ptr<PrivateKeyManager> privateKeyManager;     // 72
  std::string s0;                                           // 80
  FilePathManager *filePathManager;                         // 112
  bool unk120;                                              // 120
  uint64_t unk128, unk136;                                  // 128, 136
  std::chrono::seconds clock;                               // 144
  std::unique_ptr<MinecraftCommands> commands;              // 152
  std::unique_ptr<GameSession> session;                     // 160
  Timer *timer0, *timer1;                                   // 168, 176
  NetworkHandler *network;                                  // 184
  PacketSender *sender;                                     // 192
  DedicatedServer *server;                                  // 200
  unsigned char subId;                                      // 208
  Documentation::Writer writer;                             // 216
  EntityRegistry entityRegistry;                            // 264

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
  void setupServerCommands(std::string const &, std::string const &);
  void startClientGame(std::unique_ptr<NetEventCallback>);
  void startLeaveGame(bool);
  void tickRealtime(int, int);
  void tickSimtime(int, int);
  bool update();
  void updateScreens();
  bool usesNonLocalConnection(NetworkIdentifier const &);
  virtual ~Minecraft();
  virtual EntityRegistry &getEntityRegistry();
};

static_assert(offsetof(Minecraft, filePathManager) == 112);
static_assert(offsetof(Minecraft, clock) == 144);
static_assert(offsetof(Minecraft, subId) == 208);
static_assert(offsetof(Minecraft, writer) == 216);
static_assert(offsetof(Minecraft, entityRegistry) == 264);