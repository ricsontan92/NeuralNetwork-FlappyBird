#include "Camera.h"
#include "AppWindow.h"
#include "GUIManager.h"
#include "GLRenderer.h"
#include "DebugDrawer.h"
#include "SceneManager.h"
#include "TrainingScene.h"
#include "SceneConstants.h"
#include "PhysicsManager.h"
#include "GraphicsManager.h"

#include <Windows.h>
#include <iostream>
#include <thread>
#include <vector>
#include <future>
#include <array>

#include "SOIL/Soil.h"

int main()
{
	srand(static_cast<unsigned>(time(NULL)));

	AppWindow mainWin;

	const unsigned	winWidth = 1280, winHeight = 720;
	if (!mainWin.Init(winWidth, winHeight))
	{
		return -1; // Initialization failed
	}

	double dt = 0;

	// init graphics managers
	GraphicsManager graphicsMgr;
	graphicsMgr.SetVirtualWinsize(winWidth, winHeight);
	graphicsMgr.SetBackgroundColor(math::vec4(0.15f, 0.15f, 0.15f, 1.f));

	// scene manager
	SceneManager sceneMgr{ graphicsMgr };

	// gui creation
	GUIManager guiMgr{ sceneMgr, mainWin };

	while (!mainWin.ShouldClose())
	{
		auto time_start = std::chrono::high_resolution_clock::now();

		// fps measuring 
		mainWin.BeginFrame();

		// Keep running
		graphicsMgr.StartFrame();

		// set viewport
		int winWd, winHt;
		mainWin.GetFrameBufferSize(winWd, winHt);
		graphicsMgr.SetViewport(0, 0, winWd, winHt);

		// updating here
		float fdt = static_cast<float>(DISCRETE_DT);
		mainWin.Update();
		sceneMgr.Update(fdt);
		guiMgr.Update(fdt);

		// rendering here
		graphicsMgr.Render();
		guiMgr.Render();

		mainWin.PollEvent();
		mainWin.SwapBuffers();

		graphicsMgr.EndFrame();

		// frame rate controller
		do
		{
			std::this_thread::sleep_for(std::chrono::duration<double, std::nano>(0));
			dt = std::chrono::duration_cast<std::chrono::duration<double>>(std::chrono::high_resolution_clock::now() - time_start).count();
		} while (dt < DISCRETE_DT);

		mainWin.EndFrame();
	}

	return 0;
}
