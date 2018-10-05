#pragma once

#include "SmallSet.h"
#include "mpmc.h"
#include <chrono>
#include <memory>
#include <thread>

struct WorkerPool;
struct BackgroundTask;
struct TaskGroup;
struct BackgroundWorker;

struct Scheduler {
  char filler[48];                                              // 0
  std::unique_ptr<WorkerPool> workerPool;                       // 48
  std::unique_ptr<WorkerPool> coroutinePool;                    // 56
  std::chrono::time_point<std::chrono::system_clock> timePoint; // 64
  mpmc::Sender<BackgroundTask> sender;                          // 72
  mpmc::Receiver<BackgroundTask> receiver;                      // 88
  SmallSet<TaskGroup *> tasks;                                  // 104
  std::thread::id tid;                                          // 128
  void *p136;                                                   // 136

  Scheduler(std::string, unsigned);
  void _assignToThread(std::thread::id);
  void _fetchAllCallbacks();
  unsigned _getNumberOfStarvedFrames(); // 4 <- 4
  bool _ignoreTimeCap();                // <- 136
  bool _processNextCoroutine();
  void _removeFlushedCallbacks();
  void _removeSchedulerAssignment();
  void _runCoroutines(double);   // 8 <> 40 double -> 32
  void _updateTimeCap(unsigned); // double <- 24
  void changeThread(std::thread::id);
  mpmc::Sender<BackgroundTask> getCallbackSender();
  WorkerPool *getCoroutinePool();
  BackgroundWorker *getCoroutineWorker();
  unsigned getEffectiveTargetFPS(); // int <- 16
  std::thread::id getThread() const;
  Scheduler *getThreadLocalScheduler();
  float getTotalFrameTime(); // int <- 16
  bool isStarved() const;    // int <- 12 int <- 16
  void processCoroutines(std::chrono::duration<long, std::ratio<1l, 1000000000l>> dur);
  void registerTaskGroup(TaskGroup &);
  void setTargetFPS(unsigned);
  void unregisterTaskGroup(TaskGroup &);
  ~Scheduler();
};

static_assert(offsetof(Scheduler, p136) == 136);