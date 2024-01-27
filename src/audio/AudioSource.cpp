#include "AudioSource.h"
#include "AudioEngine.h"

bool AudioSource::mix_samples(void* target, size_t count)
{
	if(!sample_source || !playing || pitch <= 0.0f)
	{
		// this effectively waits for an audio source to be added
		return false;
	}

	audio_played = true;

	size_t sample_rate = engine->get_sample_rate() * pitch;

	int32_t jump = sample_source->mix_samples((float*)target, count, cur_sample, loops, sample_rate, engine->get_sample_rate());
	if(jump >= 0)
	{
		cur_sample = jump;
	}
	else
	{
		// Audio playback is finished
		if(loops)
		{
			cur_sample = 0;
		}
		else
		{
			playing = false;
			cur_sample = 0;
			audio_played = false;

			if(destroy_when_finished)
			{
				destroy_from_thread();
			}
		}
	}

	return true;
}

void AudioSource::destroy_from_thread()
{
	marked_for_deletion = true;
}

void AudioSource::destroy()
{
	engine->mtx.lock();
	destroy_from_thread();
	engine->mtx.unlock();
}

AudioSource::AudioSource(AudioEngine *eng, uint32_t channel)
{
	this->engine = eng;
	this->in_channel = channel;
	// By default we use a 2D audio source
	source_3d = false;
	playing = false;
	gain = 1.0f;
	pitch = 1.0f;
	sample_source = nullptr;
	audio_clip_src = AssetHandle<AudioClip>();
	generic_src = nullptr;
	loops = false;
	marked_for_deletion = false;
	audio_played = false;

	cur_sample = 0;
	destroy_when_finished = false;
}

void AudioSource::set_playing(bool value)
{
	engine->mtx.lock();
	playing = value;
	if(!value) audio_played = false;
	engine->mtx.unlock();
}

void AudioSource::set_3d_source(bool value)
{
	engine->mtx.lock();
	source_3d = value;
	engine->mtx.unlock();
}

void AudioSource::set_position(glm::dvec3 pos)
{
	engine->mtx.lock();
	this->pos = pos;
	engine->mtx.unlock();
}

void AudioSource::set_gain(float val)
{
	engine->mtx.lock();
	gain = val;
	engine->mtx.unlock();
}

void AudioSource::set_pitch(float val)
{
	engine->mtx.lock();
	pitch = val;
	engine->mtx.unlock();
}

void AudioSource::set_source_clip(const AssetHandle<AudioClip>& ast)
{
	// We obtain a new reference
	audio_clip_src = ast.duplicate();
	// We can now safely store a pointer
	engine->mtx.lock();
	sample_source = ast.data;
	generic_src = nullptr;
	engine->mtx.unlock();
}

void AudioSource::set_looping(bool val)
{
	engine->mtx.lock();
	loops = val;
	engine->mtx.unlock();
}

void AudioSource::set_source_generic(std::unique_ptr<SampleSource>& src)
{
	sample_source = src.get();
	audio_clip_src = AssetHandle<AudioClip>();
	generic_src = std::move(src);
}

