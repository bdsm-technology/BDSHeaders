#pragma once

#include "Abilities.h"
#include "ActorDefinition.h"
#include "Attribute.h"
#include "Components.h"
#include "Container.h"
#include "DataItem.h"
#include "NetworkIdentifier.h"
#include "types.h"
#include <nonstd/optional.hpp>

namespace entt {

enum struct InternalRegistryComponentFamily;

template <typename TFamily> struct Family {
  static unsigned long identifier;
  template <typename TComponent> static unsigned long family();
  template <typename TComponent> static unsigned long type();
};

template <typename... TS> struct SparseSet;
template <typename TEntity> struct SparseSet<TEntity> {
  struct Iterator {
    std::vector<TEntity> const *vec;
    std::size_t pos;
    Iterator(std::vector<TEntity> const &, std::size_t);
    Iterator &operator++();
    bool operator==(Iterator const &) const;
    TEntity operator*() const;
  };
  std::vector<TEntity> dense;
  std::vector<TEntity> sparse;
  SparseSet();

  static TEntity in_use();

  void construct(TEntity);
  bool fast(TEntity) const;
  bool has(TEntity) const;
  TEntity get(TEntity);
  Iterator begin() const;
  Iterator end() const;
  std::size_t extent() const;
  std::size_t size() const;

  virtual ~SparseSet();
  virtual void destroy(TEntity);
  virtual void reset();
};

template <typename TEntity, typename TComponent> struct SparseSet<TEntity, TComponent> : SparseSet<TEntity> {
  std::vector<TComponent> components;
  template <typename... PS> TComponent &construct(TEntity t, PS... ps) {
    SparseSet<TEntity>::construct(t);
    components.emplace_back(t, ps...);
    return components.back();
  }
  TComponent const &get(TEntity) const;

  virtual ~SparseSet() override;
  virtual void destroy(TEntity) override;
  virtual void reset() override;
};

template <typename TEntity, typename... PS> struct View {
  struct Iterator {
    std::array<SparseSet<TEntity>, sizeof...(PS)> data;
    std::size_t pos;
    typename SparseSet<TEntity>::Iterator _begin;
    typename SparseSet<TEntity>::Iterator _end;
    Iterator(std::array<SparseSet<TEntity>, sizeof...(PS)>, std::size_t, typename SparseSet<TEntity>::Iterator, typename SparseSet<TEntity>::Iterator);

    bool operator!=(Iterator const &) const;
    bool operator==(Iterator const &) const;
    bool valid() const;
    Iterator &operator++();
    TEntity operator*(Iterator const &)const;
  };

  std::tuple<PS...> data; // 0
  void *unk24;            // 24
  char mem32[16];         // 32

  View(PS...);
  void reset();

  Iterator begin() const;
  Iterator end() const;

  template <typename TComponent> TComponent const &get(TEntity) const;
  template <typename TComponent> TComponent &get(TEntity);
};

template <typename TEntity> struct Registry {
  template <typename TComponent> struct Pool : SparseSet<TEntity, TComponent> {
    std::vector<std::pair<entt::SparseSet<EntityId> *, bool (entt::Registry<EntityId>::*)(EntityId) const>> data;
    Pool();
    template <typename... PS> TComponent &construct(Registry &, TEntity t, PS... ps);

    virtual ~Pool() override;
    virtual void destroy(TEntity) override;
  };
  struct Attachee;
  std::vector<std::unique_ptr<SparseSet<TEntity>>> entset;  // 0
  std::vector<std::unique_ptr<SparseSet<TEntity>>> entset2; // 24
  std::vector<std::unique_ptr<Attachee>> attachees;         // 48
  std::vector<TEntity> entityIds;                           // 72
  void *unk96;                                              // 96
  char mem104[4];                                           // 104

  Registry();

  template <typename TComponent, typename... PS> TComponent &assign(TEntity, PS...);
  template <typename TComponent> SparseSet<TEntity> const &assure();
  EntityId create();
  void destroy(TEntity);
  template <typename Func> void each(Func func) const {
    if (unk96) {
      for (std::size_t i = entityIds.size(); i != 0; --i) {
        EntityId temp{ i - 1 };
        auto id = entityIds[temp];
        EntityId target{ ((std::size_t)id) & 0xFFFFF };
        if (temp == target) func(id);
      }
    } else {
      for (std::size_t i = entityIds.size(); i != 0; --i) {
        auto id = entityIds[i];
        func(id);
      }
    }
  }

  template <typename TComponent> TComponent const &get(TEntity) const;
  template <typename TComponent> bool has(TEntity) const;
  template <typename TComponent> bool mangled(TEntity) const;
  template <typename TComponent> SparseSet<TEntity> const &pool() const;
  template <typename TComponent> bool reset() const;

  bool valid(TEntity) const;
  template <typename... PS> View<TEntity, PS...> view() const;

  ~Registry();
};

} // namespace entt

struct EntityRegistry;
struct EntityRegistryShareable;

struct EntityContext {
  EntityRegistry &reg;
  EntityId id;
  EntityContext(EntityRegistry &, EntityId);
  template <typename TComponent> TComponent const &addComponent();
  template <typename TComponent> TComponent const &getOrAddComponent();
  template <typename TComponent> TComponent const &removeComponent();
  template <typename TComponent> TComponent const *tryGetComponent();

  bool operator!=(EntityContext const &) const;
  bool operator==(EntityContext const &) const;
};

struct EntityContextStackRef : EntityContext {
  EntityContextStackRef(EntityRegistryShareable &, EntityId);
  // getWeakRef

  bool operator!=(EntityContextStackRef const &) const;
  bool operator==(EntityContextStackRef const &) const;
};

struct EntityRegistry {
  template <typename TFlagComponent, typename TActorComponent, typename TActorFlagComponent> struct View {
    EntityRegistry &reg;
    View(EntityRegistry &);
    template <typename Func> void each(Func);
  };

  entt::Registry<EntityId> &ereg;
  EntityRegistry(entt::Registry<EntityId> &);

  bool isValidEntity(EntityContext const &);
  template <typename TComponent> void removeComponentFromAllEntities();
  template <typename TFlagComponent, typename TActorComponent, typename TActorFlagComponent, typename F> void viewEach(F);
};

struct EntityRegistryShareable : EntityRegistry, std::enable_shared_from_this<EntityRegistryShareable> {
  EntityRegistryShareable(entt::Registry<EntityId> &);
  std::weak_ptr<EntityRegistryShareable> getWeakRef();
  ~EntityRegistryShareable();
};

struct EntityRegistryOwned : EntityRegistryShareable {
  entt::Registry<EntityId> real;
  EntityRegistryOwned();
  EntityContextStackRef createEntity();
  void destroyEntity(EntityContextStackRef);
  ~EntityRegistryOwned();
};

struct IEntityRegistryOwner {
  virtual ~IEntityRegistryOwner();
  virtual EntityRegistry &getEntityRegistry() = 0;
};

struct OwnerStorageEntity {
  struct EntityContextOwned {
    EntityRegistryOwned &owned;
    EntityContextStackRef &context;
    EntityContextOwned(EntityRegistryOwned &);
    void destroy();
  };

  nonstd::optional<EntityContextOwned> owned;
  OwnerStorageEntity &operator=(OwnerStorageEntity const &);
  EntityContextOwned const &_getStackRef() const;
  EntityContextOwned &_getStackRef();
  bool _hasValue() const;
  void _remake(EntityRegistryOwned &);
  void _reset();
  ~OwnerStorageEntity();
};

struct WeakStorageEntity;

struct EntityRefTraits {
  using ref       = OwnerStorageEntity;
  using ref_value = typename OwnerStorageEntity::EntityContextOwned;
  using weak_ref  = WeakStorageEntity;
};

struct EntityRegistryRefTraits {
  using ref       = OwnerStorageSharePtr<EntityRegistryOwned>;
  using ref_value = EntityRegistry;
  using weak_ref  = WeakStorageSharePtr<EntityRegistryShareable>;
};

struct StackResultStorageEntity {
  nonstd::optional<EntityContextStackRef> ref;

  EntityContextStackRef const &_getStackRef() const;
  EntityContextStackRef &_getStackRef();
  bool _hasValue() const;
};

struct WeakStorageEntity {
  enum struct EmptyInit;
  enum struct VariadicInit;
  WeakRefT<EntityRegistryRefTraits> reft;
  nonstd::optional<EntityId> eid;

  WeakStorageEntity(OwnerStorageEntity const &);
  WeakStorageEntity(StackResultStorageEntity const &);
  WeakStorageEntity(EmptyInit);
  WeakStorageEntity(VariadicInit, EntityContextStackRef const &);

  WeakStorageEntity &operator=(OwnerStorageEntity const &);
  WeakStorageEntity &operator=(StackResultStorageEntity const &);

  bool _isSet() const;
  void _reset();
};

struct alignas(8) ActorTerrainInterlockData {
  enum struct VisibilityState : char {};
  VisibilityState state;                                            // 0
  std::chrono::time_point<std::chrono::system_clock> creation_time; // 8
  bool b16;                                                         // 16
  void setCreationTime();
  void setVisibilityState(VisibilityState);
};

struct RopeSystem;
struct SpatialActorNetworkData;
struct ActorDefinitionDiffList;
struct ActionQueue;
struct SetActorDataPacket;
struct ActorLocation;
struct AddActorPacket;
struct UIProfanityContext;
struct ActorLink;
struct ChangeDimensionPacket;
struct ActorPos;
struct LootTable;
struct DistanceSortedActor;
struct MobEffect;

#define DEF_COMPONENT(name)                                                                                                                                                                            \
  std::unique_ptr<name##Component> m##name;                                                                                                                                                            \
  name##Component &get##name##Component() const

