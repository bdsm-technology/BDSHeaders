#pragma once

#define DLLX

#include "SharedMutex.h"
#include "leveldb/env.h"
#include <functional>
#include <memory>
#include <mutex>
#include <vector>

struct ContentIdentity;
struct SnapshotFilenameAndLength {
  std::string str;
  unsigned int len;
  SnapshotFilenameAndLength(std::string, unsigned int);
  SnapshotFilenameAndLength(SnapshotFilenameAndLength const &);
  SnapshotFilenameAndLength(SnapshotFilenameAndLength &&);

  SnapshotFilenameAndLength &operator=(SnapshotFilenameAndLength &&);

  ~SnapshotFilenameAndLength();
};

struct EncryptedProxyEnv : leveldb::EnvWrapper {
  leveldb::Env *env;      // 16
  std::string str24;      // 24
  std::string content_id; // 56
  EncryptedProxyEnv(leveldb::Env *, ContentIdentity const &, std::string const &);
  virtual ~EncryptedProxyEnv();
  virtual leveldb::Status NewSequentialFile(const std::string &f, leveldb::SequentialFile **r);
  virtual leveldb::Status NewRandomAccessFile(const std::string &f, leveldb::RandomAccessFile **r);
  virtual leveldb::Status NewWritableFile(const std::string &f, leveldb::WritableFile **r);
  virtual leveldb::Status DeleteFile(const std::string &f);
  virtual leveldb::Status RenameFile(const std::string &s, const std::string &t);
};

struct InMemoryFile;

struct InMemoryFileStorage {
  leveldb::Env *env;                                // 0
  std::mutex mtx;                                   // 8
  std::vector<std::shared_ptr<InMemoryFile>> files; // 48
  std::vector<std::string> list;                    // 72

  std::shared_ptr<InMemoryFile> createFile(std::string const &);
  bool deleteFile(std::string const &);
  bool findFile(std::string const &, std::shared_ptr<InMemoryFile> &);
  void flushToDisk();
  void populateFileList(std::vector<std::string> list);
};

struct InMemoryEnv : leveldb::EnvWrapper {
  leveldb::Env *env;      // 16
  std::string str24;      // 24
  std::string content_id; // 56

  InMemoryEnv(leveldb::Env *, std::string const &);
  virtual ~InMemoryEnv();
  virtual leveldb::Status NewSequentialFile(const std::string &f, leveldb::SequentialFile **r);
  virtual leveldb::Status NewRandomAccessFile(const std::string &f, leveldb::RandomAccessFile **r);
  virtual leveldb::Status NewWritableFile(const std::string &f, leveldb::WritableFile **r);
  virtual leveldb::Status NewAppendableFile(const std::string &f, leveldb::WritableFile **r);
  virtual bool FileExists(const std::string &f);
  virtual leveldb::Status GetChildren(const std::string &dir, std::vector<std::string> *r);
  virtual leveldb::Status DeleteFile(const std::string &f);
  virtual leveldb::Status RenameFile(const std::string &s, const std::string &t);
  void _flushWithTransaction();
  void flushToPermanentStorage();
};

struct SnapshotEnv : leveldb::EnvWrapper {
  leveldb::Env *env;        // 16
  SharedMutex mtx24, mtx80; // 24, 80
  std::mutex mtx136;        // 136
  bool b176;                // 176
  std::string str;          // 184

  virtual ~SnapshotEnv();
  virtual leveldb::Status NewSequentialFile(const std::string &f, leveldb::SequentialFile **r);
  virtual leveldb::Status NewRandomAccessFile(const std::string &f, leveldb::RandomAccessFile **r);
  virtual leveldb::Status NewWritableFile(const std::string &f, leveldb::WritableFile **r);
  virtual leveldb::Status NewAppendableFile(const std::string &f, leveldb::WritableFile **r);
  virtual leveldb::Status RenameFile(const std::string &s, const std::string &t);

  SnapshotEnv(leveldb::Env *, std::string const &);

  leveldb::Status _newWrappedWritableFile(const std::string &f, leveldb::WritableFile **r);
  std::vector<SnapshotFilenameAndLength> createSnapshot(std::string const &f);
  void releaseSnapshot();
};

static_assert(offsetof(SnapshotEnv, str) == 184);

enum struct CompactionStatus : int { Start = 0, Complete = 1 };

struct CompactionListenerEnv : leveldb::EnvWrapper {
  leveldb::Env *env;                                        // 16
  std::function<void(CompactionStatus)> compactionCallback; // 24
  virtual ~CompactionListenerEnv();
  virtual void Schedule(void (*f)(void *), void *a);
  CompactionListenerEnv(leveldb::Env *);
  void setCompactionCallback(std::function<void(CompactionStatus)> const &);
  void notifyStart();
  void notifyComplete();
};