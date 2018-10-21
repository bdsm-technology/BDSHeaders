#pragma once

#include "types.h"
#include <unordered_map>

struct Attribute;
struct BaseAttributeMap;
struct AttributeInstance;

struct AttributeCollection {
  std::unordered_map<std::string, Attribute *> data;
  AttributeCollection();
  static AttributeCollection &instance();

  void addAttribute(std::string const &, Attribute *);
  Attribute *getAttribute(std::string const &);
  bool hasAttribute(std::string const &);

  ~AttributeCollection();
};

enum struct RedefinitionMode;

struct Attribute {
  RedefinitionMode mode; // 0
  std::string attr;      // 8
  Attribute(std::string const &, RedefinitionMode, bool);
  static Attribute *getByName(std::string const &);
  RedefinitionMode getRedefinitionMode() const;
  bool isClientSyncable() const;
  ~Attribute();
};

enum struct AttributeBuffType;

struct AttributeBuffInfo {
  AttributeBuffType type;
  ActorUniqueID auid;
  AttributeBuffInfo(AttributeBuffType);
};

struct AttributeBuff {
  float amount;                      // 8
  AttributeBuffInfo info;            // 16
  std::shared_ptr<Amplifier> valamp; // 32
  std::shared_ptr<Amplifier> duramp; // 48
  float ampamm;                      // 64
  int amplification;                 // 68
  ActorUniqueID auid;                // 72
  unsigned operand;                  // 80

  static int buffTypeToDamageCause(int);

  AttributeBuff(AttributeBuff const &);
  AttributeBuff(float, int, AttributeBuffType);

  void setAmplificationAmount(int, float);

  float getAmount() const;
  ActorUniqueID const &getId() const;
  AttributeBuffType const &getInfo() const;
  unsigned getOperand() const;
  AttributeBuffType getType() const;

  virtual ~AttributeBuff();                                      // 0, 8
  virtual bool isInstantaneous() const = 0;                      // 16
  virtual bool isSerializable() const  = 0;                      // 24
  virtual void setDurationAmplifier(std::shared_ptr<Amplifier>); // 36
};

struct TemporalAttributeBuff : AttributeBuff {
  int duration;      // 84
  int lifeTimer;     // 88
  float baseAmount;  // 92
  bool serializable; // 96

  TemporalAttributeBuff(TemporalAttributeBuff const &);
  TemporalAttributeBuff(float, int, AttributeBuffType, std::string const &);

  float getBaseAmount() const;
  int getDuration() const;
  int getLifeTimer() const;
  void tick();

  virtual ~TemporalAttributeBuff();
  virtual bool isInstantaneous() const;
  virtual bool isSerializable() const;
  virtual void setDurationAmplifier(std::shared_ptr<Amplifier>);
  virtual bool shouldBuff() const;
  virtual bool isComplete() const;
};

struct AttributeModifier {
  float amount;       // 8
  unsigned operation; // 12
  unsigned operand;   // 16
  std::string name;   // 24
  mce::UUID uuid;     // 56
  bool serializable;  // 72

  AttributeModifier();
  AttributeModifier(mce::UUID, std::string const &, float, int, int, bool);
  AttributeModifier(AttributeModifier const &);

  AttributeModifier &operator=(AttributeModifier const &);
  bool operator==(AttributeModifier const &) const;
  bool operator!=(AttributeModifier const &) const;

  void setSerialize(bool);

  float getAmount() const;
  unsigned getOperation() const;
  unsigned getOperand() const;
  std::string const &getName() const;
  mce::UUID const &getId() const;
  bool isSerializable() const;

  virtual ~AttributeModifier();
  virtual bool isInstantaneous() const;
};

struct AttributeInstanceHandle {
  std::string handle;
  AttributeInstanceHandle();
  AttributeInstanceHandle(AttributeInstanceHandle const &);
  AttributeInstanceHandle(AttributeInstanceHandle &&);

  AttributeInstance const &getInstance() const;
  AttributeInstance &getMutableInstance() const;

  ~AttributeInstanceHandle();
};

struct AttributeInstanceDelegate {
  AttributeInstanceHandle handle;

