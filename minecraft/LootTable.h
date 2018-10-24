#pragma once

#include "types.h"
#include <mutex>
#include <unordered_map>
#include <unordered_set>

struct ResourcePackManager;
struct Level;
struct Actor;
struct Player;
struct ActorDamageSource;
struct LootTable;

enum class ActorTarget { This, KillerEntity, KillerPlayer };

struct LootTableContext {
  struct Builder {
    Level *level;                           // 0
    float luck;                             // 8
    Actor *actor;                           // 16
    Player *player;                         // 24
    ActorDamageSource const *damage_source; // 32

    Builder(Level *);
    Builder &withLuck(float);
    Builder &withThisEntity(Actor *);
    Builder &withKillerPlayer(Player *);
    Builder &withDeathSource(ActorDamageSource *);
    LootTableContext create() const;
  };
  float luck;                                         // 0
  Level *level;                                       // 8
  Actor *actor;                                       // 16
  Player *player;                                     // 24
  ActorDamageSource const *damage_source;             // 32
  std::unordered_set<LootTable const *> visitedTable; // 40

  LootTableContext(float, Level *, Actor *, Player *, ActorDamageSource const *);

  float getLuck() const;
  Level *getLevel() const;
  Actor *getThisEntity() const;
  Player *getKillerPlayer() const;
  ActorDamageSource *getDeathSource() const;
  Actor *getKillerEntity() const;
  Actor *getKillerPet() const;
  Actor *getEntity(ActorTarget) const;

  void addVisitedTable(LootTable const *);
  void removeVisitedTable(LootTable const *);

  ~LootTableContext();
};

struct LootItemCondition {
  virtual ~LootItemCondition();                       // 0, 8
  virtual bool applies(Random &, LootTableContext &); // 16
};

struct ItemInstance;
struct Trade;

struct LootItemFunction {
  std::vector<std::unique_ptr<LootItemCondition>> conditions; // 8
  static std::unique_ptr<LootItemFunction> deserialize(Json::Value);

  LootItemFunction(std::vector<std::unique_ptr<LootItemCondition>> &);

  virtual ~LootItemFunction();                                                     // 0, 8
  virtual bool apply(ItemInstance &, Random &, LootTableContext &) = 0;            // 16
  virtual bool apply(ItemInstance &, Random &, Trade const &, LootTableContext &); // 24
};

struct LootPoolEntry {
  int unk8;                                                   // 8
  int unk12;                                                  // 12
  std::vector<std::unique_ptr<LootItemCondition>> conditions; // 16
  std::unique_ptr<LootPoolEntry> sub;                         // 40

  static std::unique_ptr<LootPoolEntry> deserialize(Json::Value);

  LootPoolEntry(int, int, std::vector<std::unique_ptr<LootItemCondition>> &);
  LootPoolEntry();

  bool createItem(std::vector<ItemInstance> &, Random &, LootTableContext &);
  bool isExperimental(void) const;
  int getWeight(float) const;
  std::vector<std::unique_ptr<LootItemCondition>> const &getConditions() const;

  virtual bool _createItem(std::vector<ItemInstance> &, Random &, LootTableContext &) = 0;
  virtual bool _isExperimental(void) const                                            = 0;
  virtual ~LootPoolEntry();
};

struct LootPoolTiers {
  int initial_range;  // 0
  int bonus_rolls;    // 4
  float bonus_chance; // 8

  static std::unique_ptr<LootPoolTiers> deserialize(Json::Value);

  int getRange() const;
  int getBonusRolls() const;
  int getBonusChance() const;
};

struct RandomValueBounds {
  float min;
  float max;

  static RandomValueBounds deserialize(Json::Value);

  RandomValueBounds();
  float getMin() const;
  float getMax() const;
  int getInt(Random &) const;
  int getFloat(Random &) const;
  bool matchesValues(int);
};

struct LootPool {
  std::vector<std::unique_ptr<LootPoolEntry>> entries;        // 0
  std::vector<std::unique_ptr<LootItemCondition>> conditions; // 24
  std::unique_ptr<LootPoolTiers> tiers;                       // 48
  RandomValueBounds bounds56;                                 // 56
  RandomValueBounds bounds64;                                 // 64

  static LootPool deserialize(Json::Value);

  void addRandomItem(std::vector<ItemInstance> &, Random &, LootTableContext &);
  void addRandomItems(std::vector<ItemInstance> &, Random &, LootTableContext &);
  void addRandomTier(std::vector<ItemInstance> &, Random &, LootTableContext &);

  ~LootPool();
};

struct Container;

struct LootTable {
  std::string dir;                              // 0
  std::vector<std::unique_ptr<LootPool>> pools; // 32

  static LootTable deserialize(Json::Value);

  LootTable(std::string);
  LootTable();

  std::string getDir() const;
  void fill(Container &, Random &, LootTableContext &);
  std::vector<int> getAvailableSlots(Container &, Random &);
  std::vector<ItemInstance> getRandomItems(Random &, LootTableContext &) const;
  void shuffleAndSplitItems(std::vector<ItemInstance> &, int, Random &);

  ~LootTable();
};

struct LootTables {
  std::unordered_map<std::string, std::unique_ptr<LootTable>> map;
  std::mutex mtx;
  LootTables();

  std::unique_ptr<LootTable> lookupByName(std::string const &, ResourcePackManager &);

  ~LootTables();
};