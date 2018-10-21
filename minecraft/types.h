#pragma once

#include "UUID.h"
#include "json.h"
#include <memory>
#include <string>

struct PlayerStorageIds {
  std::string strs[6];
  PlayerStorageIds();
  ~PlayerStorageIds();
};

struct AdventureSettings {
  bool pvpEnabled;     // 0
  bool peaceful;       // 1
  bool immutableWorld; // 2
  bool b3;             // 3
  bool b4;             // 4
};

struct SemVersion {
  unsigned short major, minor, patch; // 0, 2, 4
  std::string s8, s40, s72;           // 8, 40, 72
  bool b104, valid, b106;             // 104, 105, 106
};

struct ListTag;

struct GameVersion {
  unsigned val[4];
  GameVersion();
  GameVersion(unsigned, unsigned, unsigned, unsigned);
  GameVersion(ListTag const &);

  static GameVersion current();

  int compare(GameVersion const &) const;
  bool operator!=(GameVersion const &) const;
  bool operator==(GameVersion const &) const;
  bool operator<(GameVersion const &) const;
  bool operator<=(GameVersion const &) const;

  ListTag serialize() const;
};

struct Tick {
  unsigned long timeStamp;
  Tick(unsigned long);
  Tick(Tick const &);
  Tick();

  unsigned long getTimeStamp() const;
  Tick operator%(int) const;
  Tick operator+(int) const;
  Tick operator++();
  bool operator<(Tick const &) const;
  bool operator>(Tick const &) const;
  Tick &operator=(Tick const &);
};

struct BlockPos;
struct Vec3;
struct Actor;
struct Mob;

struct ChunkPos {
  int x, z;
  ChunkPos();
  ChunkPos(int, int);
  ChunkPos(BlockPos const &);
  ChunkPos(Vec3 const &);

  static ChunkPos INVALID;
  static ChunkPos MAX;
  static ChunkPos MIN;
  static ChunkPos const &max(ChunkPos const &, ChunkPos const &);
  static ChunkPos const &min(ChunkPos const &, ChunkPos const &);

  ChunkPos &operator=(ChunkPos const &);
  bool operator==(ChunkPos const &) const;
  bool operator!=(ChunkPos const &) const;
  ChunkPos const &operator+(ChunkPos const &) const;
  ChunkPos const &operator+(int) const;
  ChunkPos const &operator-(ChunkPos const &) const;
  ChunkPos const &operator-(int) const;

  float distanceToSqr(Actor const &);
  float distanceToSqr(ChunkPos const &);
  BlockPos getMiddleBlockPosition(int);
  int getMiddleBlockX();
  int getMiddleBlockZ();
  size_t hashCode();
};

struct Facing {
  enum Direction : char { DOWN, UP, NORTH, SOUTH, WEST, EAST };
  enum Rotation { NONE, CounterClockWise, Opposite, ClockWise };
  static char const *toString(Direction);
};

struct BlockPos {
  int x, y, z;
  BlockPos();
  BlockPos(int, int, int);
  BlockPos(BlockPos const &);
  BlockPos(ChunkPos const &, int);
  BlockPos(Vec3 const &);
  BlockPos(float, float, float);
  BlockPos(int);

  static BlockPos MAX;
  static BlockPos MIN;
  static BlockPos ONE;
  static BlockPos ZERO;
  static BlockPos const &max(BlockPos const &, BlockPos const &);
  static BlockPos const &min(BlockPos const &, BlockPos const &);

  bool operator==(BlockPos const &) const;
  bool operator!=(BlockPos const &) const;
  BlockPos operator+(BlockPos const &) const;
  BlockPos operator+(int) const;
  BlockPos operator-(BlockPos const &) const;
  BlockPos operator-(int) const;
  BlockPos operator-() const;
  BlockPos operator*(int)const;
  BlockPos const &operator=(BlockPos const &);
  BlockPos const &operator+=(BlockPos const &);
  BlockPos const &operator-=(BlockPos const &);
  BlockPos const &operator*=(int);

  BlockPos above() const;
  BlockPos above(int) const;
  BlockPos below() const;
  BlockPos below(int) const;
  BlockPos east() const;
  BlockPos east(int) const;
  BlockPos south() const;
  BlockPos south(int) const;
  BlockPos west() const;
  BlockPos west(int) const;
  BlockPos north() const;
  BlockPos north(int) const;
  BlockPos neighbor(Facing::Direction);

  void set(BlockPos const &);
  void set(int, int, int);

  Vec3 center() const;
  float distSqr(BlockPos const &);
  float distSqr(float, float, float);
};

