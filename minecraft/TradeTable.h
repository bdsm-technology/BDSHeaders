#pragma once

#include "LootTable.h"
#include "types.h"

struct TradeItem {
  void *unk0;                                           // 0
  void *unk8;                                           // 8
  std::vector<std::unique_ptr<LootItemFunction>> funcs; // 16

  TradeItem(TradeItem &&);
  TradeItem();
  ~TradeItem();
};

struct Trade {
  int unk0;                       // 0
  bool unk4;                      // 4
  std::vector<TradeItem> items8;  // 8
  std::vector<TradeItem> items32; // 32

  Trade(Trade &&);
  Trade();
  ~Trade();
};

struct TradeTable {
  std::string name;                       // 0
  std::vector<std::vector<Trade>> trades; // 32

  TradeTable(std::string const &);
  ~TradeTable();
};

struct TradeTables {
  std::unordered_map<std::string, std::unique_ptr<TradeTable>> table_map;

  TradeTables();
  TradeTable *fetchTables(std::string const &);
  void reload(Level &);

  ~TradeTables();
};