  virtual ~AttributeInstanceDelegate();
  virtual void tick();
  virtual void notify(long);
  virtual bool change(float, float, AttributeBuffInfo);
  virtual float getBuffValue(AttributeBuff const &);
};

struct AttributeInstance {
  BaseAttributeMap *map;                           // 8
  Attribute &attr;                                 // 16
  std::vector<AttributeModifier> mods;             // 24
  std::vector<TemporalAttributeBuff> buffs;        // 48
  std::vector<AttributeInstanceHandle> listeners;  // 72
  std::shared_ptr<AttributeInstanceDelegate> dele; // 96
  float defaultValue[3];                           // 112
  float minValue;                                  // 124
  float maxValue;                                  // 128
  float currentValue;                              // 132

  AttributeInstance(AttributeInstance const &);
  AttributeInstance(BaseAttributeMap *, Attribute const *);

  void addModifier(AttributeModifier const &);
  void addModifier(std::shared_ptr<AttributeModifier> const &);
  void addModifiers(std::vector<std::shared_ptr<AttributeModifier>> *);
  void addBuff(AttributeBuff const &);
  void addBuff(std::shared_ptr<AttributeBuff>);

  void removeModifier(AttributeModifier const &);
  void removeModifier(std::shared_ptr<AttributeModifier> const &);
  void removeModifiers(std::string const &);
  void removeModifiers();
  void removeBuff(AttributeBuff const &);
  void removeBuff(std::shared_ptr<AttributeBuff>);

  Attribute const &getAttribute() const;
  std::vector<TemporalAttributeBuff> const &getBuffs() const;
  AttributeInstanceHandle getHandle() const;
  std::vector<AttributeInstanceHandle> const &getListeners() const;
  float getMinValue() const;
  float getMaxValue() const;
  AttributeModifier getModifier(mce::UUID const &) const;
  std::vector<AttributeModifier> getModifiers(int) const;
  std::vector<AttributeModifier> getModifiers(std::vector<std::string> const &) const;
  std::vector<AttributeModifier> getModifiers() const;

  void setMinValue(float);
  void setMaxValue(float);
  void setRange(float, float, float);
  void setDelegate(std::shared_ptr<AttributeInstanceDelegate>);

  bool hasModifier(AttributeModifier const &) const;
  bool hasModifier(std::vector<AttributeModifier> const &) const;
  bool hasTemporalBuffs() const;

  bool isValid() const;

  void notify();
  void recalculateModifiers();
  void registerListener(AttributeInstance const &);
  void resetToDefaultValue();
  void resetToMaxValue();
  void resetToMinValue();

  void serializationSetRange(float, float, float);
  void serializationSetValue(float, int, float);
  void setDefaultValue(float, int);
  void setDefaultValueOnly(float);

  virtual ~AttributeInstance();
  virtual void tick();
};

struct BaseAttributeMap {
  std::unordered_map<std::string, AttributeInstance> att_map; // 0
  std::vector<AttributeInstanceHandle> dirtyAttributes;       // 56

  static AttributeInstance mInvalidInstance;

  BaseAttributeMap();

  void registerAttribute(Attribute const &);

  void addAttributeModifier(std::string const &, std::shared_ptr<AttributeModifier>);
  void removeAttributeModifier(std::string const &, std::shared_ptr<AttributeModifier>);

  void clearDirtyAttributes();
  std::vector<AttributeInstanceHandle> &getDirtyAttributes();
  AttributeInstance const &getInstance(Attribute const &) const;
  AttributeInstance const &getInstance(std::string const &) const;
  AttributeInstance &getMutableInstance(Attribute const &);
  AttributeInstance &getMutableInstance(std::string const &);
  std::vector<AttributeInstanceHandle> getSyncableAttributes() const;
  void onAttributeModified(AttributeInstance const &);

  std::unordered_map<std::string, AttributeInstance>::iterator begin();
  std::unordered_map<std::string, AttributeInstance>::const_iterator begin() const;
  std::unordered_map<std::string, AttributeInstance>::iterator end();
  std::unordered_map<std::string, AttributeInstance>::const_iterator end() const;

  ~BaseAttributeMap();
};