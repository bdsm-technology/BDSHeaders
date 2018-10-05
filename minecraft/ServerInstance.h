#pragma once
#include "RakNet/types.h"
#include "spsc.h"
#include <atomic>
#include <condition_variable>
#include <functional>
#include <memory>
#include <mutex>
#include <thread>

struct AppPlatform;
struct PushNotificationReceived;
struct DedicatedServer;
struct Minecraft;
struct NetworkHandler;
struct LoopbackPacketSender;
struct Timer;
struct Scheduler;
struct ChemistryOptions;
struct LevelStorage;
struct VanillaServerGameplayEventListener;
struct VanillaNetworkEventListener;

struct alignas(8) AppPlatformListener {
  AppPlatform *platform; // 8
  virtual ~AppPlatformListener();
  virtual void onLowMemory();
  virtual void onAppPaused();
  virtual void onAppPreSuspended();
  virtual void onAppSuspended();
  virtual void onAppResumed();
  virtual void onAppFocusLost();
  virtual void onAppFocusGained();
  virtual void onAppTerminated();
  virtual void onOperationModeChanged();
  virtual void onPushNotificationReceived(PushNotificationReceived const &);
  void initListener(float);
  bool terminate();
};

static_assert(sizeof(AppPlatformListener) == 16);

struct ServerInstance : AppPlatformListener {
  enum struct InstanceState : int { INIT = 4 };
  DedicatedServer *server;                                                                // 16
  std::unique_ptr<Minecraft> minecraft;                                                   // 24
  std::unique_ptr<NetworkHandler> networkHandler;                                         // 32
  std::unique_ptr<LoopbackPacketSender> loopbackPacketSender;                             // 40
  std::unique_ptr<Timer> timer1, timer2;                                                  // 48, 56
  std::unique_ptr<Scheduler> scheduler;                                                   // 64
  std::unique_ptr<ChemistryOptions> chemistryOptions;                                     // 72
  std::unique_ptr<LevelStorage> levelStorage;                                             // 80
  RakNet::RakNetGUID netGUID;                                                             // 88
  std::atomic_bool flag0;                                                                 // 104
  std::atomic_int status0;                                                                // 108
  std::atomic_bool flag1;                                                                 // 112
  std::unique_ptr<VanillaServerGameplayEventListener> vanillaServerGameplayEventListener; // 120
  std::unique_ptr<VanillaNetworkEventListener> vanillaNetworkEventListener;               // 128
  std::atomic<InstanceState> state;                                                       // 136
  int padding;                                                                            // 140
  SPSCQueue<std::function<void()>> queue;                                                 // 144
  std::thread serverThread;                                                               // 224
  std::mutex serverMtx;                                                                   // 232
  std::condition_variable cv;                                                             // 272

  virtual ~ServerInstance();
  virtual void onLowMemory();
  void _assertThread();
  bool _running();
  void _threadSafeExecute(std::function<void()> fn);
  void _update();
  Minecraft *getMinecraft();
  NetworkHandler *getNetwork();
  Scheduler *getScheduler();
  LevelStorage *getStorage();
  bool isLeaveGameDone() const;
  bool isServerRunning() const;
  bool leaveGameSync();
  void queueForServerThread(std::function<void()> fn);
  bool resume();
  void setChemistryOptions(std::unique_ptr<ChemistryOptions>);
  void startLeaveGame();
  void startServerThread();
  void stop();
  bool suspend();
  std::string getLevelIdSync();
};

static_assert(offsetof(ServerInstance, server) == 16);
static_assert(offsetof(ServerInstance, levelStorage) == 80);
static_assert(offsetof(ServerInstance, serverThread) == 224);