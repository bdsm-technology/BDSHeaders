#pragma once

struct ResourcePackManager;

struct ResourcePackListener {
  ResourcePackListener();
  virtual ~ResourcePackListener();
  virtual void onActiveResourcePacksChanged(ResourcePackManager &);
  virtual void onFullPackStackInvalid();
  virtual void onVanillaPackDownloadComplete();
  virtual void onLanguageSubpacksChanged();
};

struct ChemistryOptions : ResourcePackListener {
  ResourcePackManager *resPackManager; // 8
  bool chemistryEnabled;               // 16
  virtual ~ChemistryOptions();
  virtual void onActiveResourcePacksChanged(ResourcePackManager &);

  bool _isChemistryEnabled() const;
  static bool isChemistryEnabled();
};