#pragma once

#include "Automation.h"
#include "NetworkIdentifier.h"
#include "types.h"
#include <set>

struct Minecraft;

struct AutomationCmdOutput;

enum struct OriginType : char {
  Player = 0,
  Block = 1,
  MinecartBlock = 2,
  DevConsole = 3,
  Test = 4,
  AutomationPlayer = 5,
  ClientAutomation = 6,
  DedicatedServer = 7,
  Actor = 8,
  Virtual = 9,
  GameArgument = 10,
  ActorServer = 11
};

struct CommandOriginData {};

struct CommandOrigin {
  mce::UUID uuid; // 8

  CommandOrigin();

  virtual ~CommandOrigin();                                  // 0, 8
  virtual std::string getRequestId() const;                  // 16
  virtual std::string getName() const;                       // 24
  virtual BlockPos getBlockPosition() const;                 // 32
  virtual Vec3 getWorldPosition() const;                     // 40
  virtual Level *getLevel() const;                           // 48
  virtual Dimension *getDimension() const;                   // 56
  virtual Actor *getEntity() const;                          // 64
  virtual CommandPermissionLevel getPermissionLevel() const; // 72
  virtual std::unique_ptr<CommandOrigin> clone() const;      // 80
  virtual bool canCallHiddenCommands() const;                // 88
  virtual bool hasChatPerms() const;                         // 96
  virtual bool hasTellPerms() const;                         // 104
  virtual bool canUseAbility(std::string const &) const;     // 112
  virtual NetworkIdentifier getSourceId() const;             // 120
  virtual char getSourceSubId() const;                       // 128
  virtual CommandOrigin *getOutputReceiver() const;          // 136
  virtual OriginType getOriginType() const;                  // 144
  virtual CommandOriginData toCommandOriginData() const;     // 152
  virtual mce::UUID const &getUUID() const;                  // 160
  virtual void setUUID(mce::UUID const &);                   // 168
};

struct DedicatedServerCommandOrigin : CommandOrigin {
  Minecraft *mc;    // 24
  std::string name; // 32

  DedicatedServerCommandOrigin(std::string const &, Minecraft &);

  virtual ~DedicatedServerCommandOrigin();                   // 0, 8
  virtual std::string getRequestId() const;                  // 16
  virtual std::string getName() const;                       // 24
  virtual BlockPos getBlockPosition() const;                 // 32
  virtual Vec3 getWorldPosition() const;                     // 40
  virtual Level *getLevel() const;                           // 48
  virtual Dimension *getDimension() const;                   // 56
  virtual Actor *getEntity() const;                          // 64
  virtual CommandPermissionLevel getPermissionLevel() const; // 72
  virtual std::unique_ptr<CommandOrigin> clone() const;      // 80
  virtual bool canCallHiddenCommands() const;                // 88
  virtual OriginType getOriginType() const;                  // 144
};

struct CommandOutput;
struct Packet;

struct CommandOutputSender {
  Automation::AutomationClient *client;            // 8
  std::function<void(AutomationCmdOutput &)> func; // 16

  CommandOutputSender(Automation::AutomationClient &);

  void sendToAdmins(CommandOrigin const &, CommandOutput const &, CommandPermissionLevel);
  std::vector<std::string> translate(std::vector<std::string> const &);

  virtual ~CommandOutputSender();
  virtual void send(CommandOrigin const &, CommandOutput const &);
  virtual void registerOutputCallback(std::function<void(AutomationCmdOutput &)> const &);
};

struct CommandLexer {
  std::string value; // 0
  char *c_string;    // 8
  int unk16;         // 16
  int unk20;         // 20

  enum struct TokenType;

  static bool isDigit(char);
  static bool isIdentifierCharacter(char);
  static bool isSpace(char);

  char *next();
  void step();
};

struct CommandVersion {
  int min, max; // 0, 4

  static int CurrentVersion;

  CommandVersion(int, int);

  bool isCompatible(int) const;
  bool overlaps(CommandVersion const &) const;
};

struct Command {
  int unk8;         // 8
  void *unk16;      // 16
  int unk24;        // 24
  bool b28;         // 28
  CommandFlag flag; // 29

  Command();
  virtual ~Command();                                               // 0, 8
  virtual void execute(CommandOrigin const &, CommandOutput &) = 0; // 16
};

static_assert(32 == sizeof(Command));

struct CommandParameterData;
struct ItemInstance;

struct CommandItem {
  int unk0;          // 0
  unsigned int item; // 4
  CommandItem();
  CommandItem(std::size_t);
  CommandItem(int);

