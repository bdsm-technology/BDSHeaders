#pragma once
#include "Documentation.h"
#include "Enchant.h"
#include "types.h"
#include <chrono>
#include <functional>
#include <mutex>

struct Block;
struct BlockLegacy;
struct Level;
struct ItemInstance;
struct Player;
struct BlockSource;
struct Container;
struct Mob;
struct IDataInput;
struct IDataOutput;
struct TextureUVCoordinateSet;
struct LegacyBlock;
struct CompoundTag;
struct ResourcePackManager;
struct BlockActor;

struct Item;

struct FoodItemComponent {
  struct Effect {
    char *did;          // 0
    unsigned id;        // 8
    unsigned duration;  // 12
    unsigned amplifier; // 16
    float chance;       // 20
  };
  Item *ref;                     // 0
  unsigned nutrition;            // 8
  float saturation_modifier;     // 12
  bool is_meat;                  // 16
  std::string eat_sound;         // 24
  std::string using_converts_to; // 56
  std::vector<Effect> effects;   // 88

  FoodItemComponent(Item &);
  unsigned getNutrition() const;
  float getSaturationModifier() const;
  void init(Json::Value &);
  void use(ItemInstance &, Player &);
  void useOn(ItemInstance &, Actor &, BlockPos const &, signed char, Vec3 const &);
  void useTimeDepleted(ItemInstance &, Player &, Level &);
  ~FoodItemComponent();
};

struct SeedItemComponent {
  Item *ref;    // 0
  Block *x, *y; // 8, 16
  SeedItemComponent(Item &);
  void init(Json::Value &);
  bool canUseOn(Actor &, BlockPos const &, signed char) const;
  void useOn(ItemInstance &, Actor &, BlockPos const &, signed char, Vec3 const &);
  ~SeedItemComponent();
};

struct CameraItemComponent {
  Item *ref;                     // 0
  float black_bars_duration;     // 8
  float black_bars_screen_ratio; // 12
  float unk;                     // 16
  float shutter_duration;        // 20
  float picture_duration;        // 24
  float slide_away_duration;     // 28
  bool first;                    // 32
  std::size_t ts1;               // 40
  std::size_t ts2;               // 48
  void *unk56;                   // 56

  CameraItemComponent(Item &);
  void init(Json::Value &);
  void takePictureNow(Player &, Actor *, Actor *);
  void use(ItemInstance &, Player &);
  void useOn(ItemInstance &, Actor &, BlockPos const &, signed char, Vec3 const &);
  void releaseUsing(ItemInstance &, Player &, int);
  ~CameraItemComponent();
};

struct CreativeGroupInfo {
  std::string name;
  short id;
  short aux;
  std::unique_ptr<CompoundTag> tag;

  CreativeGroupInfo(std::string const &, short, short, CompoundTag const *);
  CreativeGroupInfo(CreativeGroupInfo &&);
  CreativeGroupInfo();

  ~CreativeGroupInfo();
};

struct Texture;
struct TextureAtlas;
struct TextureAtlasItem {
  std::string name;                                     // 0
  int unk32;                                            // 32
  std::vector<std::vector<TextureUVCoordinateSet>> uvs; // 40
  TextureAtlasItem(std::string const &, std::size_t);
  TextureAtlasItem(TextureAtlasItem &&);
  TextureAtlasItem(TextureAtlasItem const &);
  TextureAtlasItem();
  TextureAtlasItem &operator=(TextureAtlasItem &);
  std::vector<TextureUVCoordinateSet> const &operator[](std::size_t) const;
  std::size_t uvCount() const;
};

struct Item {
  struct Tier {
    int level;             // 0
    int uses;              // 4
    float speed;           // 8
    int attackDamageBonus; // 12
    int enchantmentValue;  // 16

    Tier(float, int, int, int, int);

