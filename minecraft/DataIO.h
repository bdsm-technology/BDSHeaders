#pragma once

#include <string>

struct IDataInput {
  IDataInput();
  virtual ~IDataInput();
  virtual std::string readString()            = 0;
  virtual std::string readLongString()        = 0;
  virtual float readFloat()                   = 0;
  virtual double readDouble()                 = 0;
  virtual char readByte()                     = 0;
  virtual short readShort()                   = 0;
  virtual int readInt()                       = 0;
  virtual long long readLongLong()            = 0;
  virtual bool readBytes(void *, std::size_t) = 0;
  virtual std::size_t numBytesLeft()          = 0;
};

struct IDataOutput {
  IDataOutput();
  virtual ~IDataOutput();
  virtual void writeString(std::string const &)      = 0;
  virtual void writeLongString(std::string const &)  = 0;
  virtual void writeFloat(float)                     = 0;
  virtual void writeDouble(double)                   = 0;
  virtual void writeByte(char)                       = 0;
  virtual void wruteShort(short)                     = 0;
  virtual void writeInt(int)                         = 0;
  virtual void writeLongLong(long long)              = 0;
  virtual void writeBytes(void const *, std::size_t) = 0;
};

struct BytesDataInput : IDataInput {
  BytesDataInput();
  virtual ~BytesDataInput();
  virtual std::string readString();
  virtual std::string readLongString();
  virtual float readFloat();
  virtual double readDouble();
  virtual char readByte();
  virtual short readShort();
  virtual int readInt();
  virtual long long readLongLong();
  virtual bool readBytes(void *, std::size_t) = 0;
  virtual std::size_t numBytesLeft()          = 0;
};

struct BytesDataOutput : IDataOutput {
  BytesDataOutput();
  virtual ~BytesDataOutput();
  virtual void writeString(std::string const &);
  virtual void writeLongString(std::string const &);
  virtual void writeFloat(float);
  virtual void writeDouble(double);
  virtual void writeByte(char);
  virtual void wruteShort(short);
  virtual void writeInt(int);
  virtual void writeLongLong(long long);
  virtual void writeBytes(void const *, std::size_t) = 0;
};

struct ReadOnlyBinaryStream;

struct BinaryDataInput : BytesDataInput {
  ReadOnlyBinaryStream *ptr;
  BinaryDataInput(ReadOnlyBinaryStream &);
  virtual ~BinaryDataInput();
  virtual std::string readString();
  virtual std::string readLongString();
  virtual float readFloat();
  virtual double readDouble();
  virtual char readByte();
  virtual short readShort();
  virtual int readInt();
  virtual long long readLongLong();
  virtual bool readBytes(void *, std::size_t);
  virtual std::size_t numBytesLeft();
};

struct BinaryStream;

struct BinaryDataOutput : BytesDataOutput {
  BinaryStream *ptr;
  BinaryDataOutput(BinaryStream &);
  virtual ~BinaryDataOutput();
  virtual void writeString(std::string const &);
  virtual void writeLongString(std::string const &);
  virtual void writeFloat(float);
  virtual void writeDouble(double);
  virtual void writeByte(char);
  virtual void wruteShort(short);
  virtual void writeInt(int);
  virtual void writeLongLong(long long);
  virtual void writeBytes(void const *, std::size_t);
};

struct StringByteInput : BytesDataInput {
  int start, end;      // 8, 12
  std::string *buffer; // 16
  StringByteInput(std::string const &, int, int);
  virtual ~StringByteInput();
  virtual bool readBytes(void *, std::size_t);
  virtual std::size_t numBytesLeft();
};

struct StringByteOutput : BytesDataOutput {
  std::string *str;
  StringByteOutput(std::string &);
  virtual ~StringByteOutput();
  virtual void writeBytes(void const *, std::size_t);
};

struct BigEndianStringByteInput : StringByteInput {
  BigEndianStringByteInput(std::string const &, int, int);
  virtual ~BigEndianStringByteInput();
  virtual float readFloat();
  virtual double readDouble();
  virtual char readByte();
  virtual short readShort();
  virtual int readInt();
  virtual long long readLongLong();
  virtual bool readBytes(void *, std::size_t);
  bool readBigEndinanBytes(void *, std::size_t);
};

struct BigEndianStringByteOutput : StringByteOutput {
  BigEndianStringByteOutput(std::string &);
  virtual ~BigEndianStringByteOutput();
  virtual void writeFloat(float);
  virtual void writeDouble(double);
  virtual void writeByte(char);
  virtual void wruteShort(short);
  virtual void writeInt(int);
  virtual void writeLongLong(long long);
  virtual void writeReverseBytes(void const *, std::size_t);
};

namespace RakNet {
struct BitStream;
}

struct RakDataInput : BytesDataInput {
  RakNet::BitStream *ptr;
  RakDataInput(RakNet::BitStream &);
  virtual ~RakDataInput();
  virtual bool readBytes(void *, std::size_t);
  virtual std::size_t numBytesLeft();
};

struct RakDataOutput : BytesDataOutput {
  RakNet::BitStream *ptr;
  RakDataOutput(RakNet::BitStream &);
  virtual ~RakDataOutput();
  virtual void writeBytes(void const *, std::size_t);
};