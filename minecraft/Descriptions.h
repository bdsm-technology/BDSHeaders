#pragma once

#include "Documentation.h"
#include "Filter.h"
#include "Util.h"
#include "json.h"
#include "types.h"
#include <set>

struct Description {
  Description();
  Description(Description const &);

  Description &operator=(Description const &);

  static std::map<int, std::string> ID_TO_NAME_MAP;
  static std::map<std::string, int> NAME_TO_ID_MAP;
  static int getId(std::string const &);
  static void registerJsonName(std::string const &);
  static void registerAttributes();
  static void registerProperties();
  static void registerTriggers();
  static void registerComponents();

  void buildBuiltInEventDocumentation(Documentation::Writer &);

  std::string getName();
  void startParsing(Json::Value &, Json::Value &, char const *);
  void parseDescription(Json::Value &);

  virtual char const *getNodeName() const                    = 0;
  virtual char const *getJsonName() const                    = 0;
  virtual char const *getDescription() const                 = 0;
  virtual void getDocumentation(Documentation::Node &) const = 0;
  virtual ~Description();
  virtual void parseData(Json::Value &);
};

struct ComponentDescription : Description {
  ComponentDescription();
  ComponentDescription(ComponentDescription &&);
  ComponentDescription &operator=(ComponentDescription const &);
  ComponentDescription &operator=(ComponentDescription &&);
  virtual char const *getNodeName() const;
  virtual ~ComponentDescription();
};

struct AttributeDescription : Description {
  AttributeDescription();
  virtual char const *getNodeName() const;
  virtual ~AttributeDescription();
};

struct PropertyDescription : Description {
  PropertyDescription();
  virtual char const *getNodeName() const;
  virtual ~PropertyDescription();
};

struct DefinitionTrigger {
  std::string name;             // 0
  std::string desc;             // 32
  ActorFilterGroup filterGroup; // 64

  DefinitionTrigger();
  DefinitionTrigger(DefinitionTrigger &&);
  DefinitionTrigger(DefinitionTrigger const &);

  DefinitionTrigger &operator=(DefinitionTrigger &&);
  DefinitionTrigger &operator=(DefinitionTrigger const &);

  bool canTrigger(Actor const &, VariantParameterList const &) const;

  ~DefinitionTrigger();
};

struct TriggerDescription : Description {
  DefinitionTrigger trigger;
  TriggerDescription();
  virtual char const *getNodeName() const;
  virtual void getDocumentation(Documentation::Node &) const;
  virtual ~TriggerDescription();
  void parseTrigger(Json::Value &);
};

struct IdentifierDescription : ComponentDescription {
  std::string identifier;
  IdentifierDescription(char const *);
  IdentifierDescription(std::string const &);
  IdentifierDescription(IdentifierDescription &&);

  IdentifierDescription &operator=(IdentifierDescription const &);
  IdentifierDescription &operator=(IdentifierDescription &&);

  virtual char const *getJsonName() const;
  virtual char const *getDescription() const;
  virtual void getDocumentation(Documentation::Node &) const;

  void parseIdentifier(Json::Value &);

  virtual ~IdentifierDescription();
};

struct MoveControlDescription : ComponentDescription {
  float max_turn;

  MoveControlDescription();

  virtual void getDocumentation(Documentation::Node &) const;
  virtual void parseData(Json::Value &);

  virtual ~MoveControlDescription();
};

struct NavigationDescription : ComponentDescription {
  std::string name;         // 8 pathfinder
  bool is_amphibious;       // 40
  bool avoid_sun;           // 41
  bool can_pass_doors;      // 42
  bool can_open_doors;      // 43
  bool avoid_water;         // 44
  bool avoid_damage_blocks; // 45
  bool can_float;           // 46
  bool can_sink;            // 47
  bool avoid_portals;       // 48
  bool can_fly;             // 49
  bool can_walk;            // 50
  bool can_swim;            // 51
  bool can_climb;           // 52
  bool can_breach;          // 53
  bool can_jump;            // 54

  NavigationDescription();

  virtual void getDocumentation(Documentation::Node &) const;
  virtual void parseData(Json::Value &);

  virtual ~NavigationDescription();
};

