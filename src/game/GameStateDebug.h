#pragma once
#include <vector>
#include "renderer/camera/SimpleCamera.h"

class GameState;
class UniverseObject;

// Allows accesing debug for all objects, scenes itself and a bunch of useful tools
// Works as a menubar
class GameStateDebug
{
private:
	GameState* g;

	std::vector<UniverseObject*> shown_objects;
	UniverseObject* centered_camera;

	bool terminal_shown;
	bool objects_shown;

	void do_terminal();
	void do_objects();
	void do_launcher();
	void do_assets();
	void do_scene();
	void do_renderer();

	bool terminal_undocked;
	bool objects_undocked;
	bool assets_undocked;
	bool scene_undocked;
	bool renderer_undocked;

	static void do_docking_button(bool* val);

public:

	SimpleCamera cam;
	bool override_camera;

	bool show;
	bool allow_update;
	void update();
	void update_cam(double dt);

	GameStateDebug(GameState* gamestate);
};