struct alignas(8) Actor {
  enum struct InitializationMethod : char {};
  OwnerPtrT<EntityRefTraits> entityRef;                        // 8
  alignas(8) bool unk32;                                       // 32
  VariantParameterList vplist;                                 // 40
  DimensionId dim;                                             // 136
  bool in_world;                                               // 140
  void *unk144;                                                // 144
  std::unique_ptr<ActorDefinitionDescriptor> mActorDefinition; // 152
  ActorUniqueID auid;                                          // 160
  std::shared_ptr<RopeSystem> ropeSystem;                      // 168
  Vec2 rot;                                                    // 184
  Vec2 rotInterpolation;                                       // 192
  int swim_amount;                                             // 200
  int unk204;                                                  // 204
  ChunkPos chunk;                                              // 208
  Vec3 pos;                                                    // 216
  char filler228[6];                                           // 228
  int unk236;                                                  // 236
  SynchedActorData data;                                       // 240
  std::unique_ptr<SpatialActorNetworkData> actorNetworkData;   // 272
  Vec3 pos2;                                                   // 280
  char filler292[4];                                           // 292
  int unk296;                                                  // 296
  bool b300;                                                   // 300
  bool b301;                                                   // 301
  bool b302;                                                   // 302
  bool b303;                                                   // 303
  bool b304;                                                   // 304
  Block &blk;                                                  // 312
  int un_free;                                                 // 320
  bool b324;                                                   // 324
  bool b325;                                                   // 325
  Color color;                                                 // 328
  Color color2;                                                // 344
  int unk360;                                                  // 360
  int unk364;                                                  // 364
  int unk368;                                                  // 368
  char filler372[12];                                          // 372
  std::vector<AABB> bbox;                                      // 384
  int unk408;                                                  // 408
  int riding_height;                                           // 412
  int unk416;                                                  // 416
  int unk420;                                                  // 420
  int unk424;                                                  // 424
  int unk428;                                                  // 428
  int unk432;                                                  // 432
  int unk436;                                                  // 436
  int unk440;                                                  // 440
  int unk444;                                                  // 444
  bool stuck_in_web;                                           // 448
  bool immpbile;                                               // 449
  bool in_water;                                               // 450
  bool entered_water;                                          // 451
  bool b452;                                                   // 452
  Vec2 v456;                                                   // 456
  Vec3 v464;                                                   // 464
  Vec3 v476;                                                   // 476
  Vec3 v488;                                                   // 488
  Vec3 v500;                                                   // 500
  Vec3 v512;                                                   // 512
  bool b524;                                                   // 524
  int unk528;                                                  // 528
  int heal_effects;                                            // 532
  int unk536;                                                  // 536
  bool b540;                                                   // 540
  bool maek_hurt;                                              // 541
  bool was_hurt;                                               // 542
  bool b543;                                                   // 543
  bool b544;                                                   // 544
  int fire_count;                                              // 548
  int unk552;                                                  // 552
  int unk556;                                                  // 556
  bool on_magma;                                               // 560
  int unk564;                                                  // 564
  bool b568;                                                   // 568
  bool b569;                                                   // 569
  int unk572;                                                  // 572
  BlockPos bpos;                                               // 576
  int portalEntranceAxis;                                      // 588
  int unk592;                                                  // 592
  std::vector<ActorUniqueID> riders;                           // 600
  std::vector<ActorUniqueID> riderToRemove;                    // 624
  ActorUniqueID ridingActor;                                   // 648
  ActorUniqueID auid656;                                       // 656
  bool inheritRotationWhenRiding;                              // 664
  bool has_rider;                                              // 665
  void *unk672;                                                // 672
  bool b680;                                                   // 680
  bool b681;                                                   // 681
  bool b682;                                                   // 682
  bool b683;                                                   // 683
  bool teleported;                                             // 684
  bool b685;                                                   // 685
  void *unk688;                                                // 688
  Random randSeed;                                             // 696
  ActorUniqueID auid3216;                                      // 3216
  bool b3224;                                                  // 3224
  bool initialized;                                            // 3225
  Level *blocksource;                                          // 3232
  Dimension *dimension;                                        // 3240
  Level *level;                                                // 3248
  bool changed;                                                // 3256
  bool removed;                                                // 3257
  bool global;                                                 // 3258
  bool autonomous;                                             // 3259
  std::unique_ptr<BaseAttributeMap> baseAttributeMap;          // 3264
  DEF_COMPONENT(AddRider);                                     // 3272
  DEF_COMPONENT(Ageable);                                      // 3280
  DEF_COMPONENT(Angry);                                        // 3288
  DEF_COMPONENT(Balloonable);                                  // 3296
  DEF_COMPONENT(Balloon);                                      // 3304
  DEF_COMPONENT(Behavior);                                     // 3312
  DEF_COMPONENT(Breathable);                                   // 3320
  DEF_COMPONENT(Breedable);                                    // 3328
  DEF_COMPONENT(Bribeable);                                    // 3336
  DEF_COMPONENT(Container);                                    // 3344
  DEF_COMPONENT(DamageOverTime);                               // 3352
  DEF_COMPONENT(Explode);                                      // 3360
  DEF_COMPONENT(Tameable);                                     // 3368
  DEF_COMPONENT(LookAt);                                       // 3376
  DEF_COMPONENT(Projectile);                                   // 3384
  DEF_COMPONENT(Teleport);                                     // 3392
  DEF_COMPONENT(MountTaming);                                  // 3340
  DEF_COMPONENT(Timer);                                        // 3408
  DEF_COMPONENT(Rideable);                                     // 3416
  DEF_COMPONENT(Shooter);                                      // 3424
  DEF_COMPONENT(Interact);                                     // 3432
  DEF_COMPONENT(Equippable);                                   // 3440
  DEF_COMPONENT(Healable);                                     // 3448
  DEF_COMPONENT(ScaleByAge);                                   // 3456
  DEF_COMPONENT(TargetNearby);                                 // 3464
  DEF_COMPONENT(Transformation);                               // 3472
  DEF_COMPONENT(Boostable);                                    // 3480
  DEF_COMPONENT(RailMovement);                                 // 3488
  DEF_COMPONENT(DamageSensor);                                 // 3496
  DEF_COMPONENT(Leashable);                                    // 3504
  DEF_COMPONENT(Hopper);                                       // 3512
  DEF_COMPONENT(CommandBlock);                                 // 3520
  DEF_COMPONENT(Nameable);                                     // 3528
  DEF_COMPONENT(RailActivator);                                // 3536
  DEF_COMPONENT(Npc);                                          // 3544
  DEF_COMPONENT(TripodCamera);                                 // 3552
  DEF_COMPONENT(Boss);                                         // 3560
  DEF_COMPONENT(Peek);                                         // 3568
  DEF_COMPONENT(Tradeable);                                    // 3576
  DEF_COMPONENT(AgentCommand);                                 // 3584
  DEF_COMPONENT(Shareable);                                    // 3592
  DEF_COMPONENT(SpawnActor);                                   // 3600
  DEF_COMPONENT(Physics);                                      // 3608
  DEF_COMPONENT(TickWorld);                                    // 3616
  DEF_COMPONENT(Dance);                                        // 3624
  DEF_COMPONENT(Flocking);                                     // 3632
  DEF_COMPONENT(Bucketable);                                   // 3640
  DEF_COMPONENT(Home);                                         // 3648
  DEF_COMPONENT(Insomnia);                                     // 3656
  DEF_COMPONENT(ClientAnimation);                              // 3664
  AABBShapeComponent aabbShape;                                // 3672
  StateVectorComponent stateVector;                            // 3708
  ActorUniqueID targetActor;                                   // 3744
  bool persistent;                                             // 3752
  bool b3753;                                                  // 3753
  float restrictRadius;                                        // 3756
  BlockPos restrictCenter;                                     // 3760
  ActorUniqueID lovePartberId;                                 // 3776
  std::vector<MobEffectInstance> effects;                      // 3784
  bool b3808;                                                  // 3808
  ActorRuntimeID runtimeID;                                    // 3816
  Color hurtColor;                                             // 3824
  bool enforce_rider_rotation_limit;                           // 3840
  std::unique_ptr<ActorDefinitionDiffList> difflist;           // 3848
  bool b3856;                                                  // 3856
  int unk3860;                                                 // 3860
  int unk3864;                                                 // 3864
  std::string s3872;                                           // 3872
  bool b3904;                                                  // 3904
  float unk3908;                                               // 3908
  bool safeToSleepNear;                                        // 3912
  ActorTerrainInterlockData terrainInterlockData;              // 3920
  SimpleContainer armorContainer;                              // 3944 size: 4
  char filler4216[16];                                         // 4216
  SimpleContainer handContainer;                               // 4232 size: 2
  char filler4504[8];                                          // 4504
  bool b4512;                                                  // 4512
  std::vector<AABB> aabbs;                                     // 4520
  char filler4544[4552 - 4544];                                // 4544
  bool collidable_mob_near;                                    // 4552
  bool collidable_mob;                                         // 4553
  bool chained_damage_effect;                                  // 4554
  bool b4555;                                                  // 4555
  bool b4556;                                                  // 4556
  bool b4557;                                                  // 4557
  bool b4558;                                                  // 4558
  bool b4559;                                                  // 4559
  bool b4560;                                                  // 4560
  bool b4561;                                                  // 4561
  int unk4564;                                                 // 4564
  bool experimental;                                           // 4568
  std::unique_ptr<ActionQueue> action_queue;                   // 4576
  MolangVariableMap varmap;                                    // 4584

  static int DAMAGE_NEARBY_MOBS_DURATION;
  static int TOTAL_AIR_SUPPLY;

  Actor(ActorDefinitionGroup *, ActorDefinitionIdentifier const &);
  Actor(Level &);

  bool operator==(Actor &);

  Vec3 buildForward() const;
  void burn(int, bool);
  Vec3 calcCenterPos() const;
  int calculateAttackDamage(Actor &);
  bool canClimb() const;
  bool canFly() const;
  bool canMate() const;
  bool canOpenContainer(Player &) const;
  void checkEntityOnewayCollision(BlockSource &, BlockPos const &);
  void clearActionQueue();
  void clearRestriction();
  void createBehaviorComponent();
  UpdateEquipPacket createUpdateEquipPacket(int);
  UpdateTradePacket createUpdateTradePacket(int);
  float distanceSqrToBlockPosCenter() const;
  float distanceTo(Actor const &) const;
  float distanceTo(Vec3 const &) const;
  float distanceToSqr(Actor const &) const;
  float distanceToSqr(Vec3 const &) const;
  void doEnchantDamageEffects(Mob &, Mob &);
  void dropLeash(bool, bool);
  void dropTowards(ItemInstance const &, Vec3);
  void enableAutoSendPosRot(bool);
  bool enforceRiderRotationLimit();
  std::vector<DistanceSortedActor> fetchNearbyActorsSorted(Vec3 const &, ActorType);
  AABBShapeComponent const &getAABBShapeComponent() const &;
  std::size_t getActiveEffectCount() const;
  int getAge() const;
  short getAirSupply() const;
  std::vector<MobEffectInstance> const &getAllEffects() const;
  bool getAmbientSoundIntervalMin(float &) const;
  bool getAmbientSoundIntervalRange(float &) const;
  BaseAttributeMap const &getAttributes();
  BaseAttributeMap &getMutableAttributes();
  BlockPos getBlockTarget() const;
  Vec3 getCenter() const;
  bool getChainedDamageEffects() const;
  bool getCollidableMob();
  bool getCollidableMobNear();
  PaletteColor getColor();
  PaletteColor getColor2();
  int getControllingSeat();
  float getCurrentSwimAmount() const;
  Dimension &getDimension() const;
  Dimension const &getDimensionConst() const;
  MobEffectInstance const *getEffect(MobEffect const &);
  EntityContext &getEntity();
  SynchedActorData const &getEntityData() const;
  SynchedActorData &getEntityData();
  ActorTerrainInterlockData &getEntityTerrainInterlockData();
  bool getFirstAvailableSeatPos(Actor &, Vec3 &) const;
  Actor *getFirstRider() const;
  int getHealth() const;
  Color &getHurtColor() const;
  int getHurtDir() const;
  int getHurtTime() const;
  Vec3 getInterpolatedPosition(float) const;
  Vec3 getInterpolatedPosition2(float) const;
  Vec2 getInterpolatedRotation(float) const;
  int getJumpDuration() const;
  ActorUniqueID getLeashHolder() const;
  Level const &getLevel() const;
  Level &getLevel();
  std::vector<ActorLink> getLinks() const;
  int getMarkVariant() const;
  int getMaxHealth() const;
  MolangVariableMap &getMolangVariables();
  bool getNextActionEvent();
  ItemInstance &getOffhandSlot() const;
  Actor &getOwner() const;
  ActorUniqueID &getOwnerId() const;
  Player &getPlayerOwner() const;
  int getPortalEntranceAxis() const;
  float getRadius() const;
  Vec3 getRandomPointInAABB(Random &);
  BlockSource &getRegion();
  BlockSource const &getRegionConst() const;
  Actor &getRenderLeashHolder();
  BlockPos getRestrictCenter() const;
  float getRestrictRadius() const;
  Actor *getRide() const;
  ActorRuntimeID getRideRuntimeID() const;
  Actor *getRideRoot() const;
  int getRiderIndex() const;
  Vec2 getRotation() const;
  ActorRuntimeID getRuntimeID() const;
  std::vector<ItemInstance> getSlotItems();
  SpatialActorNetworkData &getSpatialNetworkData();
  SpatialActorNetworkData const &getSpatialNetworkData() const;
  float getSpeedInMetersPerSecond() const;
  StateVectorComponent const &getStateVectorComponent() const;
  StateVectorComponent &getStateVectorComponentNonConst();
  bool getStatusFlag(ActorFlags) const;
  int getStrength() const;
  int getStrengthMax() const;
  int getStructuralIntegrity() const;
  float getSwimAmount() const;
  Actor *getTarget();
  ActorUniqueID getTargetId();
  bool getTempted() const;
  short getTotalAirSupply() const;
  MerchantRecipeList *getTradeOffers();
  Player *getTradingPlayer() const;
  ActorUniqueID getUniqueID() const;
  int getVariant() const;
  Vec3 getViewVector() const;
  Vec3 getViewVector2() const;
  bool hasAnyEffects() const;
  bool hasAnyVisibleEffects() const;
  bool hasAttributeBuff(AttributeBuffType) const;
  bool hasCategory(ActorCategory) const;
  bool hasDefinitionGroup(std::string const &) const;
  bool hasEffect(MobEffect const &) const;
  bool hasEnoughFoodToBreed();
  bool hasEntity() const;
  bool hasExcessFood();
  bool hasFamily(Util::HashString const &) const &;
  bool hasLevel() const;
  bool hasPriorityAmmunition() const;
  bool hasRestriction() const;
  bool hasRider() const;
  bool hasRuntimeID() const;
  bool hasSaddle() const;
  bool hasTeleported() const;
  bool hasTickingArea() const;
  bool hasTotemEquipped() const;
  bool hasType(ActorType) const;
  bool hasUniqueID() const;
  void healEffects();
  bool hurt(ActorDamageSource const &, int, bool, bool);
  void initEntity(EntityRegistryOwned &);
  void initParams(VariantParameterList &);
  bool isActionQueueEmpty() const;
  bool isAngry() const;
  bool isAutonomous() const;
  bool isBaby() const;
  bool isBribed() const;
  bool isCharged() const;
  bool isChested() const;
  bool isClimbing() const;
  bool isControlledByLocalInstance() const;
  bool isDancing() const;
  bool isDebugging() const;
  bool isExperimental() const;
  bool isFireImmune() const;
  bool isGlobal() const;
  bool isIgnited() const;
  bool isInClouds() const;
  bool isInLove() const;
  bool isInWorld() const;
  bool isInitialized() const;
  bool isInsideBorderBlock(float) const;
  bool isInsidePortal() const;
  bool isLeashed() const;
  bool isMoving() const;
  bool isPersistent() const;
  bool isPowered() const;
  bool isRegionValid() const;
  bool isRemoved() const;
  bool isResting() const;
  bool isRider(Actor &) const;
  bool isRider(ActorUniqueID const &) const;
  bool isRiding() const;
  bool isSafeToSleepNear() const;
  bool isSheared() const;
  bool isSitting() const;
  bool isSpawnableInCreative() const;
  bool isStackable() const;
  bool isStanding() const;
  bool isSwimmer() const;
  bool isSwimming() const;
  bool isTame() const;
  bool isTickingEntity() const;
  bool isTrading() const;
  bool isUnderWaterfall() const;
  bool isWASDControlled() const;
  bool isWalker() const;
  bool isWithinRestriction(BlockPos const &) const;
  bool isWithinRestriction() const;
  ActorUniqueID lovePartnerId() const;
  void migrateUniqueID(ActorUniqueID);
  void moveBBs(Vec3 const &);
  void moveChunks();
  void moveTo(Vec3 const &, Vec2 const &);
  void onChunkDiscarded();
  bool onLadder() const;
  void onOnewayCollision(AABB const &);
  void onOnewayCollision(BlockPos);
  void positionAllRiders();
  bool pullInEntity(Actor &);
  void pushBackActionEventToActionQueue(ActionEvent);
  void reload();
  void removeAllEffects();
  void removeAllRiders(bool, bool);
  void removeDefinitionGroup(std::string const &);
  void removeEffect(int);
  void removeEffectNoUpdate(int);
  void removeEffectParticles();
  void removeEntity();
  void resetRegion();
  void restrictTo(BlockPos const &, float);
  void saveEntityFlags(CompoundTag &);
  void saveLinks();
  void sendMotionToServer();
  void serializationSetHealth();
  void setAABBDim(Vec2 const &);
  void setAutonomous(bool);
  void setBaseDefinition(std::string const &, bool, bool);
  void setBlockTarget(BlockPos const &);
  void setCanClimb(bool);
  void setCanFly(bool);
  void setChainedDamageEffects(bool);
  void setChanged();
  void setCharged(bool);
  void setClimbing(bool);
  void setCollidableMob(bool);
  void setCollidableMobNear(bool);
  void setColor(PaletteColor);
  void setColor2(PaletteColor);
  void setControllingSeat(int);
  void setDacing(bool);
  void setEnchanted(bool);
  void setEnforceRiderRotationLimit(bool);
  void setEquipFromPacket(UpdateEquipPacket const &);
  void setExperimental(bool);
  void setGlobal(bool);
  void setHurtDir(int);
  void setHurtTime(int);
  void setInLove(Actor *);
  void setInheritRotationWhenRiding(bool);
  template <typename T> void setInitialParameter(FilterSubject, T *);
  void setInvisible(bool);
  void setJumpDuration(int);
  void setLastHitBB(Vec3 const &, Vec3 const &);
  void setLeashHolder(ActorUniqueID);
  void setLimitedLife(int);
  void setMarkVariant(int);
  void setMoving(bool);
  void setOffersFromPacket(UpdateTradePacket const &);
  void setPersistent();
  void setPosDirectLegacy(Vec3 const &);
  void setPosPrev2(Vec3 const &);
  void setPowered(bool);
  void setPreviousPosRot(Vec3 const &, Vec2 const &);
  void setRegion(BlockSource &);
  void setResting(bool);
  void setRuntimeID(ActorRuntimeID);
  void setSaddle(bool);
  void setSeatDescription(Vec3 const &, SeatDescription const &);
  void setShakeTime(int);
  void setStatusFlag(ActorFlags, bool);
  void setStrength(int);
  void setStrengthMax(int);
  void setStructuralIntegrity(int);
  void setSwimmer(bool);
  void setTempted(bool);
  void setTradingPlayer(Player *);
  void setUniqueID(ActorUniqueID);
  void setVariant(int);
  void setWASDControlled(bool);
  void setWalker(bool);
  bool shouldOrphan(BlockSource &);
  void spawnBalloonPopParticles();
  void spawnDeathParticles();
  void spawnDustParticles();
  void spawnEatParticles();
  void spawnTamingParticles();
  void teleportRidersTo(Vec3 const &, int, int);
  bool testForCollidableMobs(BlockSource &, AABB const &, std::vector<AABB, std::allocator<AABB>> &);
  bool testForEntityStacking(BlockSource &, AABB const &, std::vector<AABB, std::allocator<AABB>> &);
  bool tick(BlockSource &);
  void transferTickingArea(Dimension &);
  void updateBBFromDescription();
  void updateDescription();
  void updateInBubbleColumnState();
  void updateInvisibilityStatus();
  void updateTickingData();
  bool wantsMoreFood();
  void normalizeRotationWithDependency(float &, float &);

