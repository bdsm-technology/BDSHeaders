#pragma once

#include "types.h"
#include <deque>
#include <functional>
#include <unordered_set>

struct ContainerContentChangeListener;
struct ContainerSizeChangeListener;
struct ItemInstance;
struct CompoundTag;
struct BlockSource;
struct Player;

struct Container {
  ContainerType type;                                                                                         // 8
  std::unordered_set<ContainerContentChangeListener *> content_change_listener;                               // 16
  std::unordered_set<ContainerSizeChangeListener *> size_change_listener;                                     // 72
  std::deque<std::function<void(Container &, int, ItemInstance const &, ItemInstance const &)>> transcations; // 128
  std::string customName;                                                                                     // 208
  bool useCustomName;                                                                                         // 240

  static BidirectionalUnorderedMap<ContainerType, std::string> containerTypeMap;
  static std::string getContainerTypeName(ContainerType);

  void addSizeChangeListener(ContainerSizeChangeListener *);
  void removeSizeChangeListener(ContainerSizeChangeListener *);

  virtual ~Container();                                                                                                                            // 0, 8
  virtual void addContentChangeListener(ContainerContentChangeListener *);                                                                         // 16
  virtual void removeContentChangeListener(ContainerContentChangeListener *);                                                                      // 24
  virtual ItemInstance const &getItem(int) const = 0;                                                                                              // 32
  virtual bool hasRoomForItem(ItemInstance &);                                                                                                     // 40
  virtual bool addItem(ItemInstance &);                                                                                                            // 48
  virtual bool addItemToFirstEmptySlot(ItemInstance &);                                                                                            // 56
  virtual void setItem(int, ItemInstance const &) = 0;                                                                                             // 64
  virtual void removeItem(int, int);                                                                                                               // 72
  virtual void removeAllItems();                                                                                                                   // 80
  virtual void dropContents(BlockSource &, Vec3 const &, bool);                                                                                    // 88
  virtual int getContainerSize() const = 0;                                                                                                        // 96
  virtual int getMaxStackSize() const  = 0;                                                                                                        // 104
  virtual void startOpen(Player &)     = 0;                                                                                                        // 112
  virtual void stopOpen(Player &)      = 0;                                                                                                        // 120
  virtual std::vector<ItemInstance> getSlotCopies() const;                                                                                         // 128
  virtual std::vector<ItemInstance const *> getSlots() const;                                                                                      // 136
  virtual int getItemCount();                                                                                                                      // 144
  virtual int getItemCount(ItemInstance const &);                                                                                                  // 152
  virtual bool canPushInItem(BlockSource &, int, int, ItemInstance const &) const;                                                                 // 160
  virtual bool canPullOutItem(BlockSource &, int, int, ItemInstance const &) const;                                                                // 168
  virtual void setContainerChanged(int);                                                                                                           // 176
  virtual void setContainerMoved();                                                                                                                // 184
  virtual void setCustomName(std::string const &);                                                                                                 // 192
  virtual bool hasCustomName() const;                                                                                                              // 200
  virtual void readAdditionalSaveData(CompoundTag const &);                                                                                        // 208
  virtual void addAdditionalSaveData(CompoundTag &);                                                                                               // 216
  virtual void createTransactionContext(std::function<void(Container &, int, ItemInstance const &, ItemInstance const &)>, std::function<void()>); // 224
  virtual void triggerTransactionChange(int, ItemInstance const &, ItemInstance const &);                                                          // 232
};
struct SimpleContainer : Container {
  int size;                       // 244
  std::vector<ItemInstance> data; // 248
  SimpleContainer(std::string const &, bool, int);
  void clearContent();
  virtual ~SimpleContainer();
  virtual ItemInstance const &getItem(int) const;
  virtual void setItem(int, ItemInstance const &);
  virtual int getContainerSize() const;
  virtual int getMaxStackSize() const;
  virtual void startOpen(Player &);
  virtual void stopOpen(Player &);
};