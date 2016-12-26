#pragma once

#include <cstddef>
#include <cstring>
#include <mutex>

class MP3Processor {
 private:
  const size_t NUM_CHANNELS = 2;
  const size_t TMP_BUF_MULT = 50;
  void *gfp = nullptr;
  void *dec = nullptr;
  const size_t wavBufferSize;
  const size_t tmpBufferSize;
  const size_t mp3BufferSize;
  unsigned char *pMP3Buffer = nullptr;
  float *tmp_buffer = nullptr;
  short *pcm_l = nullptr;
  short *pcm_r = nullptr;
  bool mp3Ready = false;
  size_t tmpBufferPos = 0;
  bool bInit = false;
  size_t getMPEGBufferSize();
  typedef std::lock_guard<std::recursive_mutex> guard;
  mutable std::recursive_mutex mtx_;

 public:
  MP3Processor();
  ~MP3Processor();
  size_t getWavBufferSize() const;
  int getBitRate() const;
  bool canReinit() const;
  bool init(const int sampleRate, const int bitrate, const int mode);
  bool isInit() const;
  void deInit();
  bool processWav(float *src);
};
