#include "LuaCore.h"
#include <assets/AssetManager.h>

#include "libs/LuaLogger.h"
#include "libs/LuaGlm.h"
#include "libs/LuaNoise.h"
#include "libs/LuaAssets.h"
#include "libs/LuaBullet.h"
#include "libs/LuaToml.h"
#include "libs/LuaDebugDrawer.h"
#include "libs/LuaUniverse.h"
#include "libs/LuaNanoVG.h"
#include "libs/LuaGUI.h"
#include "libs/LuaImGUI.h"
#include "libs/LuaSceneLib.h"
#include "libs/LuaRenderer.h"
#include "libs/LuaModel.h"
#include "libs/LuaInput.h"
#include "libs/LuaOrbit.h"
#include "libs/LuaEvents.h"
#include "libs/LuaAudio.h"

// Used for setting up "hgr" usertype
#include "renderer/Renderer.h"
#include "audio/AudioEngine.h"
#include "universe/Universe.h"


LuaCore* lua_core;

LuaCore::LibraryID LuaCore::name_to_id(const std::string & name)
{
	if (name == "logger")
		return LibraryID::LOGGER;
	else if (name == "debug_drawer")
		return LibraryID::DEBUG_DRAWER;
	else if (name == "glm")
		return LibraryID::GLM;
	else if (name == "noise")
		return LibraryID::NOISE;
	else if (name == "assets")
		return LibraryID::ASSETS;
	else if(name == "bullet")
		return LibraryID::BULLET;
	else if(name == "toml")
		return LibraryID::TOML;
	else if(name == "universe")
		return LibraryID::UNIVERSE;
	else if(name == "nano_vg")
		return LibraryID::NANO_VG;
	else if(name == "gui")
		return LibraryID::GUI;
	else if(name == "imgui")
		return LibraryID::IMGUI;
	else if(name == "scene")
		return LibraryID::SCENE;
	else if(name == "renderer")
		return LibraryID::RENDERER;
	else if(name == "model")
		return LibraryID::MODEL;
	else if(name == "input")
		return LibraryID::INPUT;
	else if(name == "orbit")
		return LibraryID::ORBIT;
	else if(name == "events")
		return LibraryID::EVENTS;
	else if(name == "audio")
		return LibraryID::AUDIO;
#ifdef HOLMGARD_PLUGINS
	HOLMGARD_PLUGINS(HOLMGARD_MAKE_REQUIRE)
#endif
	else
		return LibraryID::UNKNOWN;
}

void LuaCore::load_library(sol::table& table, LibraryID id)
{
	libraries[id]->load_to(table);
}

void LuaCore::load(sol::state& to, const std::string& pkg)
{
	to.open_libraries(
		sol::lib::base, sol::lib::coroutine,
		sol::lib::string, sol::lib::math, sol::lib::table, sol::lib::utf8,
		sol::lib::jit);

	to["print"] = sol::nil;

	to.create_named_table("__loaded_modules");
	load((sol::table&)to.globals(), pkg);

	//to.add_package_loader(LoadFileRequire, true);

	// We also create Holmgard usertype (but don't actually set "hgr" to a value, as it may not be available)
	// so that all subsystems can be accessed without having many globals
	to.new_usertype<Holmgard>("__ut_hgr", sol::no_constructor,
							  "renderer", &Holmgard::renderer,
							  "audio", &Holmgard::audio,
							  "universe", &Holmgard::universe,
							  "game_dt", sol::readonly(&Holmgard::game_dt));

}


