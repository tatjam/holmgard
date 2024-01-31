#include "LuaAudio.h"
#include <audio/AudioEngine.h>

void LuaAudio::load_to(sol::table &table)
{
	table.new_usertype<AudioEngine>("audio", sol::no_constructor,
		"set_listener", &AudioEngine::set_listener,
		"get_listener_pos", &AudioEngine::get_listener_pos,
		"get_listener_fwd", &AudioEngine::get_listener_fwd,
		"get_listener_up", &AudioEngine::get_listener_up,
		"create_source", [](AudioEngine* eng, int in_channel)
		{
			auto hnd = std::make_shared<LuaAudioHandler>();
			auto weakptr = eng->create_audio_source(in_channel);
			hnd->ptr = weakptr;
			return hnd;
		});

	table.new_usertype<LuaAudioHandler>("audio_handler", sol::no_constructor,
		"play_and_forget", &LuaAudioHandler::play_and_forget,
		"is_available", &LuaAudioHandler::is_available,
		"set_playing", &LuaAudioHandler::set_playing,
		"is_playing", &LuaAudioHandler::is_playing,
		"is_3d_source", &LuaAudioHandler::is_3d_source,
		"set_3d_source", &LuaAudioHandler::set_3d_source,
		"get_position", &LuaAudioHandler::get_position,
		"set_position", &LuaAudioHandler::set_position,
		"get_gain", &LuaAudioHandler::get_gain,
		"set_gain", &LuaAudioHandler::set_gain,
		"get_pitch", &LuaAudioHandler::get_pitch,
		"set_pitch", &LuaAudioHandler::set_pitch,
		"is_looping", &LuaAudioHandler::is_looping,
		"set_looping", &LuaAudioHandler::set_looping,
		"set_source", &LuaAudioHandler::set_source_clip
		);

	table.new_usertype<SimpleSampleSource>("simple_sample_source", sol::no_constructor,
		   sol::base_classes, sol::bases<SampleSource>());

	table.new_usertype<SampleSource>("sample_source", sol::no_constructor);

}
