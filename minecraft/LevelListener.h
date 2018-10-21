#pragma once

#include "types.h"

struct BlockSource;
struct Block;
struct ActorBlockSyncMessage;
struct BlockActor;
struct CompoundTag;
struct LevelChunk;
struct Player;
struct LevelEvent;
struct LevelSoundEvent;
namespace cg {
struct ImageBuffer;
}
struct ScreenshotOptions;

struct LevelListener {
  LevelListener();
  virtual ~LevelListener();                                                                                                             // 0, 8
  virtual void onSourceCreated(BlockSource &);                                                                                          // 16
  virtual void onSourceDestroyed(BlockSource &);                                                                                        // 24
  virtual void onAreaChanged(BlockSource &, BlockPos const &, BlockPos const &);                                                        // 32
  virtual void onBlockChanged(BlockSource &, BlockPos const &, uint, Block const &, Block const &, int, ActorBlockSyncMessage const *); // 40
  virtual void onBrightnessChanged(BlockSource &, BlockPos const &);                                                                    // 48
  virtual void onBlockEntityChanged(BlockSource &, BlockActor &);                                                                       // 56
  virtual void onBlockEntityAboutToBeRemoved(BlockSource &, std::shared_ptr<BlockActor>);                                               // 64
  virtual void onEntityChanged(BlockSource &, Actor &);                                                                                 // 72
  virtual void onBlockEvent(BlockSource &, int, int, int, int, int);                                                                    // 80
  virtual void allChanged();                                                                                                            // 88
  virtual void addParticle(ParticleType, Vec3 const &, Vec3 const &, int, CompoundTag const *, bool);                                   // 96
  virtual void playMusic(std::string const &, Vec3 const &, float, float);                                                              // 104
  virtual void playStreamingMusic(std::string const &, int, int, int);                                                                  // 112
  virtual void onEntityAdded(Actor &);                                                                                                  // 120
  virtual void onEntityRemoved(Actor &);                                                                                                // 128
  virtual void onNewChunk(BlockSource &, LevelChunk &);                                                                                 // 136
  virtual void onNewChunkFor(Player &, LevelChunk &);                                                                                   // 144
  virtual void onChunkLoaded(LevelChunk &);                                                                                             // 152
  virtual void onChunkUnloaded(LevelChunk &);                                                                                           // 160
  virtual void onLevelDestruction(std::string const &);                                                                                 // 168
  virtual void levelEvent(LevelEvent, Vec3 const &, int);                                                                               // 176
  virtual void levelSoundEvent(LevelSoundEvent, Vec3 const &, int, ActorType, bool, bool);                                              // 184
  virtual void levelSoundEvent(std::string const &, Vec3 const &, float, float);                                                        // 192
  virtual void stopSoundEvent(std::string const &);                                                                                     // 200
  virtual void stopAllSounds();                                                                                                         // 208
  virtual void takePicture(cg::ImageBuffer &, Actor *, Actor *, ScreenshotOptions &);                                                   // 216
  virtual void playerListChanged();                                                                                                     // 224
};