#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include "types.h"

struct CompoundTag;

enum struct PlayerPermissions : unsigned char;

struct PermissionsHandler {
  CommandPermissionLevel permissionsLevel;  // 0
  PlayerPermissions playerPermissionsLevel; // 1
  PermissionsHandler();
  PermissionsHandler(PermissionsHandler const &);

  void addSaveData(CompoundTag &) const;
  bool loadSaveData(CompoundTag const &);

  CommandPermissionLevel getCommandPermissions() const;
  void setCommandPermissions(CommandPermissionLevel);
  PlayerPermissions getPlayerPermissions() const;
  void setPlayerPermissions(PlayerPermissions);
};

struct Ability {
  enum struct Type : char { Invalid = 0, Boolean = 1, Float = 2 };
  enum struct Options : char { Unset = 0, Saved = 1, Command = 2, Permission = 4 };
  union Value {
    bool val_bool;
    float val_float;
    Value(bool);
    Value(float);
    Value();
  };
  Type type;
  Value value;
  Options opts;

  Ability();
  Ability(float, Options);
  Ability(int, Options);

  Type getType() const;
  bool getBool() const;
  float getFloat() const;
  void setBool(bool) const;
  void setFloat(float) const;
  bool hasOption(Options) const;
};

struct Abilities {
  std::unique_ptr<PermissionsHandler> permissionsHandler; // 0
  std::unordered_map<std::string, Ability> ab_map;        // 8
  std::vector<std::string> permissions_names;             // 64
  std::unordered_map<std::string, bool> custom;           // 88

  Abilities(Abilities const &);
  Abilities();

  Abilities &operator=(Abilities const &);

  void _addTempCustomAbility(std::string const &);
  void _registerAbilities();
  void _registerAbility(std::string const &, Ability const &);

  Ability const &getAbility(std::string const &) const;
  Ability &getAbility(std::string const &);
  bool getBool(std::string const &) const;
  float getFloat(std::string const &) const;
  void setAbility(std::string const &, bool);
  void setAbility(std::string const &, float);
  void setAbilityDiff(std::string const &, bool, bool &);
  std::unordered_map<std::string, Ability> const &getAbilities() const;
  std::vector<std::string> const &getPermissionsAbilitiesNames() const;
  std::unordered_map<std::string, bool> const &getStoredCustomAbilities() const;

  CommandPermissionLevel getCommandPermissions() const;
  void setCommandPermissions(CommandPermissionLevel);
  PlayerPermissions getPlayerPermissions() const;
  void setPlayerPermissions(PlayerPermissions);

  void addSaveData(CompoundTag &) const;
  void loadSaveData(CompoundTag const &);

  bool isFlying() const;

  ~Abilities();

  static std::string INVULNERABLE;
  static std::string FLYING;
  static std::string MAYFLY;
  static std::string INSTABUILD;
  static std::string LIGHTNING;
  static std::string FLYSPEED;
  static std::string WALKSPEED;
  static std::string MUTED;
  static std::string WORLDBUILDER;
  static std::string NOCLIP;
  static std::string BUILD_AND_MINE;
  static std::string DOORS_AND_SWITCHES;
  static std::string OPEN_CONTAINERS;
  static std::string ATTACK_PLAYERS;
  static std::string ATTACK_MOBS;
  static std::string OPERATOR;
  static std::string TELEPORT;
};