    int getLevel() const;
    int getUses() const;
    float getSpeed() const;
    int getAttackDamageBonus() const;
    int getEnchantmentValue() const;
  };
  unsigned char maxStackSize;                      // 8
  int frameCount;                                  // 12
  std::string atlas;                               // 16
  LegacyBlock *block;                              // 30
  int unk48;                                       // 48
  bool unk52;                                      // 52
  bool mirroredArt;                                // 53
  UseAnimation useAnimation;                       // 54
  long long maxUseDuration;                        // 56
  Texture *iconTexture;                            // 64
  void *unk72;                                     // 72
  bool unk80;                                      // 80
  Vec3 unk84;                                      // 84
  Vec3 unk96;                                      // 96
  unsigned short maxDamage;                        // 108
  short id;                                        // 112
  std::string descriptionId;                       // 120
  std::string rawname;                             // 152
  std::string ns;                                  // 184
  short unk216;                                    // 216
  bool glint;                                      // 218
  bool handEquipped;                               // 219
  bool stackedByData;                              // 220
  char permissions;                                // 221 1-RequiresWorldBuilder
  int unk224;                                      // 224
  bool explodable;                                 // 228
  bool _shouldDespawn;                             // 229
  bool offhand;                                    // 230
  bool unk231;                                     // 231
  bool experimental;                               // 232
  void *unk240;                                    // 240
  CreativeItemCategory creativeItemCategory;       // 248
  void *unk256;                                    // 256
  std::unique_ptr<FoodItemComponent> foodItem;     // 264
  std::unique_ptr<SeedItemComponent> seedItem;     // 272
  std::unique_ptr<CameraItemComponent> cameraItem; // 280
  std::vector<std::function<void()>> onResetBAI;   // 288

  Item(std::string const &, short);

  static void addCreativeItem(Block const &, short);
  static void addCreativeItem(Item const &, short);
  static void addCreativeItem(ItemInstance const &);
  static void addCreativeItem(short, short);

  static void beginCreativeGroup(std::string const &, Block const *, short, CompoundTag const *);
  static void beginCreativeGroup(std::string const &, Item *, short, CompoundTag const *);
  static void beginCreativeGroup(std::string const &, ItemInstance const &);
  static void beginCreativeGroup(std::string const &, short, short, CompoundTag const *);
  static void endCreativeGroup();

  static void initClient(Json::Value &, Json::Value &);
  static void initServer(Json::Value &);
  static void initCreativeItems(bool, std::function<void()>);

  static void setTextureAtlas(std::shared_ptr<TextureAtlas>);

  static bool mAllowExperimental;
  static std::vector<CreativeGroupInfo> mCreativeGroupInfo;
  static std::vector<std::vector<ItemInstance>> mCreativeGroups;
  static std::vector<ItemInstance> mCreativeList;
  static std::mutex mCreativeListMutex;
  static bool mGenerateDenyParticleEffect;
  static bool mInCreativeGroup;
  static TextureUVCoordinateSet mInvalidTextureUVCoordinateSet;
  static std::weak_ptr<TextureAtlas> mItemTextureAtlas;
  static std::string ICON_DESCRIPTION_PREFIX;

  bool findCreativeItem(ItemInstance const &);
  CreativeItemCategory getCreativeCategory() const;

  FoodItemComponent const &getFood() const;
  SeedItemComponent const &getSeed() const;
  CameraItemComponent const &getCamera() const;

  short getId() const;
  NewBlockID toBlockId();
  void setAllowOffhand(bool);
  bool allowOffhand() const;
  LegacyBlock const &getLegacyBlock() const;
  void setIsMirroredArt(bool);
  bool isMirroredArt() const;
  void setExperimental();
  bool isExperimental() const;
  bool isExplodable() const;
  bool shouldDespawn() const;
  bool canUseSeed(Actor &, BlockPos, signed char);
  float destroySpeedBonus(ItemInstance const &);
  int getMaxUseDuration() const;
  UseAnimation getUseAnimation() const;

  std::string getFullName() const;
  std::string const &getNamespace() const;
  std::string const &getRawNameId() const;
  std::string const &getDescriptionId() const;
  std::string getSerializedName() const;
  std::string buildDescriptionName(ItemInstance const &);

  Texture const &getIconTexture() const;
  static TextureUVCoordinateSet &getIconTextureUVSet(TextureAtlasItem const &, int, int);
  static TextureAtlasItem &getTextureItem(std::string const &);
  static TextureUVCoordinateSet getTextureUVCoordinateSet();

  void appendFormattedHovertext(ItemInstance const &, Level &, std::string const &, bool);
  void updateCustomBlockEntityTag(BlockSource &, ItemInstance &, BlockPos &);
  void useOn(ItemInstance &, Actor &, int, int, int, signed char, float, float, float) const;
  void executeOnResetBAIcallbacks();