  virtual void reloadHardcoded(Actor::InitializationMethod, VariantParameterList const &);       // 0
  virtual void reloadHardcodedClient(Actor::InitializationMethod, VariantParameterList const &); // 8
  virtual void initializeComponents(Actor::InitializationMethod, VariantParameterList const &);  // 16
  virtual void reloadComponents(Actor::InitializationMethod, VariantParameterList const &);      // 24
  virtual bool hasComponent(Util::HashString const &) const;                                     // 32
  virtual bool hasInventory() const;                                                             // 40
  virtual Container &getInventory() const;                                                       // 48
  virtual ~Actor();                                                                              // 56, 64
  virtual void reset();                                                                          // 72
  virtual ActorType getOwnerEntityType();                                                        // 80
  virtual void remove();                                                                         // 88
  virtual void setPos(Vec3 const &);                                                             // 96
  virtual Vec3 const &getPos() const;                                                            // 104
  virtual Vec3 const &getPosOld() const;                                                         // 112
  virtual Vec3 getPosExtrapolated(float) const;                                                  // 120
  virtual Vec3 getAttachPos(ActorLocation, float) const;                                         // 128
  virtual Vec3 getFiringPos() const;                                                             // 136
  virtual void setRot(Vec2 const &);                                                             // 144
  virtual void move(Vec3 const &);                                                               // 152
  virtual Vec3 getInterpolatedRidingPosition(float) const;                                       // 160
  virtual Vec2 getInterpolatedBodyRot(float) const;                                              // 168
  virtual bool checkBlockCollisions(AABB const &);                                               // 176
  virtual bool checkBlockCollisions();                                                           // 184
  virtual bool breaksFallingBlocks() const;                                                      // 192
  virtual void moveRelative(float, float, float, float);                                         // 200
  virtual void teleportTo(Vec3 const &, bool, int, int);                                         // 208
  virtual void tryTeleportTo(Vec3 const &, bool, bool, int, int);                                // 216
  virtual void lerpTo(Vec3 const &, Vec2 const &, int);                                          // 224
  virtual void lerpMotion(Vec3 const &);                                                         // 232
  virtual std::unique_ptr<AddActorPacket> getAddPacket();                                        // 240
  virtual void normalTick();                                                                     // 248
  virtual void baseTick();                                                                       // 256
  virtual void rideTick();                                                                       // 264
  virtual void positionRider(Actor &, float) const;                                              // 272
  virtual float getRidingHeight();                                                               // 280
  virtual bool startRiding(Actor &);                                                             // 288
  virtual void addRider(Actor &);                                                                // 296
  virtual void flagRiderToRemove(Actor &);                                                       // 304
  virtual void getExitTip(std::string const &, InputMode) const;                                 // 312
  virtual bool intersects(Vec3 const &, Vec3 const &) const;                                     // 320
  virtual void isFree(Vec3 const &, float);                                                      // 328
  virtual void isFree(Vec3 const &);                                                             // 336
  virtual bool isInWall() const;                                                                 // 344
  virtual bool isInvisible() const;                                                              // 352
  virtual bool canShowNameTag() const;                                                           // 360
  virtual bool canExistInPeaceful() const;                                                       // 368
  virtual void setNameTagVisible(bool);                                                          // 376
  virtual std::string const &getNameTag() const;                                                 // 384
  virtual std::string getFormattedNameTag() const;                                               // 392
  virtual std::string filterFormattedNameTag(UIProfanityContext const &);                        // 400
  virtual void setNameTag(std::string const &);                                                  // 408
  virtual bool getAlwaysShowNameTag() const;                                                     // 416
  virtual void setScoreTag(std::string const &);                                                 // 424
  virtual std::string const &getScoreTag() const;                                                // 432
  virtual bool isInWater() const;                                                                // 440
  virtual bool hasEnteredWater() const;                                                          // 448
  virtual bool isImmersedInWater() const;                                                        // 456
  virtual bool isInWaterOrRain() const;                                                          // 464
  virtual bool isInLava() const;                                                                 // 472
  virtual bool isUnderLiquid(MaterialType) const;                                                // 480
  virtual bool isOverWater() const;                                                              // 488
  virtual void makeStuckInWeb();                                                                 // 496
  virtual float getCameraOffset() const;                                                         // 504
  virtual float getShadowHeightOffs();                                                           // 512
  virtual float getShadowRadius() const;                                                         // 520
  virtual Vec3 getHeadLookVector(float);                                                         // 528
  virtual bool canSeeInvisible() const;                                                          // 536
  virtual bool canSee(Actor const &) const;                                                      // 544
  virtual bool canSee(Vec3 const &) const;                                                       // 552
  virtual bool isSkyLit(float);                                                                  // 560
  virtual float getBrightness(float) const;                                                      // 568
  virtual bool interactPreventDefault();                                                         // 576
  virtual void playerTouch(Player &);                                                            // 584
  virtual void push(Actor &, bool);                                                              // 592
  virtual void push(Vec3 const &);                                                               // 600
  virtual void onAboveBubbleColumn(bool);                                                        // 608
  virtual void onInsideBubbleColumn(bool);                                                       // 616
  virtual void partialPush(Vec3 const &);                                                        // 624
  virtual bool isImmobile() const;                                                               // 632
  virtual bool isSilent();                                                                       // 640
  virtual bool isPickable();                                                                     // 648
  virtual bool isFishable() const;                                                               // 656
  virtual bool isPushable() const;                                                               // 664
  virtual bool isPushableByPiston() const;                                                       // 672
  virtual bool isShootable();                                                                    // 680
  virtual bool isSneaking() const;                                                               // 688
  virtual bool isAlive() const;                                                                  // 696
  virtual bool isOnFire() const;                                                                 // 704
  virtual bool isOnMagma() const;                                                                // 712
  virtual bool isCreativeModeAllowed();                                                          // 720
  virtual bool isSurfaceMob() const;                                                             // 728
  virtual bool isTargetable() const;                                                             // 736
  virtual bool canAttack(Actor *, bool) const;                                                   // 744
  virtual void setTarget(Actor *);                                                               // 752
  virtual bool findAttackTarget();                                                               // 760
  virtual bool isValidTarget(Actor *) const;                                                     // 768
  virtual bool attack(Actor &);                                                                  // 776
  virtual void adjustDamageAmount(int &) const;                                                  // 784
  virtual bool onRiderUseItem(ItemInstance &, Player &);                                         // 792
  virtual void setOwner(ActorUniqueID);                                                          // 800
  virtual void setSitting(bool);                                                                 // 808
  virtual void onTame();                                                                         // 816
  virtual void onFailedTame();                                                                   // 824
  virtual void onMate(Mob &);                                                                    // 832
  virtual int getInventorySize() const;                                                          // 840
  virtual int getEquipSlots() const;                                                             // 848
  virtual int getChestSlots() const;                                                             // 856
  virtual void setStanding(bool);                                                                // 864
  virtual bool canPowerJump() const;                                                             // 872
  virtual void setCanPowerJump(bool);                                                            // 880
  virtual bool isEnchanted() const;                                                              // 888
  virtual void rideJumped();                                                                     // 896
  virtual void rideLanded(Vec3 const &, Vec3 const &);                                           // 904
  virtual bool shouldRender() const;                                                             // 912
  virtual bool isInvulnerableTo(ActorDamageSource const &) const;                                // 920
  virtual void animateHurt();                                                                    // 928
  virtual bool doFireHurt(int);                                                                  // 936
  virtual void onLightningHit();                                                                 // 944
  virtual void onBounceStarted(BlockPos const &, Block const &);                                 // 952
  virtual void feed(int);                                                                        // 960
  virtual void handleEntityEvent(ActorEvent, int);                                               // 968
  virtual void getPickRadius();                                                                  // 976
  virtual void spawnAtLocation(int, int);                                                        // 984
  virtual void spawnAtLocation(int, int, float);                                                 // 992
  virtual void spawnAtLocation(Block const &, int);                                              // 1000
  virtual void spawnAtLocation(Block const &, int, float);                                       // 1008
  virtual void spawnAtLocation(ItemInstance const &, float);                                     // 1016
  virtual void despawn();                                                                        // 1024
  virtual void killed(Actor &);                                                                  // 1032
  virtual void awardKillScore(Actor &, int);                                                     // 1040
  virtual void setArmor(ArmorSlot, ItemInstance const &);                                        // 1048
  virtual ItemInstance const &getArmor(ArmorSlot) const;                                         // 1056
  virtual void setEquippedSlot(ArmorSlot, int, int);                                             // 1064
  virtual void setEquippedSlot(ArmorSlot, ItemInstance const &);                                 // 1072
  virtual ItemInstance const &getCarriedItem() const;                                            // 1080
  virtual void setCarriedItem(ItemInstance const &);                                             // 1088
  virtual void setOffhandSlot(ItemInstance const &);                                             // 1096
  virtual ItemInstance const &getEquippedTotem() const;                                          // 1104
  virtual bool consumeTotem();                                                                   // 1112
  virtual bool save(CompoundTag &);                                                              // 1120
  virtual bool saveWithoutId(CompoundTag &);                                                     // 1128
  virtual bool load(CompoundTag const &);                                                        // 1136
  virtual bool loadLinks(CompoundTag const &, std::vector<ActorLink> &);                         // 1144
  virtual ActorType getEntityTypeId() const = 0;                                                 // 1152
  virtual bool acceptClientsideEntityData(Player &, SetActorDataPacket const &);                 // 1160
  virtual void *queryEntityRenderer();                                                           // 1168
  virtual ActorUniqueID getSourceUniqueID() const;                                               // 1176
  virtual void setOnFire(int);                                                                   // 1184
  virtual AABB getHandleWaterAABB() const;                                                       // 1192
  virtual void handleInsidePortal(BlockPos const &);                                             // 1200
  virtual int getPortalCooldown() const;                                                         // 1208
  virtual int getPortalWaitTime() const;                                                         // 1216
  virtual DimensionId getDimensionId() const;                                                    // 1224
  virtual bool canChangeDimensions() const;                                                      // 1232
  virtual void changeDimension(DimensionId, bool);                                               // 1240
  virtual void changeDimension(ChangeDimensionPacket const &);                                   // 1248
  virtual ActorUniqueID getControllingPlayer() const;                                            // 1256
  virtual void checkFallDamage(float, bool);                                                     // 1264
  virtual void causeFallDamage(float);                                                           // 1272
  virtual void handleFallDistanceOnServer(float, bool);                                          // 1280
  virtual void playSynchronizedSound(LevelSoundEvent, Vec3 const &, Block const &, bool);        // 1288
  virtual void playSynchronizedSound(LevelSoundEvent, Vec3 const &, int, bool);                  // 1296
  virtual void onSynchedDataUpdate(int);                                                         // 1304
  virtual bool canAddRider(Actor &) const;                                                       // 1312
  virtual bool canBePulledIntoVehicle() const;                                                   // 1320
  virtual bool inCaravan() const;                                                                // 1328
  virtual bool isLeashableType();                                                                // 1336
  virtual void tickLeash();                                                                      // 1344
  virtual void sendMotionPacketIfNeeded();                                                       // 1352
  virtual bool canSynchronizeNewEntity() const;                                                  // 1360
  virtual void stopRiding(bool, bool, bool);                                                     // 1368
  virtual void startSwimming();                                                                  // 1376
  virtual void stopSwimming();                                                                   // 1384
  virtual void buildDebugInfo(std::string &) const;                                              // 1392
  virtual int getCommandPermissionLevel() const;                                                 // 1400
  virtual AttributeInstance &getMutableAttribute(Attribute const &);                             // 1408
  virtual AttributeInstance const &getAttribute(Attribute const &) const;                        // 1416
  virtual int getDeathTime() const;                                                              // 1424
  virtual void heal(int);                                                                        // 1432
  virtual bool isInvertedHealAndHarm() const;                                                    // 1440
  virtual bool canBeAffected(int) const;                                                         // 1448
  virtual bool canBeAffected(MobEffectInstance const &) const;                                   // 1456
  virtual bool canBeAffectedByArrow(MobEffectInstance const &) const;                            // 1464
  virtual void onEffectAdded(MobEffectInstance &);                                               // 1472
  virtual void onEffectUpdated(MobEffectInstance const &);                                       // 1480
  virtual void onEffectRemoved(MobEffectInstance &);                                             // 1488
  virtual void openContainerComponent(Player &);                                                 // 1496
  virtual void swing();                                                                          // 1504
  virtual void useItem(ItemInstance &);                                                          // 1512
  virtual bool hasOutputSignal(signed char) const;                                               // 1520
  virtual char getOutputSignal() const;                                                          // 1528
  virtual void getDebugText(std::vector<std::string> &);                                         // 1536
  virtual void startSeenByPlayer(Player &);                                                      // 1544
  virtual void stopSeenByPlayer(Player &);                                                       // 1552
  virtual Vec2 getMapDecorationRotation();                                                       // 1560
  virtual Vec2 getRiderDecorationRotation(Player &);                                             // 1568
  virtual float getYHeadRot() const;                                                             // 1576
  virtual bool isWorldBuilder();                                                                 // 1584
  virtual bool isCreative() const;                                                               // 1592
  virtual bool isAdventure() const;                                                              // 1600
  virtual void add(ItemInstance &);                                                              // 1608
  virtual void drop(ItemInstance const &, bool);                                                 // 1616
  virtual bool getInteraction(Player &, ActorInteraction &, Vec3 const &);                       // 1624
  virtual bool canDestroyBlock(Block const &) const;                                             // 1632
  virtual void setAuxValue(int);                                                                 // 1640
  virtual void setSize(float, float);                                                            // 1648
  virtual float getYawSpeedInDegreesPerSecond() const;                                           // 1656
  virtual void onOrphan();                                                                       // 1664
  virtual void wobble();                                                                         // 1672
  virtual bool wasHurt();                                                                        // 1680
  virtual void startSpinAttack();                                                                // 1688
  virtual void stopSpinAttack();                                                                 // 1696
  virtual void setDamageNearbyMobs(bool);                                                        // 1704
  virtual void renderDebugServerState(Options const &);                                          // 1712
  virtual void reloadLootTable();                                                                // 1720
  virtual float getDeletionDelayTimeSeconds() const;                                             // 1728
  virtual void die(ActorDamageSource const &);                                                   // 1736
  virtual bool shouldTick() const;                                                               // 1744
  virtual void updateMolangVariables(RenderParams &);                                            // 1752
  virtual bool canMakeStepSound() const;                                                         // 1760
  virtual void setPos(ActorPos const &);                                                         // 1768
  virtual void outOfWorld();                                                                     // 1776
  virtual bool _hurt(ActorDamageSource const &, int, bool, bool);                                // 1784
  virtual void markHurt();                                                                       // 1792
  virtual void lavaHurt();                                                                       // 1800
  virtual void readAdditionalSaveData(CompoundTag const &);                                      // 1808
  virtual void addAdditionalSaveData(CompoundTag &);                                             // 1816
  virtual void _playStepSound(BlockPos const &, Block const &);                                  // 1824
  virtual void _playFlySound(BlockPos const &, Block const &);                                   // 1832
  virtual bool _makeFlySound() const;                                                            // 1840
  virtual bool checkInsideBlocks(float);                                                         // 1848
  virtual void pushOutOfBlocks(Vec3 const &);                                                    // 1856
  virtual void updateWaterState();                                                               // 1864
  virtual void doWaterSplashEffect();                                                            // 1872
  virtual void spawnTrailBubbles();                                                              // 1880
  virtual void updateInsideBlock();                                                              // 1888
  virtual LootTable &getLootTable();                                                             // 1896
  virtual LootTable &getDefaultLootTable();                                                      // 1904
  virtual void _removeRider(ActorUniqueID const &, bool, bool);                                  // 1912
  virtual void onSizeUpdated();                                                                  // 1920
  virtual void _doAutoAttackOnTouch(Actor &);                                                    // 1928
};

