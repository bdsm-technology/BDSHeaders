#pragma once

#include "Abilities.h"
#include "GameRules.h"
#include "types.h"
#include <vector>

struct LevelData;
struct LevelSummary;
struct Dimension;

struct LevelSettings {
  int seed;                                         // 0
  GameType gametype;                                // 4
  Difficulty difficulty;                            // 8
  bool _forceGameType;                              // 12
  GeneratorType generator;                          // 16
  bool achievementsDisabled;                        // 20
  DimensionId dimension;                            // 24
  int time;                                         // 28
  bool edu;                                         // 32
  bool eduFeatures;                                 // 33
  bool immutableWorld;                              // 34
  float rainLevel;                                  // 36
  float lightningLevel;                             // 40
  bool xblBroadcastIntent;                          // 44
  bool mpGame;                                      // 45
  bool lanBroadcast;                                // 46
  bool xblBroadcast;                                // 47
  Social::GamePublishSetting xblBroadcastMode;      // 48
  bool commandsEnabled;                             // 52
  bool texturepacksRequired;                        // 53
  bool lockedBehaviourPack;                         // 54
  bool lockedResourcePack;                          // 55
  bool fromLockedTemplate;                          // 56
  bool msaGamertagsOnly;                            // 57
  bool _overrideSavedSettings;                      // 58
  bool bonusChestEnabled;                           // 59
  bool startWithMap;                                // 60
  unsigned serverChunkTickRange;                    // 64
  bool forceExperimentalGameplay;                   // 68
  bool platformBroadcast;                           // 69
  Social::GamePublishSetting platformBroadcastMode; // 72
  Abilities defaultAbilities;                       // 80
  BlockPos spawnPos;                                // 224
  std::vector<PackInstanceId> behaviorPackIds;      // 240
  std::vector<PackInstanceId> resourcePackIds;      // 264
  GameRules rules;                                  // 288

  LevelSettings(LevelData const &, DimensionId);
  LevelSettings(LevelSettings const &);
  LevelSettings(LevelSettings &&);
  LevelSettings(LevelSummary const &);
  LevelSettings();
  LevelSettings(unsigned int, GameType, Difficulty, DimensionId, GeneratorType, BlockPos const &, bool, int, bool, float, float, bool, bool, bool, Social::GamePublishSetting, bool, bool, bool, bool,
                bool, GameRules, bool, bool, Abilities, unsigned int, bool, Social::GamePublishSetting, bool);

  LevelSettings operator=(LevelSettings const &);

  static unsigned parseSeedString(std::string const &);
  static Difficulty validateGameDifficulty(Difficulty);

  bool achievementsWillBeDisabledOnLoad() const;
  bool educationFeaturesEnabled() const;
  bool forceGameType() const;
  Abilities const &getDefaultAbilities() const;
  BlockPos const &getDefaultSpawn() const;
  DimensionId getDimension() const;
  Difficulty getGameDifficulty() const;
  GameRules const &getGameRules() const;
  GameType getGameType() const;
  GeneratorType getGenerator() const;
  bool getImmutableWorld() const;
  float getLightningLevel() const;
  float getRainLevel() const;
  unsigned getSeed() const;
  unsigned getServerChunkTickRange() const;
  int getTime() const;
  bool getXBLBroadcastIntent() const;
  Social::GamePublishSetting getXBLBroadcastMode() const;
  std::vector<PackInstanceId> const &getNewWorldBehaviorPackIdentities() const;
  std::vector<PackInstanceId> const &getNewWorldResourcePackIdentities() const;
  Social::GamePublishSetting getPlatformBroadcastMode() const;

  bool hasAchievementsDisabled() const;
  bool hasBounsChestEnabled() const;
  bool hasCommandsEnabled() const;
  bool hasLANBroadcast() const;
  bool hasLockedBehaviorPack() const;
  bool hasLockedResourcePack() const;
  bool hasPlatformBoardcast() const;
  bool hasStartWithMapEnabled() const;
  bool hasXBLBroadcast() const;

  bool isEducationEditionLevel() const;
  bool isFromLockedTemplate() const;
  bool isMultiplayerGame() const;
  bool isTexturepacksRequired() const;

  bool shouldForceExperimentalGameplay();
  bool shouldOverrideSavedSettings();

  bool useMsaGamertagsOnly() const;

  void overrideSavedSettings(LevelData &);

  void setEducationFeaturesEnabled(bool);
  void setForceExperimentalGameplay(bool);
  void setForceGameType(bool);
  void setImmutableWorld(bool);
  void setIsEducationEditionLevel(bool);
  void setNewWorldBehaviorPackIdentities(std::vector<PackInstanceId> const &);
  void setNewWorldResourcePackIdentities(std::vector<PackInstanceId> const &);
  void setOverrideSavedSettings(bool);

  ~LevelSettings();
};