  virtual ~Item();                                                                                           // 0, 8
  virtual void setMaxStackSize(unsigned char);                                                               // 16
  virtual void setCategory(CreativeItemCategory);                                                            // 24
  virtual void setStackedByData(bool);                                                                       // 32
  virtual void setMaxDamage(int);                                                                            // 40
  virtual void setHandEquipped();                                                                            // 48
  virtual void setUseAnimation(UseAnimation);                                                                // 56
  virtual void setMaxUseDuration(int);                                                                       // 64
  virtual void setRequiresWorldBuilder(bool);                                                                // 72
  virtual void setExplodable(bool);                                                                          // 80
  virtual void setIsGlint(bool);                                                                             // 88
  virtual void setShouldDespawn(bool);                                                                       // 96
  virtual int getBlockShape() const;                                                                         // 104
  virtual bool canBeDepleted() const;                                                                        // 112
  virtual bool canDestroySpecial(Block const &) const;                                                       // 120
  virtual long long getLevelDataForAuxValue(int) const;                                                      // 128
  virtual bool isStackedByData() const;                                                                      // 136
  virtual unsigned short getMaxDamage() const;                                                               // 144
  virtual int getAttackDamage() const;                                                                       // 152
  virtual bool isHandEquipped() const;                                                                       // 160
  virtual bool isArmor() const;                                                                              // 168
  virtual bool isDye() const;                                                                                // 176
  virtual bool isGlint(ItemInstance const &) const;                                                          // 184
  virtual bool isThrowable() const;                                                                          // 192
  virtual bool showsDurabilityInCreative() const;                                                            // 200
  virtual bool canDestroyInCreative() const;                                                                 // 208
  virtual bool isDestructive(int) const;                                                                     // 216
  virtual bool isLiquidClipItem(int) const;                                                                  // 224
  virtual bool requiresInteract() const;                                                                     // 232
  virtual void appendFormattedHovertext(ItemInstance const &, Level &, std::string &, bool) const;           // 240
  virtual bool isValidRepairItem(ItemInstance const &, ItemInstance const &);                                // 248
  virtual Enchant::Slot getEnchantSlot() const;                                                              // 256
  virtual unsigned getEnchantValue() const;                                                                  // 264
  virtual unsigned getArmorValue() const;                                                                    // 272
  virtual bool isComplex() const;                                                                            // 280
  virtual bool isValidAuxValue(int) const;                                                                   // 288
  virtual int getDamageChance(int) const;                                                                    // 296
  virtual bool uniqueAuxValues() const;                                                                      // 304
  virtual Color getColor(ItemInstance const &) const;                                                        // 312
  virtual bool isTintable() const;                                                                           // 320
  virtual bool use(ItemInstance &, Player &) const;                                                          // 328
  virtual bool dispense(BlockSource &, Container &, int, Vec3 const &, signed char) const;                   // 336
  virtual void useTimeDepleted(ItemInstance &, Player *, Level *) const;                                     // 344
  virtual bool releaseUsing(ItemInstance &, Player *, int) const;                                            // 352
  virtual int getDestroySpeed(ItemInstance const &, Block const &) const;                                    // 360
  virtual bool hurtEnemy(ItemInstance &, Mob *, Mob *) const;                                                // 368
  virtual bool mineBlock(ItemInstance &, Block const &, int, int, int, Actor *) const;                       // 376
  virtual std::string buildDescriptionId(ItemInstance const &) const;                                        // 384
  virtual std::string buildEffectDescriptionName(ItemInstance const &) const;                                // 392
  virtual std::string buildCategoryDescriptionName(ItemInstance const &) const;                              // 400
  virtual void readUserData(ItemInstance &, IDataInput &) const;                                             // 408
  virtual void writeUserData(ItemInstance &, IDataInput &) const;                                            // 416
  virtual unsigned char getMaxStackSize(ItemInstance const &) const;                                         // 424
  virtual bool inventoryTick(ItemInstance &, Level &, Actor &, int, bool) const;                             // 432
  virtual void refreshedInContainer(ItemInstance &, Level &) const;                                          // 440
  virtual CooldownType getCooldownType() const;                                                              // 448
  virtual int getCooldownTime() const;                                                                       // 456
  virtual void fixupOnLoad(ItemInstance &) const;                                                            // 464
  virtual int getDamageValue(ItemInstance const &) const;                                                    // 472
  virtual bool isSameItem(ItemInstance const &, ItemInstance const &) const;                                 // 480
  virtual std::string getInteractText(Player const &) const;                                                 // 488
  virtual int getAnimationFrameFor(Mob &, bool) const;                                                       // 496
  virtual bool isEmissive(int) const;                                                                        // 504
  virtual TextureUVCoordinateSet const &getIcon(int, int, bool) const;                                       // 512
  virtual int getIconYOffset() const;                                                                        // 520
  virtual void setIcon(std::string const &, int);                                                            // 528
  virtual void setIcon(TextureUVCoordinateSet const &);                                                      // 536
  virtual std::string getAuxValuesDescription() const;                                                       // 544
  virtual bool _checkUseOnPermissions(Actor &, ItemInstance &, signed char const &, BlockPos const &) const; // 552
  virtual bool _calculatePlacePos(ItemInstance &, Actor &, signed char &, BlockPos &) const;                 // 560
  virtual bool _useOn(ItemInstance &, Actor &, BlockPos, signed char, float, float, float) const;            // 568
};

