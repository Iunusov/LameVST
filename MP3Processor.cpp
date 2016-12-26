#include "MP3Processor.h"

#include <lame.h>

size_t MP3Processor::getMPEGBufferSize() { return 0; }

int MP3Processor::getBitRate() const {
	guard lock(mtx_);
	if (!bInit || !gfp) {
		return 0;
	}
	return lame_get_brate((lame_global_flags *)gfp);
}

MP3Processor::MP3Processor()
// For MPEG-I, 576 samples per frame per channel
	: wavBufferSize(576 * NUM_CHANNELS),
	tmpBufferSize(wavBufferSize * TMP_BUF_MULT),
	// Set MP3 buffer size, conservative estimate
	mp3BufferSize((size_t)(1.25f * (wavBufferSize / NUM_CHANNELS) + 7200)) {
	pMP3Buffer = new unsigned char[mp3BufferSize];
	pcm_l = new short[tmpBufferSize];
	pcm_r = new short[tmpBufferSize];
	tmp_buffer = new float[tmpBufferSize];
}

MP3Processor::~MP3Processor() {
	deInit();
	delete[] pMP3Buffer;
	delete[] tmp_buffer;
	delete[] pcm_l;
	delete[] pcm_r;
}

size_t MP3Processor::getWavBufferSize() const {
	guard lock(mtx_);
	return wavBufferSize;
}

bool MP3Processor::canReinit() const {
	guard lock(mtx_);
	return tmpBufferPos >= wavBufferSize;
}

bool MP3Processor::init(const int sampleRate, const int bitrate,
	const int mode) {
	guard lock(mtx_);
	deInit();
	gfp = lame_init();
	if (!gfp) {
		return false;
	}
	lame_set_free_format((lame_global_flags *)gfp, 1);
	// lame_set_quality((lame_global_flags *)gfp, 0);
	lame_set_in_samplerate((lame_global_flags *)gfp, sampleRate);
	lame_set_preset((lame_global_flags *)gfp, bitrate);
	lame_set_bWriteVbrTag((lame_global_flags *)gfp, 0);
	lame_set_VBR((lame_global_flags *)gfp, vbr_off);
	lame_set_num_channels((lame_global_flags *)gfp, NUM_CHANNELS);
	lame_set_mode((lame_global_flags *)gfp, STEREO);
	if (mode == 1) {
		lame_set_mode((lame_global_flags *)gfp, JOINT_STEREO);
	}

	lame_set_brate((lame_global_flags *)gfp, bitrate);
	lame_set_original((lame_global_flags *)gfp, 0);
	lame_set_error_protection((lame_global_flags *)gfp, 0);
	lame_set_extension((lame_global_flags *)gfp, 0);
	lame_set_strict_ISO((lame_global_flags *)gfp, 0);
	lame_set_out_samplerate((lame_global_flags *)gfp, sampleRate);
	if (lame_init_params((lame_global_flags *)gfp) != 0) {
		lame_close((lame_global_flags *)gfp);
		gfp = nullptr;
		return false;
	}
	dec = hip_decode_init();
	tmpBufferPos = 0;
	mp3Ready = false;
	bInit = true;
	return true;
}

bool MP3Processor::isInit() const {
	guard lock(mtx_);
	return bInit;
}

void MP3Processor::deInit() {
	guard lock(mtx_);
	bInit = false;
	if (gfp) {
		lame_close((lame_global_flags *)gfp);
		gfp = nullptr;
	}
	if (dec) {
		hip_decode_exit((hip_global_flags *)dec);
		dec = nullptr;
	}
}

bool MP3Processor::processWav(float *src) {
	if (!src) {
		return false;
	}
	guard lock(mtx_);
	if (bInit) {
		if (!gfp) {
			return false;
		}
		const int encodedLength = lame_encode_buffer_interleaved_ieee_float(
			(lame_global_flags *)gfp, src, (int)(wavBufferSize / NUM_CHANNELS),
			pMP3Buffer, mp3BufferSize);
		if (encodedLength > 0) {
			if (!dec) {
				return false;
			}
			const int decodedLength = hip_decode((hip_global_flags *)dec, pMP3Buffer,
				(size_t)encodedLength, pcm_l, pcm_r);

			if (decodedLength > 0 &&
				((tmpBufferPos + (size_t)(decodedLength)*NUM_CHANNELS) <
					tmpBufferSize)) {
				for (size_t i(0); i < (size_t)decodedLength; i++) {
					tmp_buffer[tmpBufferPos + NUM_CHANNELS * i] =
						pcm_l[i] / (float)std::numeric_limits<short>::max();
					tmp_buffer[tmpBufferPos + NUM_CHANNELS * i + 1] =
						pcm_r[i] / (float)std::numeric_limits<short>::max();
				}
				tmpBufferPos += (size_t)decodedLength * NUM_CHANNELS;
				if (!mp3Ready && tmpBufferPos >= tmpBufferSize / 2) {
					mp3Ready = true;
				}
			}
		}
	}
	memset(src, 0, sizeof(float) * wavBufferSize);
	if (mp3Ready && tmpBufferPos >= wavBufferSize) {
		memcpy(src, tmp_buffer, sizeof(float) * wavBufferSize);
		tmpBufferPos -= wavBufferSize;
		if (mp3Ready && tmpBufferPos < wavBufferSize) {
			mp3Ready = false;
		}
		memmove(&tmp_buffer[0], &tmp_buffer[wavBufferSize],
			(tmpBufferPos) * sizeof(float));
	}

	return true;
}