static_assert(32 == offsetof(Actor, unk32));
static_assert(140 == offsetof(Actor, in_world));
static_assert(200 == offsetof(Actor, swim_amount));
static_assert(300 == offsetof(Actor, b300));
static_assert(408 == offsetof(Actor, unk408));
static_assert(500 == offsetof(Actor, v500));
static_assert(600 == offsetof(Actor, riders));
static_assert(3216 == offsetof(Actor, auid3216));
static_assert(3264 == offsetof(Actor, baseAttributeMap));
static_assert(3672 == offsetof(Actor, aabbShape));
static_assert(3808 == offsetof(Actor, b3808));
static_assert(3904 == offsetof(Actor, b3904));
static_assert(4232 == offsetof(Actor, handContainer));
static_assert(4512 == offsetof(Actor, b4512));
static_assert(4584 == offsetof(Actor, varmap));

#undef DEF_COMPONENT

struct CompassSpriteCalculator {
  int frame;
  int min;
  int max;

  CompassSpriteCalculator();
  void calculateFrame(BlockSource const &, Vec3 const &, float);
  void calculateFrame(BlockSource const &, float, float, float);
  void updateFromPosition(BlockSource const &, float, float, float, bool);
  void update(Actor &, bool);
  int getFrame() const;
};

struct ClockSpriteCalculator {
  int frame;
  int min;
  int max;

  ClockSpriteCalculator();
  void calculateFrame(BlockSource const &, Vec3 const &, float);
  void calculateFrame(BlockSource const &, float, float, float);
  void updateFromPosition(BlockSource const &, float, float, float, bool);
  void update(Actor &, bool);
  int getFrame() const;
};

struct MovementInterpolator {
  Vec3 pos;    // 0
  Vec2 tgt;    // 12
  float yaw;   // 20
  bool rot;    // 24
  int step;    // 28
  bool active; // 32

  bool isActive();
  void lerpTo(Vec3 const &, Vec2 const &, int);
  void reset();
  void setHeadYawLerpTarget(float);
  void start();
  void stop();
  void tick(Actor &);
};

struct Goal;

struct GoalSelector {
  struct InternalGoal {
    std::unique_ptr<Goal> goal; // 0
    int priority;               // 8
    bool used;                  // 12
    bool to_start;              // 13

    InternalGoal(int, std::unique_ptr<Goal>);
    InternalGoal(InternalGoal &&);

    InternalGoal &operator=(InternalGoal &&);

    int getPriority() const;
    bool getToStart() const;
    bool getUsed() const;
    void setPriority(int) const;
    void setToStart(bool) const;
    void setUsed(bool) const;

    ~InternalGoal();
  };
  std::vector<InternalGoal> goals;

  GoalSelector();
  void addGoal(int, std::unique_ptr<Goal>);
  void clearGoals();
  void removeGoal(Goal *);
  void stopGoals();
  void onPlayerDimensionChanged(Player *, DimensionId);
  void buildDebugInfo(std::string &);
  void tick();
  ~GoalSelector();
};

struct LookControl;
struct BodyControl;
struct Sensing;
struct Village;
struct PathNavigation;
struct JumpControl;
struct MoveControl;

