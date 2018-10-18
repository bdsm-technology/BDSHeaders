#pragma once
#include "item.h"
#include "types.h"

struct DataItem {
  DataItemType type;
  unsigned short id;
  bool dirty;
  DataItem(DataItemType, unsigned short);
  unsigned short getId() const;
  DataItemType getType() const;
  void setDirty(bool);
  bool isDirty() const;

  virtual ~DataItem();
  virtual bool isDataEqual(DataItem const &) const;
  virtual std::unique_ptr<DataItem> clone() const = 0;
};

template <typename T> struct DataItem2 : DataItem {
  T data;
  DataItem2(DataItemType, unsigned short, T const &);
  virtual ~DataItem2();
  virtual std::unique_ptr<DataItem> clone() const;
};

template <typename T> struct DataItem2Flag : DataItem {
  T data;
  void clearFlag(long);
  void setFlag(long);
  long getFlag() const;
  virtual std::unique_ptr<DataItem> clone() const;
};

template <> struct DataItem2<long> : DataItem2Flag<long> {
  DataItem2(DataItemType, unsigned short, long const &);
  virtual ~DataItem2();
};

template <> struct DataItem2<char> : DataItem2Flag<char> {
  DataItem2(DataItemType, unsigned short, char const &);
  virtual ~DataItem2();
};

struct SynchedActorData {
  std::vector<std::unique_ptr<DataItem>> items; // 0
  unsigned short unk24;
  unsigned short unk26;

  SynchedActorData();
  SynchedActorData(SynchedActorData &&);

  SynchedActorData &operator=(SynchedActorData &&);

  void assignValues(std::vector<std::unique_ptr<DataItem>> const &, Actor *);
  template <typename T> void clearFlag(unsigned short, int);
  template <typename T> T getFlag(unsigned short, int);
  template <typename T> void setFlag(unsigned short, int);
  template <typename T> unsigned short define(unsigned short, T const &);
  template <typename T> void set(DataItem *, T const &);
  template <typename T> void set(unsigned short, T const &);
  void markDirty(unsigned short);
  void markDirty(DataItem &);
  bool hasData(unsigned short) const;
  char getInt8(unsigned short) const;
  short getShort(unsigned short) const;
  int getInt(unsigned short) const;
  long long getInt64(unsigned short) const;
  float getFloat(unsigned short) const;
  Vec3 getVec3(unsigned short) const;
  std::string getString(unsigned short) const;
  ItemInstance getItemInstance(unsigned short) const;

  std::vector<std::unique_ptr<DataItem>> packAll() const;
  std::vector<std::unique_ptr<DataItem>> packDirty() const;

  ~SynchedActorData();
};

namespace DataTypeMap {
template <typename T> struct typeFor { operator DataItemType() const; };
} // namespace DataTypeMap