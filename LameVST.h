#pragma once

#include "audioeffectx.h"

#include "MP3Processor.h"

#include <atomic>
#include <vector>

class LameVST : public AudioEffectX {
public:
  explicit LameVST(audioMasterCallback) noexcept;
  ~LameVST() noexcept;
  virtual void processReplacing(float **, float **, VstInt32) noexcept override;
  virtual bool getEffectName(char *) noexcept override;
  virtual bool getVendorString(char *) noexcept override;
  virtual bool getProductString(char *) noexcept override;
  void setParameter(VstInt32, float) noexcept override;
  virtual VstPlugCategory getPlugCategory() noexcept override;
  virtual VstInt32 getVendorVersion() noexcept override;
  virtual void getParameterLabel(VstInt32, char *) noexcept override;
  virtual void getParameterName(VstInt32, char *) noexcept override;
  virtual void getParameterDisplay(VstInt32, char *) noexcept override;
  virtual float getParameter(VstInt32) noexcept override;

private:
  std::atomic<int> bitrateValue;
  int lastBitrateValue;
  std::atomic<int> channelValue;
  int lastChannelValue;

  MP3Processor mp3Processor;
  std::vector<float> inputStereoBuffer;
  std::vector<float> outputStereoBuffer;
  size_t inputStereoPos = 0;
  bool readyToOutput = false;
};
