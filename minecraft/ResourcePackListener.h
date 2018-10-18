#pragma once

#include <gsl/gsl>

struct ResourcePackManager;

struct ResourcePackListener {
  ResourcePackListener();
  virtual ~ResourcePackListener();
  virtual void onActiveResourcePacksChanged(ResourcePackManager &);
  virtual void onFullPackStackInvalid();
  virtual void onVanillaPackDownloadComplete();
  virtual void onLanguageSubpacksChanged();
};

enum struct EducationFeature : char { Chemistry = 1, Education = 2, CodeBuilder = 4 };
struct LevelData;

struct EducationOptions : ResourcePackListener {
  static gsl::string_span<> CHEMISTRY_ENABLED;
  static EducationOptions DEFAULT_OPTION;

  ResourcePackManager *resPackManager; // 8
  EducationFeature features;           // 16

  EducationOptions(ResourcePackManager *);

  bool _isFeatureEnabled(EducationFeature) const;
  void _setFeature(EducationFeature, bool);

  void init(LevelData const &);

  virtual ~EducationOptions();
  virtual void onActiveResourcePacksChanged(ResourcePackManager &);

  static bool isChemistryEnabled();
  static bool isEducationEnabled();
  static bool isCodeBuilderEnabled();
};