#pragma once

#include "AgentCommands.h"
#include "BehaviorTree.h"
#include "Descriptions.h"
#include "LevelListener.h"
#include "types.h"
#include <chrono>
#include <set>

struct Block;
struct Actor;
struct Player;
struct ActorDamageCause;
struct ActorDamageSource;
struct ActorInteraction;
struct Container;
struct ItemInstance;
struct ItemActor;
struct UpdateEquipPacket;
struct OnHitSubcomponent;
struct CompoundTag;
struct AttributeModifier;
enum struct ContainerID;
struct Command;
struct CommandOrigin;
struct NpcAction;
struct MerchantRecipeList;
struct UpdateTradePacket;
struct ActorEvent;
struct ITickingArea;
struct ActorAnimationControllerPlayer;
struct ActorAnimationPlayer;
struct HashedString;
struct ModelPartLocator;
struct RenderParams;

struct ContainerContentChangeListener {
  ContainerContentChangeListener();
  virtual bool containerContentChanged(int) = 0;
  virtual ~ContainerContentChangeListener();
};

struct Hopper {
  int cooldown;  // 0
  bool unk4;     // 4
  bool unk5;     // 5
  int moveSpeed; // 8
  bool isOnCooldown();
  void setCooldown(int);
  void setMoveItemSpeed(int);
};

struct BaseCommandBlock {
  std::string output;             // 0
  std::vector<std::string> vlist; // 32
  std::string command;            // 56
  std::string customName;         // 88
  std::unique_ptr<Command> cmd;   // 120
  void *unk128;                   // 128
  union {
    char *commandJson; // 136
    int version;
  };
  int successCount; // 140
  bool trackOutput; // 144

  static std::string DefaultCommandBlockName; //@

  BaseCommandBlock();
  void compile(CommandOrigin const &, Level &);
  std::string const &getCommand() const;
  std::string const &getRawName() const;
  void setSuccessCount(int) const;
  int getSuccessCount() const;
  void setTrackOutput(int) const;
  bool getTrackOutput() const;
  void load(CompoundTag const &);
  void save(CompoundTag &);
  std::string getLastOutput() const;
  void setLastOutput(std::string const &, std::vector<std::string> const &);
  void setOutput(std::string const &, std::vector<std::string> const &);
  void setCommand(BlockSource &, ActorUniqueID const &, std::string const &);
  void setCommand(BlockSource &, BlockPos const &, std::string const &);
  std::string const &getName() const;
  void setName(std::string const &);
  void performCommand(BlockSource &, ActorUniqueID const &);
  void performCommand(BlockSource &, BlockPos const &, bool &);
  ~BaseCommandBlock();
};

#define F_INIT_FROM_DEFINITION void initFromDefinition()
#define F_TICK void tick()
#define F_RELOAD_COMPONENT void reloadComponent()
#define F_GET_INTERACTION bool getInteraction(Player &, ActorInteraction &)
#define F_ADDITIONAL_SAVED_DATA                                                                                                                                                                        \
  void readAdditionalSaveData(CompoundTag const &);                                                                                                                                                   \
  void addAdditionalSaveData(CompoundTag &)