#define BASE_DESC(CLAZZ)                                                                                                                                                                               \
  CLAZZ();                                                                                                                                                                                             \
  virtual char const *getJsonName() const;                                                                                                                                                             \
  virtual char const *getDescription() const;                                                                                                                                                          \
  virtual void getDocumentation(Documentation::Node &) const;                                                                                                                                          \
  virtual void parseData(Json::Value &);                                                                                                                                                               \
  virtual ~CLAZZ()

#define MIN_DESC(CLAZZ)                                                                                                                                                                                \
  CLAZZ();                                                                                                                                                                                             \
  virtual char const *getJsonName() const;                                                                                                                                                             \
  virtual char const *getDescription() const;                                                                                                                                                          \
  virtual void getDocumentation(Documentation::Node &) const;                                                                                                                                          \
  virtual ~CLAZZ()

#define EMPTY_DESC(CLAZZ)                                                                                                                                                                              \
  CLAZZ();                                                                                                                                                                                             \
  virtual char const *getJsonName() const;                                                                                                                                                             \
  virtual char const *getDescription() const;                                                                                                                                                          \
  virtual ~CLAZZ()

struct AttackDescription : AttributeDescription {
  FloatRange damage;       // 8
  std::string effect_name; // 16
  float effect_duration;   // 48

  BASE_DESC(AttackDescription);
};

struct MobEffectInstance;

struct MobEffectChangeDescription : AttributeDescription {
  std::vector<MobEffectInstance> effects; // 8
  std::vector<std::string> removed;       // 32

  BASE_DESC(MobEffectChangeDescription);
};

struct AmbientSoundIntervalDescription : PropertyDescription {
  float value; // 8
  float range; // 12
  BASE_DESC(AmbientSoundIntervalDescription);
};

struct BurnsInSunlightDescription : PropertyDescription {
  MIN_DESC(BurnsInSunlightDescription);
};

struct CanClimbDescription : PropertyDescription {
  MIN_DESC(CanClimbDescription);
};

struct CanFlyDescription : PropertyDescription {
  MIN_DESC(CanFlyDescription);
};

struct CanPowerJumpDescription : PropertyDescription {
  MIN_DESC(CanPowerJumpDescription);
};

struct CollisionBoxDescription : PropertyDescription {
  Vec2 box;
  MIN_DESC(CollisionBoxDescription);
};

struct ColorDescription : PropertyDescription {
  unsigned value;
  BASE_DESC(ColorDescription);
};

struct Color2Description : ColorDescription {
  MIN_DESC(Color2Description);
};

struct DefaultLookAngleDescription : PropertyDescription {
  float value;
  BASE_DESC(DefaultLookAngleDescription);
};

struct DyeableDescription : PropertyDescription {
  std::string interact_text;
  BASE_DESC(DyeableDescription);
};

struct SlotDropChance;

struct EquipmentTableDescription : PropertyDescription {
  std::string table;                            // 8
  std::vector<SlotDropChance> slot_drop_chance; // 40
  BASE_DESC(EquipmentTableDescription);
};

struct FireImmuneDescription : PropertyDescription {
  MIN_DESC(FireImmuneDescription);
};

struct FamilyTypeDescription : PropertyDescription {
  std::set<Util::HashString> family;
  BASE_DESC(FamilyTypeDescription);
};

struct FloatsInLiquidDescription : PropertyDescription {
  MIN_DESC(FloatsInLiquidDescription);
};

struct FlyingSpeedDescription : PropertyDescription {
  float value;
  BASE_DESC(FlyingSpeedDescription);
};

struct FootSizeDescription : PropertyDescription {
  float value;
  BASE_DESC(FootSizeDescription);
};

struct FrictionModifierDescription : PropertyDescription {
  float value;
  BASE_DESC(FrictionModifierDescription);
};

struct HurtWhenWetDescription : PropertyDescription {
  MIN_DESC(HurtWhenWetDescription);
};

struct GroundOffsetDescription : PropertyDescription {
  float value;
  BASE_DESC(GroundOffsetDescription);
};

struct IsBabyDescription : PropertyDescription {
  MIN_DESC(IsBabyDescription);
};

