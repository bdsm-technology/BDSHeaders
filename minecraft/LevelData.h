#pragma once

#include "Abilities.h"
#include "GameRules.h"
#include "RakNet/BitStream.h"
#include "json.h"
#include "tags.h"
#include "types.h"
#include <memory>
#include <string>

struct LevelSettings;

struct LevelData {
  AdventureSettings adventureSettings;              // 0
  GameRules gameRules;                              // 8
  Abilities defalutAbilities;                       // 32
  std::string str176;                               // 176
  StorageVersion storageVersion;                    // 208
  int networkVersion;                               // 212
  SemVersion inventoryVersion;                      // 216
  Tick tick;                                        // 328
  unsigned randomSeed;                              // 336
  BlockPos spawnPos;                                // 340
  BlockPos worldCenter;                             // 352
  int dayCycleStopTime;                             // 364
  unsigned long long lastPlayed;                    // 368
  CompoundTag playerTag;                            // 376
  unsigned serverChunkTickRange;                    // 432
  float rainLevel;                                  // 436
  int rainTime;                                     // 440
  float lightningLevel;                             // 444
  int lightningTime;                                // 448
  unsigned netherScale;                             // 452
  GameVersion lastOpenedWithVersion;                // 456
  GameType gameType;                                // 472
  Difficulty gameDifficulty;                        // 476
  bool forceGameType;                               // 480
  bool spawnMobs;                                   // 481
  GeneratorType generator;                          // 484
  Json::Value flatWorldGeneratorOptions;            // 488
  unsigned worldStartCount;                         // 504
  bool hasBeenLoadedInCreative;                     // 508
  bool eduLevel;                                    // 509
  bool educationFeature;                            // 510
  bool xblBroadcastIntent;                          // 511
  bool multiplayerGame;                             // 512
  bool lanBoardcast;                                // 513
  bool xblBoardcast;                                // 514
  Social::GamePublishSetting xblBoardcastMode;      // 516
  bool platformBoardcast;                           // 520
  Social::GamePublishSetting platformBoardcastMode; // 524
  bool commandEnabled;                              // 528
  bool texturepacksRequired;                        // 529
  bool lockedBehaviorPack;                          // 530
  bool lockedResourcePack;                          // 531
  bool lockedTemplate;                              // 532
  ContentIdentity prid;                             // 536
  bool useMsaGamertagsOnly;                         // 560
  bool bounsChestEnabled;                           // 561
  bool bonusChestSpawned;                           // 562
  bool startWithMapEnabled;                         // 563
  bool centerMapsToOrigin;                          // 564

  LevelData();
  LevelData(LevelData &&);
  LevelData(CompoundTag const &);
  LevelData(LevelSettings const &, std::string const &, GeneratorType, BlockPos const &, bool hasBeenLoadedInCreative, bool eduLevel, float, float);

  LevelData &operator=(LevelData &&);

  void _updateLimitedWorldOrigin(BlockPos const &);
  void clearLoadedPlayerTag();
  void disableAchievements();

  bool achievementsWillBeDisabledOnLoad() const;
  AdventureSettings &getAdventureSettings();
  bool getBonusChestSpawned() const;
  Tick getCurrentTikc() const;
  Abilities const &getDefaultAbilities() const;
  Abilities &getDefaultAbilities();
  Json::Value const &getFlatWorldGeneratorOptions() const;
  bool getForceGameType() const;
  Difficulty getGameDifficulty() const;
  GameRules const &getGameRules() const;
  GameRules &getGameRules();
  GameType getGameType() const;
  GeneratorType getGenerator() const;
  GameVersion getLastLoadedWithVersion() const;
  unsigned long long getLastPlayed() const;
  float getLightningLevel() const;
  int getLightningTime() const;
  float getRainLevel() const;
  int getRainTime() const;
  CompoundTag &getLoadedPlayerTag();
  unsigned getNetherScale() const;
  int getNetworkVersion() const;
  Social::GamePublishSetting getPlatformBroadcastMode() const;
  ContentIdentity const &getPremiumTemplateContentIdentity() const;
  unsigned getSeed() const;
  unsigned getServerChunkTickRange() const;
  BlockPos const &getSpawn() const;
  bool getSpawnMobs() const;
  StorageVersion getStorageVersion() const;
  int getTime() const;
  BlockPos const &getWorldCenter() const;
  unsigned getWorldStartCount() const;
  bool getXBLBroadcastIntent() const;
  Social::GamePublishSetting getXBLBroadcastMode() const;
  bool getUseMsaGamertagsOnly() const;

  bool hasAchievementsDisabled() const;
  bool hasBounsChestEnabled() const;
  bool hasCommandsEnabled() const;
  bool hasExprimentalGameplayEnabled() const;
  bool hasLANBroadcast() const;
  bool hasLockedBehaviorPack() const;
  bool hasLockedResourcePack() const;
  bool hasMapsCenteredToOrigin() const;
  bool hasPlatformBoardcast() const;
  bool hasStartWithMapEnabled() const;
  bool hasXBLBroadcast() const;

  bool isAlwaysDay() const;
  bool isEducationEditionLevel() const;
  bool isFromLockedTemplate() const;
  bool isGameRule(GameRuleId) const;
  bool isLegacyLevel() const;
  bool isLighting() const;
  bool isRaining() const;
  bool isMultiplayerGame() const;
  bool isTexturepacksRequired() const;

  void incrementTick();
  void recordStartup();
  void updateLastTimePlayed();
  void touchLastLoadedWithVersion();

  std::unique_ptr<CompoundTag> createTag() const;
  void getTagData(CompoundTag const &);
  void setTagData(CompoundTag &);
  void v1_read(RakNet::BitStream &, StorageVersion);

  void setBounsChestEnabled(bool);
  void setBounsChestSpawned(bool);
  void setCommandsEnabled(bool);
  void setCurrentTick(Tick &);
  void setEducationFeaturesEnabled(bool);
  void setForceGameType(bool);
  void setFromLockedTemplate(bool);
  void setGameDifficulty(Difficulty);
  void setGameType(GameType);
  void setGenerator(GeneratorType);
  void setHasLockedBehaviorPack(bool);
  void setHasLockedResourcePack(bool);
  void setIsEducationEditionLevel(bool);
  void setLANBroadcast(bool);
  void setLevelName(std::string const &);
  void setLightningLevel(float);
  void setLightningTime(int);
  void setRainLevel(float);
  void setRainTime(int);
  void setMultiplayerGame(bool);
  void setNetworkVersion(int);
  void setPlatformBroadcast(bool);
  void setPlatformBroadcastMode(Social::GamePublishSetting);
  void setPremiumTemplateContentIdentity(ContentIdentity const &);
  void setSeed(unsigned);
  void setServerChunkTickRange(unsigned);
  void setSpawn(BlockPos const &);
  void setSpawnMobs(bool);
  void setStartWithMapEnabled(bool);
  void setStorageVersion(StorageVersion);
  void setTexturepacksRequired(bool);
  void setTime(int);
  void setXBLBroadcast(bool);
  void setXBLBroadcastIntent(bool);
  void setXBLBroadcastMode(Social::GamePublishSetting);
  void setUseMsaGamertagsOnly(bool);

  ~LevelData();
};

static_assert(offsetof(LevelData, str176) == 176);
static_assert(offsetof(LevelData, networkVersion) == 212);
static_assert(offsetof(LevelData, tick) == 328);
static_assert(offsetof(LevelData, forceGameType) == 480);
static_assert(sizeof(LevelData) == 568);