struct Vec2 {
  float x, z; // 0, 4
};

struct Vec3 {
  float x, y, z; // 0, 4, 8
};

struct ContentIdentity {
  mce::UUID uuid;
  bool valid;

  static ContentIdentity EMPTY;
  ContentIdentity();
  ContentIdentity(ContentIdentity const &);
  ContentIdentity(ContentIdentity &&);
  ContentIdentity(mce::UUID const &);

  static ContentIdentity fromString(std::string const &);
  std::string asString() const;

  void setUUID(mce::UUID const &);
  mce::UUID getAsUUID() const;

  bool isValid() const;
  bool operator==(ContentIdentity const &) const;
  bool operator!=(ContentIdentity const &) const;
  bool operator<(ContentIdentity const &) const;

  void operator=(ContentIdentity const &);
  void operator=(ContentIdentity &&);
};

enum struct PackType : char { ResourcePack, BehaviorPack };

struct PackIdVersion {
  mce::UUID uuid;
  SemVersion version;
  PackType type;

  PackIdVersion();
  PackIdVersion(PackIdVersion const &);
  PackIdVersion(PackIdVersion &&);
  PackIdVersion(mce::UUID const &, SemVersion const &, PackType);
  PackIdVersion(std::string const &, std::string const &, PackType);

  bool operator==(PackIdVersion const &) const;
  bool operator!=(PackIdVersion const &) const;
  bool operator<(PackIdVersion const &) const;
  PackIdVersion &operator=(PackIdVersion const &);
  PackIdVersion &operator=(PackIdVersion &&);

  bool satisfies(PackIdVersion const &) const;
  std::string asString() const;
};

static_assert(sizeof(PackIdVersion) == 136);

struct PackInstanceId {
  PackIdVersion version;
  std::string desc;

  PackInstanceId();
  PackInstanceId(PackInstanceId &&);
  PackInstanceId(PackInstanceId const &);
  PackInstanceId(PackIdVersion, std::string const &);

  PackInstanceId &operator=(PackInstanceId const &);
  bool operator==(PackInstanceId const &) const;
};

struct Random;

struct FloatRange {
  float low, high;
  void getValue(Random &);
  bool parseJson(Json::Value, float, float);
};

enum struct Difficulty { Peaceful, Easy, Normal, Hard };
enum struct GameType { Survival, Creative };
enum struct GeneratorType { Old, Normal, Flat };
enum struct StorageVersion {};

namespace Social {
enum struct GamePublishSetting {};
}

enum struct ActorType {};
enum struct ParticleType {};
enum struct ContainerType : char {
  NONE                   = -9,
  INVENTORY              = -1,
  CONTAINER              = 0,
  WORKBENCH              = 1,
  FURNACE                = 2,
  ENCHANTMENT            = 3,
  BREWING_STAND          = 4,
  ANVIL                  = 5,
  DISPENSER              = 6,
  DROPPER                = 7,
  HOPPER                 = 8,
  CAULDRON               = 9,
  MINECART_CHEST         = 10,
  MINECART_HOPPER        = 11,
  HORSE                  = 12,
  MINECART_COMMAND_BLOCK = 16,
  JUKEBOX                = 17, // COMPOUND_CREATOR
  ELEMENT_CONSTRUCTOR    = 21,
  MATERIAL_REDUCER       = 22,
  LAB_TABLE              = 23,
};

struct ActorDefinitionIdentifier {
  std::string ns;
  std::string name;
  std::string desc;

  ActorDefinitionIdentifier();
  ActorDefinitionIdentifier(ActorType);
  ActorDefinitionIdentifier(char const *);
  ActorDefinitionIdentifier(std::string const &);
  ActorDefinitionIdentifier(std::string const &, std::string const &, std::string const &);
  ActorDefinitionIdentifier(ActorDefinitionIdentifier const &);
  ActorDefinitionIdentifier(ActorDefinitionIdentifier &&);

  static std::string NAMESPACE_SEPARATOR;
  static std::string EVENT_BEGIN, EVENT_END;

  ActorDefinitionIdentifier &operator=(ActorDefinitionIdentifier const &);
  bool operator==(ActorDefinitionIdentifier const &) const;

  std::string getFullName() const;
  std::string getCanonicalName() const;

  ~ActorDefinitionIdentifier();
};

namespace ClassID {
template <typename T> std::size_t getID();
}

enum struct FilterParamType : int {};
enum struct FilterSubject : short {};
enum struct FilterOperator : short { EQ = 0, NE = 1, GT = 2, LT = 3, GE = 4, LE = 5 };
enum struct FilterParamRequirement : int {};

