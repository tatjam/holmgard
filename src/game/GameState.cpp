#include "GameState.h"
#include <game/scenes/LuaScene.h>
#include <Holmgard.h>

GameState::GameState() : universe(), debug(this)
{
}


void GameState::write(cpptoml::table& target) const
{

}

void GameState::physics_update(double bdt)
{
	scene->physics_update(bdt);
}

void GameState::update()
{
	if(to_delete != nullptr)
	{
		delete to_delete;
		to_delete = nullptr;
	}

	scene->pre_update();
	//universe.update(hgr->dt);

	if(debug.override_camera)
	{
		debug.update_cam(hgr->dt);
		debug.cam.update(hgr->dt);
		if(scene)
		{
			scene->gui_input.ext_mouse_blocked = debug.cam.mouse_blocked;
			scene->gui_input.ext_keyboard_blocked = debug.cam.keyboard_blocked;
		}
	}

	if(scene && debug.allow_update)
	{
		scene->gui_input.update();
		scene->update();
	}

	debug.update();

}

void GameState::render()
{
	if(scene)
	{
		scene->render();
	}
}


void GameState::load_scene(Scene* n_scene)
{
	if(scene != nullptr)
	{
		scene->unload();
		to_delete = scene;
	}

	scene = n_scene;
	scene->load();
}

GameState* GameState::create_with_system(const std::string &planetary_system_package)
{
	GameState* out = create_empty_gamestate();

	out->used_packages.emplace_back("core");
	out->used_packages.push_back(planetary_system_package);
	auto [pkg, name] = hgr->assets->get_package_and_name(planetary_system_package, "core");
	out->system_used = pkg + ":" + name;
	out->system_version = hgr->assets->get_package_metadata(pkg).version;

	auto root = hgr->assets->load_toml(planetary_system_package);
	// We may load the system, as it's just a gamestate file without the metadata
	out->load_inner(*root);

	// Now load the default main scene (which is determined in core:default_scene.toml)
	auto scene_toml = hgr->assets->load_toml("core:scenes/new_save_scene.toml");
	out->load_scene_from_save(*scene_toml);

	return out;
}

GameState *GameState::load(const std::string &path)
{
	GameState* out = create_empty_gamestate();
	out->path = path;
	logger->check(hgr->assets->is_path_safe(path), "Unsafe path");

	std::string full_path = hgr->assets->udata_path + "saves/" + path + "save.toml";

	auto root = SerializeUtil::load_file(full_path);
	// Read required packages
	auto pkgs = root->get_array_of<std::string>("packages");
	for(const std::string& pkg : *pkgs)
	{
		out->used_packages.push_back(pkg);
	}

	out->system_used = *root->get_as<std::string>("system_package");
	out->system_version = *root->get_as<std::string>("system_version");

	auto [pkg, name] = hgr->assets->get_package_and_name(out->system_used, "core");
	// Check system version
	const std::string& cur_version = hgr->assets->get_package_metadata(pkg).version;
	out->is_system_outdated = false;
	if(cur_version != out->system_version)
	{
		logger->warn("Gamestate system version ({}) is not equal to current system version ({}) (system = {})",
			   out->system_version, cur_version, pkg);
		out->is_system_outdated = true;
	}

	// Load packages now so they register all scripts...
	hgr->assets->load_packages(out->used_packages, lua_core, hgr->game_database);

	out->load_inner(*root);

	// Finally, we may load the scene
	auto scene_toml = root->get_table("scene");
	out->load_scene_from_save(*scene_toml);

	return out;
}

GameState *GameState::create_main_menu(const std::string &skip_to_save)
{
	if(skip_to_save.empty())
	{
#ifndef HOLMGARD_ENTRYPOINT
		logger->fatal("No save load specified, nor was HOLMGARD_ENTRYPOINT defined");
#else
		const std::string& entry_point = HOLMGARD_ENTRYPOINT;
		// Create an empty GameState
		GameState* out = create_empty_gamestate();
		out->scene = new LuaScene(out, entry_point, "core", {});
		out->scene->first_load();
		return out;
#endif
	}
	else
	{
		return load(skip_to_save);
	}
}

GameState *GameState::create_empty_gamestate()
{
	GameState* out = new GameState();
	// TODO: This could be moved somewhere else, but it's important
	// to run it quick, as otherwise lua environments get the wrong universe!
	hgr->universe = &out->universe;
	hgr->game_state = out;

	out->to_delete = nullptr;
	out->scene = nullptr;
	out->path = "";
	return out;
}

void GameState::init()
{
	scene->load();

}

void GameState::reload_system(bool buildings, bool elements)
{
	// TODO :P
}

void GameState::load_inner(cpptoml::table &from)
{
	// Read the system
	universe.system.load(from);

	double t0 = *from.get_as<double>("t");
	universe.system.t0 = t0;
	Date start_date = Date(t0);
	logger->info("Starting at: {}", start_date.to_string());

	universe.system.init(universe.bt_world);

	// These updates populate the element arrays
	universe.system.update(0.0, universe.bt_world, false);
	universe.system.update(0.0, universe.bt_world, true);

	// Load objects
	int64_t last_uid = *from.get_as<int64_t>("uid");
	auto objects = from.get_table_array("object");

	if (objects)
	{
		for (const auto& object : *objects)
		{
			int64_t id = object->get_as<int64_t>("id").value_or(0);
			if (id > last_uid || id <= 0)
			{
				logger->fatal("Invalid UID {} in save", id);
			}
			std::string type = *object->get_as<std::string>("type");

			auto n_ent = UniverseObject::load(type, object);
			universe.objects.push_back(n_ent);

			ent_to_id[&(*n_ent)] = id;
		}
	}


	// Init the hashtable
	for(std::shared_ptr<UniverseObject> ent : universe.objects)
	{
		universe.objects_by_id[ent_to_id[&(*ent)]] = &(*ent);
	}

	universe.uid = last_uid;

	// Finally, init the objects
	for(std::shared_ptr<UniverseObject> ent : universe.objects)
	{
		ent->setup(&universe, ent_to_id[&(*ent)]);
	}

}

void GameState::load_scene_from_save(cpptoml::table& scene_toml)
{
	const std::string scene_path = *scene_toml.get_as<std::string>("name");
	// TODO: create the args
	std::vector<sol::object> args;
	auto* sc = new LuaScene(this, scene_path, "", args);
	scene = sc;
}

GameState::~GameState()
{
	debug.clear();
	if(scene != nullptr)
	{
		scene->unload();
		delete scene;
	}
}