struct ItemRegistry {
  static std::unordered_map<int, Item const *> mItems;
  static std::unordered_map<std::string, Item const *> mItemLookupMap;
  static std::unordered_map<std::string, std::string> mItemAliasLookupMap;
  static std::vector<std::unique_ptr<Item>> mItemRegistry;

  static void buildDocumentation(Documentation::Writer &);
  static void startRegistration();
  static void finishedRegistration();
  static void teardonwAll();
  static Item const *getItem(short);
  static Item const *getItem(std::string const &);
  static Item const *lookupByName(std::string const &, bool);
  static unsigned getItemCount();
  static void initClientData();
  static void initServerData(ResourcePackManager &);
  static std::unordered_map<std::string, Item const *> const &allItems();

  static void registerAlias(std::string const &, std::string const &);
  template <typename T, typename... A> static void registerBlockItem(std::string const &, Block const &, A... a);
  template <typename T, typename... A> static void registerBlockItem(std::string const &, BlockLegacy const &, A... a);
  static void registerItem(std::unique_ptr<Item> &&);
  template <typename T, typename... A> static void registerItem(std::string const &, short, A... a);
  template <typename T, typename... A> static void registerItemShared(A... a);
  static void unregisterItem(std::string const &);
};

struct ItemEnchants;

struct ItemInstance {
  static ItemInstance EMPTY_ITEM;
  static std::string TAG_DISPLAY;            // display
  static std::string TAG_DISPLAY_NAME;       // Name
  static std::string TAG_LORE;               // Lore
  static std::string TAG_REPAIR_COST;        // RepairCost
  static std::string TAG_ENCHANTS;           // ench
  static std::string TAG_CAN_PLACE_ON;       // minecraft:can_place_on
  static std::string TAG_DESTROY;            // minecraft:can_destroy
  static std::string TAG_STORE_CAN_PLACE_ON; // CanPlaceOn
  static std::string TAG_STORE_CAN_DESTROY;  // CanDestroy
  static short retrieveAuxValFromIDAux(int);
  static unsigned char retrieveEnchantFromIDAux(int);
  static short retrieveIDFromIDAux(int);

  Item *item;                                                    // 0
  std::unique_ptr<CompoundTag> userdata;                         // 8
  short aux;                                                     // 16
  unsigned char count;                                           // 18
  bool valid;                                                    // 19
  std::chrono::time_point<std::chrono::steady_clock> pickupTime; // 24
  std::chrono::milliseconds pickupDur;                           // 32
  bool showPickup;                                               // 40
  std::vector<BlockLegacy const *> canPlaceList;                 // 48
  std::size_t canPlaceHash;                                      // 72
  std::vector<BlockLegacy const *> canDestroyList;               // 80
  std::size_t canDestroyListHash;                                // 104

  ItemInstance();
  ItemInstance(BlockLegacy const &);
  ItemInstance(BlockLegacy const &, int);
  ItemInstance(BlockLegacy const &, int, int);
  ItemInstance(Item const &);
  ItemInstance(Item const &, int);
  ItemInstance(Item const &, int, int);
  ItemInstance(Item const &, int, int, CompoundTag const *);
  ItemInstance(ItemInstance const &);
  ItemInstance(int, int, int);
  ItemInstance(int, int, int, CompoundTag const *);

  ItemInstance clone() const;
  operator bool() const;
  bool operator==(ItemInstance const &) const;
  bool operator!=(ItemInstance const &) const;
  ItemInstance &operator=(ItemInstance const &);

  void init(BlockLegacy const &, int, int);
  void init(int, int, int);
  void load(CompoundTag const &);
  void save(CompoundTag &);

