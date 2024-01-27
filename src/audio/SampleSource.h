#pragma once
#include <cstddef>
#include <cstdint>

// Generic class for everything that provides audio to an audio source
// Designed so clips, streams and generated audio can be used simultaneously
// Held by an AudioSource and lifetime managed by it, as only a single
// AudioSource may use each SampleSource
class SampleSource
{
public:

	// You must set the values in the target, you dont do the mixing
	// Return a negative number if the audio is finished (always false if loop is true!)
	// Otherwise, return current frame position
	// If you are a generative stream, feel free to ignore cur_sample
	// If you cannot generate enough samples, make sure to zero-fill the array to avoid artifacts
	virtual int32_t mix_samples(float* target, uint32_t count, uint32_t cur_frame, bool loop, uint32_t sample_rate,
				uint32_t target_sample_rate) = 0;

	virtual ~SampleSource()=default;

};