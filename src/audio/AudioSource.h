#pragma once
#include "SampleSource.h"
#include "assets/AssetManager.h"
#include "assets/AudioClip.h"

class AudioEngine;
// The audio source is the multi-purpose class used for all audio sources
// It allows both 2D and 3D audio
// Their lifetime is handled by AudioEngine, std::weak_ptr is recommended
class AudioSource
{
private:

	// Asset references if using an AudioClip, we actually use the sample_source pointer
	AssetHandle<AudioClip> audio_clip_src;
	std::unique_ptr<SampleSource> generic_src;

	SampleSource* sample_source;

	float gain;
	// Pitch basically means relative play speed.
	// Values other than 1 involve resampling as all audio is at the same frequency by default
	float pitch;

	AudioEngine* engine;
	uint32_t in_channel;
	bool source_3d;
	bool playing;
	bool loops;

	uint32_t cur_sample;

	glm::dvec3 pos, vel;

	bool destroy_when_finished = false;
	bool audio_played;

public:
	bool marked_for_deletion;

	void set_destroy_when_finished(){ destroy_when_finished = true; }

	void set_playing(bool value);
	// Returns true if the audio is actually generating sound
	bool is_playing() const { return audio_played; }
	// Returns true if the audio is playing, or waiting to play (if for
	// example no source is set, but set_playing(true) has been called)
	bool is_playing_or_queued() const { return playing; }

	// Return true if anything was played
	bool mix_samples(void* target, size_t count);

	bool is_3d_source() const { return source_3d; }
	void set_3d_source(bool value);

	// Only on 3d sources
	glm::dvec3 get_position() { return pos; }
	void set_position(glm::dvec3 pos);

	float get_gain() const { return gain; }
	void set_gain(float val);

	float get_pitch() const { return pitch; }
	void set_pitch(float val);

	bool is_looping() const { return loops; }
	void set_looping(bool val);

	// We duplicate the asset
	void set_source_clip(const AssetHandle<AudioClip>& ast);
	// Moves the sample source
	void set_source_generic(std::unique_ptr<SampleSource>& src);

	// Be aware, all other pointers to this will be invalidated
	void destroy();
	void destroy_from_thread();
	AudioSource(AudioEngine* eng, uint32_t channel);
};