struct VariantParameterList {
  struct Parameter {
    std::size_t type;
    void *ptr;
    Parameter();
  };
  Parameter params[6];
  VariantParameterList();
  void clear();
  void clearParameter(FilterSubject);
  template <typename T> T *getParameter(FilterSubject) const;
  template <typename T> void setParameter(FilterSubject, T *) const;
  short getParameterCount() const;
  std::size_t getParameterType(FilterSubject) const;
  bool hasParameter(FilterSubject) const;
};

enum struct DataItemType : char {};
enum struct CreativeItemCategory {};
enum struct UseAnimation : char {};
enum struct BlockShape { invalid = -1 };
enum struct CooldownType { invalid = -1, chorus = 0, enderpear = 1, ice_bomb = 2 };

struct Color {
  float r, g, b, a;

  Color(float, float, float, float);
  Color();

  template <typename Num> static Color from255Range(Num, Num, Num, Num);
  static Color fromARGB(int);
  static Color fromRGB(int);
  static Color fromHSB(float, float, float);
  static Color fromIntensity(float, float);

  static Color lerp(Color const &, Color const &, float);

  bool isNil() const;

  bool operator!=(Color const &) const;
  bool operator==(Color const &) const;
  Color operator*(Color const &)const;
  Color operator*(float)const;
  Color operator/(float) const;
  Color &operator+=(Color const &);
  float &operator[](int) const;

  Color brighter(float) const;
  void clamp();
  Color clamped() const;
  float distanceFrom(Color const &);

  unsigned toARGB() const;
  unsigned toABGR() const;
};

template <typename T> struct NewType {
  T value;
  NewType();
  NewType(T const &);
  NewType(NewType const &);
  operator T const &();
  bool operator<(NewType const &);
  bool operator==(NewType const &);
  NewType &operator=(NewType const &);
};

struct NewBlockID : NewType<unsigned short> {
  NewBlockID(unsigned short);
};

template <typename Type, typename Store> struct AutomaticID {
  Store v;
  AutomaticID();
  AutomaticID(Store v)
      : v(v) {}
  Store value() const;
  bool operator!=(AutomaticID const &) const;
  bool operator==(AutomaticID const &) const;
  operator Store() const { return v; }
  static Store _makeRuntimeID();
};

struct Dimension;
struct Biome;
using DimensionId = AutomaticID<Dimension, int>;
using BiomeId     = AutomaticID<Biome, int>;

struct EntityId {
  unsigned id;
  EntityId(std::size_t);
  operator unsigned &();
  operator unsigned const() const;
  bool operator==(EntityId const &) const;
};

template <typename TValue> struct OwnerStorageSharePtr {
  enum struct EmptyInit;
  enum struct VariadicInit;

  std::shared_ptr<TValue> ptr;
  template <typename... PS> OwnerStorageSharePtr(PS...);
  TValue &_getStackRef();
  bool _hasValue() const;
};

template <typename TValue> struct WeakStorageSharePtr {
  enum struct EmptyInit;
  enum struct VariadicInit;
  std::weak_ptr<TValue> ptr;
  WeakStorageSharePtr(EmptyInit);
  WeakStorageSharePtr(VariadicInit, std::weak_ptr<TValue>);

  WeakStorageSharePtr &operator=(WeakStorageSharePtr &&);

  bool _isSet() const;
  void _reset();

  ~WeakStorageSharePtr();
};

template <typename Traits> struct OwnerPtrT {
  typename Traits::ref ptr;
  OwnerPtrT();
  template <typename... PS> OwnerPtrT(PS...);
  bool hasValue() const;
  operator bool() const;
  template <typename R> void remake(R &);
  template <typename... PS> static OwnerPtrT make(PS...);
  void reset();
  typename Traits::ref_value &value();
};

template <typename Traits> struct StackResultStorageSharePtr {
  WeakStorageSharePtr<typename Traits::ref> ptr;
  StackResultStorageSharePtr(WeakStorageSharePtr<typename Traits::ref>);
  typename Traits::ref_value &_getStackRef();
  bool _hasValue() const;
};

template <typename Traits> struct StackRefResultT {
  operator bool() const;
  typename Traits::ref_value *operator->();
  typename Traits::ref_value &value();
  ~StackRefResultT();
};

template <typename Traits> struct WeakRefT {
  template <typename INP> WeakRefT(typename Traits::weak_ref::VariadicInit, INP);
  template <typename INP> static WeakRefT _make(INP);
  bool isSet() const;
  WeakRefT &operator=(WeakRefT &&);
  void reset();
  StackRefResultT<Traits> unwrap();
  ~WeakRefT();
};

