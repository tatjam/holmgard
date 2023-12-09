#pragma once
#include "Trajectory.h"
#pragma warning(push, 0)
#include <btBulletDynamicsCommon.h>
#pragma warning(pop)
#include <sol/sol.hpp>

#include <renderer/Drawable.h>
#include <util/defines.h>
#include <set>

#include "history/EntityHistory.h"

#include <cpptoml.h>

// An UniverseObject is something which exists on the universe, and can exist on its bullet physics
// world.
// It supports trajectory propagation, and as such acts as a base for all physical objects in the game.
// It can be used for non-physical objects that are somehow related to the universe
// It may be added to the renderer as drawable.
//
// Communication between UniverseObjects is freely handled by the user, the recommended architecture
// is the event publish-subscribe system
class UniverseObject
{
	friend class Universe;
protected:
	bool is_in_universe;

private:

	Universe* universe;
	bool bullet_enabled;
	int64_t uid;
	sol::state* lua_state;
	std::string type_str;

public:
	// Returns true if the object has not been removed.
	// Useful if you keep references (not recommended, but required sometimes!)
	bool exists() const { return is_in_universe; }
	// This may not free the object itself until all references to it disappear
	// but it will remove it from the universe, thus stopping updates and similar
	void remove();
	EntityHistory history;

	sol::environment env;
	std::shared_ptr<cpptoml::table> init_toml;

	// You should start simulating bullet physics here
	void enable_bullet(btDynamicsWorld* world);
	// You must stop simulating bullet physics here
	void disable_bullet(btDynamicsWorld* world);

	// Give your current position, velocity and orientation
	// Origin may be a representative point, for example, COM
	// If physics is true, return data without interpolation / at bullet time
	glm::dvec3 get_position(bool physics);
	glm::dvec3 get_velocity(bool physics);
	glm::dquat get_orientation(bool physics);
	glm::dvec3 get_angular_velocity(bool physics);

	// If you return nullptr, it will be assumed you are n-body propagated
	// Otherwise, return a valid Trajectory for the predictor / propagator
	// Note that you need to manually call update() in the trajectory! This is only
	// for timewarp and prediction
	Trajectory* get_trajectory();

	// An approximation of our size, try to go higher than the real number
	// Values of 0.0 means that we don't have a limit for physics loading
	double get_physics_radius();

	// Return true if physics are required around this entity
	// (This entity will also be loaded)
	bool is_physics_loader();

	// Visual update, always realtime
	void update(double dt);

	// Ticks alongside bullet (bullet tick callback)
	// Note: Ticks before bullet update! (pretick)
	void physics_update(double pdt);

	// Called when the entity is added into the universe
	// Universe is already initialized
	// Note: Not the same as create, this is called also on loading save-games
	// but create only once when the entity is first created
	void init();

	// Return true if the physics have stabilized enough for timewarp
	// Vehicles should return false when they are close enough to surfaces
	// or in atmospheric flight
	bool timewarp_safe();

	void setup(Universe* universe, int64_t uid);

	inline int64_t get_uid()
	{
		return uid;
	}

	inline Universe* get_universe()
	{
		return universe;
	}

	void enable_bullet_wrapper(bool value, btDynamicsWorld* world)
	{
		bullet_enabled = value;

		if (value)
		{
			enable_bullet(world);
		}
		else
		{
			disable_bullet(world);
		}
	}

	std::string get_type();

	// Used while loading saves
	static std::shared_ptr<UniverseObject> load(std::string type, std::shared_ptr<cpptoml::table> toml);
	void save(cpptoml::table& to);

	explicit UniverseObject(std::string script_path, std::string in_pkg, std::shared_ptr<cpptoml::table> init_toml,
							std::vector<sol::object> args, bool is_create);

	// This is called already within a window so don't create one
	void do_debug_imgui();


	// Objects with higher priority get drawn first
	virtual int get_forward_priority() { return 0.0; }

	~UniverseObject();

};
