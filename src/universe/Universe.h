#pragma once
#include "PlanetarySystem.h"
#include "universe/entity/UniverseObject.h"
#include <any>
#include "Events.h"
#pragma warning(push, 0)
#include <btBulletDynamicsCommon.h>
#include <BulletDynamics/ConstraintSolver/btNNCGConstraintSolver.h>
#pragma warning(pop)

#include <physics/debug/BulletDebugDrawer.h>
#include <sol/sol.hpp>

// The Universe is the central class of the game. It stores both the system
// and everything else in the system (buildings and vehicles).
// It implements a little UniverseObject system to simplify a lot of the code
// and make modder's life easier. 
// Note that events are implemented fully dynamic as they are needed
// from the lua side. Otherwise we could simply use a events library.
//
// Events can carry any set of arguments, handled as a std::vector<std::any>
// It's up to the event how are these arguments handled.
// Global events have "emitter" set to nullptr
// Event naming:
// - OSPGL events are prefixed with 'core:'
// - Mod events should be prefixed with the mod's id and ':', for example interstellar:start_hyperspace
// This will hopefully avoid event name clashing.
// 
// It's the responsability of the event receiver to remove the handler once it's deleted / not needed!
class GameState;

class Universe : public EventEmitter
{
private:

	btDefaultCollisionConfiguration* bt_collision_config;
	btCollisionDispatcher* bt_dispatcher;
	btBroadphaseInterface* bt_brf_interface;
	btSequentialImpulseConstraintSolver* bt_solver;

	BulletDebugDrawer* bt_debug;

	int64_t uid;

public:
	// Should updates run?
	bool paused;

	friend class UniverseObject;
	friend class GameState;

	static constexpr double PHYSICS_STEPSIZE = 1.0 / 30.0;
	static constexpr int MAX_PHYSICS_STEPS = 1;

	// We use a global state for everything in the universe so data can 
	// be shared between lua scripts without "hacks"
	// Planet surfaces are independent, and other stuff, as they are not sharing
	// any data with the rest of the universe
	sol::state lua_state;

#ifdef OSPGL_LRDB
	void* lua_debug_server;
	// This function will block until the debugger is connected!
	void enable_debugging();
	void disable_debugging();
#endif


	btDiscreteDynamicsWorld* bt_world;

	PlanetarySystem system;
	// Exposed to lua to be addable as a drawable
	// TODO: This feels like a horrible hack
	std::shared_ptr<PlanetarySystem> system_ptr;

	// Objects are stored here for lifetime management, so the lua user can
	// "fire-and-forget" when creating objects
	std::vector<std::shared_ptr<UniverseObject>> objects;
	std::unordered_map<int64_t, UniverseObject*> objects_by_id;

	std::shared_ptr<UniverseObject> create_object(std::string script_path, std::string in_pkg,
												  std::shared_ptr<cpptoml::table> init_toml, std::vector<sol::object> args,
												  bool is_create);

	void remove_object(UniverseObject* ent);

	// Returns nullptr if not found
	// Do not hold the pointer for long, calling this each time you access the entity is better
	UniverseObject* get_object(int64_t id);

	// Note: This is automatically called from bullet
	void physics_update(double pdt);
	void update(double dt);
	
	int64_t get_uid();

	Universe();
	~Universe();
};