struct Mob : Actor {
  enum struct TravelType {};
  int unk4640;                                    // 4640
  int unk4644;                                    // 4644
  float yhead_rot;                                // 4648
  char filler4652[4720 - 4652];                   // 4652
  Vec3 v4720;                                     // 4720
  CompassSpriteCalculator compass;                // 4732
  ClockSpriteCalculator clock;                    // 4744
  float xxa;                                      // 4756
  float yya;                                      // 4760
  float zza;                                      // 4764
  float y_rot;                                    // 4768
  char filler4772[12];                            // 4772
  int time_along_swing;                           // 4784
  int time_no_action;                             // 4788
  int unk4792;                                    // 4792
  float friction_modifier;                        // 4796
  float flight_speed;                             // 4800
  char filler4800[28];                            // 4804
  float rider_locked_body_rot;                    // 4832
  float rider_rot_limit;                          // 4836
  MovementInterpolator interp;                    // 4840
  char filler4876[12];                            // 4876
  bool jumping;                                   // 4888
  bool jump_vel_redux;                            // 4889
  int unk4892;                                    // 4892
  int unk4896;                                    // 4896
  bool b4900;                                     // 4900
  Vec3 v4904;                                     // 4904
  float speed;                                    // 4916
  std::unique_ptr<LookControl> lookCtl;           // 4920
  std::unique_ptr<BodyControl> bodyCtl;           // 4928
  std::unique_ptr<Sensing> sensing;               // 4936
  GoalSelector goal4944;                          // 4944
  GoalSelector goal4968;                          // 4968
  bool surfaceMob;                                // 4992
  bool natural_spawned;                           // 4993
  bool b4994;                                     // 4994
  std::weak_ptr<Village> village;                 // 5000
  bool wants_to_be_jockey;                        // 5016
  int unk5020;                                    // 5020
  bool unk5024;                                   // 5024
  ActorUniqueID auid5032;                         // 5032
  ActorUniqueID auid5040;                         // 5040
  ActorUniqueID auid5048;                         // 5048
  ActorUniqueID auid5056;                         // 5056
  ActorUniqueID carvanHead;                       // 5064
  ActorUniqueID carvanTail;                       // 5072
  int last_hurt_mob_timestamp;                    // 5080
  int last_hurt_by_mob_timestamp;                 // 5084
  int unk5088;                                    // 5088
  int unk5092;                                    // 5092
  int arrow_count;                                // 5096
  int unk5100;                                    // 5100
  int gliding_ticks;                              // 5104
  bool b5108;                                     // 5108
  BlockPos boundOrigin;                           // 5112
  std::unique_ptr<PathNavigation> pathNavigation; // 5128
  std::unique_ptr<JumpControl> jumpCtl;           // 5136
  std::unique_ptr<MoveControl> moveCtl;           // 5144
  MobSpawnMethod spawnMethod;                     // 5152
  bool b5156;                                     // 5156

  static int PLAYER_HURT_EXPERIENCE_TIME;
  static int ABSORPTION_FACTOR_MAX;
  static int ABSORPTION_FACTOR_MIN;
  static int PLAYER_SWIMMING_SURFACE_OFFSET;
  static int GLIDING_FALL_RESET_DELTA;
  static int SLOW_FALL_GRAVITY;
  static int DEFAULT_GRAVITY;

  Mob(ActorDefinitionGroup *, ActorDefinitionIdentifier const &);
  Mob(Level &);

  void calcMoveRelativeSpeed(TravelType);
  void calculateAmbientSoundTime(int);
  bool canPickUpLoot(ItemInstance const &) const;
  bool checkTotemDeathProtection(ActorDamageSource const &);
  void createAI(std::vector<GoalDefinition>);
  void frostWalk();
  void setBoundOrigin(BlockPos);
  BlockPos getBoundOrigin() const;
  bool hasBoundOrigin() const;
  int getCaravanSize() const;
  int getCurrentSwingDuration();
  ActorUniqueID getFirstCaravanHead();
  void setFlightSpeed(float);
  float getFlightSpeed();
  void setFrictionModifier(float);
  float getFrictionModifier();
  int getGlidingTicks() const;
  ItemInstance const &getItemSlot(EquipmentSlot) const;
  JumpControl &getJumpControl();
  LookControl &getLookControl();
  MoveControl &getMoveControl();
  PathNavigation &getNavigation();
  Sensing &getSensing();
  int getLastHurtByMobTimestamp();
  int getLastHurtMobTimestamp();
  int getNoActionTime() const;
  void setSpawnMethod(MobSpawnMethod);
  MobSpawnMethod getSpawnMethod();
  TravelType getTravelType();
  std::weak_ptr<Village> getVillage();
  std::weak_ptr<Village> const &getVillage() const;
  void setXxa(float);
  void setYya(float);
  void setZza(float);
  float getXxa() const;
  float getYya() const;
  float getZza() const;
  void setYHeadRotA(float);
  void setYRotA(float);
  float getYRotA() const;
  void incrementArrowCount(int);
  bool isFrostWalking() const;
  bool isGliding() const;
  void setIsLayingEgg(bool);
  bool isLayingEgg() const;
  void setNaturallySpawned(bool);
  bool isNaturallySpawned() const;
  void setIsPregnant(bool);
  bool isPregnant() const;
  void lerpTo(Vec3 const &, Vec2 const &, float, int);
  void loadArmor(ListTag const *);
  std::unique_ptr<ListTag> saveArmor();
  void loadMainhand(ListTag const *);
  std::unique_ptr<ListTag> saveMainhand();
  void loadOffhand(ListTag const *);
  std::unique_ptr<ListTag> saveOffhand();
  void onPlayerJump(int);
  void pickUpItem(ItemActor &);
  void playBornSound();
  void playSpawnSound();
  void registerAttributes();
  void resetAttributes();
  void setJumpVelRedux(bool);
  void setJumping(bool);
  void setRiderLockedBodyRot(float);
  void setRiderRotLimit(float);
  void setSpeedModifier(float);
  void setSurfaceMob(bool);
  void setWantsToBeJockey(bool);
  bool wantsToBeJockey() const;
  bool shouldApplyWaterGravity();
  void snapToYHeadRot(float);
  void stopAI();
  void tickAttributes();
  void tickEffects();
  void tickMagmaDamage();
  void updateAttackAnim();
  void updateMobId(ActorUniqueID &);

  virtual void reloadHardcodedClient(Actor::InitializationMethod, VariantParameterList const &) override; // 8
  virtual void initializeComponents(Actor::InitializationMethod, VariantParameterList const &) override;  // 16
  virtual bool hasComponent(Util::HashString const &) const override;                                     // 32
  virtual ~Mob() override;                                                                                // 56, 64
  virtual Vec2 getInterpolatedBodyRot(float) const override;                                              // 168
  virtual void teleportTo(Vec3 const &, bool, int, int) override;                                         // 208
  virtual void lerpTo(Vec3 const &, Vec2 const &, int) override;                                          // 232
  virtual void normalTick() override;                                                                     // 248
  virtual void baseTick() override;                                                                       // 256
  virtual void rideTick() override;                                                                       // 264
  virtual bool startRiding(Actor &) override;                                                             // 288
  virtual void addRider(Actor &) override;                                                                // 296
  virtual void playerTouch(Player &) override;                                                            // 584
  virtual bool isImmobile() const override;                                                               // 632
  virtual bool isPickable() override;                                                                     // 648
  virtual bool isPushable() const override;                                                               // 664
  virtual bool isShootable() override;                                                                    // 680
  virtual bool isSneaking() const override;                                                               // 688
  virtual bool isAlive() const override;                                                                  // 696
  virtual bool isSurfaceMob() const override;                                                             // 278
  virtual void setTarget(Actor *) override;                                                               // 752
  virtual bool attack(Actor &) override;                                                                  // 776
  virtual bool canPowerJump() const override;                                                             // 872
  virtual void animateHurt() override;                                                                    // 880
  virtual bool doFireHurt(int) override;                                                                  // 936
  virtual void handleEntityEvent(ActorEvent, int) override;                                               // 968
  virtual void setEquippedSlot(ArmorSlot, int, int) override;                                             // 1064
  virtual void setEquippedSlot(ArmorSlot, ItemInstance const &) override;                                 // 1072
  virtual void setOnFire(int) override;                                                                   // 1184
  virtual void causeFallDamage(float) override;                                                           // 1272
  virtual bool canBePulledIntoVehicle() const override;                                                   // 1320
  virtual bool inCaravan() const override;                                                                // 1328
  virtual void stopRiding(bool, bool, bool) override;                                                     // 1368
  virtual void buildDebugInfo(std::string &) const override;                                              // 1392
  virtual int getDeathTime() const override;                                                              // 1424
  virtual void swing() override;                                                                          // 1504
  virtual float getYHeadRot() const override;                                                             // 1576
  virtual float getYawSpeedInDegreesPerSecond() const override;                                           // 1656
  virtual void renderDebugServerState(Options const &) override;                                          // 1712
  virtual void die(ActorDamageSource const &) override;                                                   // 1736
  virtual void outOfWorld() override;                                                                     // 1776
  virtual bool _hurt(ActorDamageSource const &, int, bool, bool) override;                                // 1784
  virtual void readAdditionalSaveData(CompoundTag const &) override;                                      // 1808
  virtual void addAdditionalSaveData(CompoundTag &) override;                                             // 1816
  virtual void _playStepSound(BlockPos const &, Block const &) override;                                  // 1824
  virtual void _removeRider(ActorUniqueID const &, bool, bool) override;                                  // 1912
  virtual void onSizeUpdated() override;                                                                  // 1920
  virtual void knockback(Actor *, int, float, float, float, float, float);                                // 1936
  virtual void resolveDeathLoot(bool, int, ActorDamageSource const &);                                    // 1944
  virtual void spawnAnim();                                                                               // 1952
  virtual bool isSleeping() const;                                                                        // 1960
  virtual void setSneaking(bool);                                                                         // 1968
  virtual bool isSprinting() const;                                                                       // 1976
  virtual void setSprinting(bool);                                                                        // 1984
  virtual float getVoicePitch();                                                                          // 1992
  virtual void playAmbientSound();                                                                        // 2000
  virtual int getAmbientSound();                                                                          // 2008
  virtual int getAmbientSoundPostponeTicks();                                                             // 2016
  virtual TextureUVCoordinateSet const &getItemInHandIcon(ItemInstance const &, int);                     // 2024
  virtual float getSpeed() const;                                                                         // 2036
  virtual void setSpeed(float);                                                                           // 2040
  virtual float getWaterSpeed() const;                                                                    // 2048
  virtual bool isJumping() const;                                                                         // 2056
  virtual float getJumpPower() const;                                                                     // 2064
  virtual bool hurtEffects(ActorDamageSource const &, int, bool, bool);                                   // 2072
  virtual int getMeleeWeaponDamageBonus(Mob *);                                                           // 2080
  virtual int getMeleeKnockbackBonus();                                                                   // 2088
  virtual void actuallyHurt(int, ActorDamageSource const *, bool);                                        // 2096
  virtual void travel(float, float, float);                                                               // 2104
  virtual float applyFinalFriction(float, bool);                                                          // 2112
  virtual void updateWalkAnim();                                                                          // 2120
  virtual void aiStep();                                                                                  // 2128
  virtual void pushEntities();                                                                            // 2136
  virtual void lookAt(Actor *, float, float);                                                             // 2144
  virtual bool isLookingAtAnEntity();                                                                     // 2152
  virtual bool checkSpawnRules(bool);                                                                     // 2160
  virtual bool checkSpawnObstruction() const;                                                             // 2168
  virtual bool shouldDespawn() const;                                                                     // 2176
  virtual float getAttackAnim(float);                                                                     // 2184
  virtual void performRangedAttack(Actor &, float);                                                       // 2192
  virtual int getItemUseDuration();                                                                       // 2200
  virtual int getItemUseStartupProgress();                                                                // 2208
  virtual int getItemuseIntervalProgress();                                                               // 2216
  virtual int getItemuseIntervalAxis();                                                                   // 2224
  virtual int getTimeAlongSwing();                                                                        // 2232
  virtual void ate();                                                                                     // 2240
  virtual float getMaxHeadXRot();                                                                         // 2248
  virtual Mob *getLastHurtByMob();                                                                        // 2256
  virtual void setLastHurtByMob(Mob *);                                                                   // 2264
  virtual Player *getLastHurtByPlayer();                                                                  // 2272
  virtual void setLastHurtByPlayer(Player *);                                                             // 2280
  virtual Actor *getLastHurtMob();                                                                        // 2288
  virtual void setLastHurtMob(Actor *);                                                                   // 2296
  virtual bool isAlliedTo(Mob *);                                                                         // 2304
  virtual bool doHurtTarget(Actor *);                                                                     // 2312
  virtual bool canBeControlledByRider();                                                                  // 2320
  virtual void leaveCaravan();                                                                            // 2328
  virtual void joinCaravan(Mob *);                                                                        // 2336
  virtual bool hasCaravanTail() const;                                                                    // 2344
  virtual ActorUniqueID getCaravanHead() const;                                                           // 2352
  virtual int getEquipmentCount() const;                                                                  // 2360
  virtual int getArmorValue();                                                                            // 2368
  virtual float getArmorCoverPercentage() const;                                                          // 2376
  virtual void hurtArmor(int);                                                                            // 2384
  virtual void containerChanged(int);                                                                     // 2392
  virtual void updateEquipment();                                                                         // 2400
  virtual void clearEquipment();                                                                          // 2408
  virtual std::vector<ItemInstance const *> getAllArmor() const;                                          // 2416
  virtual std::vector<int> getAllArmorID();                                                               // 2424
  virtual std::vector<ItemInstance const *> getAllHand() const;                                           // 2432
  virtual std::vector<ItemInstance const *> getAllEquipment() const;                                      // 2440
  virtual std::size_t getArmorTypeHash();                                                                 // 2448
  virtual void sendInventory(bool);                                                                       // 2456
  virtual void sendArmor();                                                                               // 2464
  virtual int getDamageAfterMagicAbsorb(ActorDamageSource const &, int);                                  // 2472
  virtual void createAIGoals();                                                                           // 2480
  virtual void onBorn(Mob &, Mob &);                                                                      // 2488
  virtual void onLove();                                                                                  // 2496
  virtual void setItemSlot(EquipmentSlot, ItemInstance const &);                                          // 2504
  virtual void goDownInWater();                                                                           // 2512
  virtual float getWaterSlowDown() const;                                                                 // 2520
  virtual void attackAnimation(Actor *, float);                                                           // 2528
  virtual int getAttackTime();                                                                            // 2536
  virtual void _getWalkTargetValue(BlockPos const &);                                                     // 2544
  virtual bool canExistWhenDisallowMob() const;                                                           // 2552
  virtual bool _removeWhenFarAway();                                                                      // 2560
  virtual void jumpFromGround();                                                                          // 2568
  virtual void updateAi();                                                                                // 2576
  virtual void newServerAiStep();                                                                         // 2584
  virtual void _serverAiMobStep();                                                                        // 2592
  virtual int getDamageAfterEnchantReduction(ActorDamageSource const &, int);                             // 2600
  virtual int getDamageAfterArmorAbsorb(ActorDamageSource const &, int);                                  // 2608
  virtual int getExperienceReward() const;                                                                // 2616
  virtual void dropEquipment(ActorDamageSource const &, int);                                             // 2624
  virtual void dropEquipment();                                                                           // 2632
  virtual void dropBags();                                                                                // 2640
  virtual void dropContainer();                                                                           // 2648
  virtual bool useNewAi() const;                                                                          // 2656
  virtual void tickDeath();                                                                               // 2664
  virtual void _endJump();                                                                                // 2672
  virtual void updateGliding();                                                                           // 2680
};

