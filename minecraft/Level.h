#pragma once

#include "LevelData.h"
#include "LootTable.h"
#include "SmallSet.h"
#include "TradeTable.h"
#include "types.h"
#include <chrono>
#include <queue>
#include <unordered_map>
#include <unordered_set>
struct BlockSource;
struct Block;
struct ActorBlockSyncMessage;
struct BlockActor;

struct BlockSourceListener {
  virtual ~BlockSourceListener();
  virtual void onSourceCreated(BlockSource &);
  virtual void onSourceDestroyed(BlockSource &);
  virtual void onAreaChanged(BlockSource &, BlockPos const &, BlockPos const &);
  virtual void onBlockChanged(BlockSource &, BlockPos const &, uint, Block const &, Block const &, int, ActorBlockSyncMessage const *);
  virtual void onBrightnessChanged(BlockSource &, BlockPos const &);
  virtual void onBlockEntityChanged(BlockSource &, BlockActor &);
  virtual void onBlockEntityAboutToBeRemoved(BlockSource &, std::shared_ptr<BlockActor>);
  virtual void onEntityChanged(BlockSource &, Actor &);
  virtual void onBlockEvent(BlockSource &, int, int, int, int, int);
};

struct Player;
struct PlayerSuspension;
struct SoundPlayer;
struct LevelListener;
struct LevelStorage;
struct SavedDataStorage;
struct PhotoStorage;

struct LevelDataWrapper {
  LevelData *remote; // 0
  LevelData local;   // 8
  LevelDataWrapper();
  LevelData const &operator*() const;
  LevelData &operator*();
  LevelData const *operator->() const;
  LevelData *operator->();
  void setLevelData(LevelData &);
  ~LevelDataWrapper();
};

struct BlockPalette;
struct Recipes;
struct FurnaceRecipes;
struct PortalForcer;
struct ActorDefinitionGroup;
struct Spawner;
struct ProjectileFactory;
struct BossEventListener;
struct BehaviorFactory;
struct BehaviorTreeGroup;
struct ChangeDimensionRequest;
struct BinaryStream;

struct PlayerListEntry {
  ActorUniqueID auid;                  // 0
  mce::UUID uuid;                      // 8
  std::string username;                // 24
  std::string skinId;                  // 56
  std::vector<unsigned char> skinData; // 88
  std::vector<unsigned char> capeData; // 112
  std::string geometryName;            // 136
  std::string geometryData;            // 168
  std::string xboxUserId;              // 200
  std::string platformChatId;          // 232

  PlayerListEntry(Player const &);
  PlayerListEntry(PlayerListEntry const &);
  PlayerListEntry(mce::UUID);
  PlayerListEntry();

  void read(BinaryStream &);
  void readRemove(BinaryStream &);
  void write(BinaryStream &);
  void writeRemove(BinaryStream &);

  ~PlayerListEntry();
};

struct Villages;
struct PendingRemovalInfo;
struct TaskGroup;
struct IEntityRegistryOwner;
struct TickingAreasManager;
struct _TickPtr;
struct MapItemSavedData;
struct IMinecraftEventing;
struct PlayerEventCoordinator;
struct ServerPlayerEventCoordinator;
struct ClientPlayerEventCoordinator;
struct ActorEventCoordinator;
struct ClientLevelEventCoordinator;
struct ServerLevelEventCoordinator;
struct BlockEventCoordinator;
struct ItemEventCoordinator;
struct ResourcePackManager;
struct Scoreboard;
struct Level;

struct ActorFactory {
  Level *level; // 0
  ActorFactory(Level &);

  static void setDefinitionGroup(ActorDefinitionGroup *);
  static void clearDefinitionGroup();
  static ActorType lookupEntityType(ActorDefinitionIdentifier const &);
  static std::vector<std::pair<std::string, ActorType>> buildSummonEntityTypeEnum(bool);

  std::unique_ptr<Actor> createBornEntity(ActorDefinitionIdentifier const &, Actor *);
  std::unique_ptr<Actor> createBornEntity(ActorDefinitionIdentifier const &, BlockPos const &);
  std::unique_ptr<Actor> createSpawnedEntity(ActorDefinitionIdentifier const &, Actor *, Vec3 const &, Vec2 const &);
  std::unique_ptr<Actor> createSummonedEntity(ActorDefinitionIdentifier const &, Actor *, Vec3 const &);
  std::unique_ptr<Actor> createTransformedEntity(ActorDefinitionIdentifier const &, Actor *);
  std::unique_ptr<Actor> createEntity(std::string const &, ActorDefinitionIdentifier const &, Actor *, Vec3 const &, Vec2 const &);
  void fixLegacyEntity(BlockSource &, CompoundTag const *);
  Level &getLevel();
  std::unique_ptr<Actor> loadEntity(CompoundTag const *);
};

