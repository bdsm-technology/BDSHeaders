#pragma once // 6184ragma once

#include "Descriptions.h"
#include "Filter.h"
#include "Util.h"
#include <mutex>
#include <unordered_set>

struct MobDescriptor {
  ActorFilterGroup filterGroup; // 0
  unsigned long long x0, x1;    // 64, 72
  bool flag;                    // 80
  MobDescriptor();
  MobDescriptor(MobDescriptor const &);

  MobDescriptor &operator=(MobDescriptor const &);
  ~MobDescriptor();
};

// TODO
struct GoalDefinition {
  char filler[960];
};

struct ActorDefinitionAttribute {
  std::string name; // 0
  float vf0, vf1;   // 32, 36
  FloatRange range; // 40
  ActorDefinitionAttribute();
  ActorDefinitionAttribute(ActorDefinitionAttribute const &);
  ActorDefinitionAttribute(ActorDefinitionAttribute &&);

  ActorDefinitionAttribute &operator=(ActorDefinitionAttribute const &);
  ActorDefinitionAttribute &operator=(ActorDefinitionAttribute &&);

  ~ActorDefinitionAttribute();
};

struct DefinitionEvent {
  unsigned long long sig;            // 0
  ActorFilterGroup filterGroup;      // 8
  std::string name;                  // 72
  unsigned int type;                 // 104
  std::vector<std::string> vs0, vs1; // 112, 136
  std::vector<DefinitionEvent> subs; // 160

  DefinitionEvent();
  DefinitionEvent(DefinitionEvent const &);
};

struct ActorDefinitionDescriptor {
  std::unordered_set<Util::HashString, Util::HashString::HashFunc> hashStrings; // 0
  IdentifierDescription id_desc;                                                // 56
  std::vector<GoalDefinition> goals;                                            // 96
  std::vector<ActorDefinitionAttribute> attrs;                                  // 120
  std::unordered_map<std::string, DefinitionEvent> eventMap;                    // 144
  char filler[920];                                                             // 200

  ActorDefinitionDescriptor();
  ActorDefinitionDescriptor(ActorDefinitionDescriptor &&);

  ActorDefinitionDescriptor &operator=(ActorDefinitionDescriptor const &);
  ActorDefinitionDescriptor &operator=(ActorDefinitionDescriptor &&);

  void combine(ActorDefinitionDescriptor const &);
  void substract(ActorDefinitionDescriptor const &);
  bool contains(ActorDefinitionDescriptor const &) const;
  bool overlaps(ActorDefinitionDescriptor const &) const;
  bool empty() const;
  void executeEvent(Actor &, std::string const &, VariantParameterList const &);
  bool executeTrigger(Actor &, DefinitionTrigger const &, VariantParameterList const &);
  bool forceExecuteTrigger(Actor &, DefinitionTrigger const &, VariantParameterList const &);
  bool hasComponent(Util::HashString const &) const;

  ~ActorDefinitionDescriptor();
};

struct ActorDefinitionGroup;
struct ActorDefinition;

struct ActorDefinitionPtr {
  ActorDefinitionGroup *group; // 0
  ActorDefinition *def;        // 8
  ActorDefinitionPtr(ActorDefinitionGroup &, ActorDefinition &);
  ActorDefinitionPtr(ActorDefinitionPtr const &);
  ActorDefinitionPtr(ActorDefinitionPtr &&);
  ActorDefinitionPtr();

  ActorDefinitionPtr &operator=(ActorDefinitionPtr const &);
  ActorDefinitionPtr &operator=(ActorDefinitionPtr &&);

  static ActorDefinitionPtr NONE;
  static ActorDefinitionPtr &&_move(ActorDefinitionPtr &&);

  operator bool() const;
  ActorDefinition *operator->();
  ActorDefinition *operator->() const;

  void _deref();
  void onGroupDestroyed();
  void onGroupReloaded();

  ~ActorDefinitionPtr();
};

struct ResourcePackManager;
struct IMinecraftEventing;

struct ActorDefinitionGroup {
  struct EDLWrapper {
    std::unordered_map<std::string, std::unique_ptr<ActorDefinition>> mtd;
    EDLWrapper();
    ~EDLWrapper();
  };
  std::unordered_set<ActorDefinitionPtr *> ptrs;                              // 8
  std::unordered_map<std::string, std::unique_ptr<ActorDefinition>> mapToDef; // 64
  std::unordered_map<std::string, EDLWrapper> mapToEdl;                       // 120
  ResourcePackManager *rpm;                                                   // 176
  std::mutex mtx;                                                             // 184
  IMinecraftEventing *eventing;                                               // 224