#define START_COMPONENT(name, type, ...)                                                                                                                                                               \
  struct alignas(8) name {                                                                                                                                                                             \
    name(type &, ##__VA_ARGS__)
#define START_COMPONENT_EXTENDED(name, ext, type, ...)                                                                                                                                                 \
  struct alignas(8) name : ext {                                                                                                                                                                       \
    name(type &, ##__VA_ARGS__)
#define END_COMPONENT }

START_COMPONENT(AddRiderComponent, Actor);
Actor &act;                   // 0
ActorDefinitionIdentifier id; // 8
void addRider();
F_RELOAD_COMPONENT;
F_INIT_FROM_DEFINITION;
END_COMPONENT;

START_COMPONENT(AgeableComponent, Actor);
Actor &act; // 0
int age;    // 8
int getAge();
void setAge(int);
bool isBaby() const;
F_ADDITIONAL_SAVED_DATA;
F_INIT_FROM_DEFINITION;
F_TICK;
F_GET_INTERACTION;
END_COMPONENT;

START_COMPONENT(AngryComponent, Mob);
unsigned timer;       // 0
Mob &mob;             // 8
bool broadcastAnger;  // 16
float broadcastRange; // 20
bool unk24;           // 24
void restartTimer();
void setAngry(bool);
F_INIT_FROM_DEFINITION;
F_TICK;
END_COMPONENT;

START_COMPONENT(BalloonableComponent, Mob);
Mob &mob; // 0
void attach(Actor &, ItemInstance const &);
void detach(Actor &);
F_INIT_FROM_DEFINITION;
F_TICK;
F_GET_INTERACTION;
END_COMPONENT;

START_COMPONENT(BalloonComponent, Actor);
Actor &act;         // 0
ActorUniqueID auid; // 8
float unk16;        // 16
bool unk20;         // 20
static float FENCE_BALLOON_RANGE, MAX_BALLOON_RANGE;
static std::string ATTACHED_TAGB, MAX_HEIGHT_TAG, SHOULD_DROP_TAG;
void detach();
Actor *getAttachedEntity();
void setAttachedEntity(Actor *);
Actor *getBalloonForEntity(Actor const &);
void onRemoved(bool);
F_INIT_FROM_DEFINITION;
F_ADDITIONAL_SAVED_DATA;
F_TICK;
END_COMPONENT;

START_COMPONENT(BehaviorComponent, Actor);
Actor &act;                             // 0
bool flag;                              // 8
BehaviorTreeDefinitionPtr tree;         // 16
std::unique_ptr<BehaviorNode> behavior; // 32
BehaviorData behavior_data;             // 40
Actor &getEntity();
BehaviorData &getBehaviorData();
void reset();
void runTree(BehaviorTreeDefinitionPtr &);
F_TICK;
END_COMPONENT;

START_COMPONENT(BreathableComponent, Mob);
int unk0;                                 // 0
float unk4;                               // 4
int unk8;                                 // 8
bool breathesAir;                         // 12
bool breathesWater;                       // 13
bool breathesLava;                        // 14
bool breathesSolids;                      // 15
std::set<Block const *> breatheBlocks;    // 24
std::set<Block const *> nonBreatheBlocks; // 72
Mob &mob;                                 // 120
int unk128;                               // 128
bool canBreathe();
bool canBreatheAir();
bool canBreatheLava();
bool canBreatheWater();
short getAirSupply();
short getTotalAirSupply();
void setAirSupply(short);
void setTotalAirSupply(short);
F_ADDITIONAL_SAVED_DATA;
F_TICK;
F_INIT_FROM_DEFINITION;
END_COMPONENT;

START_COMPONENT(BreedableComponent, Mob);
BreedableDescription *desc; // 0
Mob &mob;                   // 8
int loveTicks;              // 16
int unk20;                  // 20
int unk24;                  // 24
bool unk28;                 // 28
bool unk29;                 // 29
ActorUniqueID auid;         // 32
bool canMate(Actor const &);
Actor *getLoveCause() const;
int getLoveTicks();
void mate(Mob &);
void resetLove();
void setInLove(Player const *);
void doHurt(ActorDamageSource const &, int);
F_TICK;
F_ADDITIONAL_SAVED_DATA;
F_GET_INTERACTION;
F_INIT_FROM_DEFINITION;
END_COMPONENT;

START_COMPONENT(BribeableComponent, Mob);
BribeableDescription *desc; // 0
Mob &mob;                   // 8
int unk16;                  // 16
int unk20;                  // 20
int unk24;                  // 24
void resetBribe();
void setBribed(Player const *);
F_ADDITIONAL_SAVED_DATA;
F_GET_INTERACTION;
F_INIT_FROM_DEFINITION;
F_TICK;
END_COMPONENT;

START_COMPONENT_EXTENDED(ContainerComponent, ContainerContentChangeListener, Actor);
std::unique_ptr<Container> pContainer; // 8
Actor &act;                            // 16
bool can_be_siphoned_from;             // 24
bool mPrivate;                         // 25
bool restrict_to_owner;                // 26
std::string loot_table;                // 32
int loot;                              // 64
void addItem(BlockSource &, ItemInstance &, int, int);
void addItem(ItemActor &);
bool canBeSiphonedFrom() const;
bool isPrivate() const;
bool canOpenContainer(Player &) const;
bool openContainer(Player &);
int countItemsOfType(ItemInstance const &);
void removeItemsOfType(ItemInstance const &);
void setItem(ItemInstance const &, int);
void dropContents(BlockSource &, Vec3 const &, bool);
bool hasRoomForItem(ItemActor &);
void rebuildContainer(ContainerType, int, bool, int, bool);
void setLootTable(std::string, int);
virtual bool containerContentChanged(int);
virtual ~ContainerComponent();
F_INIT_FROM_DEFINITION;
F_ADDITIONAL_SAVED_DATA;
END_COMPONENT;

START_COMPONENT(DamageOverTimeComponent, Mob);
int damagePerHurt;   // 0
int damageTime;      // 4
int timeBetweenHurt; // 8
Mob &mob;            // 16
F_TICK;
F_INIT_FROM_DEFINITION;
F_ADDITIONAL_SAVED_DATA;
END_COMPONENT;

START_COMPONENT(ExplodeComponent, Actor);
Actor &act;                     // 0
int fuse;                       // 8
int maxFuse;                    // 12
float power;                    // 16
float maxResistance;            // 20
bool fuseLit;                   // 24
bool causesFire;                // 25
bool breaks_blocks;             // 26
bool fireAffectedByGriefing;    // 27
bool destroyAffectedByGriefing; // 28
bool allowUnderwater;           // 29
void explode(BlockSource &, Vec3 const &, float);
int getFuse();
int getMaxFuse();
void setFuseLength(int);
void triggerFuse();
bool isPrime();
F_INIT_FROM_DEFINITION;
F_ADDITIONAL_SAVED_DATA;
F_TICK;
END_COMPONENT;

START_COMPONENT(TameableComponent, Actor);
Actor &act;                   // 0
float probability;            // 8
std::set<Item const *> items; // 16
F_INIT_FROM_DEFINITION;
F_GET_INTERACTION;
END_COMPONENT;

START_COMPONENT(LookAtComponent, Actor);
Actor &act;              // 0
bool setTarget;          // 8
float searchRadius;      // 12
bool allow_invulnerable; // 16
F_INIT_FROM_DEFINITION;
F_TICK;
END_COMPONENT;

START_COMPONENT(ProjectileComponent, Actor);
enum struct EAxis;
short unk0;                              // 0
bool noPhysics;                          // 2
ActorUniqueID ownerId;                   // 8
Actor &act;                              // 16
Vec3 throwPos;                           // 24
Vec3 apexPos;                            // 36
bool unk48;                              // 48
int inGroundTime;                        // 52
float damage;                            // 56
int flightTime;                          // 60
int shakeTime;                           // 64
BlockPos bp68;                           // 68
int unk80;                               // 80
BlockPos blockStuckTo;                   // 84
Vec3 collisionPos;                       // 96
int unk108;                              // 108
int enchantPower;                        // 112
int enchantImpaler;                      // 116
HitResult hit;                           // 120
char hitFace;                            // 216
char unk217;                             // 217
float throwPower;                        // 220
Vec3 offset;                             // 228
ParticleType particle;                   // 240
float gravity;                           // 244
float angleoffset;                       // 248
std::vector<OnHitSubcomponent *> onHits; // 256
DefinitionTrigger onHitTrigger;          // 280
float uncertaintyBase;                   // 408
float uncertaintyMultiplier;             // 412
ActorType filter;                        // 416
float onFireTime;                        // 420
int potionEffect;                        // 424
float splashRange;                       // 428
bool knockback;                          // 432
float knockbackForce;                    // 436
bool catchFire;                          // 440
bool lightning;                          // 441
bool splashPotion;                       // 445
bool semirandomdiffdamage;               // 456
float inertia;                           // 448
float liquid_inertia;                    // 452
int anchor;                              // 460
char hitSoundId;                         // 464
char realHitSoundId;                     // 465
char shootSoundId;                       // 466
bool isdangerous;                        // 467
bool shootTarget;                        // 468
bool damagesOwner;                       // 469
bool reflectOnHurt;                      // 470
bool destroyOnHurt;                      // 471
bool stopOnHurt;                         // 472
bool critParticleOnHurt;                 // 473
bool hitWater;                           // 474
bool multipletargets;                    // 475
bool homing;                             // 476
ActorUniqueID targetId;                  // 480
ActorUniqueID auid496;                   // 496
Vec3 v504;                               // 504
bool shouldbounce;                       // 524
HitResult hit536;                        // 536
int getAnchor();
Vec3 const &getApexPos() const;
BlockPos const &getBlockStuckTo() const;
void setBlockStuckTo(BlockPos const &);
bool getCatchFire();
float getDamage();
void setDamage(float);
bool getDamagesOwner();
void setDamagesOwner(bool);
bool getEnchantChanneling() const;
void setChanneling();
int getEnchantImpaler() const;
void setEnchantImparler(int const &);
int getEnchantPower();
void setEnchantPower(int);
Actor &getEntity();
float getGravity();
HitResult getHitResult();
char getHitSound();
bool getHitWater();
bool getIgniteOnHit();
bool getInGround();
void setInGround(bool);
int getInGroundTime() const;
bool getIsDangerous();
bool getKnockback();
float getKnockbackForce() const;
void setKnockbackForce(float);
bool getNoPhysics();
void setNoPhysics(bool);
Vec3 getOffset();
float getOnFireTime();
ActorUniqueID &getOwnerId();
void setOwnerId(ActorUniqueID const &);
ParticleType getParticleType();
int getPotionEffect();
bool getSemiRandomDiffDamage();
char getShootSound();
bool getShootTarget();
Vec3 getShooterAngle() const;
bool getShouldBounce();
void setShouldBounce(bool);
float getSplashRange();
float getThrowPower() const;
float getThrowUpAngleOffset() const;
Vec3 const &getThrowPos() const;
float getUncertainty() const;
float getUncertaintyBase() const;
float getUncertaintyMultiplier() const;
bool isCritical();
void lerpMotion(Vec3 const &);
void onHit(HitResult const &);
int selectNextMoveDirection(EAxis);
void setCollisionPos(Vec3 const &);
void setFlightTime(int);
void setHitFace(signed char const &);
void setShakeTime(int);
void shoot(Mob &);
void shoot(Vec3 const &, float, float, Vec3 const &, Actor *);
void handleLightningOnHit();
void handleMovementGravity();
void handleMovementHoming();
void hurt(ActorDamageSource const &);
F_INIT_FROM_DEFINITION;
F_ADDITIONAL_SAVED_DATA;
F_TICK;
END_COMPONENT;

START_COMPONENT(TeleportComponent, Actor);
Actor &act;
bool randomTeleports;         // 8
float minRandomTeleportTime;  // 12
float maxRandomTeleportTime;  // 16
int ticks;                    // 20
Vec3 randomTeleportCube;      // 24
float targetDistance;         // 36
float target_teleport_chance; // 40
float lightTeleportChance;    // 44
float darkTeleportChance;     // 48
void randomTeleport();
void teleport(Vec3 const &);
void teleportTowards(Actor const &);
F_INIT_FROM_DEFINITION;
F_TICK;
END_COMPONENT;

START_COMPONENT(MountTamingComponent, Actor);
Actor &act;             // 0
int minTemper;          // 8
int maxTemper;          // 12
int temper;             // 16
int attempt_temper_mod; // 20
void tameWithOwner(Player *, bool);
F_INIT_FROM_DEFINITION;
F_ADDITIONAL_SAVED_DATA;
F_GET_INTERACTION;
F_TICK;
END_COMPONENT;

START_COMPONENT(TimerComponent, Actor);
Actor &act;          // 0
bool looping;        // 8
bool hasExecuted;    // 9
bool randomInterval; // 10
int countTime;       // 12
int startTime;       // 16
FloatRange time;     // 20
void restartTimer();
F_INIT_FROM_DEFINITION;
F_ADDITIONAL_SAVED_DATA;
F_TICK;
END_COMPONENT;

START_COMPONENT(RideableComponent, Actor);
Actor &act;
bool canAddRider(Actor &);
bool getFirstAvailableSeatPosition(Actor &, Vec3 &);
int getRiderIndex(Actor &);
SeatDescription getValidSeat() const;
void positionRider(Actor &);
bool pullInEntity(Actor &);
F_INIT_FROM_DEFINITION;
F_GET_INTERACTION;
END_COMPONENT;

START_COMPONENT(ShooterComponent, Actor);
Actor &act;
std::string def;
int aux;
int getAuxValue();
void onShoot();
F_INIT_FROM_DEFINITION;
F_ADDITIONAL_SAVED_DATA;
END_COMPONENT;

START_COMPONENT(InteractComponent, Actor);
Actor &act;
short delay;
F_INIT_FROM_DEFINITION;
F_GET_INTERACTION;
F_TICK;
END_COMPONENT;

START_COMPONENT(EquippableComponent, Actor);
Actor &act;
std::vector<SlotDescriptor> slots;
UpdateEquipPacket createDataPacket(ContainerID) const;
std::unique_ptr<CompoundTag> createTag() const;
std::vector<ItemInstance> getSlot() const;
std::size_t getSlotCount() const;
std::vector<SlotDescriptor> const &getSlots();
void onItemChanged(unsigned, Item const *);
void setDataFromPacket(UpdateEquipPacket const &);
F_INIT_FROM_DEFINITION;
END_COMPONENT;

START_COMPONENT(HealableComponent, Actor);
Actor &act;
F_INIT_FROM_DEFINITION;
F_GET_INTERACTION;
END_COMPONENT;

START_COMPONENT(ScaleByAgeComponent, Actor);
Actor &act;        // 0
float start_scale; // 8
float end_scale;   // 12
float getScale() const;
F_INIT_FROM_DEFINITION;
F_TICK;
END_COMPONENT;

START_COMPONENT(TargetNearbyComponent, Actor);
Actor &act;          // 0
float inside_range;  // 8
float outside_range; // 12
F_INIT_FROM_DEFINITION;
F_TICK;
END_COMPONENT;

START_COMPONENT(TransformationComponent, Actor);
Actor &act;  // 0
float delay; // 8
void transformIfAble();
F_INIT_FROM_DEFINITION;
F_ADDITIONAL_SAVED_DATA;
F_TICK;
F_RELOAD_COMPONENT;
END_COMPONENT;

START_COMPONENT(BoostableComponent, Mob);
Mob &mob;      // 0
bool boosting; // 8
int ticks;     // 12
int duration;  // 16
float unk20;   // 20
static std::shared_ptr<AttributeModifier> SPEED_MODIFIER_BOOSTING;
static mce::UUID SPEED_MODIFIER_BOOSTING_UUID;
void boost(Player &);
bool isBoosting() const;
bool itemUseText(Player const &, std::string &);
bool onItemInteract(ItemInstance &, Player &);
void removeRider(Player &);
F_INIT_FROM_DEFINITION;
F_TICK;
END_COMPONENT;

START_COMPONENT(RailMovementComponent, Actor);
Actor &act;      // 0
float max_speed; // 8
void comeOffTrack(float);
void moveAlongTrack(BlockPos const &, float, float, Block const &);
F_INIT_FROM_DEFINITION;
F_TICK;
END_COMPONENT;

START_COMPONENT(DamageSensorComponent, Actor);
Actor &act;    // 0
int type;      // 8
bool is_fatal; // 12
int cause;     // 16
bool isFatal() const;
int getCause() const;
void recordDamage(Actor *, ActorDamageCause, int, bool);
F_INIT_FROM_DEFINITION;
F_ADDITIONAL_SAVED_DATA;
F_TICK;
END_COMPONENT;

START_COMPONENT(LeashableComponent, Actor);
Actor &act; // 0
void leash(Player *);
void unleash();
F_INIT_FROM_DEFINITION;
F_GET_INTERACTION;
F_TICK;
END_COMPONENT;

START_COMPONENT_EXTENDED(HopperComponent, Hopper, Actor);
Actor &act;   // 16
BlockPos pos; // 24
bool pullInItems();
F_INIT_FROM_DEFINITION;
F_TICK;
END_COMPONENT;

START_COMPONENT(CommandBlockComponent, Actor);
Actor &act;                // 0
BaseCommandBlock cmdBlock; // 8
int unk160;                // 160
int unk164;                // 164
bool _shouldTick;          // 168
BaseCommandBlock &getBaseCommandBlock();
BaseCommandBlock const &getBaseCommandBlock() const;
void onCommandBlockUpdate(std::string const &, bool, std::string);
void setLastOutput(std::string const &);
void setName(std::string const &);
bool shouldTick();
void setShouldTick(bool);
void setTrackOutput(bool);
F_INIT_FROM_DEFINITION;
F_ADDITIONAL_SAVED_DATA;
F_TICK;
END_COMPONENT;

START_COMPONENT(NameableComponent, Actor);
Actor &act;                // 0
bool allowNameTagRenaming; // 8
bool alwaysShow;           // 9
void nameEntity(std::string const &);
bool getAlwaysShow();
F_INIT_FROM_DEFINITION;
F_GET_INTERACTION;
END_COMPONENT;

START_COMPONENT(RailActivatorComponent, Actor);
Actor &act;
F_INIT_FROM_DEFINITION;
F_TICK;
END_COMPONENT;

START_COMPONENT(NpcComponent, Actor);
Actor &act;                                      // 0
std::vector<std::unique_ptr<NpcAction>> actions; // 8
static std::string ACTIONS_TAG;                  // Actions
static std::string URL_TAG;                      // Url
static std::string NAME_TAG;                     // Name
static std::string INTERACTIVE_TAG;              // InterativeText
void compileCommand(CommandOrigin const &, std::string const &, int);
void executeClosingCommands();
void executeCommandAction(int);
void addAction(std::unique_ptr<NpcAction>);
void setActions(std::vector<std::unique_ptr<NpcAction>> &&);
NpcAction *getActionAt(std::size_t);
void removeActionAt(std::size_t);
std::size_t getActionCount();
void getUpdatedActions(std::string const &, std::vector<std::string> &, bool &, bool &);
void loadActions();
void requestClosingCommandActions();
void requestCommandAction();
void sendChangedPacket();
std::string getInteractiveText() const;
void setInteractiveText(std::string const &, bool);
std::string const &getName() const &;
void setName(std::string const &, bool);
void syncActionsWithClient();
void syncActionsWithServer();
F_INIT_FROM_DEFINITION;
F_ADDITIONAL_SAVED_DATA;
F_GET_INTERACTION;
END_COMPONENT;

START_COMPONENT(TripodCameraComponent, Actor);
Actor &act;
F_ADDITIONAL_SAVED_DATA;
F_GET_INTERACTION;
END_COMPONENT;

START_COMPONENT(BossComponent, Mob, bool);
Mob &mob;                                       // 0
bool bossBarViaible;                            // 8
float healthPercent;                            // 12
bool darkenSky;                                 // 16
bool createWorldFog;                            // 17
BossBarColor color;                             // 20
BossBarOverlay overlay;                         // 24
bool unk28;                                     // 28
int unk32;                                      // 32
int health;                                     // 36
int rangeSqr;                                   // 40
std::chrono::steady_clock clock;                // 48
std::unordered_map<mce::UUID, int> playerParty; // 56
BossComponent(BossComponent const &);
void addPlayerToParty(mce::UUID, int);
BossBarColor getColor();
BossBarOverlay getOverlay();
bool getCreateWorldFog();
float getHealthPercent();
ActorUniqueID getOwnerUniqueID();
std::unordered_map<mce::UUID, int> const &getPlayerParty() const;
int getRangeSqr() const;
bool getShouldDarkenSky();
bool isHealthBarVisible();
bool isWithinRange(Player const *);
void registerPlayer(Player *);
void unregister(Player *);
void sendDeathTelemetry();
void setColor(BossBarColor);
BossComponent setCreateWorldFog(bool);
void setDarkenSky(bool);
void setHealthBarVisible(bool);
void setName(std::string const &);
std::string getName();
void setOverlay(BossBarOverlay);
void setPercent(float);
void setRangeSqr(int);
F_TICK;
END_COMPONENT;

START_COMPONENT(PeekComponent, Actor);
int ticks;     // 0
bool executed; // 4
Actor &act;    // 8
F_INIT_FROM_DEFINITION;
F_TICK;
END_COMPONENT;

START_COMPONENT(TradeableComponent, Actor);
int tradeTier;                             // 0
bool willing;                              // 21
int riches;                                // 24
Actor &act;                                // 32
std::unique_ptr<MerchantRecipeList> lists; // 40
std::string displayName;                   // 48
std::vector<int> firstTimeTradeVector;     // 80
UpdateTradePacket createDataPacket(ContainerID);
void setDataFromPacket(UpdateTradePacket const &);
MerchantRecipeList *getOffers();
void handleEntityEvent(ActorEvent, int);
void newServerAiStep();
void notifyTrade();
void notifyTradeUpdated(ItemInstance &, bool);
void setWillingToBreed(bool);
std::string const &getDisplayName() const;
F_INIT_FROM_DEFINITION;
F_ADDITIONAL_SAVED_DATA;
F_GET_INTERACTION;
END_COMPONENT;

START_COMPONENT(AgentCommandComponent, Actor);
Actor &act;                                  // 0
std::unique_ptr<AgentCommands::Command> cmd; // 8
int unk16;                                   // 16
Actor &getEntity();
bool hasCommand();
void addCommand(std::unique_ptr<AgentCommands::Command>);
F_INIT_FROM_DEFINITION;
F_TICK;
END_COMPONENT;

START_COMPONENT(ShareableComponent, Actor);
Actor &act;
bool hasSurplus(ItemInstance const &, bool);
bool hasSurplus(ItemInstance const &, ItemInstance const &, bool);
bool wantsMore(ItemInstance const &);
bool willPickup(ItemInstance const &);
F_INIT_FROM_DEFINITION;
END_COMPONENT;

START_COMPONENT(SpawnActorComponent, Actor);
Actor &act;
bool flag;                // 8
int min_wait_time;        // 12
int max_wait_time;        // 16
int spawn_timer;          // 20
char const *sound;        // 24
Item const *item;         // 32
std::string spawn_entity; // 40
std::string spawn_method; // 72
void spawnEntity();
F_INIT_FROM_DEFINITION;
F_ADDITIONAL_SAVED_DATA;
F_TICK;
END_COMPONENT;

START_COMPONENT(PhysicsComponent, Actor);
Actor &act;
bool isAffectedByGravity() const;
void setAffectedByGravity(bool);
bool setHasCollision(bool);
F_INIT_FROM_DEFINITION;
F_ADDITIONAL_SAVED_DATA;
END_COMPONENT;

START_COMPONENT(TickWorldComponent, Actor);
Actor &act;                              // 0
unsigned radius;                         // 8
float distance_to_players;               // 12
bool never_despawn;                      // 16
bool loaded;                             // 17
std::weak_ptr<ITickingArea> tickingArea; // 24
unsigned getChunkRadius() const;
float getMaxDistanceToPlayers() const;
void setTickingArea(std::shared_ptr<ITickingArea>);
void updateArea();
std::shared_ptr<ITickingArea> getTickingArea();
bool hasTickingArea() const;
bool isAlwaysActive() const;
void removeArea();
F_INIT_FROM_DEFINITION;
END_COMPONENT;

START_COMPONENT_EXTENDED(DanceComponent, LevelListener, Actor, float);
Actor &act;  // 8
float unk16; // 16
Vec3 unk20;  // 20
virtual ~DanceComponent();
virtual void levelSoundEvent(LevelSoundEvent, Vec3 const &, int, ActorType, bool, bool);
F_TICK;
END_COMPONENT;

START_COMPONENT(FlockingComponent, Actor);
Actor &act;                              // 0
std::vector<ActorUniqueID> neighborhood; // 8
Vec3 v32;                                // 32
Vec3 group_velocity;                     // 44
Vec3 goal_heading;                       // 56
Vec3 current_heading;                    // 68
bool in_water;                           // 80
bool match_variants;                     // 81
bool unk82;                              // 82
bool is_leader;                          // 83
bool in_flock;                           // 84
bool is_enabled;                         // 85
bool has_goal_target;                    // 86
bool using_direction;                    // 87
int flock_limit;                         // 88
float loner_chance;                      // 96
float goal_weight;                       // 100
float influence_radius;                  // 104
float breach_influence;                  // 108
float separation_weight;                 // 112
float separation_threshold;              // 116
float cohesion_weight;                   // 120
float cohesion_threshold;                // 124
float innner_cohesion_threshold;         // 128
float min_height;                        // 132
float max_height;                        // 136
float block_distance;                    // 140
float block_weight;                      // 144
bool needs_double_speed;                 // 148
void addFlockMember(ActorUniqueID);
void breakFlock();
void calculateFlockVector();
void calculateGoalHeading();
bool canJoinFlock() const;
bool checkForSteeringCollision(Vec3 const &, float, Vec3 &);
float chooseRandomDirection(Vec3 &);
void clearNeighborhood();
bool containsMember(ActorUniqueID);
float getCohesionWeight() const;
int getFlockLimit() const;
Vec3 getCurrentHeading() const;
Vec3 getGoalHeading() const;
bool getInFlock() const;
bool getIsEnabled() const;
bool getIsLeader() const;
Mob &getMob();
std::vector<ActorUniqueID> &getNeighborhood();
float getSeparationWeight() const;
bool getUsingDir() const;
void getWaterHeights(short &, float &, float &, float &);
bool hasGoalTarget() const;
void joinFlock();
void manageNeighborhood();
void mergeNeighborhood();
bool needsDoubleSpeed();
void setCurrentHeading(Vec3 const &);
void setFlockLimit(int);
void setGoalHeading(Vec3 const &);
void setGroupVelocity(Vec3 const &);
void setInFlock(bool);
void setIsLeader(bool);
void setUsingDirection(bool);
void toggleFlock(bool);
void updateNeighborhoodData();
bool validateVariantEntityTypes(Actor const &, Actor const &);
F_INIT_FROM_DEFINITION;
F_TICK;
END_COMPONENT;

START_COMPONENT(BucketableComponent, Actor);
Actor &act;
F_INIT_FROM_DEFINITION;
F_GET_INTERACTION;
F_TICK;
END_COMPONENT;

START_COMPONENT(HomeComponent, Actor);
BlockPos homePos;
BlockPos getHomePos() const;
void setHomePos(BlockPos const &);
F_INIT_FROM_DEFINITION;
F_ADDITIONAL_SAVED_DATA;
END_COMPONENT;

START_COMPONENT(InsomniaComponent, Mob);
Mob &mob;                  // 0
int timeSinceRest;         // 8
float days_until_insomnia; // 12
int ticksUntilInsomnia;    // 16
int getTicksUntilInsomnia() const;
bool hasInsomnia() const;
void restartTimer();
void setInsomnia(int);
F_INIT_FROM_DEFINITION;
F_ADDITIONAL_SAVED_DATA;
F_TICK;
END_COMPONENT;

START_COMPONENT(ClientAnimationComponent, Actor);
std::unique_ptr<std::vector<ActorAnimationControllerPlayer>> controllers;      // 0
std::unique_ptr<std::vector<ActorAnimationPlayer>> players;                    // 8
std::unique_ptr<std::unordered_map<HashedString, ModelPartLocator>> loctorMap; // 16
std::unique_ptr<RenderParams> params;                                          // 24
std::vector<std::unique_ptr<ClientAnimationComponent>> children;               // 32
ActorAnimationControllerPlayer &getAnimationControllerPlayer(std::string const &);
std::vector<ActorAnimationControllerPlayer> &getAnimationControllerPlayers(std::string const &);
ActorAnimationPlayer &getAnimationPlayer(unsigned);
int getAnimationPlayerIndex(HashedString);
std::vector<ActorAnimationPlayer> &getAnimationPlayers();
std::vector<std::unique_ptr<ClientAnimationComponent>> &getChildAnimationComponents();
ModelPartLocator *getLocator(HashedString const &);
std::unordered_map<HashedString, ModelPartLocator> &getLocators();
RenderParams &getRenderParams();
F_INIT_FROM_DEFINITION;
END_COMPONENT;

#undef START_COMPONENT
#undef START_COMPONENT_EXTENDED
#undef END_COMPONENT
#undef F_INIT_FROM_DEFINITION
#undef F_TICK
#undef F_RELOAD_COMPONENT
#undef F_GET_INTERACTION
#undef F_ADDITIONAL_SAVED_DATA