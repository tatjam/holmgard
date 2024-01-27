#pragma once
#include <audio/SampleSource.h>

// Allows playing a set of samples, given in the float audio format
// NOTE: samples are externally managed!
class SimpleSampleSource : public SampleSource
{
public:
	size_t channel_count;
	// WARNING: Externally managed!
	void* samples;
	size_t frame_count;

	~SimpleSampleSource() override = default;

	int32_t mix_samples(float* target, uint32_t count, uint32_t cur_frame, bool loop, uint32_t sample_rate,
						uint32_t target_sample_rate) override;

	size_t get_channel_count() const { return channel_count; }
	size_t get_frame_count() const { return frame_count; }
	void* get_samples() const { return samples; }



};
