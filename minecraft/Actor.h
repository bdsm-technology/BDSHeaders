#pragma once

#include "ActorDefinition.h"
#include "Attribute.h"
#include "Components.h"
#include "Container.h"
#include "DataItem.h"
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
    EntityContextStackRef context;
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
  bool unk32;                                                  // 32
  VariantParameterList vplist;                                 // 40
  DimensionId dim;                                             // 68
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
  int unk236;                                                  // 236
  SynchedActorData data;                                       // 240
  std::unique_ptr<SpatialActorNetworkData> actorNetworkData;   // 272
  Vec3 pos2;                                                   // 280
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
  SimpleContainer handContainer;                               // 4232 size: 2
  bool b4512;                                                  // 4512
  std::vector<AABB> aabbs;                                     // 4520
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
  bool testForCollidableMobs(BlockSource &,AABB const&,std::vector<AABB,std::allocator<AABB>> &);
  bool testForEntityStacking(BlockSource &,AABB const&,std::vector<AABB,std::allocator<AABB>> &);
  bool tick(BlockSource &);
  void transferTickingArea(Dimension &);
  void updateBBFromDescription();
  void updateDescription();
  void updateInBubbleColumnState();
  void updateInvisibilityStatus();
  void updateTickingData();
  bool wantsMoreFood();

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
  virtual bool findAttackTarget(void);                                                           // 760
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
  virtual void despawn(void);                                                                    // 1024
  virtual void killed(Actor &);                                                                  // 1032
  virtual void awardKillScore(Actor &, int);                                                     // 1040
  virtual void setArmor(ArmorSlot, ItemInstance const &);                                        // 1048
  virtual ItemInstance const &getArmor(ArmorSlot) const;                                         // 1056
  virtual void setEquippedSlot(ArmorSlot, int, int);                                             // 1064
  virtual void setEquippedSlot(ArmorSlot, ItemInstance const &);                                 // 1072
  virtual ItemInstance const &getCarriedItem() const;                                            // 1080
  virtual void setCarriedItem(ItemInstance const &);                                             // 1088
  virtual void setOffhandSlot(ItemInstance const &);                                             // 1096
  virtual ItemInstance const &getEquippedTotem(void) const;                                      // 1104
  virtual bool consumeTotem(void);                                                               // 1112
  virtual bool save(CompoundTag &);                                                              // 1120
  virtual bool saveWithoutId(CompoundTag &);                                                     // 1128
  virtual bool load(CompoundTag const &);                                                        // 1136
  virtual bool loadLinks(CompoundTag const &, std::vector<ActorLink> &);                         // 1144
  virtual ActorType getEntityTypeId(void);                                                       // 1152
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

#undef DEF_COMPONENT