  unsigned int getId() const;
  ItemInstance getInstance(int, int) const;
  operator bool() const;
  operator unsigned int() const;
};

struct AutoCompleteOption {
  std::string s0;   // 0
  std::string s32;  // 32
  std::string s64;  // 64
  unsigned unk96;   // 96
  unsigned unk100;  // 100
  unsigned unk104;  // 104
  CommandItem item; // 108
  bool b116;        // 116

  AutoCompleteOption(std::string const &, std::string const &, std::string const &, unsigned int, unsigned int, unsigned int);
  AutoCompleteOption(AutoCompleteOption &&);
  AutoCompleteOption &operator=(AutoCompleteOption &&);
  ~AutoCompleteOption();
};

struct AutoCompleteInformation {
  std::vector<AutoCompleteOption> options;
  AutoCompleteInformation();
  ~AutoCompleteInformation();
};

struct OverloadSyntaxInformation {
  std::string data; // 0
  unsigned unk32;   // 32
  unsigned unk36;   // 36

  OverloadSyntaxInformation(OverloadSyntaxInformation &&);
  OverloadSyntaxInformation(std::string const &, unsigned, unsigned);

  ~OverloadSyntaxInformation();
};

struct CommandSyntaxInformation {
  bool b0;                                          // 0
  std::string name;                                 // 8
  std::vector<OverloadSyntaxInformation> overloads; // 40

  CommandSyntaxInformation(bool, std::string const &, std::vector<OverloadSyntaxInformation> &&);
  CommandSyntaxInformation();

  ~CommandSyntaxInformation();
};

struct CommandOperator;

struct AvailableCommandsPacket;

struct CommandSelectorBase;

struct CommandRegistry {
  enum struct HardNonTerminal {};
  struct Symbol {
    unsigned val;

    Symbol(unsigned long);
    Symbol();
    Symbol(Symbol const &);
    Symbol(CommandLexer::TokenType);
    Symbol(HardNonTerminal);

    static Symbol fromEnumIndex(unsigned long);
    static Symbol fromEnumValueIndex(unsigned long);
    static Symbol fromFactorizationIndex(unsigned long);
    static Symbol fromOptionalIndex(unsigned long);
    static Symbol fromPostfixIndex(unsigned long);
    static Symbol fromSoftEnumIndex(unsigned long);

    Symbol &operator=(Symbol const &);
    bool operator==(Symbol const &) const;
    bool operator!=(Symbol const &) const;
    bool operator<(Symbol const &) const;

    bool isEnum() const;
    bool isEnumValue() const;
    bool isFactorization() const;
    bool isOptional() const;
    bool isPostfix() const;
    bool isSoftEnum() const;
    bool isTerminal() const;

    unsigned toIndex() const;
    unsigned value() const;
  };
  struct ParseToken {
    std::unique_ptr<CommandRegistry::ParseToken> tk0; // 0
    std::unique_ptr<CommandRegistry::ParseToken> tk8; // 8
    CommandRegistry::ParseToken *tk16;                // 16
    void *unk24;                                      // 24
    int unk32;                                        // 32
    Symbol symbol;                                    // 36

    ParseToken(Symbol, ParseToken *);
    ~ParseToken();
  };
  struct ParseRule {
    Symbol symbol;                                        // 0
    std::function<ParseToken *(ParseToken &, Symbol)> fn; // 8
    std::vector<CommandRegistry::Symbol> symbols;         // 40
    CommandVersion version;                               // 64

    ParseRule(CommandRegistry::Symbol, std::vector<CommandRegistry::Symbol> &&, std::function<CommandRegistry::ParseToken *(CommandRegistry::ParseToken &, CommandRegistry::Symbol)>, CommandVersion);
    ParseRule &operator=(ParseRule const &);
    ~ParseRule();
  };
  struct ParseTable {
    std::map<Symbol, std::vector<Symbol>> msvs0;                                      // 0
    std::map<Symbol, std::vector<Symbol>> msvs48;                                     // 48
    std::map<std::pair<CommandRegistry::Symbol, CommandRegistry::Symbol>, int> mpssi; // 96
    ParseTable();
    ~ParseTable();
  };
  struct OptionalParameterChain {
    int unk0, unk4; // 0, 4
    Symbol symbol;  // 8

