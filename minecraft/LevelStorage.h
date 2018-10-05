#pragma once

#define DLLX

#include "DataIO.h"
#include "SmallSet.h"
#include "ThreadLocal.h"
#include "envs.h"
#include "leveldb/cache.h"
#include "leveldb/compressor.h"
#include "leveldb/db.h"
#include "leveldb/decompress_allocator.h"
#include "leveldb/filter_policy.h"
#include "tags.h"
#include "types.h"
#include <atomic>
#include <chrono>
#include <functional>
#include <map>
#include <memory>
#include <string>
#include <vector>
#include <gsl/string_span>

struct Player;
struct Actor;
struct LevelData;
struct ChunkSource;
struct Scheduler;
struct ContentIdentity;
struct IContentKeyProvider;
struct SaveTransactionManager;
struct TaskGroup;
struct DBChunkStorage;

struct LevelStorageObserver {
  std::function<void(std::string const &)> fn;
  LevelStorageObserver(std::function<void(std::string const &)>);
  ~LevelStorageObserver();
  void onSave(std::string const &);
};

struct KeySpan {
  std::string key;
  int start, end;
  KeySpan(KeySpan const &);
  KeySpan(std::string const &, int, int);
  ~KeySpan();
};

struct KeyValueInput {
  struct KeyStream {
    StringByteInput sbi; // 0
    KeySpan span;        // 24
    KeyStream(KeySpan const &, std::string const &);
    ~KeyStream();
  };
  struct Iterator {
    int pos;
    KeyValueInput *src;
    Iterator(KeyValueInput &, int);
    bool operator!=(Iterator const &) const;
    KeyStream operator*();
    Iterator &operator++();
  };
  std::vector<KeySpan> list;
  std::string title;
  KeyValueInput();
  void add(std::string const &, char const *, int);
};

struct LevelStorageResult {
  int flag;
  std::string data;
  LevelStorageResult(LevelStorageResult const &);
  ~LevelStorageResult();
};

struct LevelStorage {
  LevelStorage();
  virtual ~LevelStorage();
  virtual void addStorageObserver(std::unique_ptr<LevelStorageObserver>)   = 0;
  virtual void tick()                                                      = 0;
  virtual std::unique_ptr<CompoundTag> getCompoundTag(std::string const &) = 0;
  virtual KeyValueInput getKeysForPrefix(std::string const &)              = 0;
  virtual bool hasKey(gsl::cstring_span<gsl::dynamic_extent>) const        = 0;
  virtual void deserializeKey(KeySpan const &, StringByteInput &);
  virtual bool loadLevelData(LevelData &)                                                               = 0;
  virtual std::unique_ptr<ChunkSource> createChunkStorage(std::unique_ptr<ChunkSource>, StorageVersion) = 0;
  virtual void saveLevelData(LevelData const &)                                                         = 0;
  virtual std::string getFullPath() const                                                               = 0;
  virtual void saveData(std::string const &, std::string &&)                                            = 0;
  virtual void deleteData(std::string const &)                                                          = 0;
  virtual void syncIO()                                                                                 = 0;
  virtual void getStatistics(std::string &) const                                                       = 0;
  virtual bool clonePlayerData(std::string const &, std::string const &);
  virtual bool isCorrupted() const    = 0;
  virtual bool isShutdownDone() const = 0;
  virtual std::string loadData(std::string const &); //
  virtual LevelStorageResult getState() const                     = 0;
  virtual std::string getLevelId() const                          = 0;
  virtual std::vector<SnapshotFilenameAndLength> createSnapshot() = 0;
  virtual void releaseSnapshot()                                  = 0;
  virtual void compactStorage()                                   = 0;
  virtual void syncAndSuspendStorage()                            = 0;
  virtual void resumeStorage()                                    = 0;
  virtual void setFlushAllowed(bool)                              = 0;
  virtual void flushToPermanentStorage()                          = 0;
  virtual void freeCaches(); //
  virtual void setCompactionCallback(std::function<void(CompactionStatus)>)             = 0;
  virtual void setCriticalSyncSaveCallback(std::function<void(std::string const &)>)    = 0;
  virtual void _saveDataSync(std::string const &, std::string &&)                       = 0;
  virtual bool _readWithPrefix(gsl::cstring_span<gsl::dynamic_extent>, KeyValueInput &) = 0;

  std::vector<std::string> loadAllPlayerIDs();
  std::unique_ptr<CompoundTag> loadLocalPlayerData();
  std::unique_ptr<CompoundTag> loadLostPlayerData(Player const &, bool);
  std::unique_ptr<CompoundTag> loadPlayerData(PlayerStorageIds const &, bool);
  std::unique_ptr<CompoundTag> loadPlayerDataFromTag(std::string const &);
  std::unique_ptr<CompoundTag> loadServersPlayerData(Player const &, bool);
  std::unique_ptr<CompoundTag> save(Actor *);
  void saveData(std::string const &, CompoundTag const &);
  void savePlayerData(std::string const &, CompoundTag const &);
  std::string getServerId(PlayerStorageIds const &);
  std::string getServerId(Player const &, bool);

  static CompoundTag LOCAL_PLAYER_TAG;
};

