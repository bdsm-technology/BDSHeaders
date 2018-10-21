#pragma once

#include "item.h"
#include "types.h"

struct Player;
struct AgentCommandComponent;

namespace AgentCommands {

enum struct Direction : char { up, down, forward, back, left, right };

struct Command {
  Actor *act;       // 8
  Player *owner;    // 16
  std::string name; // 24
  bool b56;         // 56
  bool b57;         // 57

  static std::string InspectKey;       // blockName
  static std::string InspectDataKey;   // data
  static std::string GetItemCountKey;  // stackCount
  static std::string GetItemSpaceKey;  // spaceCount
  static std::string GetItemDetailKey; // itemName

  Command(Player &, AgentCommandComponent &, std::string);

  virtual ~Command();                  // 0, 8
  virtual void execute() = 0;          // 16
  virtual bool isDone()  = 0;          // 24
  virtual void tick();                 // 32
  virtual void fireCommandDoneEvent(); // 40
};

struct AttackCommand : Command {
  Direction dir; // 58
  AttackCommand(Player &, AgentCommandComponent &, Direction);
  virtual ~AttackCommand();
  virtual void execute();
  virtual bool isDone();
};

struct DestroyCommand : Command {
  Direction dir; // 58
  DestroyCommand(Player &, AgentCommandComponent &, Direction);
  virtual ~DestroyCommand();
  virtual void execute();
  virtual bool isDone();
};

struct DetectCommand : Command {
  Direction dir; // 58
  DetectCommand(Player &, AgentCommandComponent &, Direction);
  virtual ~DetectCommand();
  virtual void execute();
  virtual bool isDone();
};

struct DetectRedStoneCommand : Command {
  Direction dir; // 58
  DetectRedStoneCommand(Player &, AgentCommandComponent &, Direction);
  virtual ~DetectRedStoneCommand();
  virtual void execute();
  virtual bool isDone();
};

struct DropAllCommand : Command {
  Direction dir; // 58
  DropAllCommand(Player &, AgentCommandComponent &, Direction);
  virtual ~DropAllCommand();
  virtual void execute();
  virtual bool isDone();
};

struct TurnCommand : Command {
  Direction dir; // 58
  TurnCommand(Player &, AgentCommandComponent &, Direction);
  virtual ~TurnCommand();
  virtual void execute();
  virtual bool isDone();
};

struct InspectCommand : Command {
  Direction dir; // 58
  std::string data;
  InspectCommand(Player &, AgentCommandComponent &, Direction);
  virtual ~InspectCommand();
  virtual void execute();
  virtual bool isDone();
};

struct MoveCommand : Command {
  Direction dir; // 58
  Vec3 v60;      // 60
  bool unk72;    // 72
  Vec3 v76;      // 76
  Vec3 v88;      // 88
  MoveCommand(Player &, AgentCommandComponent &, Direction);
  virtual ~MoveCommand();
  virtual void execute();
  virtual bool isDone();
};

struct PlaceCommand : Command {
  ItemInstance item; // 64
  int unk176;        // 176
  Direction dir;     // 180
  PlaceCommand(Player &, AgentCommandComponent &, ItemInstance const &, int, Direction);
  virtual ~PlaceCommand();
  virtual void execute();
  virtual bool isDone();
};

struct TileCommand : Command {
  ItemInstance item; // 64
  Direction dir;     // 176
  TileCommand(Player &, AgentCommandComponent &, std::string, Direction);
  virtual ~TileCommand();
  virtual void execute();
  virtual bool isDone();
};

struct DropCommand : Command {
  Direction dir; // 58
  int unk60;     // 60
  int unk64;     // 64
  DropCommand(Player &, AgentCommandComponent &, int, int, Direction);
  virtual ~DropCommand();
  virtual void execute();
  virtual bool isDone();
};

struct CollectCommand : Command {
  enum struct CollectionSpecification { all };
  int unk60;                    // 60
  CollectionSpecification spec; // 64
  bool unk68;                   // 68
  CollectCommand(Player &, AgentCommandComponent &, int, CollectionSpecification);
  virtual ~CollectCommand();
  virtual void execute();
  virtual bool isDone();
};

struct GetItemCountCommand : Command {
  int itemId; // 60
  GetItemCountCommand(Player &, AgentCommandComponent &, int);
  virtual ~GetItemCountCommand();
  virtual void execute();
  virtual bool isDone();
  virtual void fireCommandDoneEvent();
};

struct GetItemDetailsCommand : Command {
  ItemInstance item; // 64
  GetItemDetailsCommand(Player &, AgentCommandComponent &, ItemInstance const &);
  virtual ~GetItemDetailsCommand();
  virtual void execute();
  virtual bool isDone();
  virtual void fireCommandDoneEvent();
};

struct GetItemSpaceCommand : Command {
  int itemId; // 60
  int unk64;  // 64
  GetItemSpaceCommand(Player &, AgentCommandComponent &, int, int);
  virtual ~GetItemSpaceCommand();
  virtual void execute();
  virtual bool isDone();
  virtual void fireCommandDoneEvent();
};

struct TransferToCommand : Command {
  int unk60; // 60
  int unk64; // 64
  int unk68; // 68
  TransferToCommand(Player &, AgentCommandComponent &, int, int, int);
  virtual ~TransferToCommand();
  virtual void execute();
  virtual bool isDone();
  virtual void fireCommandDoneEvent();
};

} // namespace AgentCommands