    OptionalParameterChain(int, int, Symbol);
    OptionalParameterChain(OptionalParameterChain const &);
    OptionalParameterChain &operator=(OptionalParameterChain &&);
  };
  struct Enum {
    using ParseFn = bool (CommandRegistry::*)(void *, CommandRegistry::ParseToken const &, CommandOrigin const &, int, std::string &, std::vector<std::string> &);
    std::string name;                                          // 0
    typeid_t<CommandRegistry> tid;                             // 32
    ParseFn parseFn;                                           // 40
    std::vector<std::pair<unsigned long, unsigned long>> vpuu; // 56
    Enum(std::string const &, typeid_t<CommandRegistry>, ParseFn const);
    Enum(Enum &&);
    ~Enum();
  };
  struct Factorization {
    Symbol symbol;
    Factorization(Factorization &&);
    Factorization(Symbol);
    Factorization &operator=(Factorization &&);
  };
  struct Overload {
    using FetchFn = std::unique_ptr<Command> (*)(void);
    CommandVersion version;                   // 0
    FetchFn fetch;                            // 8
    std::vector<CommandParameterData> params; // 16
    int unk40;                                // 40
    Overload(CommandVersion, FetchFn);
    Overload(Overload &&);
    ~Overload();
  };
  struct Signature {
    std::string name;                // 0
    std::string desc;                // 32
    std::vector<Overload> overloads; // 64
    CommandPermissionLevel perm;     // 88
    Symbol main_symbol;              // 92
    Symbol alt_symbol;               // 96
    CommandFlag flag;                // 100
    int unk104;                      // 104
    int unk108;                      // 108
    int unk112;                      // 112
    bool b116;                       // 116

    Signature(std::string const &, char const *, CommandPermissionLevel, Symbol, CommandFlag);
    Signature(Signature &&);
    ~Signature();
  };
  struct SoftEnum {
    std::string name;              // 0
    std::vector<std::string> list; // 32
    SoftEnum(std::string const &, std::vector<std::string>);
    SoftEnum(SoftEnum &&);
    SoftEnum &operator=(SoftEnum &&);
    ~SoftEnum();
  };
  struct RegistryState {
    char filler0[32];                 // 0
    std::vector<unsigned int> status; // 32
    RegistryState();
    RegistryState(RegistryState const &);
    RegistryState(RegistryState &&);
    ~RegistryState();
  };
  struct ParamSymbols {
    Symbol symbols[18];
    ParamSymbols();
  };
  template <typename T> struct DefaultIdConverter {
    std::size_t operator()(T const &) const;
    T operator()(std::size_t) const;
  };
  struct Parser;

  using ParseEnumFn = bool (CommandRegistry::*)(void *, CommandRegistry::ParseToken const &, CommandOrigin const &, int, std::string &, std::vector<std::string> &) const;

  std::function<void(Packet const &)> packet_funcs;                         // 0
  std::function<void(bool &, std::string const &, Actor const &)> handlers; // 32
  std::vector<CommandRegistry::ParseRule> rules;                            // 64
  std::map<unsigned int, CommandRegistry::ParseTable> tables_map;           // 88
  std::vector<CommandRegistry::OptionalParameterChain> optional_chain;      // 136
  std::vector<std::string> vs160;                                           // 160
  std::vector<CommandRegistry::Enum> enums;                                 // 184
  std::vector<CommandRegistry::Factorization> factorization;                // 208
  std::vector<std::string> vs232;                                           // 232
  std::map<std::string, unsigned> msu256;                                   // 256
  std::map<std::string, unsigned> msu304;                                   // 304
  std::vector<CommandRegistry::Symbol> symbols;                             // 352
  std::map<std::string, Signature> command_sigs;                            // 376
  std::map<typeid_t<CommandRegistry>, int> mti;                             // 424
  std::map<std::string, std::string> mss;                                   // 472
  std::vector<CommandRegistry::SoftEnum> soft_enums;                        // 520
  std::map<std::string, unsigned> msu544;                                   // 544
  std::vector<CommandRegistry::RegistryState> states;                       // 592
  ParamSymbols param_symbols;                                               // 616

  static void *getFieldSet(Command *, CommandParameterData const &);
  static CommandVersion getVersionField(Command *, CommandParameterData const &);
  static bool isParseMatch(CommandParameterData const &, Symbol);
  static bool originCanRun(CommandOrigin const &, Signature const &);

  template <typename Type, typename TConverter> bool parseEnum(void *, ParseToken const &, CommandOrigin const &, int, std::string &, std::vector<std::string> &) const { return true; }

  template <typename Type> bool parse(void *, ParseToken const &, CommandOrigin const &, int, std::string &, std::vector<std::string> &) const;