struct ITickingSystem;
struct PlayerInteractionSystem;
struct EntityRegistry;

struct EntitySystems {
  std::vector<std::unique_ptr<ITickingSystem>> tickings;       // 0
  std::unique_ptr<PlayerInteractionSystem> player_interaction; // 24

  EntitySystems();

  PlayerInteractionSystem *getPlayerInteractionSystem();
  void tick(EntityRegistry &);

  ~EntitySystems();
};

struct LevelChunk;
struct TradeTables;
struct StructureManager;
struct BossEventUpdateType;
struct LevelEvent;
struct ActorEvent;
struct LevelSoundEvent;
struct NetEventCallback;
struct PacketSender;
struct BlockLegacy;

namespace cg {
struct ImageBuffer;
}
struct ScreenshotOptions;

struct Level : BlockSourceListener {
  struct LevelChunkQueuedSavingElement;
  std::vector<ChunkPos> ticking_offsets;                                                 // 8
  std::vector<ChunkPos> client_ticking_offsets;                                          // 32
  std::vector<std::unique_ptr<Player>> players;                                          // 56
  std::vector<Player *> plist80;                                                         // 80
  std::vector<Player *> plist104;                                                        // 104
  std::vector<PlayerSuspension> suspension_players;                                      // 128
  SoundPlayer *sound;                                                                    // 152
  bool b160;                                                                             // 160
  bool b161;                                                                             // 161
  ActorUniqueID last_unique_id;                                                          // 168
  ActorRuntimeID last_runtime_od;                                                        // 176
  std::unordered_map<ActorRuntimeID, ActorUniqueID> actor_map;                           // 184
  std::vector<LevelListener *> listeners;                                                // 240
  std::unique_ptr<LevelStorage> storage;                                                 // 264
  std::unique_ptr<SavedDataStorage> saved_data_storage;                                  // 272
  std::unique_ptr<PhotoStorage> photo_storage;                                           // 280
  LevelDataWrapper level_data;                                                           // 288
  std::unordered_set<BlockSource *> block_source_set;                                    // 864
  std::unique_ptr<ActorDefinitionGroup> actor_defs;                                      // 920
  std::unique_ptr<Spawner> spawner;                                                      // 928
  std::unique_ptr<ProjectileFactory> projectile;                                         // 936
  std::unique_ptr<BehaviorFactory> behavior_factory;                                     // 944
  std::unique_ptr<BehaviorTreeGroup> behavior_tree_group;                                // 952
  std::unique_ptr<BlockPalette> block_palette;                                           // 960
  std::unique_ptr<Recipes> recipes;                                                      // 968
  std::unique_ptr<FurnaceRecipes> furnace_recipes;                                       // 976
  std::unordered_map<DimensionId, std::unique_ptr<Dimension>> dim_map;                   // 984
  std::unique_ptr<PortalForcer> portal_forcer;                                           // 1040
  std::vector<BossEventListener *> boss_event_listener;                                  // 1048
  std::unordered_map<Player *, std::unique_ptr<ChangeDimensionRequest>> change_dim_reqs; // 1072
  std::unordered_map<mce::UUID, PlayerListEntry> player_list_entrys;                     // 1128
  PacketSender *sender;                                                                  // 1184
  HitResult main_hit_result;                                                             // 1192
  HitResult liquid_hit_result;                                                           // 1288
  Random rand1384;                                                                       // 1384
  Random rand3900;                                                                       // 3900
  NetEventCallback *net_event;                                                           // 6416
  SmallSet<std::unique_ptr<Actor>> actors6424;                                           // 6424
  bool b6448;                                                                            // 6448
  SmallSet<Actor *> actor_set_global;                                                    // 6456
  SmallSet<Actor *> actor_set_autonomous;                                                // 6480
  std::vector<std::unique_ptr<Actor>> autonomous_loaded_entities;                        // 6504
  std::unique_ptr<Villages> villages;                                                    // 6528
  bool client_side;                                                                      // 6536
  bool exporting;                                                                        // 6537
  std::vector<PendingRemovalInfo> pending_removal_info;                                  // 6544
  void *unk6568;                                                                         // 6568
  bool b6576;                                                                            // 6576
  bool sim_paused;                                                                       // 6577
  std::unique_ptr<TaskGroup> level_tasks;                                                // 6584
  std::unique_ptr<TaskGroup> level_io;                                                   // 6592
  Scheduler *scheduler;                                                                  // 6600
  std::string s6608;                                                                     // 6608
  std::priority_queue<LevelChunkQueuedSavingElement> queued_saving_element_queue;        // 6640
  std::unique_ptr<TickingAreasManager> ticking_area_manager;                             // 6672
  std::unordered_set<_TickPtr *> ticks;                                                  // 6680
  std::unordered_map<ActorUniqueID, std::unique_ptr<MapItemSavedData>> map_data;         // 6736
  std::unordered_map<ActorUniqueID, Abilities> abilities;                                // 6792
  bool tearing_down;                                                                     // 6848
  IMinecraftEventing *eventing;                                                          // 6856
  std::unique_ptr<PlayerEventCoordinator> event_coord_player;                            // 6864
  std::unique_ptr<ServerPlayerEventCoordinator> event_coord_player_server;               // 6872
  std::unique_ptr<ClientPlayerEventCoordinator> event_coord_player_client;               // 6880
  std::unique_ptr<ActorEventCoordinator> event_coord_actor;                              // 6888
  std::unique_ptr<ClientLevelEventCoordinator> event_coord_level_client;                 // 6896
  std::unique_ptr<ServerLevelEventCoordinator> event_coord_level_server;                 // 6904
  std::unique_ptr<BlockEventCoordinator> event_coord_block;                              // 6912
  std::unique_ptr<ItemEventCoordinator> event_coord_item;                                // 6920
  StructureManager *structure_manager;                                                   // 6928
  ActorUniqueID auid6936;                                                                // 6936
  bool b6944;                                                                            // 6944
  std::chrono::time_point<std::chrono::system_clock> time6952;                           // 6952
  std::chrono::time_point<std::chrono::system_clock> time6960;                           // 6960
  bool b6968;                                                                            // 6968
  LootTables loots;                                                                      // 6976
  int unk7072;                                                                           // 7072
  int ticked_mob_count_previous;                                                         // 7076
  std::unique_ptr<Scoreboard> scoreboard;                                                // 7080
  BlockLegacy *border_block;                                                             // 7088
  ActorFactory actor_factory;                                                            // 7096
  IEntityRegistryOwner *entity_registry_owner;                                           // 7104
  EntitySystems entity_systems;                                                          // 7112
  static std::string createUniqueLevelID(int);