struct IsChargedDescription : PropertyDescription {
  MIN_DESC(IsChargedDescription);
};

struct IsChestedDescription : PropertyDescription {
  MIN_DESC(IsChestedDescription);
};

struct IsIgnitedDescription : PropertyDescription {
  MIN_DESC(IsIgnitedDescription);
};

struct IsSaddledDescription : PropertyDescription {
  MIN_DESC(IsSaddledDescription);
};

struct IsShakingDescription : PropertyDescription {
  MIN_DESC(IsShakingDescription);
};

struct IsShearedDescription : PropertyDescription {
  MIN_DESC(IsShearedDescription);
};

struct IsStackableDescription : PropertyDescription {
  MIN_DESC(IsStackableDescription);
};

struct IsTamedDescription : PropertyDescription {
  MIN_DESC(IsTamedDescription);
};

struct Item;

struct ItemControlDescription : PropertyDescription {
  std::set<Item const *> control_items;
  BASE_DESC(ItemControlDescription);
};

struct LootTableDescription : PropertyDescription {
  std::string table;
  BASE_DESC(LootTableDescription);
};

struct PushThroughDescription : PropertyDescription {
  float value;
  BASE_DESC(PushThroughDescription);
};

struct ScaleDescription : PropertyDescription {
  float value;
  BASE_DESC(ScaleDescription);
};

struct SoundVolumeDescription : PropertyDescription {
  float value;
  BASE_DESC(SoundVolumeDescription);
};

struct WalkAnimationSpeedDescription : PropertyDescription {
  float value;
  BASE_DESC(WalkAnimationSpeedDescription);
};

struct WantsJockeyDescription : PropertyDescription {
  MIN_DESC(WantsJockeyDescription);
};

struct WASDControlledDescription : PropertyDescription {
  MIN_DESC(WASDControlledDescription);
};

struct OnDeathDescription : TriggerDescription {
  EMPTY_DESC(OnDeathDescription);
};

struct OnFriendlyAngerDescription : TriggerDescription {
  EMPTY_DESC(OnFriendlyAngerDescription);
};

struct OnHurtDescription : TriggerDescription {
  EMPTY_DESC(OnHurtDescription);
};

struct OnHurtByPlayerDescription : TriggerDescription {
  EMPTY_DESC(OnHurtByPlayerDescription);
};

struct OnIgniteDescription : TriggerDescription {
  EMPTY_DESC(OnIgniteDescription);
};

struct OnStartLandingDescription : TriggerDescription {
  EMPTY_DESC(OnStartLandingDescription);
};

struct OnStartTakeoffDescription : TriggerDescription {
  EMPTY_DESC(OnStartTakeoffDescription);
};

struct OnTargetAcquiredDescription : TriggerDescription {
  EMPTY_DESC(OnTargetAcquiredDescription);
};

struct OnTargetEscapeDescription : TriggerDescription {
  EMPTY_DESC(OnTargetEscapeDescription);
};

struct AddRiderDescription : ComponentDescription {
  ActorDefinitionIdentifier entity_type;
  BASE_DESC(AddRiderDescription);
};

struct ActorDefinitionFeedItem;

struct AgeableDescription : ComponentDescription {
  float duration;                                  // 8
  std::vector<ActorDefinitionFeedItem> drop_items; // 16
  std::vector<Item const *> feedItems;             // 40
  DefinitionTrigger grow_up;                       // 64

  BASE_DESC(AgeableDescription);
};

struct AngryDescription : ComponentDescription {
  int duration;                 // 8
  int duration_delta;           // 12
  bool broadcastAnger;          // 16
  int broadcastRange;           // 20
  DefinitionTrigger calm_event; // 24

  BASE_DESC(AngryDescription);
};

struct BalloonableDescription : ComponentDescription {
  float soft_distance;            // 8
  float max_distance;             // 12
  float mass;                     // 16
  DefinitionTrigger on_balloon;   // 24
  DefinitionTrigger on_unballoon; // 152

  BASE_DESC(BalloonableDescription);
};

struct BalloonDescription : ComponentDescription {
  Vec3 lift_force;

  BASE_DESC(BalloonDescription);
};

struct BoostItem;