  Symbol addEnumValuesInternal(std::string const &, std::vector<std::pair<std::string, unsigned long>> const &, typeid_t<CommandRegistry>, ParseEnumFn);
  Symbol addEnumValuesInternal(std::string const &, std::vector<std::pair<unsigned long, unsigned long>> const &, typeid_t<CommandRegistry>, ParseEnumFn);
  int addEnumValues(std::string const &, std::vector<std::string> const &);
  template <typename Type, typename TConverter> int addEnumValues(std::string const &name, std::vector<std::pair<std::string, Type>> const &data) {
    TConverter cvt;
    std::vector<std::pair<std::string, unsigned long>> result;
    for (auto &[text, value] : data) {
      auto real = cvt(value);
      result.emplace_back({text, real});
    }
    return addEnumValuesInternal(name, result, &parseEnum<Type, TConverter>).value;
  }
  void addEnumValuesToExisting(unsigned int, std::vector<std::pair<unsigned long, unsigned long>> const &);
  void addPostfix(std::string const &);
  void addRule(Symbol, std::vector<Symbol> &&, std::function<ParseToken *(ParseToken &, Symbol)>, CommandVersion);
  void addSoftEnum(std::string const &, std::vector<std::string>);
  void addSoftEnumValues(std::string const &, std::vector<std::string>);
  void addSoftTerminal(std::string const &);
  void addSymbols(std::vector<Symbol> &, std::vector<Symbol> const &, bool);
  template <typename TCommand> std::unique_ptr<Command> allocateCommand() { return std::make_unique<TCommand>(); }
  void autoComplete(Symbol, std::string const &, bool, CommandOrigin const &, AutoCompleteInformation &) const;
  void autoCompleteCommand(std::string const &, CommandOrigin const &, AutoCompleteInformation &) const;
  void buildFirstSet(ParseTable &, Symbol, unsigned int);
  void buildFollowSet(ParseTable &, Symbol, unsigned int, std::set<Symbol> &);
  void buildOptionalRuleChain(Signature const &, std::vector<CommandParameterData> const &, CommandParameterData const *, unsigned long long);
  void buildOptionalRuleChain(Signature const &, std::vector<CommandParameterData> const &, std::vector<Symbol> const &);
  void buildParseTable(unsigned);
  void buildRules(Signature &, std::vector<Overload *> const &, unsigned long long);
  void buildOverload(Overload &);
  template <typename... PS> void buildOverload(Overload &, CommandParameterData const &, PS const &...);
  bool canCommandBeUsed(std::string const &, CommandPermissionLevel const &, bool) const;
  ParseToken &collapse(ParseToken &, Symbol);
  ParseToken &collapseOn(ParseToken &, Symbol, Symbol);
  std::unique_ptr<Command> createCommand(ParseToken const &, CommandOrigin const &, int, std::string &, std::vector<std::string> &) const;
  std::string describe(Signature const &, std::string const &, Overload const &, unsigned int, unsigned int *, unsigned int *) const;
  ParseToken *expand(ParseToken &, Symbol);
  ParseToken *expandExcept(ParseToken &, Symbol, Symbol);
  ParseToken *expandOn(ParseToken &, Symbol, Symbol);
  Signature const *findCommand(std::string const &) const;
  Signature *findCommand(std::string const &);
  Symbol findEnumValue(std::string const &);
  Symbol findPostfix(std::string const &);
  std::vector<Symbol> first(ParseTable &, std::vector<Symbol> const &);
  ParseToken &fold(ParseToken &, Symbol, Symbol);
  void forEachNonTerminal(std::function<void(Symbol)>) const;
  void forEachTerminal(std::function<void(Symbol)>) const;
  std::vector<std::string> getAlias(std::string const &) const;
  std::unique_ptr<AutoCompleteInformation> getAutoCompleteOptions(CommandOrigin const &, std::string const &, unsigned) const;
  std::string getCommandName(std::string const &) const;
  CommandSyntaxInformation getCommandOverloadSyntaxInformation(CommandOrigin const &, std::string const &) const;
  unsigned getCommandStatus(std::string const &) const;
  CommandPermissionLevel getCommandsPermissionLevel(std::string const &) const;
  Enum &getEnumData(ParseToken const &) const;
  void getOverloadSyntaxInformation(CommandOrigin const &, CommandSyntaxInformation &, ParseToken const *) const;
  CommandSyntaxInformation getOverloadSyntaxInformation(CommandOrigin const &, std::string const &, unsigned) const;
  Symbol getParseSymbol(CommandParameterData const &);
  bool hasCommands() const;
  bool isCommandOfType(std::string const &, CommandTypeFlag) const;
  bool isOptionalChainMatch(Symbol const *, OptionalParameterChain const *) const;
  bool isValid(Symbol) const;
  void kill(ParseToken &, Symbol);
  void loadRemoteCommands(AvailableCommandsPacket const &);
  bool parseOperator(CommandOperator *, ParseToken const &, std::string &, std::vector<std::string> &) const;
  bool parseParameter(Command *, CommandParameterData const &, ParseToken const &, CommandOrigin const &, int, std::string &, std::vector<std::string> &) const;
  bool parsePartialCommand(Parser &, std::string, unsigned int, bool) const;
  bool parseSelector(CommandSelectorBase *, ParseToken const &, CommandOrigin const &, int, std::string &, std::vector<std::string> &, bool) const;
  void pushState();
  void popState();
  bool readFloat(float &, ParseToken const &, std::string &, std::vector<std::string> &);
  bool readInt(int &, ParseToken const &, std::string &, std::vector<std::string> &);
  bool readRelativeCoordinate(bool &, float &, ParseToken const &, std::string &, std::vector<std::string> &);
  void registerAlias(std::string, std::string);
  void registerCommand(std::string const &, char const *, CommandPermissionLevel, CommandFlag, CommandFlag);
  template <typename TCommand, typename... TParams> void registerOverload(char const *cmd, CommandVersion version, TParams... params) {
    auto sig = findCommand(cmd);
    sig->overloads.emplace_back(version, &allocateCommand<TCommand>);
    Overload &overload = *sig->overloads.rbegin();
    buildOverload(overload);
    overload.params = {params...};
    registerOverloadInternal(*sig, overload);
  };
  void registerOverloadInternal(Signature &, Overload &);
  void removeSoftEnumValues(std::string const &, std::vector<std::string>);
  bool requiresCheatsEnabled(std::string const &) const;
  AvailableCommandsPacket serializeAvailableCommands() const;
  void setNetworkUpdateCallback(std::function<void(Packet const &)>);
  void setSoftEnumValues(std::string const &, std::vector<std::string>);
  void setupOverloadRules(Signature &, Overload &);