  Level(SoundPlayer &, std::unique_ptr<LevelStorage>, LevelData *, std::string const &, LevelSettings const &, bool, IMinecraftEventing &, ResourcePackManager &, StructureManager &, Scheduler &,
        IEntityRegistryOwner &);

  void addBossEventListener(BossEventListener *);
  void addListener(LevelListener &);
  void addParticle(ParticleType, Vec3 const &, Vec3 const &, int, CompoundTag const *, bool);
  void animateTick(Actor &);
  void broadcastBossEvent(BossEventUpdateType, Mob *);
  void broadcastDimensionEvent(BlockSource &, LevelEvent, Vec3 const &, Block const &, Player *);
  void broadcastDimensionEvent(BlockSource &, LevelEvent, Vec3 const &, int, Player *);
  void broadcastEntityEvent(Actor *, ActorEvent, int);
  void broadcastLevelEvent(LevelEvent, Vec3 const &, int, Player *);
  void broadcastSoundEvent(BlockSource &, LevelSoundEvent, Vec3 const &, Block const &, ActorType, bool, bool);
  void broadcastSoundEvent(BlockSource &, LevelSoundEvent, Vec3 const &, int, ActorType, bool, bool);
  bool checkAndHandleMaterial(AABB const &, MaterialType, Actor *);
  bool checkMaterial(AABB const &, MaterialType, Actor *);
  bool _checkUserStorage();
  void _cleanupDisconnectedPlayers();
  void createDimension(DimensionId);
  void _createMapSavedData(ActorUniqueID const &);
  void createMapSavedData(ActorUniqueID const &, BlockPos const &, DimensionId, int);
  void createMapSavedData(std::vector<ActorUniqueID> const &, BlockPos const &, DimensionId, int);
  void createPhotoStorage();
  void createRandomSeed();
  void deferTimedStorageActions(bool);
  void denyEffect(Vec3 const &);
  bool destroyBlock(BlockSource &, BlockPos const &, bool);
  void _destroyEffect(BlockPos const &, Block const &, Vec3 const &, int);
  void entityChangeDimension(Actor &, DimensionId);
  void expandMapByID(ActorUniqueID, bool);
  void explode(BlockSource &, Actor *, Vec3 const &, float, bool, bool, float, bool);
  void extinguishFire(BlockSource &, BlockPos const &, signed char);
  void fetchEntity(ActorUniqueID, bool) const;
  void findPath(Actor &, Actor &, float, bool, bool, bool, bool, bool, bool, bool, bool, bool);
  void findPath(Actor &, int, int, int, float, bool, bool, bool, bool, bool, bool, bool, bool, bool);
  void findPlayer(std::function<bool(Player const &)>) const;
  void _fixEntitiesRegion(std::vector<Actor *, std::allocator<Actor *>> &, BlockSource const &, Dimension &);
  void forceFlushRemovedPlayers();
  void forceRemoveEntity(Actor &);
  void forEachDimension(std::function<bool(Dimension &)>);
  void forEachDimension(std::function<bool(Dimension const &)>) const;
  void forEachPlayer(std::function<bool(Player &)>);
  void forEachPlayer(std::function<bool(Player const &)>) const;
  std::size_t getActivePlayerCount() const;
  std::vector<Player *> const &getActivePlayers() const;
  ActorEventCoordinator *getActorEventCoordinator();
  ActorFactory &getActorFactory();
  AdventureSettings &getAdventureSettings();
  Actor const *getAutonomousActiveEntity(ActorUniqueID) const;
  SmallSet<Actor *> &getAutonomousEntities();
  Actor const *getAutonomousEntity(ActorUniqueID) const;
  Actor const *getAutonomousInactiveEntity(ActorUniqueID) const;
  std::vector<std::unique_ptr<Actor>> &getAutonomousLoadedEntities();
  BehaviorFactory *getBehaviorFactory() const;
  BehaviorTreeGroup *getBehaviorTreeGroup() const;
  BlockEventCoordinator *getBlockEventCoordinator();
  ClientLevelEventCoordinator *_getClientLevelEventCoordinator();
  ClientPlayerEventCoordinator *getClientPlayerEventCoordinator();
  std::vector<ChunkPos> const &getClientTickingOffsets() const;
  int getCurrentTick() const;
  Abilities &getDefaultAbilities();
  GameType getDefaultGameType() const;
  BlockPos const &getDefaultSpawn() const;
  Difficulty getDifficulty() const;
  Dimension const &getDimension(DimensionId) const;
  DimensionConversionData getDimensionConversionData() const;
  ActorDefinitionGroup *getEntityDefinitions() const;
  IEntityRegistryOwner *getEntityRegistryOwner();
  EntitySystems &getEntitySystems();
  IMinecraftEventing *getEventing();
  FurnaceRecipes *getFurnaceRecipes() const;
  GameRules &getGameRules();
  GameRules const &getGameRules() const;
  BlockPalette *getGlobalBlockPalette() const;
  SmallSet<Actor *> &getGlobalEntities();
  HitResult &getHitResult();
  TaskGroup *getIOTasksGroup();
  ItemEventCoordinator *getItemEventCoordinator();
  LevelData *getLevelData();
  LevelData const *getLevelData() const;
  std::string getLevelId() const;
  LevelStorage *getLevelStorage();
  LevelStorage const *getLevelStorage() const;
  HitResult &getLiquidHitResult();
  LootTables &getLootTables();
  MapItemSavedData *getMapSavedData(ActorUniqueID);
  MapItemSavedData *getMapSavedData(ItemInstance const &);
  Actor *getMob(ActorUniqueID) const;
  NetEventCallback *getNetEventCallback() const;
  unsigned getNetherScale() const;
  unsigned getNewPlayerId() const;
  ActorUniqueID getNewUniqueID();
  Player *getNextPlayer(ActorUniqueID const &, bool);
  ActorRuntimeID getNextRuntimeID();
  unsigned getNumRemotePlayers();
  PacketSender *getPacketSender() const;
  PhotoStorage *getPhotoStorage();
  Social::GamePublishSetting getPlatformBroadcastMode() const;
  Player *getPlatformPlayer(std::string const &) const;
  Abilities *getPlayerAbilities(ActorUniqueID const &);
  Player *getPlayer(ActorUniqueID) const;
  PaletteColor getPlayerColor(Player const &) const;
  Player *getPlayerFromServerId(std::string const &) const;
  std::unordered_map<mce::UUID, PlayerListEntry> &getPlayerList();
  std::unordered_map<mce::UUID, PlayerListEntry> const &getPlayerList() const;
  Player *getPlayer(mce::UUID const &) const;
  std::string getPlayerNames();
  std::string &getPlayerPlatformOnlineId(mce::UUID const &) const;
  Player *getPlayer(std::string const &) const;
  std::string &getPlayerXUID(mce::UUID const &) const;
  PortalForcer *getPortalForcer();
  Player *getPrevPlayer(ActorUniqueID const &, bool);
  Player *getPrimaryLocalPlayer() const;
  ProjectileFactory *getProjectileFactory() const;
  Random &getRandom() const;
  Player *getRandomPlayer();
  Recipes *getRecipes() const;
  BlockLegacy *getRegisteredBorderBlock() const;
  PlayerEventCoordinator *getRemotePlayerEventCoordinator();
  Actor const *getRuntimeEntity(ActorRuntimeID, bool) const;
  Actor const *getRuntimePlayer(ActorRuntimeID) const;
  SavedDataStorage *getSavedData() const;
  Scoreboard *getScoreboard();
  std::string getScreenshotsFolder() const;
  unsigned getSeed();
  ServerLevelEventCoordinator *_getServerLevelEventCoordinator();
  ServerPlayerEventCoordinator *getServerPlayerEventCoordinator();
  BlockPos const &getSharedSpawnPos();
  bool getSimPaused();
  SoundPlayer &getSoundPlayer();
  Spawner *getSpawner() const;
  float getSpecialMultiplier(DimensionId);
  StructureManager *getStructureManager();
  TaskGroup *getSyncTasksGroup();
  int getTearingDown() const;
  int getTickedMobCountPrevious() const;
  TickingAreasManager *getTickingAreasMgr();
  std::vector<ChunkPos> const &getTickingOffsets() const;
  int getTime() const;
  std::size_t getUserCount() const;
  std::vector<std::unique_ptr<Player>> &getUsers();
  std::vector<std::unique_ptr<Player>> const &getUsers() const;
  std::string _getValidatedPlayerName(std::string const &);
  std::unique_ptr<Villages> getVillages();
  bool getXBLBroadcastIntent() const;
  Social::GamePublishSetting getXBLBroadcastMode() const;
  void _handleChangeDimensionRequests();
  void handleLevelEvent(LevelEvent, Vec3 const &, int);
  void _handlePlayerSuspension();
  void handleSoundEvent(LevelSoundEvent, Vec3 const &, int, ActorType, bool, bool);
  void handleSoundEvent(std::string const &, Vec3 const &, float, float);
  void handleStopAllSounds();
  void handleStopSoundEvent(std::string const &);
  bool hasCommandsEnabled() const;
  bool hasExperimentalGameplayEnabled() const;
  bool hasLANBroadcast() const;
  bool hasLevelStorage() const;
  bool hasPlatformBroadcast() const;
  bool hasStartWithMapEnabled() const;
  bool hasXBLBroadcast() const;
  bool isClientSide() const;
  bool isEdu() const;
  bool isExporting() const;
  bool isLeaveGameDone() const;
  bool isMultiplayerGame() const;
  bool isPlayerSuspended(Player &) const;
  bool _isSavingRequired() const;
  bool isUsableLevel(Level *);
  void levelCleanupQueueEntityRemoval(std::unique_ptr<Actor> &&, bool);
  void _loadAutonomousEntities();
  void _loadMapData(ActorUniqueID const &);
  void moveAutonomousEntityTo(Vec3 const &, ActorUniqueID, BlockSource &);
  void onChunkDiscarded(LevelChunk &);
  void _playerChangeDimension(Player *, ChangeDimensionRequest &);
  void playSound(BlockSource &, LevelSoundEvent, Vec3 const &, int, ActorType, bool, bool);
  void playSound(LevelSoundEvent, Vec3 const &, int, ActorType, bool, bool);
  void playSynchronizedSound(BlockSource &, LevelSoundEvent, Vec3 const &, Block const &, ActorType, bool, bool);
  void playSynchronizedSound(BlockSource &, LevelSoundEvent, Vec3 const &, int, ActorType, bool, bool);
  void _pollSaveGameStatistics();
  void potionSplash(Vec3 const &, Color const &, bool);
  void registerBorderBlock(BlockLegacy const &);
  void registerTemporaryPointer(_TickPtr &);
  void removeAllNonPlayerEntities(ActorUniqueID);
  void _removeAllPlayers();
  void removeAutonomousEntity(Actor *, LevelChunk *);
  void removeBossEventListener(BossEventListener *);
  void removeListener(LevelListener &);
  void requestMapInfo(ActorUniqueID);
  void requestPlayerChangeDimension(Player &, std::unique_ptr<ChangeDimensionRequest, std::default_delete<ChangeDimensionRequest>>);
  void _resumePlayer(mce::UUID const &);
  void resumePlayer(Player &);
  void _saveAllMapData();
  void _saveAutonomousEntities();
  void saveBiomeData();
  void saveDirtyChunks();
  void saveGameData();
  void saveLevelData();
  void savePlayers();
  void _saveSomeChunks();
  void sendAllPlayerAbilities(Player const &);
  void setActorEventCoordinator(std::unique_ptr<ActorEventCoordinator, std::default_delete<ActorEventCoordinator>> &&);
  void setBlockEventCoordinator(std::unique_ptr<BlockEventCoordinator, std::default_delete<BlockEventCoordinator>> &&);
  void _setClientLevelEventCoordinator(std::unique_ptr<ClientLevelEventCoordinator, std::default_delete<ClientLevelEventCoordinator>> &&);
  void setClientPlayerEventCoordinator(std::unique_ptr<ClientPlayerEventCoordinator, std::default_delete<ClientPlayerEventCoordinator>> &&);
  void setDefaultGameType(GameType);
  void setDefaultSpawn(BlockPos const &);
  void setFinishedInitializing();
  void setHasLockedBehaviorPack(bool);
  void setHasLockedResourcePack(bool);
  void setIsClientSide(bool);
  void setIsExporting(bool);
  void setItemEventCoordinator(std::unique_ptr<ItemEventCoordinator, std::default_delete<ItemEventCoordinator>> &&);
  void setLANBroadcast(bool);
  void setLevelId(std::string);
  void setMsaGamertagsOnly(bool);
  void setMultiplayerGame(bool);
  void setNetEventCallback(NetEventCallback *);
  void setPacketSender(PacketSender *);
  void setPlatformBroadcast(bool);
  void setPlatformBroadcastMode(Social::GamePublishSetting);
  void setPlayerAbilities(ActorUniqueID const &, Abilities);
  void setRemotePlayerEventCoordinator(std::unique_ptr<PlayerEventCoordinator, std::default_delete<PlayerEventCoordinator>> &&);
  void _setServerLevelEventCoordinator(std::unique_ptr<ServerLevelEventCoordinator, std::default_delete<ServerLevelEventCoordinator>> &&);
  void setServerPlayerEventCoordinator(std::unique_ptr<ServerPlayerEventCoordinator, std::default_delete<ServerPlayerEventCoordinator>> &&);
  void setSimPaused(bool);
  void setTime(int);
  void setXBLBroadcast(bool);
  void setXBLBroadcastIntent(bool);
  void setXBLBroadcastMode(Social::GamePublishSetting);
  void suspendAndSave();
  void _suspendPlayer(mce::UUID const &);
  void suspendPlayer(Player &);
  void _syncTime(int);
  void takePicture(cg::ImageBuffer &, Actor *, Actor *, ScreenshotOptions &);
  void tickedMob();
  void tickEntities();
  void tickEntitySystems();
  void _tickTemporaryPointers();
  void unregisterTemporaryPointer(_TickPtr &);
  void upgradeStorageVersion(StorageVersion);
  bool useMsaGamertagsOnly() const;
  bool _validatePlayerName(Player &);
  void waitAsyncSuspendWork();