void LuaCore::load(sol::table& to, const std::string& pkg)
{
	to["__pkg"] = pkg;

	// We override the default dofile and loadfile for package behaviour
	// The file will be passed the environment of the caller (!)
	to["dofile"] = [](const std::string& path, sol::this_environment te, sol::this_state ts)
	{
		sol::environment env = te;
		sol::state_view sv = ts;
		auto name = hgr->assets->get_filename(path);
		// Enforce global and class files, which may not be "dofiled"
		if(name.rfind("g_", 0) == 0 || name.rfind("c_", 0) == 0)
		{
			logger->fatal("Tried to dofile {}, which is meant to be included with require", path);
		}
		std::string spath = hgr->assets->resolve_path(path, env["__pkg"]);

		// We load and execute the file, and return the result
		// We obey the standard and pass errors to the caller with throw
		sol::load_result lresult = sv.load_file(spath);
		if(!lresult.valid())
		{
			sol::error err = lresult;
			throw(err);
		}
		sol::safe_function fnc = lresult;
		sol::set_environment(env, fnc);
		sol::safe_function_result result = fnc();
		if(!result.valid())
		{
			sol::error err = result;
			throw(err);
		}

		return result;
	};

	// The file will be passed the environment of the caller (!)
	to["loadfile"] = [](const std::string& path, sol::this_environment te, sol::this_state ts)
	{
		sol::environment env = te;
		sol::state_view sv = ts;
		auto name = hgr->assets->get_filename(path);
		// Enforce global files, which may not be "dofiled"
		if(name.rfind("g_", 0) == 0)
		{
			logger->fatal("Tried to loadfile {}, which is meant to be included with require", path);
		}
		std::string spath = hgr->assets->resolve_path(path, env["__pkg"]);

		sol::load_result lresult = sv.load_file(spath);
		if(!lresult.valid())
		{
			std::string err = "Could not find file: " + spath;
			return std::make_tuple((sol::function)sol::nil, err);
		}

		sol::function fnc = lresult;
		sol::set_environment(env, fnc);
		return std::make_tuple(fnc, std::string(""));
	};

	to["require"] = [](const std::string& path, sol::this_environment te, sol::this_state ts)
	{
		sol::environment env = te;
		sol::state_view sv = ts;

		LuaCore::LibraryID id = LuaCore::name_to_id(path);
		sol::table tb;

		if (id != LuaCore::LibraryID::UNKNOWN)
		{
			std::string idname = "__corelib_";
			idname.append(path);
			if(sv[idname] == sol::nil)
			{
				sol::table module_table = sv.create_table();
				lua_core->load_library(module_table, id);
				sv[idname] = module_table;
				tb = module_table;
			}

			tb = sv[idname];
		}
		else
		{
			// Not a module, in that case it must be a file, or a typo
			auto name = hgr->assets->get_filename(path);
			logger->check(name.rfind("l_", 0) != 0, "Tried to require {}, which is meant to be included with dofile", path);

			std::string pkg = env["__pkg"];
			if(pkg == "__UNDEFINED__")
			{
				// We are running on the global environment, use package stack instead
				pkg = hgr->assets->get_current_package();
			}
			std::string fpath = hgr->assets->resolve_path(path, pkg);
			logger->check(hgr->assets->file_exists(fpath), "require(\"{}\") cannot be resolved to a package or lua file (searched for {})", path, fpath);

			// It may have already been loaded, in that case return it
			std::string idname = "__required_" + fpath;
			if(sv[idname] == sol::nil)
			{
				// Load the file
				auto[sub_pkg, _] = hgr->assets->get_package_and_name(path, pkg);
				std::string script = hgr->assets->load_string_raw(fpath);

				std::string orig = hgr->assets->get_current_package();
				hgr->assets->set_current_package(sub_pkg);

				sol::load_result fx = sv.load(script, fpath);
				sol::function ffx = fx;
				// Note that the function runs on the global environment, but we
				// set package previously!
				sol::table result = ffx();
				sv[idname] = result;
				tb = result;

				// Restore default package
				hgr->assets->set_current_package(orig);
			}

			tb = sv[idname];
		}

		return tb;

	};
}

LuaCore::LuaCore()
{
	libraries[LibraryID::UNKNOWN] = nullptr;
	libraries[LibraryID::LOGGER] = new LuaLogger();
	libraries[LibraryID::DEBUG_DRAWER] = new LuaDebugDrawer();
	libraries[LibraryID::GLM] = new LuaGlm();
	libraries[LibraryID::NOISE] = new LuaNoise();
	libraries[LibraryID::ASSETS] = new LuaAssets();
	libraries[LibraryID::BULLET] = new LuaBullet();
	libraries[LibraryID::TOML] = new LuaToml();
	libraries[LibraryID::UNIVERSE] = new LuaUniverse();
	libraries[LibraryID::NANO_VG] = new LuaNanoVG();
	libraries[LibraryID::GUI] = new LuaGUI();
	libraries[LibraryID::IMGUI] = new LuaImGUI();
	libraries[LibraryID::SCENE] = new LuaSceneLib();
	libraries[LibraryID::RENDERER] = new LuaRenderer();
	libraries[LibraryID::MODEL] = new LuaModel();
	libraries[LibraryID::INPUT] = new LuaInput();
	libraries[LibraryID::ORBIT] = new LuaOrbit();
	libraries[LibraryID::EVENTS] = new LuaEvents();
	libraries[LibraryID::AUDIO] = new LuaAudio();
#ifdef HOLMGARD_PLUGINS
	HOLMGARD_PLUGINS(HOLMGARD_MAKE_INIT)
#endif
}

LuaCore::~LuaCore()
{
	for (size_t i = 0; i < (size_t)LibraryID::COUNT; i++)
	{
		delete libraries[i];
	}
}

void create_global_lua_core()
{
	lua_core = new LuaCore();
}

void destroy_global_lua_core()
{
	delete lua_core;
}
