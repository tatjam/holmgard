#pragma once
#include "../LuaLib.h"
#include <audio/AudioSource.h>

#define WRAP auto l = ptr.lock(); if(l)

// Wrapper to prevent direct calls to AudioSource, the lifetime of this
// soft-wrapper is fully lua managed and doesn't affect the AudioSource itself
// All functions silent-fail if the source is not available anymore, returning
// sane default values
struct LuaAudioHandler
{
	bool fire_and_forget;
	std::weak_ptr<AudioSource> ptr;

	bool is_available(){ return !ptr.expired(); }

	void play_and_forget()
	{
		if(fire_and_forget)
		{
			logger->warn("Already playing and forgetting, ignoring request.");
		}

		if(is_looping())
		{
			logger->warn("Warning, audio is set to loop but will be used in \"play and forget\" mode. Looping disabled");
			set_looping(false);
		}
		fire_and_forget = true;
		auto l = ptr.lock();
		l->set_destroy_when_finished();
		l->set_playing(true);
	}
	void set_playing(bool value) const
	{ WRAP l->set_playing(value); }
	bool is_playing() const
	{ WRAP return l->is_playing(); else return false;}
	bool is_playing_or_queued() const
	{ WRAP return l->is_playing_or_queued(); else return false;}
	bool is_3d_source() const
	{ WRAP return l->is_3d_source(); else return false;}
	void set_3d_source(bool value) const
	{ WRAP l->set_3d_source(value); }
	glm::dvec3 get_position() const
	{ WRAP return l->get_position(); else return glm::dvec3(0, 0, 0);}
	void set_position(glm::dvec3 pos) const
	{ WRAP l->set_position(pos);}
	float get_gain() const
	{ WRAP return l->get_gain(); else return 0.0; }
	void set_gain(float val) const
	{ WRAP l->set_gain(val);}
	float get_pitch() const
	{ WRAP return l->get_pitch(); else return 1.0;}
	void set_pitch(float val) const
	{ WRAP l->set_pitch(val); }
	bool is_looping() const
	{ WRAP return l->is_looping(); else return false; }
	void set_looping(bool val) const
	{ WRAP l->set_looping(val); }
	void set_source_clip(const AssetHandle<AudioClip>& ast) const
	{ WRAP l->set_source_clip(std::move(ast)); }

	// NOT EXPOSED TO LUA! Use specialized load_into functions
	void set_source_generic(std::unique_ptr<SampleSource>& ssource) const
	{ WRAP l->set_source_generic(ssource); }

	// We manage lifetime of audio as long as it's not fire and forget
	~LuaAudioHandler(){
		if(!fire_and_forget)
		{
			auto l = ptr.lock();
			if(l)
			{
				l->destroy();
			}
		}
	}

};

class LuaAudio : public LuaLib
{
public:
	void load_to(sol::table& table) override;

};