struct BoostableDescription : ComponentDescription {
  std::vector<BoostItem> boost_items; // 8
  float duration;                     // 32
  float speed_multiplier;             // 36
  float unk40;

  BASE_DESC(BoostableDescription);
};

struct Block;

struct BreatheableDescription : ComponentDescription {
  int totalSupply;                          // 8
  int suffocateTime;                        // 12
  float inhaleTime;                         // 16
  bool breathesAir;                         // 20
  bool breathesWater;                       // 21
  bool breathesLava;                        // 22
  bool breathesSolids;                      // 23
  bool generatesBubbles;                    // 24
  std::set<Block const *> breatheBlocks;    // 32
  std::set<Block const *> nonBreatheBlocks; // 80

  BASE_DESC(BreatheableDescription);
};

struct StrengthDescription : AttributeDescription {
  int min, value, max; // 8, 12, 16

  BASE_DESC(StrengthDescription);
};

struct BreedableType {
  std::string mateType;
  std::string babyType;
  DefinitionTrigger breed_event;

  BreedableType();
  BreedableType(BreedableType const &);

  BreedableType &operator=(BreedableType const &);

  ~BreedableType();
};

struct BreedableDescription : ComponentDescription {
  struct MutationFactorData {
    float variant;       // 0
    float extra_variant; // 4
    float color;         // 8
  };

  bool requireTame;                      // 8
  float extraBabyChance;                 // 12
  float breedCooldown;                   // 16
  bool inheritTamed;                     // 20
  bool allowSitting;                     // 21
  std::set<Item const *> breedItems;     // 24
  std::vector<BreedableType> breedsWith; // 72
  MutationFactorData muta;               // 96
  bool causes_pregnancy;                 // 108

  BASE_DESC(BreedableDescription);
};

struct BribeableDescription : ComponentDescription {
  float bribe_cooldown;
  std::set<Item const *> bribe_items;

  BASE_DESC(BribeableDescription);
};

struct CommandBlockDescription : ComponentDescription {
  int ticks_per_commands; // 8
  int current_tick_count; // 12
  bool ticking;           // 16

  BASE_DESC(CommandBlockDescription);
};

struct ContainerDescription : ComponentDescription {
  int inventory_size;                 // 8
  bool can_be_siphoned_from;          // 12
  bool mPrivate;                      // 13
  bool restrict_to_owner;             // 14
  bool additional_slots_per_strength; // 16
  ContainerType container_type;       // 20

  BASE_DESC(ContainerDescription);
};

struct DamageSensorTrigger : DefinitionTrigger {
  bool deals_damage; // 128
  int cause;         // 132

  DamageSensorTrigger();
  DamageSensorTrigger(DamageSensorTrigger const &);
  DamageSensorTrigger(DamageSensorTrigger &&);
  ~DamageSensorTrigger();
};

struct DamageSensorDescription : ComponentDescription {
  std::vector<DamageSensorTrigger> triggers;

  BASE_DESC(DamageSensorDescription);
};

struct EnvironmentSensorDescription : ComponentDescription {
  std::vector<DefinitionTrigger> triggers;

  BASE_DESC(EnvironmentSensorDescription);
};

struct ItemInstance;

struct SlotDescriptor {
  int slot;                                 // 0
  std::vector<ItemInstance> accepted_items; // 8
  std::string interact_text;                // 40
  DefinitionTrigger on_equip;               // 72
  DefinitionTrigger on_unequip;             // 200

  SlotDescriptor();
  SlotDescriptor(SlotDescriptor const &);

  SlotDescriptor &operator=(SlotDescriptor const &);

  ~SlotDescriptor();
};

struct EquippableDescription : ComponentDescription {
  std::vector<SlotDescriptor> slots;

  BASE_DESC(EquippableDescription);
};

struct ExplodableDescription : ComponentDescription {
  FloatRange fuseLength;          // 8
  float power;                    // 16
  float maxResistance;            // 20
  bool fuseLit;                   // 24
  bool causesFire;                // 25
  bool breaks_blocks;             // 26
  bool fireAffectedByGriefing;    // 27
  bool destroyAffectedByGriefing; // 28

  BASE_DESC(ExplodableDescription);
};

