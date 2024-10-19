#include "LameVST.h"
#include "version.h"

#define VST_PROGRAMMS_COUNT (1)
#define VST_PARAMS_COUNT (2)
#define VST_CHANNELS (2)
#define MIN_BITRATE (16)
#define MAX_BITRATE (320)
#define DEFAULT_BITRATE (128)
#define DEFAULT_MODE (0) // STEREO

AudioEffect *createEffectInstance(audioMasterCallback master) noexcept {
  return new LameVST(master);
}

LameVST::LameVST(audioMasterCallback master) noexcept
    : AudioEffectX(master, VST_PROGRAMMS_COUNT, VST_PARAMS_COUNT),
      bitrateValue(DEFAULT_BITRATE), lastBitrateValue(DEFAULT_BITRATE),
      channelValue(DEFAULT_MODE), lastChannelValue(DEFAULT_MODE) {
  setNumInputs(VST_CHANNELS);
  setNumOutputs(VST_CHANNELS);
  setUniqueID(CCONST('i', 'z', 'v', 'w'));
  canProcessReplacing(true);
  canDoubleReplacing(false);
  inputStereoBuffer.resize(mp3Processor.getWorkBufferSize());
  mp3Processor.init((int)getSampleRate(), bitrateValue, channelValue);
  // TODO: calculate initial delay
  setInitialDelay(17408);
}

LameVST::~LameVST() noexcept {}

void LameVST::processReplacing(float **inputs, float **outputs,
                               VstInt32 sampleFrames) noexcept {
  const size_t frames = (size_t)sampleFrames;
  const size_t stereoBufSize = frames * 2;

  const float *in1 = inputs[0];
  const float *in2 = inputs[1];
  float *out1 = outputs[0];
  float *out2 = outputs[1];

  for (size_t i(0); i < frames; ++i) {
    inputStereoBuffer[inputStereoPos++] = in1[i];
    inputStereoBuffer[inputStereoPos++] = in2[i];
    if (inputStereoPos >= mp3Processor.getWorkBufferSize()) {
      inputStereoPos = 0;
      if ((bitrateValue != lastBitrateValue ||
           channelValue != lastChannelValue) &&
          readyToOutput) {
        readyToOutput = false;
        mp3Processor.init((int)getSampleRate(), bitrateValue, channelValue);
        lastBitrateValue = bitrateValue;
        lastChannelValue = channelValue;
      }
      mp3Processor.addNextInput(inputStereoBuffer.data());
      if (mp3Processor.buffered(0.5)) {
        readyToOutput = true;
      }
    }
  }

  for (size_t i(0); i < frames; i++) {
    out1[i] = 0;
    out2[i] = 0;
  }

  if (readyToOutput && mp3Processor.hasReadyOutput(stereoBufSize)) {
    if (outputStereoBuffer.capacity() < stereoBufSize) {
      outputStereoBuffer.resize(stereoBufSize);
    }
    mp3Processor.getNextOutput(outputStereoBuffer.data(), stereoBufSize);
    size_t pos = 0;
    for (size_t i(0); i < frames; i++) {
      out1[i] = outputStereoBuffer[pos++];
      out2[i] = outputStereoBuffer[pos++];
    }

    if (!mp3Processor.hasReadyOutput(stereoBufSize)) {
      readyToOutput = false;
    }
  }
}

VstInt32 LameVST::getVendorVersion() noexcept { return LAMEVST_VERSION_INT; }

VstPlugCategory LameVST::getPlugCategory() noexcept { return kPlugCategEffect; }

bool LameVST::getEffectName(char *name) noexcept {
  vst_strncpy(name, "LameVST", kVstMaxEffectNameLen);
  return true;
}

bool LameVST::getVendorString(char *text) noexcept {
  vst_strncpy(text, "github.com/Iunusov/LameVST", kVstMaxVendorStrLen);
  return true;
}

bool LameVST::getProductString(char *text) noexcept {
  vst_strncpy(text, "LameVST", kVstMaxProductStrLen);
  return true;
}

void LameVST::setParameter(VstInt32 index, float value) noexcept {
  switch (index) {
  case 0: {
    const auto new_value{(int)(value * MAX_BITRATE)};
    bitrateValue = (new_value < MIN_BITRATE) ? MIN_BITRATE : new_value;
    break;
  }
  case 1: {
    channelValue = (value > 0.5f) ? 1 : 0;
    break;
  }
  default:
    return;
  }
}

void LameVST::getParameterLabel(VstInt32 index, char *label) noexcept {
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

void LameVST::getParameterName(VstInt32 index, char *label) noexcept {
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

void LameVST::getParameterDisplay(VstInt32 index, char *text) noexcept {
  switch (index) {
  case 0: {
    int2string(bitrateValue, text, kVstMaxParamStrLen);
    break;
  }
  case 1: {
    if (1 == channelValue) {
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

float LameVST::getParameter(VstInt32 index) noexcept {
  switch (index) {
  case 0:
    return (float)bitrateValue / (float)MAX_BITRATE;
  case 1:
    return (float)channelValue;
  default:
    return 0;
  }
}
