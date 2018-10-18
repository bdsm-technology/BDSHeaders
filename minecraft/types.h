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
  char const *toString(Direction) const;
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
enum struct ContainerType : int {};

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
  EntityId(unsigned long);
  operator unsigned &();
  operator unsigned const() const;
  bool operator==(EntityId const &) const;
};