struct NavigationFloatDescription : NavigationDescription {
  EMPTY_DESC(NavigationFloatDescription);
};

struct FeedItem;

struct HealableDescription : ComponentDescription {
  std::vector<FeedItem> feeds; // 8
  bool force_use;              // 32
  ActorFilterGroup filters;    // 40

  BASE_DESC(HealableDescription);
};

struct HomeDescription : ComponentDescription {
  BASE_DESC(HomeDescription);
};

struct DamageOverTimeDescription : ComponentDescription {
  int damagePerHurt;
  int timeBetweenHurt;

  BASE_DESC(DamageOverTimeDescription);
};

struct Interaction;

struct InteractDescription : ComponentDescription {
  std::vector<Interaction> value;

  BASE_DESC(InteractDescription);
};

struct LeashableDescription : ComponentDescription {
  float soft_distance;          // 8
  float hard_distance;          // 12
  float max_distance;           // 16
  DefinitionTrigger on_leash;   // 24
  DefinitionTrigger on_unleash; // 152

  BASE_DESC(LeashableDescription);
};

struct LookAtDescription : ComponentDescription {
  bool setTarget;               // 8
  float searchRadius;           // 12
  bool allow_invulnerable;      // 16
  FloatRange look_cooldown;     // 20
  ActorFilterGroup filters;     // 32
  DefinitionTrigger look_event; // 96

  BASE_DESC(LookAtDescription);
};

struct MountTameableDescription : ComponentDescription {
  int minTemper;                             // 8
  int maxTemper;                             // 12
  int attempt_temper_mod;                    // 16
  std::vector<FeedItem> feedItems;           // 24
  std::vector<Item const *> autoRejectItems; // 48
  std::string feed_text;                     // 72
  std::string ride_text;                     // 104
  DefinitionTrigger tame_event;              // 136

  BASE_DESC(MountTameableDescription);
};

struct GenericMoveControlDescription : MoveControlDescription {
  EMPTY_DESC(GenericMoveControlDescription);
};

struct AmphibiousMoveControlDescription : MoveControlDescription {
  EMPTY_DESC(AmphibiousMoveControlDescription);
};

struct MoveControlSwayDescription : MoveControlDescription {
  float sway_frequency; // 12
  float sway_amplitude; // 16

  virtual void parseData(Json::Value &);
  EMPTY_DESC(MoveControlSwayDescription);
};

struct MoveControlSkipDescription : MoveControlDescription {
  EMPTY_DESC(MoveControlSkipDescription);
};

struct NameAction;

struct NameableDescription : ComponentDescription {
  std::vector<NameAction> name_actions;
  DefinitionTrigger default_trigger;
  bool alwaysShow;
  bool allowNameTagRenaming;

  BASE_DESC(NameableDescription);
};

struct NavigationWalkDescription : NavigationDescription {
  EMPTY_DESC(NavigationWalkDescription);
};

struct PeekDescription : ComponentDescription {
  DefinitionTrigger on_close;       // 8
  DefinitionTrigger on_open;        // 136
  DefinitionTrigger on_target_open; // 264

  BASE_DESC(PeekDescription);
};

struct OnHitSubcomponent;

struct ProjectileDescription : ComponentDescription {
  Vec3 offset;                                            // 8
  float power;                                            // 20
  bool knockback;                                         // 24
  bool unk25;                                             // 25
  bool lightning;                                         // 26
  float gravity;                                          // 28
  float angleoffset;                                      // 32
  ParticleType particle;                                  // 36
  float uncertaintyBase;                                  // 40
  float uncertaintyMultiplier;                            // 44
  float onFireTime;                                       // 48
  int potionEffect;                                       // 52
  float splashRange;                                      // 56
  ActorType filter;                                       // 60
  bool catchFire;                                         // 64
  bool splashPotion;                                      // 65
  float inertia;                                          // 68
  float liquid_inertia;                                   // 72
  bool semirandomdiffdamage;                              // 76
  std::string hitSound;                                   // 80
  std::string hitGroundSound;                             // 112
  std::string shootSound;                                 // 144
  int anchor;                                             // 176
  std::vector<std::unique_ptr<OnHitSubcomponent>> onHits; // 184
  DefinitionTrigger onHit;                                // 208
  bool isdangerous;                                       // 336
  bool reflectOnHurt;                                     // 337
  bool destroyOnHurt;                                     // 338
  bool stopOnHurt;                                        // 339
  bool critParticleOnHurt;                                // 340
  bool unk341;                                            // 341
  bool homing;                                            // 342
  bool shouldbounce;                                      // 343
  bool fireAffectedByGriefing;                            // 344
  bool hitWater;                                          // 345
  bool multipletargets;                                   // 346

