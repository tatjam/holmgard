#include "util/Timer.h"

#include "assets/Model.h"
#include "physics/ground/GroundShape.h"
#include "Holmgard.h"
#include "game/GameState.h"
#include <util/Profiler.h>

#include <renderer/Renderer.h>

#include <assets/AudioClip.h>
#include <audio/AudioSource.h>
#include <audio/AudioEngine.h>

int main(int argc, char** argv)
{
	hgr = new Holmgard();
	hgr->init(argc, argv);

	double fps_t = 0.0;
	double dt_avg = 0.0;

	PROFILE_FUNC();

	while (hgr->should_loop())
	{
		PROFILE_BLOCK("frame");

		hgr->start_frame();
		hgr->update();
		hgr->render();
		hgr->finish_frame();

	}

	hgr->finish();
}