  virtual ~Level();                                                         // 0,8
  virtual void onSourceCreated(BlockSource &) override;                     // 16
  virtual void onSourceDestroyed(BlockSource &) override;                   // 24
  virtual void startLeaveGame();                                            // 88
  virtual void addEntity(BlockSource &, std::unique_ptr<Actor>);            // 96
  virtual void addGlobalEntity(BlockSource &, std::unique_ptr<Actor>);      // 104
  virtual void addAutonomousEntity(BlockSource &, std::unique_ptr<Actor>);  // 112
  virtual void addPlayer(std::unique_ptr<Player>);                          // 120
  virtual std::unique_ptr<Actor> takeEntity(ActorUniqueID);                 // 128
  virtual std::unique_ptr<Actor> borrowEntity(ActorUniqueID, LevelChunk *); // 136
  virtual void onPlayerDeath(Player &, ActorDamageSource const &);          // 144
  virtual void tick();                                                      // 152
  virtual void directTickEntities(BlockSource &);                           // 160
  virtual void updateSleepingPlayerList();                                  // 168
  virtual void setDifficulty(Difficulty);                                   // 176
  virtual void setCommandsEnabled(bool);                                    // 184
  virtual ServerLevelEventCoordinator *getLevelEventCoordinator(void);      // 192
  virtual void onNewChunk(BlockSource &, LevelChunk &);                     // 200
  virtual void onNewChunkFor(Player &, LevelChunk &);                       // 208
  virtual void onChunkLoaded(LevelChunk &);                                 // 216
  virtual void queueEntityRemoval(std::unique_ptr<Actor> &&, bool);         // 224
  virtual void removeEntityReferences(Actor &, bool);                       // 232
  virtual ResourcePackManager *getClientResourcePackManager() const;        // 240
  virtual ResourcePackManager *getServerResourcePackManager() const;        // 248
  virtual TradeTables &getTradeTables();                                    // 255
};

