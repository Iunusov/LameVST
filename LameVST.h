#pragma once

#include <audioeffectx.h>

#include "MP3Processor.h"

#include <vector>
#include <atomic>

class LameVST : public AudioEffectX {
public:
  explicit LameVST(audioMasterCallback audioMaster);
  ~LameVST();
  virtual void processReplacing(float **inputs, float **outputs,
                                VstInt32 sampleFrames) override;
  virtual bool getEffectName(char *name) override;
  virtual bool getVendorString(char *text) override;
  virtual bool getProductString(char *text) override;
  void setParameter(VstInt32 index, float value) override;
  virtual VstPlugCategory getPlugCategory() override;
  virtual VstInt32 getVendorVersion() override;
  virtual void getParameterLabel(VstInt32 index, char *label) override;
  virtual void getParameterName(VstInt32 index, char *label) override;
  virtual void getParameterDisplay(VstInt32 index, char *text) override;
  virtual float getParameter(VstInt32 index) override;

private:
  MP3Processor mp3Processor;
  std::vector<float> inputStereoBuffer;
  std::vector<float> outputStereoBuffer;
  std::atomic<int> bitrateValue;
  std::atomic<int> channelValue;
  int lastBitrateValue;
  int lastChannelValue;
  size_t inputStereoPos = 0;
  bool readyToOutput = false;
};