  BASE_DESC(ProjectileDescription);
};

struct RailActivatorDescription : ComponentDescription {
  DefinitionTrigger on_activate;         // 8
  DefinitionTrigger on_deactivate;       // 136
  bool check_block_types;                // 264
  bool tick_command_block_on_activate;   // 265
  bool tick_command_block_on_deactivate; // 266
  bool eject_on_activate;                // 267
  bool eject_on_deactivate;              // 268

  BASE_DESC(RailActivatorDescription);
};

struct RailMovementDescription : ComponentDescription {
  float max_speed; // 8

  BASE_DESC(RailMovementDescription);
};

struct SeatDescription;

struct RideableDescription : ComponentDescription {
  int seat_count;                          // 8
  int controlling_seat;                    // 12
  bool crouching_skip_interact;            // 16
  std::vector<SeatDescription> seats;      // 24
  std::set<Util::HashString> family_types; // 48
  std::string interact_text;               // 96
  bool pull_in_entities;                   // 128
  bool rider_can_interact;                 // 129

  BASE_DESC(RideableDescription);
};

struct ScaleByAgeDescription : ComponentDescription {
  float start_scale; // 8
  float end_scale;   // 12

  BASE_DESC(ScaleByAgeDescription);
};

struct SittableDescription : ComponentDescription {
  DefinitionTrigger sit_event;
  DefinitionTrigger stand_event;

  BASE_DESC(SittableDescription);
};

struct SlimeMoveControlDescription : MoveControlDescription {
  FloatRange jump_delay;

  BASE_DESC(SlimeMoveControlDescription);
};

struct MoveControlBasicDescription : MoveControlDescription {
  EMPTY_DESC(MoveControlBasicDescription);
};

struct SpawnActorDescription : ComponentDescription {
  bool flag;                // 8
  int min_wait_time;        // 12
  int max_wait_time;        // 16
  std::string spawn_sound;  // 24
  Item const *spawn_item;   // 56
  std::string spawn_entity; // 64
  std::string spawn_method; // 96

  BASE_DESC(SpawnActorDescription);
};

struct Shareable;

struct ShareableDescription : ComponentDescription {
  std::vector<Shareable> items; // 8

  BASE_DESC(ShareableDescription);
};

struct ShooterDescription : ComponentDescription {
  std::string def; // 8
  int aux;         // 40

  BASE_DESC(ShooterDescription);
};

struct TameableDescription : ComponentDescription {
  float probability;                // 8
  std::set<Item const *> tameItems; // 16
  DefinitionTrigger tame_event;     // 64

  BASE_DESC(TameableDescription);
};

struct TargetNearbyDescription : ComponentDescription {
  float inside_range;                 // 8
  float outside_range;                // 12
  DefinitionTrigger on_inside_range;  // 16
  DefinitionTrigger on_outside_range; // 16

  BASE_DESC(TargetNearbyDescription);
};

struct TeleportDescription : ComponentDescription {
  bool randomTeleports;         // 8
  float minRandomTeleportTime;  // 12
  float maxRandomTeleportTime;  // 16
  Vec3 randomTeleportCube;      // 20
  float targetDistance;         // 32
  float target_teleport_chance; // 36
  float lightTeleportChance;    // 40
  float darkTeleportChance;     // 44

  BASE_DESC(TeleportDescription);
};

struct TickWorldDescription : ComponentDescription {
  unsigned radius;           // 8
  float distance_to_players; // 12
  bool never_despawn;        // 16

  BASE_DESC(TickWorldDescription);
};

struct TimerDescription : ComponentDescription {
  bool looping;                      // 8
  bool randomInterval;               // 9
  FloatRange time;                   // 12
  DefinitionTrigger time_down_event; // 24

