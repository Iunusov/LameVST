#pragma once

#include "RingBuffer.h"

#include <mutex>
#include <vector>

class MP3Processor {
public:
  MP3Processor();
  ~MP3Processor();
  size_t getWorkBufferSize() const;
  bool init(const int sampleRate, const int bitrate, const int mode);
  void deInit();
  void addNextInput(float *src);
  bool buffered(const double amount) const;
  bool hasReadyOutput(const size_t size) const;
  size_t getNextOutput(float *dst, const size_t maxsize);

private:
  typedef std::lock_guard<std::recursive_mutex> guard;
  mutable std::recursive_mutex mtx_;
  bool bInitialized = false;
  void *lame_enc_handler = nullptr;
  void *lame_dec_handler = nullptr;
  std::vector<unsigned char> mp3Buffer;
  std::vector<short> decodedLeftChannel;
  std::vector<short> decodedRightChannel;
  RingBuffer<float> outputPCMBuffer;
  std::vector<float> readBuf;
};
