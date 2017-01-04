#pragma once

#include <audioeffectx.h>

#include "MP3Processor.h"

#include <mutex>
#include <vector>

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
  std::vector<float> pWAVBufferSrc;
  int lameBitrate;
  int lameBitrateLast;
  int lameChannelMode;
  int lameChannelModeLast;
  size_t srcWavPos = 0;
  size_t dstWavPos = 0;
  bool bufferReady = false;
  
  typedef std::lock_guard<std::recursive_mutex> guard;
  mutable std::recursive_mutex mtx_;
};
