#pragma once

#include "Util.h"
#include "types.h"

struct Item;
struct ItemInstance;
struct BlockSource;
struct ActorDamageSource;

struct Enchant {
  enum struct Type {
    protection            = 0,
    fire_protection       = 1,
    feather_falling       = 2,
    blast_protection      = 3,
    projectile_protection = 4,
    respiration           = 6,
    depth_strider         = 7,
    aqua_affinity         = 8,
    sharpness             = 9,
    smite                 = 10,
    bane_of_arthropods    = 11,
    knockback             = 12,
    fire_aspect           = 13,
    looting               = 14,
    efficiency            = 15,
    silk_touch            = 16,
    unbreaking            = 17,
    fortune               = 18,
    power                 = 19,
    punch                 = 20,
    flame                 = 21,
    infinity              = 22,
    luck_of_the_sea       = 23,
    lure                  = 24,
    frost_walker          = 25,
    mending               = 26,
    binding               = 27,
    vanishing             = 28,
    impaling              = 29,
    riptide               = 30,
    loyalty               = 31,
    channeling            = 32,
  };
  enum struct Frequency {};
  enum struct Slot {};

  static bool mAllowExperimental;
  static std::vector<std::unique_ptr<Enchant>> mEnchants;

  static void setAllowExperimental();

  static void initEnchants(bool);
  static void teardownEnchants();

  Type type;                 // 8
  Frequency freq;            // 12
  bool lootable;             // 16
  Slot primarySlot;          // 20
  Slot secondarySlot;        // 24
  int compatibility;         // 28
  std::string description;   // 32
  Util::HashString stringId; // 64
  bool experimental;         // 104
  bool disabled;             // 105

  bool canEnchant(ItemInstance const &) const;
  bool canEnchant(int) const;
  bool canPrimaryEnchant(ItemInstance const &) const;
  bool canPrimaryEnchant(int) const;
  bool canSecondaryEnchant(ItemInstance const &) const;
  bool canSecondaryEnchant(int) const;

  Frequency getFrequency() const;
  std::string getDescription() const;
  std::string &getDescriptionId() const;
  Util::HashString getStringId() const;

  bool isAvailable() const;
  bool isLootable() const;
  bool isExperimental() const;
  bool isDisabled() const;

  void setDisabled();
  void setExperimental();

  virtual ~Enchant();
  virtual bool isCompatibleWith(Type) const;
  virtual int getMinCost(int) const;
  virtual int getMaxCost(int) const;
  virtual short getMinLevel(void) const;
  virtual short getMaxLevel(void) const;
  virtual int getDamageProtection(int, ActorDamageSource const &) const;
  virtual float getDamageBonus(int, Actor const &) const;
  virtual void doPostAttack(Actor &, Actor &, int) const;
  virtual void doPostHurt(ItemInstance &, Actor &, Actor &, int) const;
  virtual bool isMeleeDamageEnchant() const;
  virtual bool isProtectionEnchant() const;
  virtual bool isTreasureOnly() const;
};

struct EnchantmentInstance {
  Enchant::Type type;
  int level;

  EnchantmentInstance(Enchant::Type, int);
  EnchantmentInstance();
  Enchant::Type getEnchantType();
  int getEnchantLevel();
  void setEnchantType(Enchant::Type);
  void setEnchantLevel(int);
};

struct ItemEnchants {
  int slot;                                 // 0
  std::vector<EnchantmentInstance> list[3]; // 8

  ItemEnchants(ItemEnchants &&);
  ItemEnchants(int);
  ItemEnchants(int, ListTag const &);

  void addEnchant(EnchantmentInstance);
  void addEnchants(ItemEnchants const &);
  bool canEnchant(EnchantmentInstance);
  bool hasEnchant(Enchant::Type) const;

  std::string getEnchantNames() const;

  int count() const;
  std::vector<EnchantmentInstance> getAllEnchants() const;
  std::vector<EnchantmentInstance> getEnchants(int) const;
  int getSlot() const;
  int getTotalValue(bool);

  ~ItemEnchants();
};
enum struct EquipmentFilter;

namespace EnchantUtils {
int PROTECTIONFACTOR_PRIMARYCAP;
int PROTECTIONFACTOR_SECONDARYCAP;
std::vector<std::string> mEnchantmentNames;

void applyEnchant(ItemInstance &, Enchant::Type, int);
void applyEnchant(ItemInstance &, EnchantmentInstance const &);
void applyEnchant(ItemInstance &, ItemEnchants const &);
unsigned canEnchant(ItemInstance &, Enchant::Type, int);
unsigned canEnchant(ItemInstance &, EnchantmentInstance const &);
bool combineEnchantedItems(ItemInstance const &, ItemInstance const &, ItemInstance &, bool);

int determineActivation(Enchant::Type);
int determineCompatibility(Enchant::Type);

void doPostDamageEffects(Actor &, Actor &);
void doPostHurtEffects(Mob &, Mob &);

ItemInstance generateEnchantedBook(EnchantmentInstance const &);
ItemInstance generateEnchantedBook(ItemEnchants const &);
ItemInstance generateRandomEnchantedBook(int, int);
ItemInstance generateRandomEnchantedBook();
ItemInstance generateRandomEnchantedItem(Item const &, int, int);

void randomlyEnchant(ItemInstance &);
void randomlyEnchant(ItemInstance &, int, int, bool);
void removeEnchants(ItemInstance &);
ItemEnchants selectEnchantments(Item const *, int, int, bool);
std::string getRandomName();
std::string getEnchantNameAndLevel(Enchant::Type, int);

std::vector<EnchantmentInstance> getAvailableEnchantmentResults(Item const &, int, bool);
int getBestEnchantLevel(Enchant::Type, Mob const &, EquipmentFilter);
float getDamageReduction(ActorDamageSource const &, Mob const &);
std::vector<int> getEnchantCosts(ItemInstance const &, int);
int getEnchantLevel(Enchant::Type, ItemInstance const &);
int getEnchantmentId(Util::HashString const &);
std::vector<ItemInstance const *> getEquipmentSet(Mob const &, EquipmentFilter);
std::vector<int> getLegalEnchants(Item const *);
int getLootableRandomEnchantIndex(Random &);
bool getRandomItemWith(Enchant::Type, Mob const &, EquipmentFilter);
ItemInstance const *getRandomItemWithMending(Mob const &);
int getTotalProtectionLevels(Enchant::Type, Mob const &);

int getBookcaseCount(BlockSource &, Vec3 const &);
std::vector<Vec3> getBookCasePositions(BlockSource &, Vec3 const &);
std::vector<Vec3> getEnchantingTablePositions(BlockSource &, Vec3 const &);

bool hasEnchant(Enchant::Type, ItemInstance const &);
bool isCompatibleWith(Enchant::Type, Enchant::Type);
} // namespace EnchantUtils