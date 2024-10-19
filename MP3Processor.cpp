#include "MP3Processor.h"

#include <cstring>

#include <lame.h>

#include <limits> // std::numeric_limits

#define NUM_CHANNELS (2)
#define BUFFERS_COUNT (50)
#define WAV_BUF_SIZE (576 * NUM_CHANNELS)
// Set MP3 buffer size, conservative estimate
#define MP3_BUF_SIZE ((size_t)(1.25f * (WAV_BUF_SIZE / NUM_CHANNELS) + 7200))

MP3Processor::MP3Processor() noexcept
    : outputPCMBuffer(WAV_BUF_SIZE * BUFFERS_COUNT) {
  mp3Buffer.resize(MP3_BUF_SIZE);
  decodedLeftChannel.resize(MP3_BUF_SIZE * 10);
  decodedRightChannel.resize(MP3_BUF_SIZE * 10);
}

MP3Processor::~MP3Processor() noexcept { deInit(); }

size_t MP3Processor::getWorkBufferSize() const noexcept { return WAV_BUF_SIZE; }

bool MP3Processor::init(const int sampleRate, const int bitrate,
                        const int mode) noexcept {
  deInit();
  lame_enc_handler = lame_init();
  if (!lame_enc_handler) {
    return false;
  }
  lame_set_free_format((lame_global_flags *)lame_enc_handler, 1);
  // lame_set_quality((lame_global_flags *)gfp, 0);
  lame_set_in_samplerate((lame_global_flags *)lame_enc_handler, sampleRate);
  lame_set_preset((lame_global_flags *)lame_enc_handler, bitrate);
  lame_set_bWriteVbrTag((lame_global_flags *)lame_enc_handler, 0);
  lame_set_VBR((lame_global_flags *)lame_enc_handler, vbr_off);
  lame_set_num_channels((lame_global_flags *)lame_enc_handler, NUM_CHANNELS);
  lame_set_mode((lame_global_flags *)lame_enc_handler, STEREO);
  if (mode == 1) {
    lame_set_mode((lame_global_flags *)lame_enc_handler, JOINT_STEREO);
  }

  lame_set_brate((lame_global_flags *)lame_enc_handler, bitrate);
  lame_set_original((lame_global_flags *)lame_enc_handler, 0);
  lame_set_error_protection((lame_global_flags *)lame_enc_handler, 0);
  lame_set_extension((lame_global_flags *)lame_enc_handler, 0);
  lame_set_strict_ISO((lame_global_flags *)lame_enc_handler, 0);
  lame_set_out_samplerate((lame_global_flags *)lame_enc_handler, sampleRate);
  if (lame_init_params((lame_global_flags *)lame_enc_handler) != 0) {
    lame_close((lame_global_flags *)lame_enc_handler);
    lame_enc_handler = nullptr;
    return false;
  }
  lame_dec_handler = hip_decode_init();
  outputPCMBuffer.reset();
  bInitialized = true;
  return true;
}

void MP3Processor::deInit() noexcept {
  bInitialized = false;
  if (lame_enc_handler) {
    lame_close((lame_global_flags *)lame_enc_handler);
    lame_enc_handler = nullptr;
  }
  if (lame_dec_handler) {
    hip_decode_exit((hip_global_flags *)lame_dec_handler);
    lame_dec_handler = nullptr;
  }
}

void MP3Processor::addNextInput(float *src) noexcept {
  if (bInitialized) {
    const int encodedLength = lame_encode_buffer_interleaved_ieee_float(
        (lame_global_flags *)lame_enc_handler, src,
        (int)(WAV_BUF_SIZE / NUM_CHANNELS), mp3Buffer.data(), MP3_BUF_SIZE);
    if (encodedLength > 0) {
      const int decodedLength =
          hip_decode((hip_global_flags *)lame_dec_handler, mp3Buffer.data(),
                     (size_t)encodedLength, decodedLeftChannel.data(),
                     decodedRightChannel.data());

      if (decodedLength > 0) {
        const size_t decodedSize = (size_t)decodedLength * NUM_CHANNELS;
        if (readBuf.capacity() < decodedSize) {
          readBuf.resize(decodedSize);
        }
        for (size_t i(0); i < (size_t)decodedLength; i++) {
          readBuf[NUM_CHANNELS * i] =
              decodedLeftChannel[i] / (float)std::numeric_limits<short>::max();
          readBuf[NUM_CHANNELS * i + 1] =
              decodedRightChannel[i] / (float)std::numeric_limits<short>::max();
        }
        outputPCMBuffer.push(readBuf.data(), decodedSize);
      }
    }
  }
}

bool MP3Processor::buffered(const double amount) const noexcept {
  return (double)outputPCMBuffer.size() >=
         (double)outputPCMBuffer.capacity() * amount;
}

bool MP3Processor::hasReadyOutput(const size_t size) const noexcept {
  return outputPCMBuffer.size() >= size;
}

size_t MP3Processor::getNextOutput(float *dst, const size_t maxsize) noexcept {
  return outputPCMBuffer.pull(dst, maxsize);
}