  BASE_DESC(TimerDescription);
};

struct TradeableDescription : ComponentDescription {
  std::string display_name; // 8
  std::string table;        // 40

  BASE_DESC(TradeableDescription);
};

struct LevelSoundEvent;

struct TransformationDescription : ComponentDescription {
  ActorDefinitionIdentifier into;                     // 8
  float delay;                                        // 104
  bool drop_equipment;                                // 108
  std::vector<LevelSoundEvent> begin_transform_sound; // 112
  std::vector<LevelSoundEvent> transformation_sound;  // 136
  float block_assist_chance;                          // 160
  int block_radius;                                   // 164
  int block_max;                                      // 168
  float block_chance;                                 // 172
  std::vector<std::string> block_types;               // 176
  std::vector<std::string> component_groups;          // 200
  bool keep_owner;                                    // 224

  BASE_DESC(TransformationDescription);
};

struct VariantDescription : PropertyDescription {
  int value;

  BASE_DESC(VariantDescription);
};

struct MarkVariantDescription : PropertyDescription {
  int value;

  BASE_DESC(MarkVariantDescription);
};

struct NavigationGenericDescription : NavigationDescription {
  EMPTY_DESC(NavigationGenericDescription);
};

struct NavigationClimbDescription : NavigationDescription {
  EMPTY_DESC(NavigationClimbDescription);
};

struct NavigationSwimDescription : NavigationDescription {
  bool unk55; // 55
  bool unk56; // 56
  int unk60;  // 60
  int unk64;  // 64
  int unk68;  // 68
  int unk72;  // 72
  int unk76;  // 76
  int unk80;  // 80
  BASE_DESC(NavigationSwimDescription);
};

struct FlockingDescription : ComponentDescription {
  std::string flocking;            // 8
  bool in_water;                   // 40
  bool match_variants;             // 41
  bool use_center_of_mass;         // 42
  int low_flock_limit;             // 44
  int high_flock_limit;            // 48
  float goal_weight;               // 52
  float loner_chance;              // 56
  float influence_radius;          // 60
  float breach_influence;          // 64
  float separation_weight;         // 68
  float separation_threshold;      // 72
  float cohesion_weight;           // 76
  float cohesion_threshold;        // 80
  float innner_cohesion_threshold; // 84
  float min_height;                // 88
  float max_height;                // 92
  float block_distance;            // 96
  float block_weight;              // 100

  BASE_DESC(FlockingDescription);
};

struct BucketableDescription : ComponentDescription {
  BASE_DESC(BucketableDescription);
};

struct BehaviorTreeDescription : ComponentDescription {
  std::string unk; // 8
  MIN_DESC(BehaviorTreeDescription);
};

struct PhysicsDescription : ComponentDescription {
  bool has_gravity;   // 8
  bool has_collision; // 9

  BASE_DESC(PhysicsDescription);
};

struct NavigationFlyDescription : NavigationDescription {
  EMPTY_DESC(NavigationFlyDescription);
};

struct MoveControlFlyDescription : MoveControlDescription {
  EMPTY_DESC(MoveControlFlyDescription);
};

struct MoveControlDolphinDescription : MoveControlDescription {
  EMPTY_DESC(MoveControlDolphinDescription);
};

struct GlideMoveControlDescription : MoveControlDescription {
  float start_speed;        // 12
  float speed_when_turning; // 16
  BASE_DESC(GlideMoveControlDescription);
};

struct InsomniaDescription : ComponentDescription {
  float days_until_insomnia;
  BASE_DESC(InsomniaDescription);
};

struct DynamicJumpControlDescription : ComponentDescription {
  MIN_DESC(DynamicJumpControlDescription);
};

struct HopperDescription : ComponentDescription {
  MIN_DESC(HopperDescription);
};

struct JumpControlDescription : ComponentDescription {
  float jump_power;

  BASE_DESC(JumpControlDescription);
};

struct NpcDescription : ComponentDescription {
  MIN_DESC(NpcDescription);
};

struct TripodCameraDescription : ComponentDescription {
  MIN_DESC(TripodCameraDescription);
};

#undef BASE_DESC
#undef MIN_DESC
#undef EMPTY_DESC