static_assert(4640 == offsetof(Mob, unk4640));
static_assert(4720 == offsetof(Mob, v4720));
static_assert(4800 == offsetof(Mob, flight_speed));
static_assert(4900 == offsetof(Mob, b4900));
static_assert(5000 == offsetof(Mob, village));
static_assert(5100 == offsetof(Mob, unk5100));
static_assert(5156 == offsetof(Mob, b5156));

struct Certificate;
struct PlayerChunkSource;
struct IContainerManager;
struct PlayerInventoryProxy;
struct SkinInfoData;
struct PacketSender;
struct HudContainerManagerModel;
struct EnderChestContainer;
struct FillingContainer;
struct InventoryTransaction;
struct InventoryAction;
struct GameMode;
struct PlayerListener;
struct NetworkIdentifier;
struct ChunkSource;
struct StructureFeature;
struct ChalkboardBlockActor;
struct MinecraftEventing;
struct EventPacket;
struct ComplexInventoryTransaction;
struct Packet;
struct ServerPlayerEventCoordinator;

struct InventoryTransactionManager {
  Player &player;
  std::unique_ptr<InventoryTransaction> transaction;
  std::vector<InventoryAction> expected_actions;

  InventoryTransactionManager(Player &);
  void addAction(InventoryAction const &);
  void addExpectedAction(InventoryAction const &);
  void checkExpectedAction(InventoryAction const &);
  void forceBalanceTransaction();
  std::unique_ptr<InventoryTransaction> const &getCurrentTransaction() const;
  int getSourceCount() const;
  void reset();
  void resetExpectedActions();
  ~InventoryTransactionManager();
};

struct SkinAdjustments {
  int adjustment;
  SkinAdjustments();
};

namespace mce {
struct Image;
}
struct Agent;

struct alignas(8) Player : Mob {
  enum struct PositionMode { Normal, Direct, Teleport, Rotate };
  int unk5160;                                                           // 5160
  bool b5164;                                                            // 5164
  int unk5168;                                                           // 5168
  std::vector<int> v5176;                                                // 5176
  bool b5200;                                                            // 5200
  bool b5201;                                                            // 5201
  int unk5204;                                                           // 5204
  bool b5208;                                                            // 5208
  bool b5209;                                                            // 5209
  int score;                                                             // 5212
  int unk5216;                                                           // 5216
  int unk5220;                                                           // 5220
  bool unk5224;                                                          // 5224
  std::string s5232;                                                     // 5232
  int platform;                                                          // 5264
  Abilities abilities;                                                   // 5272
  NetworkIdentifier netId;                                               // 5416
  std::string s5568;                                                     // 5568
  std::string s5600;                                                     // 5600
  std::string s5632;                                                     // 5632
  std::string s5664;                                                     // 5664
  void *unk5696;                                                         // 5696
  mce::UUID uuid;                                                        // 5704
  std::unique_ptr<Certificate> cert;                                     // 5720
  std::string s5728;                                                     // 5728
  int unk5760;                                                           // 5760
  ActorUniqueID auid5768;                                                // 5768
  ActorUniqueID auid5776;                                                // 5776
  Vec3 v5784;                                                            // 5784
  bool b5796;                                                            // 5796
  std::unique_ptr<PlayerChunkSource> chunk_source1;                      // 5800
  std::unique_ptr<PlayerChunkSource> chunk_source2;                      // 5808
  std::unique_ptr<BlockSource> block_sorce;                              // 5816
  BlockPos sleep_block_pos;                                              // 5824
  Vec3 bed_offset;                                                       // 5836
  Vec3 v5848;                                                            // 5848
  bool b5860;                                                            // 5860
  Vec3 v5864;                                                            // 5864
  Vec3 v5876;                                                            // 5876
  Vec3 v5888;                                                            // 5888
  Vec3 v5900;                                                            // 5900
  int unk5912;                                                           // 5912
  int unk5916;                                                           // 5916
  std::shared_ptr<IContainerManager> container_mgr;                      // 5920
  std::unique_ptr<PlayerInventoryProxy> inv_proxy;                       // 5936
  std::unique_ptr<SkinInfoData> skin_info;                               // 5944
  std::vector<ItemInstance> items;                                       // 5952
  std::array<std::vector<ItemGroup>, 4ul> filtered_creative_item_groups; // 5976
  char client_sub_id;                                                    // 6072
  std::string platform_online_id;                                        // 6080
  bool respawn_ready;                                                    // 6112
  bool b6113;                                                            // 6113
  bool b6114;                                                            // 6114
  DimensionId dim_id;                                                    // 6116
  bool b6120;                                                            // 6120
  bool spawned;                                                          // 6121
  ItemInstance item_in_use;                                              // 6128
  int item_use_count_down;                                               // 6240
  short sleep_timer;                                                     // 6244
  short prev_sleep_timer;                                                // 6246
  bool b6248;                                                            // 6248
  ActorUniqueID auid6256;                                                // 6256
  int unk6264;                                                           // 6264
  PacketSender *sender;                                                  // 6272
  BlockPos bpos6280;                                                     // 6280
  long long unk6296;                                                     // 6296
  long long unk6304;                                                     // 6304
  std::shared_ptr<HudContainerManagerModel> hud_container;               // 6312
  std::unique_ptr<EnderChestContainer> ender_chest;                      // 6328
  std::unique_ptr<FillingContainer> filling;                             // 6336
  std::vector<ActorUniqueID> tracked_boss;                               // 6344
  bool update_boss_ui_binds;                                             // 6368
  bool update_boss_ui_controls;                                          // 6369
  bool b6370;                                                            // 6370
  ActorType last_hurt;                                                   // 6372
  ItemInstance cursor_selected_item;                                     // 6376
  ItemGroup cursor_selected_item_group;                                  // 6488
  InventoryTransactionManager inv_trans_mgr;                             // 6608
  std::unique_ptr<GameMode> game_mode;                                   // 6648
  std::vector<PlayerListener *> player_listener;                         // 6656
  char filler6680[12];                                                   // 6680
  BlockPos spawn_position;                                               // 6692
  bool forced_respawn;                                                   // 6704
  bool b6705;                                                            // 6705
  bool all_player_sleeping;                                              // 6706
  bool destroying_block;                                                 // 6707
  Vec3 v6708;                                                            // 6708
  GameType game_type;                                                    // 6720
  int ehcnantment_seed;                                                  // 6724
  int chunk_radius;                                                      // 6728
  int map_index;                                                         // 6732
  int unk6736;                                                           // 6736
  int unk6740;                                                           // 6740
  int underwater_vision_scale;                                           // 6744
  int underwater_light_level;                                            // 6748
  std::vector<int> v6752;                                                // 6752
  bool used_potion;                                                      // 6776
  SkinAdjustments skin_adj;                                              // 6780
  int unk6784;                                                           // 6784
  bool r5data_recovery;                                                  // 6788
  std::string device_id;                                                 // 6792

  Player(Level &, PacketSender &, GameType, NetworkIdentifier const &, unsigned char, std::unique_ptr<SkinInfoData>, mce::UUID, std::string const &, std::unique_ptr<Certificate>, std::string const &,
         std::string const &);

  void _addLevels(int);
  void _applyExhaustion(Vec3 const &);
  void _ensureSafeSpawnPosition(Vec3 &);
  void _fixup4JBedSpawnPosition(Vec3 &);
  float _getItemDestroySpeed(Block const &) const;
  void _handleCarriedItemInteractText();
  void _registerElytraLoopSound();
  void _tickCooldowns();
  void _updateInteraction();
  void addListener(PlayerListener &);
  void canDestroy(Block const &) const;
  bool canBeSeenOnMap() const;
  bool canDestroy() const;
  bool canUseAbility(std::string const &);
  bool canUseCommandBlocks() const;
  void causeFoodExhaustion(float);
  bool checkBed(bool);
  bool checkNeedAutoJump(float, float);
  void clearCreativeItemList();
  void clearUntrackedInteractionUIContainer(int);
  void crackBlock(BlockPos const &, signed char);
  void dropCursorSelectedItem();
  void eat(int, float);
  void eat(ItemInstance const &);
  void fireDimensionChangedEvent(DimensionId);
  void fixSpawnPosition(Vec3 &, std::vector<BlockSource *, std::allocator<BlockSource *>>) const;
  void fixStartSpawnPosition(BlockPos &, std::vector<BlockSource *, std::allocator<BlockSource *>>) const;
  void forceAllowEating() const;
  Agent *getAgent() const;
  ActorUniqueID getAgentID() const;
  SimpleContainer getArmorContainer();
  int getAttackDamage();
  Vec3 getCapePos(float);
  Certificate *getCertificate() const;
  int getChunkRadius() const;
  PlayerChunkSource *getChunkSource() const;
  NetworkIdentifier &getClientId() const;
  char getClientSubId() const;
  IContainerManager *getContainerManager();
  std::vector<ItemInstance> const &getCreativeItemList() const;
  ItemInstance const &getCursorSelectedItem() const;
  ItemGroup getCursorSelectedItemGroup() const;
  float getDestroyProgress(Block const &) const;
  float getDestroySpeed(Block const &) const;
  std::string getDeviceId() const;
  int getDirection() const;
  int getEnchantmentSeed() const;
  EnderChestContainer *getEnderChestContainer();
  std::array<std::vector<ItemGroup>, 4ul> const &getFilteredCreativeItemList() const;
  GameMode &getGameMode() const;
  SimpleContainer getHandContainer();
  std::weak_ptr<HudContainerManagerModel> getHudContainerManagerModel();
  std::string getInteractText() const;
  std::string getItemInteractText(Item const &) const;
  ItemInstance const &getItemInUse() const;
  float getItemUseIntervalProgress();
  ActorType getLastHurtBy() const;
  float getLevelProgress() const;
  float getLuck();
  int getMapIndex();
  int getNewEnchantmentSeed();
  int getPlatform() const;
  std::string const &getPlatformOnlineId() const;
  GameType getPlayerGameType();
  int getPlayerIndex() const;
  int getPlayerLevel() const;
  char getPlayerPermissionLevel() const;
  int getPreviousTickSleepTimer() const;
  bool getR5DataRecoverComplete() const;
  int getScore();
  ItemInstance const &getSelectedItem() const;
  int getSelectedItemSlot() const;
  SkinAdjustments const &getSkinAdjustments() const;
  SkinInfoData const &getSkin() const;
  float getSleepRotation() const;
  BlockPos getSpawnPosition();
  Vec3 getStandingPositionOnBlock(BlockPos const &);
  PlayerInventoryProxy const &getSupplies() const;
  int getTicksUsingItem();
  std::vector<ActorUniqueID> const &getTrackedBosses();
  InventoryTransactionManager &getTransactionManager();
  int getUnderwaterLightLevel() const;
  int getUnderwaterVisionScale() const;
  GameType getUnmappedPlayerGameType();
  bool getUsedPotion();
  int getXpNeededForLevelRange(int, int) const;
  int getXpNeededForNextLevel() const;
  void handleJumpEffects();
  void handleMovePlayerPacket(Player::PositionMode, Vec3 const &, Vec2 const &, float, int, int);
  bool hasOpenContainer() const;
  bool hasRespawnPosition() const;
  bool interact(Actor &, Vec3 const &);
  bool isBouncing() const;
  bool isChatAllowed();
  bool isDestroyingBlock();
  bool isForcedRespawn() const;
  bool isHiddenFrom(Mob &) const;
  bool isHungry() const;
  bool isHurt();
  bool isInCreativeMode();
  bool isRespawnReady();
  bool isSleepingLongEnough() const;
  bool isSpawned() const;
  bool isSurvival() const;
  bool isUsingItem() const;
  bool isViewer() const;
  void moveCape();
  void onMobStatusChanged(ActorUniqueID);
  void onResetBAI(int);
  void recheckSpawnPosition();
  void registerAttributes();
  void releaseUsingItem();
  void removeListener(PlayerListener &);
  void resetPlayerLevel();
  void resetToDefaultGameMode();
  void sendEventPacket(EventPacket &) const;
  void sendNetworkPacket(Packet &) const;
  void setAgent(Agent *);
  void setAllPlayersSleeping();
  void setBedOffset(int);
  void setBedRespawnPosition(BlockPos const &);
  void setChunkRadius(unsigned int);
  void setContainerManager(std::shared_ptr<IContainerManager>);
  void setCursorSelectedItemGroup(ItemGroup const &);
  void setCursorSelectedItem(ItemInstance const &);
  void setDefaultHeadHeight();
  void setEnchantmentSeed(int);
  void setLastHurtBy(ActorType);
  void setMapIndex(int);
  void setPlatform(BuildPlatform);
  void setPlatformOnlineId(std::string const &);
  void setPlayerIndex(int);
  void setR5DataRecoverComplete(bool);
  void setRespawnDimension(DimensionId);
  void setRespawnDimensionId(DimensionId);
  void setRespawnPosition(BlockPos const &, bool);
  void setSelectedItem(ItemInstance const &);
  void setSkin(std::string const &, mce::Image const *, mce::Image const *, std::string const &, std::string const &, bool);
  void setTeleportDestination(Vec3 const &);
  void setUnderwaterLightLevel(float);
  void setUnderwaterVisionScale(float);
  void setUntrackedInteractionUIItem(int, ItemInstance const &);
  void setUsedPotion(bool);
  bool shouldUpdateBosGUIControls();
  void shouldUpdateBossGUIBinds();
  void spawnExperienceOrb(Vec3 const &, int);
  void startGliding();
  void startUsingItem(ItemInstance const &, int);
  void stopGliding();
  void stopUsingItem();
  void take(Actor &, int, int);
  void tickArmor();
  void updateCreativeItemList(std::vector<ItemInstance, std::allocator<ItemInstance>> const &);
  void updateInventoryTransactions();
  void updateTeleportDestPos();
  void updateTrackedBosses();