  ~CommandRegistry();
};

enum struct CommandParameterDataType {};

struct CommandParameterData {
  using ParseFn = bool (CommandRegistry::*)(void *, CommandRegistry::ParseToken const &, CommandOrigin const &, int, std::string &, std::vector<std::string> &);
  typeid_t<CommandRegistry> tid; // 0
  ParseFn parseFn;               // 8
  std::string name;              // 24
  char const *desc;              // 56
  int unk64;                     // 64
  CommandParameterDataType type; // 68
  int unk72;                     // 72
  int offset;                    // 76
  bool optional;                 // 80

  CommandParameterData(typeid_t<CommandRegistry> tid, ParseFn const parseFn, char const *name, CommandParameterDataType, char const *desc, int offset, bool optional, int);
  CommandParameterData(CommandParameterData const &);
  CommandParameterData &operator=(CommandParameterData const &);
  ~CommandParameterData();
};

struct CommandContext {
  std::string command;                   // 0
  std::unique_ptr<CommandOrigin> origin; // 32
  int version;                           // 40

  CommandContext(std::string const &, std::unique_ptr<CommandOrigin>, int);

  CommandOrigin const *getCommandOrigin() const;
  int getVersion() const;

  ~CommandContext();
};

struct MinecraftCommands {
  std::unique_ptr<CommandOutputSender> output_sender; // 0
  std::unique_ptr<CommandRegistry> registry;          // 8
  CommandPermissionLevel op_permission;               // 16
  Minecraft *mc;                                      // 24
  std::function<bool()> chat_permissions_cb;          // 32

  MinecraftCommands(Minecraft &, Automation::AutomationClient &);
  MCRESULT executeCommand(std::shared_ptr<CommandContext>, bool) const;
  CommandPermissionLevel getOpPermissionLevel() const;
  CommandOutputSender *getOutputSender() const;
  CommandOutputType getOutputType(CommandOrigin const &);
  CommandRegistry const *getRegistry() const;
  CommandRegistry *getRegistry();
  void handleOutput(CommandOrigin const &, CommandOutput const &) const;
  void initCoreEnums(bool);
  void registerChatPermissionsCallback(std::function<bool()>);
  MCRESULT requestCommandExecution(std::unique_ptr<CommandOrigin>, std::string const &, int, bool) const;
  void setOpPermissionLevel(CommandPermissionLevel);
  void setOutputSender(std::unique_ptr<CommandOutputSender>);
  void setRegistryNetworkUpdateCallback(std::function<void(Packet const &)>) const;

  ~MinecraftCommands();
};
