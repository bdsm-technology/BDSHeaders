#pragma once

#include <functional>
#include <vector>
#include <memory>

struct NetworkHandler;
struct NetEventCallback;
struct Packet;
struct NetworkIdentifier;
struct Player;

struct alignas(8) PacketSender {
  unsigned char flag;

  PacketSender(unsigned char flag);
  virtual ~PacketSender();
  virtual void send(Packet &)                                                          = 0;
  virtual void sendToServer(Packet &)                                                  = 0;
  virtual void sendToClient(NetworkIdentifier const &, Packet const &, unsigned char)  = 0;
  virtual void sendBroadcast(Packet const &)                                           = 0;
  virtual void sendBroadcast(NetworkIdentifier const &, unsigned char, Packet const &) = 0;
  virtual void flush(NetworkIdentifier const &, std::function<void()>)                 = 0;
};

static_assert(sizeof(PacketSender) == 16);

struct LoopbackPacketSender : PacketSender {
  NetworkHandler *handler;
  std::vector<NetEventCallback *> cbs;
  void *unk;

  virtual ~LoopbackPacketSender();
  virtual void send(Packet &);
  virtual void sendToServer(Packet &);
  virtual void sendToClient(NetworkIdentifier const &, Packet const &, unsigned char);
  virtual void sendBroadcast(Packet const &);
  virtual void sendBroadcast(NetworkIdentifier const &, unsigned char, Packet const &);
  virtual void flush(NetworkIdentifier const &, std::function<void()>);

  LoopbackPacketSender(unsigned char, NetworkHandler &handler);
  void addLoopbackCallback(NetEventCallback &);
  void removeLoopbackCallback(NetEventCallback &);
  void setUserList(std::vector<std::unique_ptr<Player>> const *);
};

static_assert(offsetof(LoopbackPacketSender, unk) == 48);