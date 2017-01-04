#pragma once

#include "RingBuffer.h"

#include <mutex>
#include <vector>

class MP3Processor {
public:
  MP3Processor();
  ~MP3Processor();
  size_t getWavBufferSize() const;
  int getBitRate() const;
  bool init(const int sampleRate, const int bitrate, const int mode);
  bool isInit() const;
  bool canReinit() const;
  void deInit();
  bool processWav(float *src);

private:
  typedef std::lock_guard<std::recursive_mutex> guard;
  mutable std::recursive_mutex mtx_;
  bool bInitialized = false;
  bool mp3Ready = false;
  void *lame_enc_handler = nullptr;
  void *lame_dec_handler = nullptr;
  std::vector<unsigned char> mp3Buffer;
  std::vector<short> decodedLeftChannel;
  std::vector<short> decodedRightChannel;
  RingBuffer<float> outputPCMBuffer;
  std::vector<float> readBuf;
};
