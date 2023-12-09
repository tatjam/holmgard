#include "GameStateDebug.h"
#include <imgui/imgui.h>
#include "GameState.h"
#include <algorithm>
#include <util/InputUtil.h>
#include <renderer/Renderer.h>

void GameStateDebug::update()
{
	if(input->key_down(GLFW_KEY_GRAVE_ACCENT) ||
			(input->key_down(GLFW_KEY_F12) && input->key_pressed(GLFW_KEY_LEFT_ALT) && input->key_pressed(GLFW_KEY_LEFT_CONTROL)))
	{
		show = !show;
	}

	if(show)
	{
		do_launcher();
		if(terminal_undocked)
		{
			ImGui::Begin("Terminal");
			do_terminal();
			ImGui::End();
		}
		if(assets_undocked)
		{
			ImGui::Begin("Assets");
			do_assets();
			ImGui::End();
		}
		if(objects_undocked)
		{
			ImGui::Begin("Entities");
			do_objects();
			ImGui::End();
		}
		if(scene_undocked)
		{
			ImGui::Begin("Scene");
			do_scene();
			ImGui::End();
		}
		if(renderer_undocked)
		{
			ImGui::Begin("Renderer");
			do_renderer();
			ImGui::End();
		}

		for(std::shared_ptr<UniverseObject> e : hgr->universe->objects)
		{
			if(vector_contains(shown_objects, e))
			{
				ImGui::PushID(&(*e));
				std::string name_str = "UniverseObject (" + std::to_string(e->get_uid()) + ") " + e->get_type();
				ImGui::Begin(name_str.c_str());
				e->do_debug_imgui();
				ImGui::End();
				ImGui::PopID();
			}
		}

	}

}

GameStateDebug::GameStateDebug(GameState* gamestate)
{
	g = gamestate;
	allow_update = true;
	show = false;

	terminal_undocked = false;
	assets_undocked = false;
	objects_undocked = false;
	scene_undocked = false;
	renderer_undocked = false;

	override_camera = false;
	centered_camera = nullptr;
}

void GameStateDebug::do_terminal()
{
	do_docking_button(&terminal_undocked);
}

void GameStateDebug::do_objects()
{
	do_docking_button(&objects_undocked);

	for (std::shared_ptr<UniverseObject> e : hgr->universe->objects)
	{
		ImGui::PushID(&(*e));
		ImGui::Text("%lld (%s)", e->get_uid(), e->get_type().c_str());
		ImGui::SameLine();

		if(ImGui::Button("View"))
		{
			centered_camera = e;
			override_camera = true;
		}
		ImGui::SameLine();
		if(vector_contains(shown_objects, e))
		{
			if(ImGui::Button("Hide"))
			{
				shown_objects.erase(std::remove(shown_objects.begin(), shown_objects.end(), e),
					   shown_objects.end());
			}
		}
		else
		{
			if(ImGui::Button("Show"))
			{
				shown_objects.push_back(e);
			}
		}
		ImGui::PopID();
	}
}
void GameStateDebug::do_scene()
{
	do_docking_button(&scene_undocked);
	g->scene->do_imgui_debug();
}

void GameStateDebug::do_assets()
{
	do_docking_button(&assets_undocked);
}

void GameStateDebug::do_launcher()
{
	ImGui::Begin("Debug menu");
	ImGui::Text("Toggle with Ctrl+Alt+F12 / º / `");
	// Basic perfomance info
	ImGui::Text("FPS: %i | Game FPS: %i", (int)(1.0 / hgr->game_dt), (int)(1.0 / hgr->dt));
	if(override_camera)
	{
		if(ImGui::Button("Free camera"))
		{
			override_camera = false;
		}
	}
	ImGui::BeginTabBar("Tabs");
	if(!objects_undocked && ImGui::BeginTabItem("Entities"))
	{
		do_objects();
		ImGui::EndTabItem();
	}
	if(!terminal_undocked && ImGui::BeginTabItem("Terminal"))
	{
		do_terminal();
		ImGui::EndTabItem();
	}
	if(!assets_undocked && ImGui::BeginTabItem("Assets"))
	{
		do_assets();
		ImGui::EndTabItem();
	}
	if(!scene_undocked && ImGui::BeginTabItem("Scene"))
	{
		do_scene();
		ImGui::EndTabItem();
	}
	if(!renderer_undocked && ImGui::BeginTabItem("Renderer"))
	{
		do_renderer();
		ImGui::EndTabItem();
	}
	ImGui::EndTabBar();


	ImGui::End();
}


void GameStateDebug::do_docking_button(bool *val)
{
	if(*val)
	{
		if(ImGui::Button("Dock"))
		{
			*val = false;
		}
	}
	else
	{
		if(ImGui::Button("Undock"))
		{
			*val = true;
		}
	}

}

void GameStateDebug::update_cam(double dt)
{
	cam.update(dt);
	bool found = false;
	for (std::shared_ptr<UniverseObject> e : hgr->universe->objects)
	{
		if(e == centered_camera)
		{
			found = true;
		}
	}

	if(!found)
	{
		override_camera = false;
		centered_camera = nullptr;
	}
	else
	{
		cam.center = centered_camera->get_position(false);
	}

}

void GameStateDebug::do_renderer()
{

	if(ImGui::BeginCombo("View G-Buffer: ", DebugGBuffer::get_mode_str(hgr->renderer->debug_gbuffer.mode)))
	{
		for(int v = DebugGBuffer::NONE; v != DebugGBuffer::END_MARKER; v++)
		{
			if(ImGui::Selectable(DebugGBuffer::get_mode_str((DebugGBuffer::View)v)))
			{
				hgr->renderer->debug_gbuffer.mode = (DebugGBuffer::View)v;
			}
		}
		ImGui::EndCombo();
	}
}