  void add(int);
  void remove(int);
  void set(int);
  void setNull();
  void setPickupTime(std::chrono::time_point<std::chrono::steady_clock>);
  void setPickupTime();
  void setRepairCost(int);
  void setShowPickUp(bool);
  void setStackSize(unsigned char);
  void setUserData(std::unique_ptr<CompoundTag>);
  void setAuxValue(short);
  void resetHoverName();
  void setCustomName(std::string const &);
  void setJustBrewed(bool);
  void wasJustBrewed(bool);
  void useAsFuel();
  void addComponents(Json::Value const &, std::string &);
  void addCustomUserData(BlockActor &, BlockSource &);
  void saveEnchantsToUserData(ItemEnchants const &);
  void deserializeComponents(IDataInput &);
  void serializeComponents(IDataOutput &);
  void updateComponent(std::string const &, Json::Value const &);
  void forceSetCount(unsigned char);
  void fromTag(CompoundTag const &);
  int getAttackDamage() const;
  short getAuxValue() const;
  int getBaseRepairCost() const;
  Color getColor() const;
  short getDamageValue() const;
  float getDestroySpeed() const;
  Enchant::Slot getEnchantSlot() const;
  unsigned getEnchantValue() const;
  ItemEnchants getEnchantsFromUserData() const;
  TextureUVCoordinateSet const &getIcon(int, int, bool) const;
  short getId() const;
  int getIdAux() const;
  int getIdAuxEnchanted() const;
  bool getIsValidPickupTime() const;
  Item const &getItem() const;
  LegacyBlock const &getLegacyBlock() const;
  unsigned short getMaxDamage() const;
  unsigned char getMaxStackSize(ItemInstance const &) const;
  int getMaxUseDuration() const;
  std::unique_ptr<CompoundTag> getNetworkUserData() const;
  float getPickupPopPercentage() const;
  std::chrono::time_point<std::chrono::steady_clock> getPickupTime() const;
  unsigned char getStackSize() const;
  ItemInstance getStrippedNetworkItem() const;
  UseAnimation getUseAnimation() const;
  CompoundTag const &getUserData() const;

  bool hasComponent(std::string const &) const;
  bool hasCompoundTextUserData() const;
  bool hasCustomHoverName() const;
  bool hasUserData() const;
  bool hasSameAuxValue(ItemInstance const &) const;
  bool hasSameUserData(ItemInstance const &) const;
  bool sameItem(int, int) const;
  bool sameItem(ItemInstance const &) const;
  bool sameItemAndAux(ItemInstance const &) const;
  bool matchesItem(ItemInstance const &) const;
  bool matches(ItemInstance const &) const;

  bool isArmorItem() const;
  bool isBlock() const;
  bool isDamageableItem() const;
  bool isDamaged() const;
  bool isEmptyStack() const;
  bool isEnchanted() const;
  bool isEnchantingBook() const;
  bool isEquivalentArmor() const;
  bool isExperimental() const;
  bool isExplodable() const;
  bool isFullStack() const;
  bool isGlint() const;
  bool isHorseArmorItem() const;
  bool isInstance(BlockLegacy const &) const;
  bool isInstance(Item const &) const;
  bool isItem() const;
  bool isLiquidClipItem() const;
  bool isMusicDiscItem() const;
  bool isNull() const;
  bool isOffhandItem() const;
  bool isPotionItem() const;
  bool isStackable(ItemInstance const &) const;
  bool isStackable() const;
  bool isStackableByData() const;
  bool isThrowable() const;
  bool isValid() const;
  bool isValidAuxValue(int) const;
  bool isValidComponent(std::string const &) const;
  bool isWearableItem() const;

  bool canDestroy(Block const *) const;
  bool canDestroySpecial(Block const *) const;
  bool canPlaceOn(Block const *) const;
  bool componentsMatch(ItemInstance const &) const;

  void hurtAndBreak(int, Actor *);
  bool inventoryTick(Level &, Actor &, int, bool);
  bool mineBlock(Block const &, int, int, int, Actor *);
  void refreshedInContainer(Level &);
  void releaseUsing(Player &, int);
  bool showsDurabilityInCreative() const;
  void snap(Player *);
  void startCooldown(Player *);
  void use(Player &);
  void useOn(Actor &, BlockPos const &, signed char, Vec3 const &);
  void useTimeDepleted(Player &, Level &);

  ~ItemInstance();
};