  virtual void reloadHardcoded(Actor::InitializationMethod, VariantParameterList const &) override;         // 8
  virtual void initializeComponents(Actor::InitializationMethod, VariantParameterList const &) override;    // 16
  virtual ~Player() override;                                                                               // 56,64
  virtual void remove() override;                                                                           // 88
  virtual Vec3 getAttachPos(ActorLocation, float) const override;                                           // 128
  virtual void move(Vec3 const &) override;                                                                 // 152
  virtual void teleportTo(Vec3 const &, bool, int, int) override;                                           // 208
  virtual std::unique_ptr<AddActorPacket> getAddPacket() override;                                          // 240
  virtual void normalTick() override;                                                                       // 248
  virtual void rideTick() override;                                                                         // 264
  virtual float getRidingHeight() override;                                                                 // 280
  virtual std::string getFormattedNameTag() const override;                                                 // 392
  virtual bool getAlwaysShowNameTag() const override;                                                       // 416
  virtual float getCameraOffset() const override;                                                           // 504
  virtual bool isImmobile() const override;                                                                 // 632
  virtual bool isPushable() const override;                                                                 // 664
  virtual bool isPushableByPiston() const override;                                                         // 672
  virtual bool isShootable() override;                                                                      // 680
  virtual bool isCreativeModeAllowed() override;                                                            // 720
  virtual bool attack(Actor &) override;                                                                    // 776
  virtual void adjustDamageAmount(int &) const override;                                                    // 784
  virtual bool isInvulnerableTo(ActorDamageSource const &) const override;                                  // 920
  virtual void onBounceStarted(BlockPos const &, Block const &) override;                                   // 952
  virtual void feed(int) override;                                                                          // 960
  virtual void handleEntityEvent(ActorEvent, int) override;                                                 // 968
  virtual void awardKillScore(Actor &, int) override;                                                       // 1040
  virtual void setArmor(ArmorSlot, ItemInstance const &) override;                                          // 1048
  virtual ItemInstance const &getCarriedItem() const override;                                              // 1080
  virtual void setCarriedItem(ItemInstance const &) override;                                               // 1088
  virtual void setOffhandSlot(ItemInstance const &) override;                                               // 1096
  virtual ItemInstance const &getEquippedTotem() const override;                                            // 1104
  virtual bool consumeTotem() override;                                                                     // 1112
  virtual ActorType getEntityTypeId() const override;                                                       // 1152
  virtual int getPortalCooldown() const override;                                                           // 1208
  virtual int getPortalWaitTime() const override;                                                           // 1216
  virtual void onSynchedDataUpdate(int) override;                                                           // 1304
  virtual bool canAddRider(Actor &) const override;                                                         // 1312
  virtual bool canBePulledIntoVehicle() const override;                                                     // 1320
  virtual void sendMotionPacketIfNeeded() override;                                                         // 1352
  virtual void startSwimming() override;                                                                    // 1376
  virtual void stopSwimming() override;                                                                     // 1384
  virtual int getCommandPermissionLevel() const override;                                                   // 1400
  virtual void useItem(ItemInstance &) override;                                                            // 1512
  virtual Vec2 getMapDecorationRotation() override;                                                         // 1560
  virtual bool isWorldBuilder() override;                                                                   // 1584
  virtual bool isCreative() const override;                                                                 // 1592
  virtual bool isAdventure() const override;                                                                // 1600
  virtual void add(ItemInstance &) override;                                                                // 1608
  virtual void drop(ItemInstance const &, bool) override;                                                   // 1616
  virtual void startSpinAttack() override;                                                                  // 1688
  virtual void stopSpinAttack() override;                                                                   // 1696
  virtual void die(ActorDamageSource const &) override;                                                     // 1736
  virtual bool _hurt(ActorDamageSource const &, int, bool, bool) override;                                  // 1784
  virtual void lavaHurt() override;                                                                         // 1800
  virtual void readAdditionalSaveData(CompoundTag const &) override;                                        // 1808
  virtual void addAdditionalSaveData(CompoundTag &) override;                                               // 1816
  virtual void onSizeUpdated() override;                                                                    // 1920
  virtual bool isSleeping() const override;                                                                 // 1960
  virtual float getSpeed() const override;                                                                  // 2036
  virtual void setSpeed(float) override;                                                                    // 2040
  virtual bool isJumping() const override;                                                                  // 2056
  virtual void actuallyHurt(int, ActorDamageSource const *, bool) override;                                 // 2096
  virtual void travel(float, float, float) override;                                                        // 2104
  virtual void aiStep() override;                                                                           // 3128
  virtual int getItemUseDuration() override;                                                                // 2200
  virtual int getItemUseStartupProgress() override;                                                         // 2208
  virtual int getItemuseIntervalProgress() override;                                                        // 2216
  virtual std::vector<ItemInstance const *> getAllHand() const override;                                    // 2416
  virtual std::vector<ItemInstance const *> getAllEquipment() const override;                               // 2440
  virtual void sendInventory(bool) override;                                                                // 2456
  virtual bool canExistWhenDisallowMob() const override;                                                    // 2552
  virtual void jumpFromGround() override;                                                                   // 2568
  virtual void updateAi() override;                                                                         // 2576
  virtual int getExperienceReward() const override;                                                         // 2616
  virtual void dropEquipment(ActorDamageSource const &, int) override;                                      // 2624
  virtual void dropEquipment() override;                                                                    // 2632
  virtual bool useNewAi() const override;                                                                   // 2656
  virtual void updateGliding() override;                                                                    // 2680
  virtual void prepareRegion(ChunkSource &);                                                                // 2688
  virtual void destroyRegion();                                                                             // 2696
  virtual void suspendRegion();                                                                             // 2704
  virtual void _fireWillChangeDimension();                                                                  // 2712
  virtual void _fireDimensionChanged();                                                                     // 2720
  virtual void changeDimensionWithCredits(DimensionId);                                                     // 2728
  virtual void tickWorld(Tick const &);                                                                     // 2736
  virtual std::vector<ChunkPos> const &getTickingOffsets() const;                                           // 2744
  virtual void moveView();                                                                                  // 2752
  virtual void setName(std::string const &);                                                                // 2760
  virtual void checkMovementStats(Vec3 const &);                                                            // 2768
  virtual StructureFeature *getCurrentStructureFeature() const;                                             // 2776
  virtual void respawn();                                                                                   // 2784
  virtual void resetRot();                                                                                  // 2792
  virtual void resetPos(bool);                                                                              // 2800
  virtual bool isInTrialMode();                                                                             // 2808
  virtual bool hasResource(int);                                                                            // 2816
  virtual void completeUsingItem();                                                                         // 2824
  virtual void setPermissions(CommandPermissionLevel);                                                      // 2832
  virtual void startCrafting(BlockPos const &, bool);                                                       // 2840
  virtual void startDestroying();                                                                           // 2856
  virtual void stopDestroying();                                                                            // 2864
  virtual void openContainer(BlockPos const &);                                                             // 2872
  virtual void openContainer(ActorUniqueID const &);                                                        // 2880
  virtual void openFurnace(BlockPos const &);                                                               // 2888
  virtual void openAnvil(BlockPos const &);                                                                 // 2904
  virtual void openBrewingStand(BlockPos const &);                                                          // 2912
  virtual void openHopper(BlockPos const &);                                                                // 2920
  virtual void openHopper(ActorUniqueID const &);                                                           // 2928
  virtual void openDispenser(BlockPos const &, bool);                                                       // 2936
  virtual void openBeacon(BlockPos const &);                                                                // 2944
  virtual void openPortfolio();                                                                             // 2952
  virtual void openBook(int, bool);                                                                         // 2960
  virtual void openCommandBlock(BlockPos const &);                                                          // 2968
  virtual void openCommandBlockMinecart(ActorUniqueID const &);                                             // 2976
  virtual void openHorseInventory(ActorUniqueID const &);                                                   // 2984
  virtual bool canOpenContainerScreen();                                                                    // 3000
  virtual void openChalkboard(ChalkboardBlockActor &, bool);                                                // 3008
  virtual void openNpcInteractScreen(Actor &);                                                              // 3016
  virtual void openInventory();                                                                             // 3024
  virtual void openStructureEditor(BlockPos const &);                                                       // 3032
  virtual void openLabTable(BlockPos const &);                                                              // 3040
  virtual void openElementConstructor(BlockPos const &);                                                    // 3048
  virtual void openCompoundCreator(BlockPos const &);                                                       // 3056
  virtual void openMaterialReducer(BlockPos const &);                                                       // 3064
  virtual void displayChatMessage(std::string const &, std::string const &);                                // 3072
  virtual void displayClientMessage(std::string const &);                                                   // 3080
  virtual void displayLocalizableMessage(std::string const &, std::vector<std::string> const &, bool);      // 3088
  virtual void displayWhisperMessage(std::string const &, std::string const &);                             // 3096
  virtual void startSleepInBed(BlockPos const &);                                                           // 3104
  virtual void stopSleepInBed(bool, bool);                                                                  // 3112
  virtual bool canStartSleepInBed();                                                                        // 3120
  virtual short getSleepTimer() const;                                                                      // 3128
  virtual short getPreviousTickSleepTimer();                                                                // 3136
  virtual bool isLocalPlayer() const;                                                                       // 3152
  virtual bool isHostingPlayer() const;                                                                     // 3160
  virtual bool isLoading() const;                                                                           // 3168
  virtual bool isPlayerInitialized() const;                                                                 // 3176
  virtual void stopLoading();                                                                               // 3184
  virtual void registerTrackedBoss(ActorUniqueID);                                                          // 3192
  virtual void unRegisterTrackedBoss(ActorUniqueID);                                                        // 3200
  virtual void setPlayerGameType(GameType);                                                                 // 3208
  virtual void _crit(Actor &);                                                                              // 3216
  virtual MinecraftEventing *getEventing() const;                                                           // 3224
  virtual int getUserId() const;                                                                            // 3232
  virtual void sendEventPacket(EventPacket const &) const;                                                  // 3240
  virtual void addExperience(int);                                                                          // 3248
  virtual void addLevels(int);                                                                              // 3256
  virtual void setContainerData(IContainerManager &, int, int)                                         = 0; // 3264
  virtual void slotChanged(IContainerManager &, int, ItemInstance const &, ItemInstance const &, bool) = 0; // 3272
  virtual void inventoryChanged(Container &, int, ItemInstance const &, ItemInstance const &);              // 3280
  virtual void refreshContainer(IContainerManager &, std::vector<ItemInstance> const &) = 0;                // 3288
  virtual void deleteContainerManager();                                                                    // 3296
  virtual void setFieldOfViewModifier(float);                                                               // 3304
  virtual bool isPositionRelevant(DimensionId, BlockPos const &);                                           // 3312
  virtual bool isEntityRelevant(Actor const &);                                                             // 3320
  virtual bool isTeacher() const = 0;                                                                       // 3328
  virtual void onSuspension();                                                                              // 3336
  virtual void onLinkedSlotsChanged();                                                                      // 3344
  virtual int getItemCooldownLeft(CooldownType) const;                                                      // 3360
  virtual bool isItemInCooldown(CooldownType) const;                                                        // 3368
  virtual void sendInventoryTransaction(InventoryTransaction const &)                        = 0;           // 3376
  virtual void sendComplexInventoryTransaction(std::unique_ptr<ComplexInventoryTransaction>) = 0;           // 3384
  virtual void sendNetworkPacket(Packet &);                                                                 // 3392
  virtual void chorusFruitTeleport();                                                                       // 3400
  virtual float getUnderwaterVisionClarity();                                                               // 3408
  virtual ServerPlayerEventCoordinator &getPlayerEventCoordinator() = 0;                                    // 3416
  virtual void onMovePlayerPacketNormal(Vec3 const &, Vec2 const &, float);                                 // 3424
};

