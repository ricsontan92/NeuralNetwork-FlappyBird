#pragma once

#include "SceneManager.h"

class AppWindow;

class GUIManager
{
public:
	GUIManager(SceneManager & scene, AppWindow & appWin);
	~GUIManager();
	void Update(float dt);
	void Render();

private:
	void BegFrame();
	void EndFrame();
	void RenderOverlay(float alpha);

	void RenderToolTip( const char* message )const;
	void RenderGameInfo();
	void RenderGameConfig();

	SceneManager::ScenesConfig m_scenConfig;
	bool			m_isConfigSet;
	AppWindow &		m_appWin;
	SceneManager &	m_sceneMgr;
};