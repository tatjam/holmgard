#include "SimpleSampleSource.h"
#include <util/Logger.h>

int32_t
SimpleSampleSource::mix_samples(float *target, uint32_t count, uint32_t cur_frame, bool loop, uint32_t sample_rate,
								uint32_t target_sample_rate)
{
	// May contain one or two samples per frame
	float* fsamples = (float*)get_samples();

	if(sample_rate == target_sample_rate)
	{
		// Simple, we must add the two arrays
		uint32_t frm_ptr = cur_frame;
		for(size_t i = 0; i < count; i++)
		{
			if(channel_count == 1)
			{
				target[i * 2 + 0] = fsamples[frm_ptr];
				target[i * 2 + 1] = fsamples[frm_ptr];
			}
			else
			{
				target[i * 2 + 0] = fsamples[frm_ptr * 2 + 0];
				target[i * 2 + 1] = fsamples[frm_ptr * 2 + 1];
			}
			frm_ptr++;
			if(frm_ptr >= frame_count / 2)
			{
				if(loop)
				{
					frm_ptr = 0;
				}
				else
				{
					// We dont mix nothing more, fill with zeros and early exit
					for(; i < count; i++)
					{
						target[i * 2 + 0] = 0.0f;
						target[i * 2 + 1] = 0.0f;
					}

					return -1;
				}
			}
		}

		return (int32_t)frm_ptr;
	}
	else
	{
		logger->fatal("Real-time resampling is not supported");
		return 0;
	}

}
