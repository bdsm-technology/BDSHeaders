#pragma once

#include <map>
#include <memory>
#include <string>
#include <vector>

struct GameRule {
  enum struct Type : char { Invalid = 0, Boolean = 1, Integer = 2, Float = 3 };
  union Value {
    bool val_bool;
    int val_int;
    float val_float;
  };

  bool saved;        // 0
  Type type;         // 1
  Value value;       // 4
  std::string name;  // 8
  bool useInCommand; // 40
  bool defaultSet;   // 41

  GameRule();
  GameRule(GameRule &&);
  GameRule(GameRule const &);
  GameRule(std::string const &);
  GameRule(std::string const &, bool, bool, bool);
  GameRule(std::string const &, int, bool, bool);
  GameRule(std::string const &, float, bool, bool);

  GameRule &operator=(GameRule &&);

  bool shouldSave() const;
  Type getType() const;
  bool getBool() const;
  int getInt() const;
  float getFloat() const;
  std::string const &getName() const;
  bool allowUseInCommand() const;
  bool hasDefaultSet() const;

  void resetType(Type);
  void setBool(bool);
  void setInt(int);
  void setFloat(float);

  ~GameRule();
};
static_assert(offsetof(GameRule, name) == 8);

struct GameRuleId {
  int id;
  GameRuleId(GameRuleId const &);
  GameRuleId(GameRuleId &&);
  GameRuleId(int);
};

struct GameRulesChangedPacket;
struct CompoundTag;

struct GameRules {
  std::vector<GameRule> rules;

  GameRules();
  GameRules(GameRules &&);
  GameRules(GameRules const &);

  GameRules &operator=(GameRules &&);

  static std::unique_ptr<GameRulesChangedPacket> _createPacket(GameRule const &);
  static std::map<GameRuleId, GameRule> _getMarketplaceGameRulesDefaultMap();

  GameRule *_getRule(GameRuleId);
  void _registerRule(std::string const &, GameRuleId, bool, bool, bool);
  void _registerRule(std::string const &, GameRuleId, int, bool, bool);
  void _registerRule(std::string const &, GameRuleId, float, bool, bool);
  void _registerRules();
  void _registerEDURules();
  void _setRule(GameRuleId, GameRule::Value, GameRule::Type, bool);
  void _validateMarketplaceGameRuleTypes();
  void _validateRules();

  bool getBool(GameRuleId) const;
  int getInt(GameRuleId) const;
  float getFloat(GameRuleId) const;
  void setBool(GameRuleId, bool, bool);
  void setInt(GameRuleId, int, bool);
  void setFloat(GameRuleId, float, bool);
  GameRule const &getRule(GameRuleId) const;
  bool hasRule(GameRuleId) const;
  std::vector<GameRule> const &getRules() const;
  GameRuleId nameToGameRuleIndex(std::string const &) const;

  std::unique_ptr<GameRulesChangedPacket> createAllGameRulesPacket() const;
  void deserializeRules(GameRulesChangedPacket const &);
  void setTagData(CompoundTag &);
  void getTagData(CompoundTag const &);

  void setMarketplaceOverrides();
};