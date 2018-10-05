#pragma once

#include <memory>

struct AppPlatform;
struct Minecraft;
namespace Automation {
struct AutomationClient;
}
struct TextFilteringProcessor;

struct IMinecraftApp {
  IMinecraftApp();
  virtual ~IMinecraftApp();
};

struct DedicatedServer : IMinecraftApp {
  AppPlatform *platform;                                          // 8
  Minecraft *minecraft;                                           // 16
  std::unique_ptr<Automation::AutomationClient> automationClient; // 24
  std::unique_ptr<TextFilteringProcessor> textProcessor;          // 32
  bool stopped;

  virtual ~DedicatedServer();
  virtual Minecraft *getPrimaryMinecraft();
  virtual Automation::AutomationClient *getAutomationClient() const;
  virtual bool isEduMode() const;
  virtual bool isDedicatedServer() const;
  virtual unsigned int getDefaultNetworkMaxPlayers() const;
  virtual void onNetworkMaxPlayersChanged();

  void run();
  void start();
  void stop();
};