#pragma once
#include "Asset.h"
#include <util/SerializeUtil.h>
#include <audio/SimpleSampleSource.h>

// Audio clips. We support only mono and stereo sounds, higher channels are ignored, with a warning.
// This loads the full audio file into memory and does no streaming
// Stereo sounds may be played in 3D sources BUT only their first channel (left) will be mixed.
// All audio types are eventually converted to f32 samples
class AudioClip : public Asset, public SimpleSampleSource
{
private:

public:

	AudioClip(ASSET_INFO, void* samples, size_t frame_count, size_t channel_count);
	~AudioClip() override;
};


AudioClip* load_audio_clip(ASSET_INFO, const cpptoml::table& cfg);
