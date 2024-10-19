#pragma once

#include "RingBuffer.h"

#include <atomic>
#include <vector>

class MP3Processor {
public:
  MP3Processor() noexcept;
  ~MP3Processor() noexcept;
  size_t getWorkBufferSize() const noexcept;
  bool init(const int, const int, const int) noexcept;
  void deInit() noexcept;
  void addNextInput(float *) noexcept;
  bool buffered(const double) const noexcept;
  bool hasReadyOutput(const size_t) const noexcept;
  size_t getNextOutput(float *, const size_t) noexcept;

private:
  std::atomic_bool bInitialized{};
  void *lame_enc_handler = nullptr;
  void *lame_dec_handler = nullptr;
  std::vector<unsigned char> mp3Buffer;
  std::vector<short> decodedLeftChannel;
  std::vector<short> decodedRightChannel;
  RingBuffer<float> outputPCMBuffer;
  std::vector<float> readBuf;
};
