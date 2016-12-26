#include "LameVST.h"
#include <thread>
#include "version.h"

#define MIN_BITRATE (16)
#define MAX_BITRATE (640)
#define DEFAULT_BITRATE (128)
#define DEFAULT_MODE (0)  // STEREO

AudioEffect* createEffectInstance(audioMasterCallback audioMaster) {
  return new LameVST(audioMaster);
}

LameVST::LameVST(audioMasterCallback audioMaster)
    : AudioEffectX(audioMaster, 1, 2),
      lameBitrate(DEFAULT_BITRATE),
      lameBitrateLast(DEFAULT_BITRATE),
      lameChannelMode(DEFAULT_MODE),
      lameChannelModeLast(DEFAULT_MODE) {
  setNumInputs(2);
  setNumOutputs(2);
  setUniqueID(CCONST('i', 'z', 'v', 'w'));
  canProcessReplacing();
  canDoubleReplacing(false);
  mp3Processor.init((const int)getSampleRate(), lameBitrate, lameChannelMode);
  pWAVBufferSrc = new float[mp3Processor.getWavBufferSize()];
  noTail(false);
  //TODO: calculate initial delay
  setInitialDelay(1152);
}

LameVST::~LameVST() {}

void LameVST::processReplacing(float** inputs, float** outputs,
                               VstInt32 sampleFrames) {
  if (!inputs || !outputs || sampleFrames <= 0) {
    return;
  }

  const size_t frames = (size_t)sampleFrames;

  float* in1 = inputs[0];
  float* in2 = inputs[1];
  float* out1 = outputs[0];
  float* out2 = outputs[1];

  for (size_t i(0); i < frames; ++i) {
    out1[i] = 0;
    out2[i] = 0;
  }

  for (size_t i(0); i < frames; ++i) {
    pWAVBufferSrc[srcWavPos++] = in1[i];
    pWAVBufferSrc[srcWavPos++] = in2[i];
    if (srcWavPos >= mp3Processor.getWavBufferSize()) {
      guard lock(mtx_);
      srcWavPos = 0;
      dstWavPos = 0;
      if ((lameBitrate != lameBitrateLast ||
           lameChannelMode != lameChannelModeLast) &&
          mp3Processor.canReinit()) {
        mp3Processor.init((const int)getSampleRate(), lameBitrate,
                          lameChannelMode);
        lameBitrateLast = lameBitrate;
        lameChannelModeLast = lameChannelMode;
      }
      mp3Processor.processWav(pWAVBufferSrc);
      bufferReady = true;
    }

    if (bufferReady) {
      out1[i] = pWAVBufferSrc[dstWavPos++];
      out2[i] = pWAVBufferSrc[dstWavPos++];
      if (dstWavPos >= mp3Processor.getWavBufferSize() ||
          !mp3Processor.isInit()) {
        bufferReady = false;
      }
    }
  }
}

VstInt32 LameVST::getVendorVersion() { return LAMEVST_VERSION_INT; }

VstPlugCategory LameVST::getPlugCategory() { return kPlugCategEffect; }

bool LameVST::getEffectName(char* name) {
  vst_strncpy(name, "LameVST", kVstMaxEffectNameLen);
  return true;
}

bool LameVST::getVendorString(char* text) {
  vst_strncpy(text, "github.com/R-Tur/LameVST", kVstMaxVendorStrLen);
  return true;
}

bool LameVST::getProductString(char* text) {
  vst_strncpy(text, "LameVST", kVstMaxProductStrLen);
  return true;
}

void LameVST::setParameter(VstInt32 index, float value) {
  guard lock(mtx_);
  switch (index) {
    case 0: {
      lameBitrate = (int)(value * MAX_BITRATE);
      if (lameBitrate < MIN_BITRATE) {
        lameBitrate = MIN_BITRATE;
      }
      break;
    }
    case 1: {
      lameChannelMode = 0;
      if (value > 0.5) {
        lameChannelMode = 1;
      }
    }
    default:
      return;
  }
}

void LameVST::getParameterLabel(VstInt32 index, char* label) {
  switch (index) {
    case 0: {
      vst_strncpy(label, "kbps", kVstMaxParamStrLen);
      break;
    }
    case 1: {
      vst_strncpy(label, "", kVstMaxParamStrLen);
      break;
    }
    default: {
      vst_strncpy(label, "", kVstMaxParamStrLen);
      return;
    }
  }
}

void LameVST::getParameterName(VstInt32 index, char* label) {
  switch (index) {
    case 0: {
      vst_strncpy(label, "Bit rate", kVstMaxParamStrLen);
      break;
    }
    case 1: {
      vst_strncpy(label, "Mode", kVstMaxParamStrLen);
      break;
    }
    default: {
      vst_strncpy(label, "", kVstMaxParamStrLen);
      return;
    }
  }
}

void LameVST::getParameterDisplay(VstInt32 index, char* text) {
  switch (index) {
    case 0: {
      int2string(mp3Processor.getBitRate(), text, kVstMaxParamStrLen);
      break;
    }
    case 1: {
      if (1 == lameChannelMode) {
        vst_strncpy(text, "JOINT", kVstMaxParamStrLen);
      } else {
        vst_strncpy(text, "STEREO", kVstMaxParamStrLen);
      }
      break;
    }
    default: {
      vst_strncpy(text, "", kVstMaxParamStrLen);
      return;
    }
  }
}

float LameVST::getParameter(VstInt32 index) {
  guard lock(mtx_);
  switch (index) {
    case 0:
      return (float)mp3Processor.getBitRate() / (float)MAX_BITRATE;
    case 1:
      return (float)lameChannelMode;
    default:
      return 0;
  }
}
