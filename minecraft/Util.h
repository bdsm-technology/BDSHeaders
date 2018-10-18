#pragma once

#include "buffer_span.h"
#include "json.h"
#include "types.h"
#include <cstddef>
#include <gsl/gsl>
#include <set>
#include <string>
#include <unordered_set>
#include <utility>
#include <vector>

struct Level;
struct Container;
struct Random;
struct Actor;
struct DimensionConversionData;
struct ActorTargetFilter;

namespace Util {
extern char COLOR_CODE;          //§
extern std::string EMPTY_GUID;   // 00000000-0000-0000-0000-000000000000
extern std::string EMPTY_STRING; //""
extern std::string NEW_LINE;     //"\n"

std::string safeString(char const *);
std::string boolToString(bool);
std::string getTimeStamp();
std::string toNiceString(int);
std::string generateRandomId(int);
std::string getFilesizeString(unsigned long);
std::string formatTickDuration(int);
std::string format(char const *, ...);
std::string toLower(gsl::string_span<>);
std::string toUpper(gsl::string_span<>);
std::string caseFold(gsl::string_span<>);
std::string toString(float, int);

std::string base64_encode(unsigned char const *, unsigned int, bool);

bool compareNoCase(gsl::string_span<>, gsl::string_span<>);

struct CaseInsensitiveCompare {
  bool operator()(gsl::string_span<>, gsl::string_span<>);
};
struct CaseInsensitiveHash {
  bool operator()(gsl::string_span<>);
};
struct HashString {
  std::string data;
  struct HashFunc {
    size_t operator()(HashString const &) const;
    bool operator()(HashString const &, HashString const &) const;
  };
  HashString(HashString const &);
  HashString(std::string const &);
  HashString();

  HashString &operator=(HashString const &);
  HashString &operator=(std::string const &);
  bool operator!=(HashString const &) const;
  bool operator==(HashString const &) const;

  std::string const &getString() const;

  ~HashString();
};

template <typename T> struct FABI {
  T t;
  operator T() { return t; }
};

#define FABI_IMPL(name, ...)                                                                                                                                                                           \
  struct fabi_##name : FABI<__VA_ARGS__> {}
FABI_IMPL(string, std::string);
FABI_IMPL(wstring, std::wstring);
FABI_IMPL(vec_string, std::vector<std::string>);
#undef FABI_IMPL

fabi_string base64_encode(unsigned char const *, bool);
fabi_string base64_decode(unsigned char const *, unsigned int, bool);
fabi_string compress(std::string const &);
fabi_string decompress(std::string const &);
fabi_string decompress(std::string const &, bool &);
fabi_string cp1252ToUTF8(std::string const &);
fabi_string cp437ToUTF8(std::string const &);
fabi_string fromHex(std::string const &);
fabi_string removeAllColorCodes(std::string const &);
fabi_string removeChars(std::string, std::string const &);
fabi_string removeIllegalChars(std::string);
fabi_string simpleFormat(std::string const &, std::vector<std::string> const &);
fabi_string urlEncode(std::string const &);
fabi_string stringTrim(std::string const &);
fabi_string stringTrim(std::string const &, std::string const &);
fabi_string stringUTF16ToUTF8(std::wstring const &);
fabi_wstring stringUTF8ToUTF16(std::string const &);
fabi_vec_string split(std::string const &, char);
fabi_vec_string split(std::string const &, std::vector<std::string> const &, bool);
fabi_vec_string splitLines(std::string const &);
void trimVectorStrings(std::vector<std::string> &);
void splitString(std::string const &, char, std::vector<std::string> &);
void stringReplace(std::string &, std::string const &, std::string const &, int);
void normalizeLineEndings(std::string &);
unsigned countWordOccurrences(gsl::string_span<>, gsl::string_span<>);
bool isIntegral(std::string);
bool isNumber(std::string const &);
bool isNumberStringGreaterThan(std::string const &, std::string const &);
bool isNumberStringLessThan(std::string const &, std::string const &);
bool stringInVector(std::vector<std::string> const &, std::string const &);
bool stringContains(std::string const &, char);
bool startsWith(std::string const &, std::string const &);
bool endsWith(std::string const &, std::string const &);
bool endsWithCaseInsensitive(std::string const &, std::string const &);
bool isValidIP(gsl::string_span<>, bool, bool);
bool isValidIPv4(gsl::string_span<>);
bool isValidIPv6(gsl::string_span<>);
bool isValidPath(gsl::string_span<>);
void freeStringMemory(std::string &);
std::size_t hashCode(std::string const &);

size_t utf8len(std::string const &);
size_t utf8lenNoColorCodes(std::string const &);
ptrdiff_t utf8lenDetta(std::string const &, std::string const &);
fabi_string utf8substring(std::string const &);

namespace LootTableUtils {
bool fillContainer(Level &, Container &, Random &, std::string const &, Actor *);
}
void _breakIntoWordsAndFindProfanity(std::string &, std::string &, std::vector<std::pair<int, int>> const &, std::unordered_set<char> const &, std::set<std::pair<int, int>> &,
                                     std::unordered_map<std::string, int> const &, std::unordered_set<std::string> const &);
bool _convertPointFromEnd(Vec3 const &, Vec3 &, DimensionId, Vec3 const &);
bool _convertPointFromNether(Vec3 const &, Vec3 &, DimensionId, int);
bool _convertPointFromOverworld(Vec3 const &, Vec3 &, DimensionId, int);
bool convertPointBetweenDimensions(Vec3 const &, Vec3 &, DimensionId, DimensionId, DimensionConversionData const &);
void _recordProfanityLocationInWord(std::string const &, std::string &, std::vector<std::pair<int, int>> const &, int, int, std::set<std::pair<int, int>> &,
                                    std::unordered_map<std::string, int> const &, std::unordered_set<std::string> const &);
bool appendUtf8Character(std::string &, int);
uint64_t convertCapabilitiesStringToEnum(gsl::string_span<>);
unsigned long span_find(gsl::string_span<>, gsl::string_span<>);
HashString filterProfanityFromString(std::string const &, std::unordered_map<std::string, int> const &, std::unordered_set<std::string> const &);
void loadEntityTargetFilter(ActorTargetFilter &, Json::Value const &);
void loadTargetFilter(ActorTargetFilter &, Json::Value const &);
void loadGameVersion(GameVersion &, Json::Value const &);
size_t memcpy_strided(void *, ulong, void const *, ulong, ulong, ulong);
template <typename T> bool parseRange(std::string const &, T &, T &);

template <typename T> T swapEndian(T);
template <typename T> bool isAllZero(buffer_span<T>);
template <typename T> fabi_string toBase64(T const &);

bool toBool(std::string const &, bool &);
template <typename T, void *ptr> bool toFloat(std::string const &, T &);
template <typename T, void *ptr> bool toInt(gsl::string_span<>, T &);
char toHex(char);
fabi_string toHex(std::string const &);
bool toInt(std::string const &, int &, int, int);
fabi_string toString(std::string const &);
fabi_wstring toWideString(std::string const &);
fabi_string toString(std::wstring const &);
template <typename T, void *, void *> bool toString(T &);
uint64_t u64FromString(std::string const &);
} // namespace Util
