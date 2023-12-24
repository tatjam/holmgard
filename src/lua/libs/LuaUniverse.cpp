#include "LuaUniverse.h"

#include <utility>
#include "universe/entity/UniverseObject.h"
#include "LuaEvents.h"

void LuaUniverse::load_to(sol::table& table)
{

	table.new_usertype<Universe>("universe",
		EVENT_EMITTER_SIGN_UP(Universe),
		EVENT_EMITTER_EMIT(Universe),
		"update", &Universe::update,
		"bt_world", &Universe::bt_world,
		"system", &Universe::system_ptr,
		"get_object", &Universe::get_object,
		"objects", sol::property([](Universe* uv)
		  {
			return sol::as_table(uv->objects);
		  }),
	    "create_object", [](Universe* uv, const std::string& script_path, sol::this_environment te, sol::variadic_args args)
		 {
			sol::environment& env = te;
			 // Not sure if this is neccesary, maybe just pass args?
			 std::vector<sol::object> args_v;
			 for(auto v : args)
			 {
				 args_v.push_back(v);
			 }
			return uv->create_object(script_path, env["__pkg"].get_or<std::string>("core"), nullptr, args_v, true);

		 }
	);

	table.new_usertype<PlanetarySystem>("planetary_system", sol::base_classes, sol::bases<Drawable>(),
	        "get_element_position", &PlanetarySystem::get_element_position,
			"get_element_velocity", &PlanetarySystem::get_element_velocity,
			"get_element", &PlanetarySystem::get_element,
			"load", &PlanetarySystem::load,
			"elements", &PlanetarySystem::elements
	);

	table.new_usertype<SystemElement>("system_element",
			  "index", sol::readonly(&SystemElement::index),
			  "name", sol::readonly(&SystemElement::name),
			  "nbody", sol::readonly(&SystemElement::nbody),
			  "config", sol::readonly(&SystemElement::config),
			  "dot_factor", sol::readonly(&SystemElement::dot_factor),
			  "render_enabled", &SystemElement::render_enabled
			  );

	table.new_usertype<ElementConfig>("element_confg",
			  "mass", sol::readonly(&ElementConfig::mass),
			  "radius", sol::readonly(&ElementConfig::radius)
	);

	table.new_usertype<UniverseObject>("object", sol::no_constructor,
									   "enable_bullet", &UniverseObject::enable_bullet,
									   "disable_bullet", &UniverseObject::disable_bullet,
									   "set_bullet_enabled", &UniverseObject::enable_bullet_wrapper,
									   "get_position", &UniverseObject::get_position,
									   "get_velocity", &UniverseObject::get_velocity,
									   "get_orientation", &UniverseObject::get_orientation,
									   "get_angular_velocity", &UniverseObject::get_angular_velocity,
									   "get_physics_radius", &UniverseObject::get_physics_radius,
									   "is_physics_loader", &UniverseObject::is_physics_loader,
									   "timewarp_safe", &UniverseObject::timewarp_safe,
									   "uid", sol::property(&UniverseObject::get_uid),
									   "get_type", &UniverseObject::get_type,
									   "init_toml", &UniverseObject::init_toml,
									   "save", &UniverseObject::save,
									   "lua", &UniverseObject::env
			//"__index", [](UniverseObject* ent, const std::string& idx){ return ent->env[idx]; },
			);

	table.new_usertype<WorldState>("world_state", sol::no_constructor,
	   "pos", &WorldState::pos,
	   "vel", &WorldState::vel,
	   "rot", &WorldState::rot,
	   "ang_vel", &WorldState::ang_vel,
	   "get_tform", [](WorldState* self)
	   {
			glm::dmat4 mat = glm::dmat4();
			mat = glm::translate(mat, self->pos) * glm::toMat4(self->rot);
			return mat;
	   });

}