struct DimensionConversionData;
struct Level;
struct Scheduler;

struct VanillaDimensions {
  static DimensionId Overworld;
  static DimensionId Nether;
  static DimensionId TheEnd;
  static DimensionId Undefined;

  static bool convertPointBetweenDimension(Vec3 const &, Vec3 &, DimensionId from, DimensionId to, DimensionConversionData const &);
  static void createNew(DimensionId, Level &, Scheduler &);
  static DimensionId const &fromSerializedInt(int);
  static int toSerializedInt(DimensionId const &);
};

struct ActorUniqueID {
  std::int64_t data;
  ActorUniqueID(std::int64_t);
  ActorUniqueID();

  static ActorUniqueID fromClientId(std::int64_t);
  static ActorUniqueID fromUUID(mce::UUID const &);
  std::size_t getHash() const;
  operator bool() const;
  operator std::int64_t() const;

  bool operator!=(ActorUniqueID const &) const;
  bool operator==(ActorUniqueID const &) const;
  bool operator<(ActorUniqueID const &) const;
  ActorUniqueID &operator++();
};

struct ActorRuntimeID {
  std::int64_t data;
  ActorRuntimeID(std::int64_t);
  ActorRuntimeID();
  std::size_t getHash() const;
  operator bool() const;
  bool operator!=(ActorUniqueID const &) const;
  ActorUniqueID &operator++();
};

struct Random {
  char filler[2520];
};

struct Amplifier {
  virtual ~Amplifier();
  virtual double getAmount(int, float) const;
  virtual bool shouldBuff(int, int) const;
  virtual std::size_t getTickInterval(int) const;
};

struct HitResult {
  enum struct Type { Tile = 0, DownOverflow = 2, UpOverflow = 3, Entity = 6 };
  Vec3 unk0, unk12;                // 0, 12
  Type type;                       // 24
  Facing::Direction facing;        // 28
  BlockPos block_pos;              // 32
  Vec3 pos;                        // 44
  Actor *actor;                    // 56
  bool hitLiquid;                  // 64
  Facing::Direction liquid_facing; // 65
  BlockPos liquid;                 // 68
  Vec3 liquid_pos;                 // 80
  bool unk92;                      // 92

  HitResult();
  HitResult(HitResult const &);
  HitResult(HitResult &&);
  HitResult(Vec3 const &, Vec3 const &, Actor const &);
  HitResult(Vec3 const &, Vec3 const &, Actor const &, Vec3 const &);
  HitResult(Vec3 const &, Vec3 const &, HitResult const &);
  HitResult(Vec3 const &, Vec3 const &, Vec3 const &);
  HitResult(Vec3 const &, Vec3 const &, BlockPos const &, signed char, Vec3 const &);

  HitResult &operator=(HitResult const &);
  HitResult &operator=(HitResult &&);

  float distanceTo(Actor const &) const;
  BlockPos const &getBlock() const;
  Actor *getEntity() const;
  Facing::Direction getFacing() const;
  Type getHitResultType() const;
  BlockPos const &getLiquid() const;
  Facing::Direction getLiquidFacing() const;
  Vec3 const &getLiquidPos() const;
  Vec3 const &getPos() const;
  bool isEntity() const;
  bool isHit() const;
  bool isHitLiquid() const;
  bool isTile() const;

  void resetHitEntity();
  void resetHitType();
  void setIsHitLiquid(bool, HitResult);
  void setOutOfRange();
};

struct AABB {
  Vec3 a, b;
  bool valid;
  static AABB EMPTY;
  static bool axisInside(AABB const &, Vec3);
  static AABB fromPoints(Vec3 const &, Vec3 const &);
  static AABB shrink(Vec3 const &);

  AABB(Vec3 const &, Vec3 const &);
  AABB(Vec3 const &, float);
  AABB(float, float, float, float, float, float);
  AABB();

  AABB operator=(AABB const &);
  AABB operator=(AABB &&);
  bool operator==(AABB const &) const;

  void set(AABB const &);
  void set(Vec3 const &, Vec3 const &);

  void centerAt(Vec3 const &);
  HitResult clip(Vec3 const &, Vec3 const &);
  Vec3 clipCollide(AABB const &, Vec3 const &, bool, float *) const;
  float clipXCollide(AABB const &, float, bool) const;
  float clipYCollide(AABB const &, float, bool) const;
  float clipZCollide(AABB const &, float, bool) const;
  AABB cloneMove(Vec3 const &) const;

