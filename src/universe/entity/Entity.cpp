#include "UniverseObject.h"
#include "../Universe.h"
#include <lua/LuaCore.h>
#include <game/GameState.h>
#include <utility>

void UniverseObject::enable_bullet(btDynamicsWorld *world)
{
	LuaUtil::call_function_if_present(env["enable_bullet"], world);
}

void UniverseObject::disable_bullet(btDynamicsWorld *world)
{
	LuaUtil::call_function_if_present(env["disable_bullet"], world);
}

glm::dvec3 UniverseObject::get_position(bool physics)
{
	return LuaUtil::call_function_if_present_returns<glm::dvec3>(env["get_position"], physics)
	        .value_or(glm::dvec3(0, 0, 0));
}

glm::dvec3 UniverseObject::get_velocity(bool physics)
{
	return LuaUtil::call_function_if_present_returns<glm::dvec3>(env["get_velocity"], physics).value_or(glm::dvec3(0, 0, 0));
}

glm::dquat UniverseObject::get_orientation(bool physics)
{
	return LuaUtil::call_function_if_present_returns<glm::dquat>(env["get_orientation"], physics)
	        .value_or(glm::dquat(1.0, 0.0, 0.0, 0.0));
}

glm::dvec3 UniverseObject::get_angular_velocity(bool physics)
{
	return LuaUtil::call_function_if_present_returns<glm::dvec3>(env["get_angular_velocity"], physics)
	        .value_or(glm::dvec3(0, 0, 0));
}

double UniverseObject::get_physics_radius()
{
	auto result = LuaUtil::call_function_if_present_returns<double>(env["get_physics_radius"]);
	return result.value_or(0.0);
}

bool UniverseObject::is_physics_loader()
{
	auto result = LuaUtil::call_function_if_present_returns<bool>(env["is_physics_loader"]);
	return result.value_or(false);
}

void UniverseObject::update(double dt)
{
	LuaUtil::call_function_if_present(env["update"], dt);
}

void UniverseObject::physics_update(double pdt)
{
	LuaUtil::call_function_if_present(env["physics_update"], pdt);
}

void UniverseObject::init()
{
	LuaUtil::call_function_if_present(env["init"]);
}

bool UniverseObject::timewarp_safe()
{
	auto result = LuaUtil::call_function_if_present_returns<bool>(env["timewarp_safe"]);
	return result.value_or(true);
}

std::string UniverseObject::get_type()
{
	return type_str;
}

UniverseObject::UniverseObject(std::string script_path, std::string in_pkg, std::shared_ptr<cpptoml::table> init_toml,
							   std::vector<sol::object> args, bool is_create)
{
	this->init_toml = std::move(init_toml);
	this->lua_state = &hgr->game_state->universe.lua_state;

	auto[pkg, name] = hgr->assets->get_package_and_name(script_path, in_pkg);
	type_str = pkg + ":" + name;

	env = sol::environment(*lua_state, sol::create, lua_state->globals());
	// We need to load LuaCore to it
	lua_core->load((sol::table&)env, pkg);
	env["entity"] = this;
	env["hgr"] = hgr;

	std::string full_path = hgr->assets->res_path + pkg + "/" + name;
	auto result = (*lua_state).safe_script_file(full_path, env);
	if(!result.valid())
	{
		sol::error err = result;
		logger->fatal("Lua Error loading entity:\n{}", err.what());
	}

	if(is_create)
	{
		LuaUtil::call_function_if_present_args(env["create"], args);
	}

}

void UniverseObject::save(cpptoml::table &to)
{
	LuaUtil::call_function_if_present(env["save"], to);
}

void UniverseObject::setup(Universe* universe, int64_t uid)
{
	this->universe = universe;
	this->uid = uid;
	init();
}

UniverseObject::~UniverseObject()
{

}

UniverseObject* UniverseObject::load(std::string type, std::shared_ptr<cpptoml::table> toml)
{
	UniverseObject* n_ent = new UniverseObject(type, "core", toml, {}, false);
	return n_ent;
}

void UniverseObject::do_debug_imgui()
{
	LuaUtil::call_function_if_present(env["do_debug_imgui"]);
}