  ActorDefinitionGroup(ResourcePackManager &, IMinecraftEventing &);
  bool hasComponent(Util::HashString const &, Json::Value const &, Json::Value const &);
  bool hasPlayerAsTarget(Json::Value const &);
  bool needsUpgrade(Json::Value const &, GameVersion const &);
  ActorDefinitionPtr tryGetDefinition(std::string const &);
  ActorDefinitionPtr tryGetDefinitionGroup(std::string const &);

  virtual ~ActorDefinitionGroup();
};

struct ActorDefinition {
  ActorDefinitionDescriptor mActorDefinitionDescriptor;    // 0
  IdentifierDescription mIdentifierDesc;                   // 1120
  std::vector<GoalDefinition> goals;                       // 1160
  std::vector<ActorDefinitionAttribute> attrs;             // 1184
  std::unordered_map<std::string, DefinitionEvent> events; // 1208
  AttackDescription mAttack;                               // 1264
  MobEffectChangeDescription mMobEffectChange;             // 1320
  AmbientSoundIntervalDescription mAmbientSoundInterval;   // 1376
  BurnsInSunlightDescription mBurnsInSunlight;             // 1392
  CanClimbDescription mCanClimb;                           // 1400
  CanFlyDescription mCanFly;                               // 1408
  CanPowerJumpDescription mCanPowerJump;                   // 1416
  CollisionBoxDescription mCollisionBox;                   // 1424
  ColorDescription mColor;                                 // 1440
  Color2Description mColor2;                               // 1456
  DefaultLookAngleDescription mDefaultLookAngle;           // 1472
  DyeableDescription mDyeable;                             // 1488
  EquipmentTableDescription mEquipmentTable;               // 1528
  FireImmuneDescription mFireImmune;                       // 1592
  FamilyTypeDescription mFamilyType;                       // 1600
  FloatsInLiquidDescription mFloatsInLiquid;               // 1656
  FlyingSpeedDescription mFlyingSpeed;                     // 1664
  FootSizeDescription mFootSize;                           // 1680
  FrictionModifierDescription mFrictionModifier;           // 1696
  HurtWhenWetDescription mHurtWhenWet;                     // 1712
  GroundOffsetDescription mGroundOffset;                   // 1720
  IsBabyDescription mIsBaby;                               // 1736
  IsChargedDescription mIsCharged;                         // 1744
  IsChestedDescription mIsChested;                         // 1752
  IsIgnitedDescription mIsIgnited;                         // 1760
  IsSaddledDescription mIsSaddled;                         // 1768
  IsShakingDescription mIsShaking;                         // 1776
  IsShearedDescription mIsSheared;                         // 1784
  IsStackableDescription mIsStackable;                     // 1792
  IsTamedDescription mIsTamed;                             // 1800
  ItemControlDescription mItemControl;                     // 1808
  LootTableDescription mLootTable;                         // 1864
  PushThroughDescription mPushThrough;                     // 1904
  ScaleDescription mScale;                                 // 1920
  SoundVolumeDescription mSoundVolume;                     // 1936
  WalkAnimationSpeedDescription mWalkAnimationSpeed;       // 1952
  WantsJockeyDescription mWantsJockey;                     // 1968
  WASDControlledDescription mWASDControlled;               // 1976
  OnDeathDescription mOnDeath;                             // 1984
  OnFriendlyAngerDescription mOnFriendlyAnger;             // 2120
  OnHurtDescription mOnHurt;                               // 2256
  OnHurtByPlayerDescription mOnHurtByPlayer;               // 2392
  OnIgniteDescription mOnIgnite;                           // 2528
  OnStartLandingDescription mOnStartLanding;               // 1664
  OnStartTakeoffDescription mOnStartTakeoff;               // 2800
  OnTargetAcquiredDescription mOnTargetAcquired;           // 2936
  OnTargetEscapeDescription mOnTargetEscape;               // 3072
  AddRiderDescription mAddRider;                           // 3208
  AgeableDescription mAgeable;                             // 3312
  AngryDescription mAngry;                                 // 3504
  BalloonableDescription mBalloonable;                     // 3656
  BalloonDescription mBalloon;                             // 3936
  BoostableDescription mBoostable;                         // 3960
  BreatheableDescription mBreatheable;                     // 4008
  StrengthDescription mStrength;                           // 4136
  BreedableDescription mBreedable;                         // 4160
  BribeableDescription mBribeable;                         // 4272
  CommandBlockDescription mCommandBlock;                   // 4336
  ContainerDescription mContainer;                         // 4360
  DamageSensorDescription mDamageSensor;                   // 4384
  EnvironmentSensorDescription mEnvironmentSensor;         // 4416
  EquippableDescription mEquippable;                       // 4448
  ExplodableDescription mExplodable;                       // 4480
  NavigationFloatDescription mNavigationFloat;             // 4512
  HealableDescription mHealable;                           // 4568
  HomeDescription mHome;                                   // 4672
  DamageOverTimeDescription mDamageOverTime;               // 4680
  InteractDescription mInteract;                           // 4696
  LeashableDescription mLeashable;                         // 4728
  LookAtDescription mLookAt;                               // 5008
  MountTameableDescription mMountTameable;                 // 5232
  GenericMoveControlDescription mGenericMoveControl;       // 5496
  AmphibiousMoveControlDescription mAmphibiousMoveControl; // 5512
  MoveControlSwayDescription mMoveControlSway;             // 5528
  MoveControlSkipDescription mMoveControlSkip;             // 5552
  NameableDescription mNameable;                           // 5568
  NavigationWalkDescription mNavigationWalk;               // 5736
  PeekDescription mPeek;                                   // 5792
  ProjectileDescription mProjectile;                       // 6184
  RailActivatorDescription mRailActivator;                 // 6536
  RailMovementDescription mRailMovement;                   // 6808
  RideableDescription mRideable;                           // 6824
  ScaleByAgeDescription mScaleByAge;                       // 6960
  SittableDescription mSittable;                           // 6976
  SlimeMoveControlDescription mSlimeMoveControl;           // 7240
  MoveControlBasicDescription mMoveControlBasic;           // 7264
  SpawnActorDescription mSpawnActor;                       // 7280
  ShareableDescription mShareable;                         // 7408
  ShooterDescription mShooter;                             // 7440
  TameableDescription mTameable;                           // 7488
  TargetNearbyDescription mTargetNearby;                   // 7680
  TeleportDescription mTeleport;                           // 7952
  TickWorldDescription mTickWorld;                         // 8000
  TimerDescription mTimer;                                 // 8024
  TradeableDescription mTradeable;                         // 8176
  TransformationDescription mTransformation;               // 8248
  VariantDescription mVariant;                             // 8480
  MarkVariantDescription mMarkVariant;                     // 8496
  NavigationGenericDescription mNavigationGeneric;         // 8512
  NavigationClimbDescription mNavigationClimb;             // 8568
  NavigationSwimDescription mNavigationSwim;               // 8624
  FlockingDescription mFlocking;                           // 8712
  BucketableDescription mBucketable;                       // 8816
  BehaviorTreeDescription mBehaviorTree;                   // 8824
  PhysicsDescription mPhysics;                             // 8864
  NavigationFlyDescription mNavigationFly;                 // 8880
  MoveControlFlyDescription mMoveControlFly;               // 8936
  MoveControlDolphinDescription mMoveControlDolphin;       // 8952
  GlideMoveControlDescription mGlideMoveControl;           // 8968
  InsomniaDescription mInsomnia;                           // 8992
  DynamicJumpControlDescription mDynamicJumpControl;       // 9008
  HopperDescription mHopper;                               // 9016
  JumpControlDescription mJumpControl;                     // 9024
  NpcDescription mNpc;                                     // 9040
  TripodCameraDescription mTripodCamera;                   // 9048
};
static_assert(1264 == offsetof(ActorDefinition, mAttack));
static_assert(2120 == offsetof(ActorDefinition, mOnFriendlyAnger));
static_assert(3072 == offsetof(ActorDefinition, mOnTargetEscape));
static_assert(4008 == offsetof(ActorDefinition, mBreatheable));
static_assert(5008 == offsetof(ActorDefinition, mLookAt));
static_assert(6184 == offsetof(ActorDefinition, mProjectile));
static_assert(7240 == offsetof(ActorDefinition, mSlimeMoveControl));
static_assert(8000 == offsetof(ActorDefinition, mTickWorld));
static_assert(9008 == offsetof(ActorDefinition, mDynamicJumpControl));
static_assert(9048 == offsetof(ActorDefinition, mTripodCamera));