static_assert(5160 == offsetof(Player, unk5160));
static_assert(5200 == offsetof(Player, b5200));
static_assert(5416 == offsetof(Player, netId));
static_assert(5600 == offsetof(Player, s5600));
static_assert(5704 == offsetof(Player, uuid));
static_assert(5800 == offsetof(Player, chunk_source1));
static_assert(6072 == offsetof(Player, client_sub_id));
static_assert(6112 == offsetof(Player, respawn_ready));
static_assert(6240 == offsetof(Player, item_use_count_down));
static_assert(6304 == offsetof(Player, unk6304));
static_assert(6488 == offsetof(Player, cursor_selected_item_group));
static_assert(6608 == offsetof(Player, inv_trans_mgr));
static_assert(6648 == offsetof(Player, game_mode));
static_assert(6704 == offsetof(Player, forced_respawn));
static_assert(6792 == offsetof(Player, device_id));

struct NetworkHandler;

struct IContainerManager {
  virtual ~IContainerManager();                        // 0, 8
  virtual ContainerID getContainerId() const      = 0; // 16
  virtual void setContainerId(ContainerID)        = 0; // 24
  virtual ContainerType getContainerType()        = 0; // 32
  virtual void setContainerType(ContainerType)    = 0; // 40
  virtual std::vector<ItemInstance> getItems()    = 0; // 48
  virtual void setSlot(int, ItemInstance const &) = 0; // 56
  virtual void setData(int, int)                  = 0; // 72
  virtual void broadcastChanges(void)             = 0; // 80
};
struct BaseContainerMenu : ContainerContentChangeListener, IContainerManager {
  Player *player;                  // 16
  std::vector<ItemInstance> items; // 24
  ContainerID id;                  // 48
  ContainerType type;              // 49
  BaseContainerMenu(Player &player, ContainerType);

  virtual bool containerContentChanged(int) override;           // 0
  virtual ~BaseContainerMenu() override;                        // 8, 16 | (19) 0, 8
  virtual void removeSlot(int, int) = 0;                        // 24
  virtual bool isSlotDirty(int);                                // 32
  virtual bool isResultSlot(int);                               // 40
  virtual std::vector<ItemInstance> getItems() override    = 0; // 48 | (19) 48
  virtual void setSlot(int, ItemInstance const &) override = 0; // 56 | (19) 56
  virtual ItemInstance &getSlot(int)                       = 0; // 64 | (19) 64
  virtual void setData(int, int) override;                      // 72 | (19) 72
  virtual ContainerID getContainerId() const override;          // 80 | (19) 16
  virtual void setContainerId(ContainerID) override;            // 88 | (19) 24
  virtual ContainerType getContainerType() override;            // 96 | (19) 32
  virtual void setContainerType(ContainerType) override;        // 104 | (19) 40
  virtual void broadcastChanges(void) override;                 // 112 | (19) 80
};

struct InventoryMenu : BaseContainerMenu {
  Container *container; // 56
  InventoryMenu(Player &, Container *);

  virtual bool containerContentChanged(int) override;       // 0
  virtual ~InventoryMenu() override;                        // 8, 16 | (19) 0, 8
  virtual void removeSlot(int, int) override;               // 24
  virtual bool isSlotDirty(int) override;                   // 32
  virtual bool isResultSlot(int) override;                  // 40
  virtual std::vector<ItemInstance> getItems() override;    // 48 | (19) 48
  virtual void setSlot(int, ItemInstance const &) override; // 56 | (19) 56
  virtual ItemInstance &getSlot(int) override;              // 64 | (19) 64
  virtual ContainerID getContainerId() const override;      // 80 | (19) 16
  virtual void setContainerId(ContainerID) override;        // 88 | (19) 24
  virtual ContainerType getContainerType() override;        // 96 | (19) 32
  virtual void setContainerType(ContainerType) override;    // 104 | (19) 40
  virtual void broadcastChanges(void) override;             // 112 | (19) 80
};

struct ServerPlayer : Player {
  struct QueuedChunk;
  NetworkHandler *network_handler;                    // 6824
  std::function<void(ServerPlayer &)> callbacks;      // 6832
  InventoryMenu invertory;                            // 6864
  bool b6928;                                         // 6928
  int unk6932;                                        // 6932
  bool b6936;                                         // 6936
  bool b6937;                                         // 6937
  bool b6938;                                         // 6938
  bool b6939;                                         // 6939
  std::unique_ptr<CompoundTag> lost_data;             // 6944
  int unk6952;                                        // 6952
  std::unique_ptr<PlayerChunkSource> chunk_source;    // 6960
  int client_chunk_radius;                            // 6968
  int unk6972;                                        // 6972
  bool b6976;                                         // 6976
  std::vector<QueuedChunk> queued_chunks;             // 6984
  std::unordered_map<ActorUniqueID, Actor *> map7008; // 7008
  std::unordered_set<ActorUniqueID> set7064;          // 7064
  ServerPlayer(Level &, PacketSender &, NetworkHandler &, GameType, NetworkIdentifier const &, unsigned char, std::function<void(ServerPlayer &)>, std::unique_ptr<SkinInfoData>, mce::UUID,
               std::string const &, std::unique_ptr<Certificate>, int);

  void doDeleteContainerManager();
  InventoryMenu &getInventoryMenu();
  void selectItem(ItemInstance const &, int const &);
  void sendMobEffectPackets();
  void setClientChunkRadius(int);
  void setLostDataTag(std::unique_ptr<CompoundTag, std::default_delete<CompoundTag> >);
  void setPlayerInput(float, float, bool, bool);

  void disconnect();
  virtual ~ServerPlayer() override;                                                                              // 56,64
  virtual void normalTick() override;                                                                            // 248
  virtual void push(Vec3 const &) override;                                                                      // 600
  virtual void partialPush(Vec3 const &) override;                                                               // 624
  virtual bool isValidTarget(Actor *) const override;                                                            // 768
  virtual void setArmor(ArmorSlot, ItemInstance const &) override;                                               // 1048
  virtual void changeDimension(DimensionId, bool) override;                                                      // 1240
  virtual ActorUniqueID getControllingPlayer() const override;                                                   // 1256
  virtual void checkFallDamage(float, bool) override;                                                            // 1264
  virtual void handleFallDistanceOnServer(float, bool) override;                                                 // 1280
  virtual void onEffectAdded(MobEffectInstance &) override;                                                      // 1472
  virtual void onEffectUpdated(MobEffectInstance const &) override;                                              // 1480
  virtual void onEffectRemoved(MobEffectInstance &) override;                                                    // 1488
  virtual void die(ActorDamageSource const &) override;                                                          // 1736
  virtual void knockback(Actor *, int, float, float, float, float, float) override;                              // 1936
  virtual void aiStep() override;                                                                                // 2128
  virtual void hurtArmor(int) override;                                                                          // 2384
  virtual void sendInventory(bool) override;                                                                     // 2456
  virtual void prepareRegion(ChunkSource &) override;                                                            // 2688
  virtual void destroyRegion() override;                                                                         // 2969
  virtual void changeDimensionWithCredits(DimensionId) override;                                                 // 2728
  virtual void tickWorld(Tick const &) override;                                                                 // 2736
  virtual void moveView() override;                                                                              // 2752
  virtual void checkMovementStats(Vec3 const &) override;                                                        // 2768
  virtual void setPermissions(CommandPermissionLevel) override;                                                  // 2832
  virtual void openContainer(BlockPos const &) override;                                                         // 2872
  virtual void openContainer(ActorUniqueID const &) override;                                                    // 2880
  virtual void openFurnace(BlockPos const &) override;                                                           // 2888
  virtual void openAnvil(BlockPos const &) override;                                                             // 2904
  virtual void openBrewingStand(BlockPos const &) override;                                                      // 2912
  virtual void openHopper(BlockPos const &) override;                                                            // 2920
  virtual void openHopper(ActorUniqueID const &) override;                                                       // 2928
  virtual void openDispenser(BlockPos const &, bool) override;                                                   // 2936
  virtual void openBeacon(BlockPos const &) override;                                                            // 2944
  virtual void openPortfolio() override;                                                                         // 2952
  virtual void openCommandBlock(BlockPos const &) override;                                                      // 2968
  virtual void openCommandBlockMinecart(ActorUniqueID const &) override;                                         // 2976
  virtual void openHorseInventory(ActorUniqueID const &) override;                                               // 2984
  virtual void openNpcInteractScreen(Actor &) override;                                                          // 3016
  virtual void openInventory() override;                                                                         // 3024
  virtual void openStructureEditor(BlockPos const &) override;                                                   // 3032
  virtual void openLabTable(BlockPos const &) override;                                                          // 3040
  virtual void openElementConstructor(BlockPos const &) override;                                                // 3048
  virtual void openCompoundCreator(BlockPos const &) override;                                                   // 3056
  virtual void openMaterialReducer(BlockPos const &) override;                                                   // 3064
  virtual void displayLocalizableMessage(std::string const &, std::vector<std::string> const &, bool) override;  // 3088
  virtual void displayWhisperMessage(std::string const &, std::string const &) override;                         // 3096
  virtual void stopSleepInBed(bool, bool) override;                                                              // 3112
  virtual bool isHostingPlayer() const override;                                                                 // 3160
  virtual bool isLoading() const override;                                                                       // 3168
  virtual bool isPlayerInitialized() const override;                                                             // 3176
  virtual void setPlayerGameType(GameType) override;                                                             // 3208
  virtual void setContainerData(IContainerManager &, int, int) override;                                         // 3264
  virtual void slotChanged(IContainerManager &, int, ItemInstance const &, ItemInstance const &, bool) override; // 3272
  virtual void refreshContainer(IContainerManager &, std::vector<ItemInstance> const &) override;                // 3288
  virtual void deleteContainerManager() override;                                                                // 3296
  virtual bool isPositionRelevant(DimensionId, BlockPos const &) override;                                       // 3312
  virtual bool isEntityRelevant(Actor const &) override;                                                         // 3320
  virtual bool isTeacher() const override;                                                                       // 3328
  virtual void onSuspension() override;                                                                          // 3336
  virtual void onLinkedSlotsChanged() override;                                                                  // 3344
  virtual void sendInventoryTransaction(InventoryTransaction const &) override;                                  // 3376
  virtual void sendComplexInventoryTransaction(std::unique_ptr<ComplexInventoryTransaction>) override;           // 3384
  virtual void sendNetworkPacket(Packet &) override;                                                             // 3392
  virtual void chorusFruitTeleport() override;                                                                   // 3400
  virtual ServerPlayerEventCoordinator &getPlayerEventCoordinator() override;                                    // 3416
};