  bool contains(AABB const &) const;
  bool contains(Vec3 const &) const;
  bool containsX(Vec3 const &) const;
  bool containsY(Vec3 const &) const;
  bool containsZ(Vec3 const &) const;

  float distanceTo(AABB const &) const;
  float distanceTo(Vec3 const &) const;
  float distanceToSqr(AABB const &) const;
  float distanceToSqr(Vec3 const &) const;

  void expand(Vec3 const &);
  bool expanded(Vec3 const &) const;

  AABB flooredCeiledCopy() const;
  AABB flooredCopy() const;

  Vec3 getBounds() const;
  Vec3 getCenter() const;
  float getSize() const;
  Vec3 getSmallestAxisOut(AABB const &) const;
  float getVolume() const;

  bool intersectSegment(Vec3 const &, Vec3 const &, Vec3 &, Vec3 &) const;
  bool intersects(AABB const &) const;
  bool intersects(Vec3 const &, Vec3 const &) const;
  bool intersectsInner(AABB const &) const;

  bool isEmpty() const;
  bool isNan() const;

  void move(Vec3 const &);
  void move(float, float, float);
  void resize(Vec3 const &);

  AABB grow(Vec3 const &) const;
  AABB merge(AABB const &) const;
  AABB translated(Vec3 const &) const;
};

struct AABBShapeComponent {
  AABB aabb; // 0
  Vec2 dim;  // 28
  AABB const &getAABB() const;
  void setAABB(AABB const &);
  Vec2 const &getAABBDim() const;
  void setAABBDim(Vec2 const &);
};

struct StateVectorComponent {
  Vec3 pos0;     // 0
  Vec3 pos;      // 12
  Vec3 posPrev2; // 24
  Vec3 const &getPos();
  Vec3 const &getPosPrev();
  Vec3 const &getPosPrev2();
};

enum class BossBarColor {};
enum class BossBarOverlay {};

template <typename TKey, typename TValue> struct BidirectionalUnorderedMap {
  std::unordered_map<TKey, TValue> left;
  std::unordered_map<TValue, TKey> right;
  BidirectionalUnorderedMap(std::unordered_map<TKey, TValue> const &);
  typename std::unordered_map<TKey, TValue>::const_iterator endLeftKey() const;
  typename std::unordered_map<TValue, TKey>::const_iterator endRightKey() const;
  typename std::unordered_map<TKey, TValue>::const_iterator find(TKey const &) const;
  typename std::unordered_map<TValue, TKey>::const_iterator find(TValue const &) const;
  ~BidirectionalUnorderedMap();
};

struct HashType64 {
  std::int64_t value;
  static HashType64 NullHashType64;
  bool operator==(HashType64 const &) const;
};

namespace std {
template <> struct hash<HashType64> {
  std::size_t operator()(HashType64 const &v) const { return v.value; }
};
} // namespace std

struct HashedString {
  std::string str; // 0
  HashType64 hash; // 32
  static HashedString EMPTY_STRING;
  static HashType64 hash64(char const *);
  static HashType64 hash64(std::string const &);

  HashedString(HashedString const &);
  HashedString(HashedString &&);
  HashedString(char const *);
  HashedString(std::string const &);
  HashedString();

  HashedString &operator=(HashedString const &);
  HashedString &operator=(HashedString &&);
  bool operator==(HashedString const &);
  bool operator==(HashType64 const &);
  bool operator==(std::string const &);

  ~HashedString();
};

struct MolangVariable {
  std::string key; // 0
  float val1;      // 32
  int unk36;       // 36
  float val2;      // 40
  MolangVariable();
  ~MolangVariable();
};

struct MolangVariableMap {
  std::unordered_map<HashType64, MolangVariable> map;
  MolangVariableMap();
  float getVariable(HashType64);
  float getVariable(HashedString const &);
  std::unordered_map<HashType64, MolangVariable> const &getVariables() const;
  void setVariable(HashedString const &, float);
  ~MolangVariableMap();
};

enum struct InputMode {};
enum struct MaterialType {};
enum struct ArmorSlot {};
enum struct Options {};
enum struct ActorFlags {};
enum struct ActorCategory {};
enum struct ActionEvent {};
enum struct PaletteColor : char {
  WHITE,
  ORANGE,
  MAGENTA,
  LIGHT_BLUE,
  YELLOW,
  LIME, // LIGHT_GREEN
  PINK,
  GRAY,
  SILVER,
  CYAN,
  PURPLE,
  BLUE,
  BROWN,
  GREEN,
  RED,
  BLACK,
};