static_assert(104 == offsetof(Level, plist104));
static_assert(240 == offsetof(Level, listeners));
static_assert(288 == offsetof(Level, level_data));
static_assert(864 == offsetof(Level, block_source_set));
static_assert(920 == offsetof(Level, actor_defs));
static_assert(1040 == offsetof(Level, portal_forcer));
static_assert(1128 == offsetof(Level, player_list_entrys));
static_assert(3900 == offsetof(Level, rand3900));
static_assert(6416 == offsetof(Level, net_event));
static_assert(6504 == offsetof(Level, autonomous_loaded_entities));
static_assert(6600 == offsetof(Level, scheduler));
static_assert(6736 == offsetof(Level, map_data));
static_assert(6848 == offsetof(Level, tearing_down));
static_assert(6904 == offsetof(Level, event_coord_level_server));
static_assert(7072 == offsetof(Level, unk7072));
static_assert(7112 == offsetof(Level, entity_systems));

struct MinecraftCommands;

struct ServerLevel : Level {
  bool b7144;                                    // 7144
  ResourcePackManager *resource_pack_mgr_client; // 7152
  ResourcePackManager *resource_pack_mgr_server; // 7160
  MinecraftCommands *cmds;                       // 7168
  TradeTables trade_table;                       // 7176

  ServerLevel(SoundPlayer &, std::unique_ptr<LevelStorage>, LevelData *, std::string const &, LevelSettings const &, IMinecraftEventing &, ResourcePackManager &, ResourcePackManager &,
              StructureManager &, MinecraftCommands &, Scheduler &, IEntityRegistryOwner &);

  bool allPlayersSleeping() const;
  void awakenAllPlayers();
  MinecraftCommands *getCommands();
  void stopWeather();

  virtual ~ServerLevel() override;                                              // 0, 8
  virtual void tick() override;                                                 // 152
  virtual void updateSleepingPlayerList() override;                             // 168
  virtual void setDifficulty(Difficulty) override;                              // 176
  virtual void setCommandsEnabled(bool) override;                               // 184
  virtual ServerLevelEventCoordinator *getLevelEventCoordinator(void) override; // 192
  virtual ResourcePackManager *getClientResourcePackManager() const override;   // 240
  virtual ResourcePackManager *getServerResourcePackManager() const override;   // 248
  virtual TradeTables &getTradeTables() override;                               // 255
};

static_assert(7144 == offsetof(ServerLevel, b7144));
static_assert(7176 == offsetof(ServerLevel, trade_table));
