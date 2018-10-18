#pragma once

#include "json.h"
#include "types.h"
#include <functional>
#include <initializer_list>
#include <memory>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

struct FilterInput {
  enum struct Type : int {
    Bool   = 0,
    Int    = 1,
    Float  = 2,
    String = 3,
  };
  Type type;        // 0
  std::string text; // 8
  int intValue;     // 40
  float floatValue; // 44

  FilterInput(FilterInput &&);
  FilterInput(bool);
  FilterInput(int);
  FilterInput(unsigned);
  FilterInput(float);
  FilterInput(char const *);
  FilterInput(std::string);

  FilterInput &operator=(FilterInput const &);
  FilterInput &operator=(FilterInput &&);

  bool operator==(FilterInput const &) const;

  Type getType() const;
  bool getBool() const;
  int getInt() const;
  float getFloat() const;
  std::string getString() const;

  ~FilterInput();
};

static_assert(sizeof(FilterInput) == 48);

struct FilterInputDefinition {
  FilterInput fi;
  std::string desc;

  FilterInputDefinition(FilterInput, std::string &);
  FilterInputDefinition(FilterInputDefinition const &);
  FilterInputDefinition(int);
  FilterInputDefinition(unsigned);

  ~FilterInputDefinition();
};

struct FilterStringMap {
  std::unordered_map<std::string, FilterInputDefinition> data;
  FilterStringMap(std::initializer_list<std::pair<std::string, FilterInputDefinition>>);
  FilterStringMap();

  FilterStringMap &operator=(FilterStringMap &&);
  ~FilterStringMap();
};

struct FilterParamDefinition {
  FilterParamType type;       // 0
  FilterParamRequirement req; // 4
  std::string name;           // 8
  FilterInput filter;         // 40
  FilterStringMap map;        // 88
  FilterParamDefinition(FilterParamType, FilterParamRequirement, std::string, FilterInput, FilterStringMap);

  ~FilterParamDefinition();
};

struct FilterInputs {
  FilterSubject subject; // 0
  FilterInput input1;    // 8
  FilterOperator op;     // 56
  FilterInput onput2;    // 64

  FilterInputs(FilterSubject, FilterInput, FilterOperator, FilterInput);
  FilterInputs();

  ~FilterInputs();
};

static_assert(offsetof(FilterInputs, op) == 56);

struct Actor;
struct VariantParameterList;
struct BlockSource;
struct Dimension;
struct Level;

struct FilterContext {
  Actor *actor;               // 0
  FilterContext *pctx;        // 8
  VariantParameterList *list; // 16
  BlockSource *source;        // 24
  Dimension *dim;             // 32
  Level *level;               // 40
  BlockPos pos;               // 48
  FilterContext();
};

struct FilterTest {
  struct Definition {
    std::string name;                                   // 0
    std::string desc;                                   // 32
    FilterParamDefinition const *d1;                    // 64
    FilterParamDefinition const *d2;                    // 72
    FilterParamDefinition const *d3;                    // 80
    std::function<std::shared_ptr<FilterTest>(void)> f; // 88
    Definition(std::string const &, std::string const &, FilterParamDefinition const *, FilterParamDefinition const *, FilterParamDefinition const *, FilterParamDefinition const *,
               std::function<std::shared_ptr<FilterTest>(void)>);
    Definition(Definition &&);

    ~Definition();
  };
  FilterSubject sub; // 8
  FilterOperator op; // 10

  FilterTest();

  FilterSubject getSubject() const;

  bool _testValuesWithOperator(bool, bool) const;
  bool _testValuesWithOperator(float, float) const;
  bool _testValuesWithOperator(int, int) const;
  bool _testValuesWithOperator(std::string const &, std::string const &) const;

  virtual ~FilterTest();
  virtual bool setup(FilterTest::Definition const &, FilterInputs const &);
  virtual bool evaluate(FilterContext const &) = 0;
};

struct FilterGroup {
  enum struct CollectionType : int {};
  CollectionType type;                                 // 8
  std::vector<std::shared_ptr<FilterGroup>> subgroups; // 16
  std::vector<std::shared_ptr<FilterTest>> tests;      // 40

  FilterGroup(FilterGroup const &);
  FilterGroup(CollectionType);

  virtual ~FilterGroup();
  virtual std::shared_ptr<FilterGroup> _createSubgroup(CollectionType) = 0;
  virtual bool _setupContext(FilterTest const &, FilterContext &) const;
  virtual bool _handleUnknownMember(std::string const &, Json::Value const &);
};

struct ActorFilterGroup : FilterGroup {
  enum struct Processing {};
  struct LegacyMapping {
    CollectionType type;         // 0
    FilterTest::Definition *def; // 8
    FilterSubject sub;           // 16
    FilterOperator op;           // 18
    Processing proc;             // 20
    LegacyMapping(CollectionType, FilterTest::Definition const &, FilterSubject, FilterOperator, Processing);
  };

  ActorFilterGroup(ActorFilterGroup const &);
  ActorFilterGroup(ActorFilterGroup &&);
  ActorFilterGroup(CollectionType);

  ActorFilterGroup &operator=(ActorFilterGroup const &);
  ActorFilterGroup &operator=(ActorFilterGroup &&);

  static void initialize();

  virtual ~ActorFilterGroup();
  virtual std::shared_ptr<FilterGroup> _createSubgroup(CollectionType);
  virtual bool _handleUnknownMember(std::string const &, Json::Value const &);

  bool _addLegacyFilter(std::string const &, LegacyMapping const &, FilterInput const &);
  bool _processLegacyArray(std::string const &, Json::Value const &, LegacyMapping const &);
  bool _processLegacyMemory(std::string const &, Json::Value const &, LegacyMapping const &);

  bool evaluate(Actor const &, VariantParameterList const &);
};