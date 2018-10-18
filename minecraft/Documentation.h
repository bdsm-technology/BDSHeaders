#pragma once

#include <map>
#include <string>

namespace Core {
struct FileStream;
}

namespace Documentation {
struct Node {
  enum struct Type : unsigned { Empty, Normal };
  Type type;                                  // 0
  bool b4;                                    // 4
  bool b5;                                    // 5
  bool b6;                                    // 6
  std::string vType;                          // 8
  std::string vValue;                         // 40
  std::string vDescription;                   // 72
  std::string sParameter;                     // 104
  std::string sType;                          // 136
  std::string sName;                          // 168
  std::string sDefaultValue;                  // 200
  std::string sDescription;                   // 232
  std::string sUsage;                         // 264
  std::map<std::string, Node> children;       // 296
  std::map<std::string, std::string> example; // 296
  std::string fullName;                       // 392

  Node();
  Node(Node const &);
  Node(std::string const &);
  Node(unsigned, std::string const &, std::string const &);
  Node(unsigned, std::string const &, std::string const &, std::string const &, std::string const &);

  Node &operator=(Node &&);

  std::string const &getFullName() const;
  std::string getName() const;

  void addExample(std::string const &, std::string const &);
  void addNode(std::string const &, std::string const &);
  void addNode(std::string const &, std::string const &, std::string const &, std::string const &);

  ~Node();
};

struct Writer {
  std::map<std::string, Node> tree;

  Writer();

  static std::string NODE_ENTITYDEFINITIONPROPERTIES; //[01]Entity Definition Properties
  static std::string NODE_ATTRIBUTES;                 //[02]Attributes
  static std::string NODE_PROPERTIES;                 //[03]Properties
  static std::string NODE_COMPONENTIDS;               //[04]Component IDs
  static std::string NODE_COMPONENTS;                 //[05]Components
  static std::string NODE_AI_GOALS;                   //[06]AI Goals
  static std::string NODE_FILTERS;                    //[07]Filters
  static std::string NODE_TRIGGERS;                   //[08]Triggers
  static std::string NODE_EVENTS;                     //[09]Built-in Events
  static std::string NODE_ENTITIES;                   //[10]Entities
  static std::string NODE_BLOCKS;                     //[11]Blocks
  static std::string NODE_ITEMS;                      //[12]Items
  static std::string NODE_GEOMETRY;                   //[13]Geometry

  static std::string STRING_TYPE;  // String
  static std::string INT_TYPE;     // Integer
  static std::string UINT_TYPE;    // Positive Integer
  static std::string FLOAT_TYPE;   // Decimal
  static std::string RANGE_TYPE;   // Range [a, b]
  static std::string VEC3_TYPE;    // Vector [a, b, c]
  static std::string VEC4_TYPE;    // Vector [a, b, c, d]
  static std::string LIST_TYPE;    // List
  static std::string TRIGGER_TYPE; // Trigger
  static std::string BOOLEAN_TYPE; // Boolean
  static std::string OBJECT_TYPE;  // JSON Object
  static std::string FILTER_TYPE;  // Minecraft Filter

  static Writer mDocs;
  static Writer &getWriter();

  void buildDocumentation();
  void copyVanilla();
  Node getPage(std::string const &);
  void printDocumentation(std::string const &);
  void printDocumentationHTML(std::string const &);
  void printHTMLDescription(Core::FileStream &, std::string const &);
  void printHTMLTable(Core::FileStream &, Node const &);
  void printNode(Core::FileStream &, Node const &);
  void printNodeHTML(Core::FileStream &, std::map<std::string, Node> const &);
  void printWikiTable(Core::FileStream &, Node const &);
  void registerDocumentation(std::string, void (*)(Writer &));

  ~Writer();
};
} // namespace Documentation