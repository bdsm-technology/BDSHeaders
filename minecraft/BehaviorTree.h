#pragma once

#include "types.h"

struct CompositeDefinition;
struct ResourcePackManager;
struct BehaviorComponent;
struct BehaviorTreeGroup;
struct BehaviorTreeDefinition;
struct BehaviorFactory;

struct BehaviorTreeDefinitionPtr {
  BehaviorTreeGroup *group;    // 0
  BehaviorTreeDefinition *def; // 8
  static BehaviorTreeDefinitionPtr NONE;

  BehaviorTreeDefinitionPtr(BehaviorTreeGroup &, BehaviorTreeDefinition &);
  BehaviorTreeDefinitionPtr(BehaviorTreeDefinitionPtr const &);
  BehaviorTreeDefinitionPtr(BehaviorTreeDefinitionPtr &&);
  BehaviorTreeDefinitionPtr();

  BehaviorTreeDefinitionPtr &operator=(BehaviorTreeDefinitionPtr const &);
  BehaviorTreeDefinitionPtr &operator=(BehaviorTreeDefinitionPtr &&);
  BehaviorTreeDefinition *operator->();
  operator bool() const;

  void onGroupDestroyed();
  ~BehaviorTreeDefinitionPtr();
};
struct BehaviorData;

struct BehaviorNode {
  CompositeDefinition *comp;        // 8
  BehaviorTreeDefinitionPtr defPtr; // 16
  void *unk32;                      // 32
  BehaviorComponent *component;     // 40
  BehaviorData *data;               // 48
  unsigned status;                  // 56

  BehaviorNode();

  BehaviorData *getBehaviorData();
  BehaviorTreeDefinitionPtr getBehaviorTreeDefinition();
  unsigned getStatus() const;

  virtual ~BehaviorNode();                 // 0,8
  virtual void tick() = 0;                 // 16
  virtual void initializeFromDefinition(); // 24
};

struct BehaviorData {
  enum struct DataType;

  struct DataProxy {
    std::string name; // 8
    DataType type;    // 40
    DataProxy(std::string const &, DataType);
    virtual ~DataProxy();
    virtual std::unique_ptr<BehaviorData::DataProxy> copy() const = 0;
  };

  template <typename T> struct Data : DataProxy {
    T value; // 44
    Data(std::string const &, DataType, T);
    virtual ~Data();
    virtual std::unique_ptr<BehaviorData::DataProxy> copy() const;
  };

  std::unordered_map<std::string, std::unique_ptr<DataProxy>> xmap;
  std::vector<std::unique_ptr<BehaviorData::DataProxy>> stack;

  void copyData(std::string const &, std::string const &, BehaviorData &);
  bool hasData(std::string const &) const;
  bool hasDataOfType(std::string const &, DataType) const;
  template <typename T> void pushData(std::string const &, T);
  template <typename T> T getData(std::string const &) const;
  template <typename T> void pushDataToStack(std::string const &, T);
  void pushToStack(std::unique_ptr<BehaviorData::DataProxy>);
  void popFromStack(std::string const &);
  void reset();

  ~BehaviorData();
};

struct BehaviorDefinition {
  std::string name;
  BehaviorTreeDefinitionPtr ptr;
  BehaviorDefinition();

  std::string const &getName() const;
  BehaviorTreeDefinitionPtr getTreeDefinition() const;

  virtual ~BehaviorDefinition();
  virtual void load(Json::Value, BehaviorFactory const &);
  virtual std::unique_ptr<BehaviorNode> createNode(BehaviorFactory const &, BehaviorNode *, BehaviorData *) const;
};

struct BehaviorFactory {
  std::unordered_map<std::string, std::pair<std::function<std::unique_ptr<BehaviorDefinition>()>, std::function<std::unique_ptr<BehaviorNode>()>>> builderMap;
  BehaviorFactory();

  std::unique_ptr<BehaviorDefinition> loadNodeDefinition(std::string const &, Json::Value, BehaviorTreeDefinitionPtr &);
  std::unique_ptr<BehaviorDefinition> tryGetDefinition(std::string const &) const;
  std::unique_ptr<BehaviorNode> tryGetNode(std::string const &) const;

  ~BehaviorFactory();
};