struct DBStorage : LevelStorage {
  struct Options {
    leveldb::Options masterOptions;     // 0
    leveldb::ReadOptions readOptions;   // 2144
    leveldb::WriteOptions writeOptions; // 2168
  };
  static_assert(offsetof(Options, readOptions) == 2144);
  static_assert(offsetof(Options, writeOptions) == 2168);

  std::unique_ptr<EncryptedProxyEnv> encryptedProxyEnv;                  // 8
  std::unique_ptr<InMemoryEnv> inMemoryEnv;                              // 16
  std::unique_ptr<SnapshotEnv> snapshotEnv;                              // 24
  std::unique_ptr<CompactionListenerEnv> compactionListenerEnv;          // 32
  void *unk40;                                                           // 40
  std::unique_ptr<leveldb::Cache> db_cache;                              // 48
  std::unique_ptr<leveldb::FilterPolicy> db_filter_policy;               // 56
  std::unique_ptr<leveldb::Compressor> db_compressor1;                   // 64
  std::unique_ptr<leveldb::Compressor> db_compressor2;                   // 72
  std::unique_ptr<Options> options;                                      // 80
  std::unique_ptr<leveldb::DecompressAllocator> db_decompress_allocator; // 88
  std::unique_ptr<leveldb::DB> db;                                       // 96
  std::unique_ptr<TaskGroup> task_group;                                 // 104
  LevelStorageResult storage_result;                                     // 112
  std::string str152;                                                    // 152
  std::string str184;                                                    // 184
  ThreadLocal<std::string> str216;                                       // 216
  std::string db_path;                                                   // 320
  std::mutex mtx352;                                                     // 352
  std::chrono::steady_clock clock;                                       // 392
  std::chrono::duration<long, std::ratio<1l, 1000000000l>> dur;          // 400
  std::atomic<bool> flushAllowed, b409;                                  // 408, 409
  std::shared_ptr<SaveTransactionManager> save_transction_mgr;           // 416
  std::function<void(std::string)> critical_sync_save_callback;          // 432
  SmallSet<DBChunkStorage *> db_chunks;                                  // 464
  std::vector<std::unique_ptr<LevelStorageObserver>> level_storage_obs;  // 488
  std::atomic<bool> b512;                                                // 512
  std::map<std::string, int> map_str_int;                                // 520
  std::mutex mtx568;                                                     // 568

  virtual void addStorageObserver(std::unique_ptr<LevelStorageObserver>);
  virtual void tick();
  virtual std::unique_ptr<CompoundTag> getCompoundTag(std::string const &);
  virtual KeyValueInput getKeysForPrefix(std::string const &);
  virtual bool hasKey(gsl::cstring_span<gsl::dynamic_extent>) const;
  virtual bool loadLevelData(LevelData &);
  virtual std::unique_ptr<ChunkSource> createChunkStorage(std::unique_ptr<ChunkSource>, StorageVersion);
  virtual void saveLevelData(LevelData const &);
  virtual std::string getFullPath() const;
  virtual void saveData(std::string const &, std::string &&);
  virtual void deleteData(std::string const &);
  virtual void syncIO();
  virtual void getStatistics(std::string &) const;
  virtual bool isCorrupted() const;
  virtual bool isShutdownDone() const;
  virtual std::string loadData(std::string const &);
  virtual LevelStorageResult getState() const;
  virtual std::string getLevelId() const;
  virtual std::vector<SnapshotFilenameAndLength> createSnapshot();
  virtual void releaseSnapshot();
  virtual void compactStorage();
  virtual void syncAndSuspendStorage();
  virtual void resumeStorage();
  virtual void setFlushAllowed(bool);
  virtual void flushToPermanentStorage();
  virtual void freeCaches();
  virtual void setCompactionCallback(std::function<void(CompactionStatus)>);
  virtual void setCriticalSyncSaveCallback(std::function<void(std::string const &)>);
  virtual void _saveDataSync(std::string const &, std::string &&);
  virtual bool _readWithPrefix(gsl::cstring_span<gsl::dynamic_extent>, KeyValueInput &);

  DBStorage(std::string const &, Scheduler &, std::string const &, ContentIdentity const &, IContentKeyProvider const &, std::shared_ptr<SaveTransactionManager>,
            std::chrono::duration<long, std::ratio<1l, 1000000000l>>);

  bool _checkStatusValid(leveldb::Status const &);
  void _notifyChunkStorageDestroyed(DBChunkStorage &);
  void _queueSaveCallbacks(bool);
  bool _read(gsl::cstring_span<gsl::dynamic_extent>, KeyValueInput &);
  bool _read(gsl::cstring_span<gsl::dynamic_extent>, std::string &);
  void _suspendAndPerformAction(std::function<bool()>, std::function<void()>);
  void _tryAutoCompaction();
  void _waitForPendingKeyWrite(std::string const &);
  void _write(leveldb::WriteBatch &);
  void _write(std::string const &, std::string const &);
  void _writeAsync(std::string const &, std::string &&);
  void asyncWriteTag(std::string const &, CompoundTag &);
  bool hasAnyKeyWithPrefix(leveldb::Slice const &) const;
  void tryRepair(std::string const &);
  void writeTag(std::string const &, CompoundTag &);
};

static_assert(offsetof(DBStorage, dur) == 400);
static_assert(offsetof(DBStorage, save_transction_mgr) == 416);