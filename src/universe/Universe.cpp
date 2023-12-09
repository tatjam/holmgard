#include "Universe.h"
#include <util/Profiler.h>
#include <game/GameState.h>

#ifdef OSPGL_LRDB
#include <LRDB/server.hpp>
#endif

static void bullet_tick(btDynamicsWorld* world, btScalar tstep)
{
	Universe* uv = (Universe*)world->getWorldUserInfo();

	// Call the system physics tick
	uv->physics_update(tstep);
}



void Universe::physics_update(double pdt)
{
	hgr->game_state->physics_update(pdt);
	// Do the physics update on the system
	system.update(pdt, bt_world, true);

	for (std::shared_ptr<UniverseObject> e : objects)
	{
		e->physics_update(pdt);
	}
}

void Universe::update(double dt)
{
	PROFILE_BLOCK("universe");

	if(!paused)
	{
		system.update(dt, bt_world, false);

		for (std::shared_ptr<UniverseObject> e : objects)
		{
			e->update(dt);
		}

		bt_world->stepSimulation(dt, MAX_PHYSICS_STEPS, PHYSICS_STEPSIZE);

	}

}

int64_t Universe::get_uid()
{
	// Increase BEFORE, uid=0 is the "nullptr" of ids
	uid++;
	return uid;
}

UniverseObject* Universe::get_object(int64_t uid)
{
	auto it = objects_by_id.find(uid);
	if(it == objects_by_id.end())
	{
		return nullptr;
	}

	return it->second;
}


Universe::Universe() : system(this)
{
	uid = 0;
	paused = false;

	// We set the global Holmgard so required modules can use it
	lua_state["hgr"] = hgr;

	bt_collision_config = new btDefaultCollisionConfiguration();
	bt_dispatcher = new btCollisionDispatcher(bt_collision_config);
	bt_brf_interface = new btDbvtBroadphase();
	bt_solver = new btSequentialImpulseConstraintSolver();
	bt_world = new btDiscreteDynamicsWorld(bt_dispatcher, bt_brf_interface, bt_solver, bt_collision_config);
	
	bt_world->setGravity({ 0.0, 0.0, 0.0 });

	bt_debug = new BulletDebugDrawer();
	bt_world->setDebugDrawer(bt_debug);


	bt_debug->setDebugMode(
		btIDebugDraw::DBG_DrawConstraints |
		btIDebugDraw::DBG_DrawWireframe |
		btIDebugDraw::DBG_DrawFrames |
		btIDebugDraw::DBG_DrawConstraintLimits |
		btIDebugDraw::DBG_DrawAabb);
	

	bt_world->setInternalTickCallback(bullet_tick, this, true);

	lua_core->load(lua_state, "__UNDEFINED__");

#ifdef OSPGL_LRDB
	lua_debug_server = nullptr;
#endif

	system_ptr = std::shared_ptr<PlanetarySystem>(&system, &null_deleter<PlanetarySystem>);
}


Universe::~Universe()
{

#ifdef OSPGL_LRDB
	disable_debugging();
#endif

	lua_state.collect_garbage();
}

std::shared_ptr<UniverseObject>
Universe::create_object(std::string script_path, std::string in_pkg, std::shared_ptr<cpptoml::table> init_toml,
						std::vector<sol::object> args, bool is_create)
{
	auto n_ent = std::make_shared<UniverseObject>(script_path, in_pkg, init_toml, args, is_create);
	int64_t id = get_uid();
	objects.push_back(n_ent);
	objects_by_id[id] = &(*n_ent);

	emit_event("core:new_object", id);

	n_ent->is_in_universe = true;
	n_ent->setup(this, id);

	return n_ent;
}

void Universe::remove_object(UniverseObject *ent)
{
	ent->is_in_universe = false;
	for (auto it = objects.begin(); it != objects.end(); it++)
	{
		if (&(**it) == ent)
		{
			objects.erase(it);
			break;
		}
	}

	emit_event("core:remove_object", ent->get_uid());

	objects_by_id.erase(ent->get_uid());
}

#ifdef OSPGL_LRDB
void Universe::enable_debugging()
{
	logger->info("Enabling lua debugger. Make sure to connect via the debugger as the game will be blocked!");
	lrdb::server* lua_debug_server_tmp = new lrdb::server(21110);
	lua_debug_server_tmp->reset(lua_state.lua_state());
	// We assign the void* to be able to free it later, but not expose the type to the exterior!
	lua_debug_server = lua_debug_server_tmp;
}

void Universe::disable_debugging()
{
	if(lua_debug_server)
	{
		lrdb::server *lua_debug_server_tmp = (lrdb::server *) lua_debug_server;
		lua_debug_server_tmp->reset();
		delete lua_debug_server_tmp;
		lua_debug_server = nullptr;
	}
}

#endif
