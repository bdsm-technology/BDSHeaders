#pragma once

#include "types.h"

struct ActivationUri;
struct IMinecraftApp;

struct UriListener {
  UriListener();
  virtual ~UriListener();                        // 0, 8
  virtual void onUri(ActivationUri const &) = 0; // 16
};

namespace Automation {
struct Response;
struct AutomationClient : UriListener {
  AutomationClient(IMinecraftApp &);

  void connect(std::string const &);
  void disconnect();
  void insertMessage(std::string const &);
  bool isConnecting();
  bool isReady();
  bool isReadyForGameCommands();
  bool isSubscribedtoEvent(std::string const &);
  void send(Response const &);
  void setLocalConnectionHandler(std::function<void(std::string const &)>);
  void setMaxQueueSize(std::size_t);
  void setRequireEncryption(bool);
  void setServerRetryTime(float);
  void tick();

  virtual ~AutomationClient() override;               // 0, 8
  virtual void onUri(ActivationUri const &) override; // 16
};
} // namespace Automation