#include "GUIManager.h"

#include <GL/glew.h>

#include "AppWindow.h"
#include "ANNWrapper.h"
#include "SceneManager.h"
#include "TrainingScene.h"
#include "SceneConstants.h"

#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_glfw.h"
#include "ImGui/imgui_impl_opengl3.h"

GUIManager::GUIManager(SceneManager & sceneMgr, AppWindow & appWin) :
	m_sceneMgr(sceneMgr),
	m_appWin(appWin),
	m_isConfigSet(false)
{
	// imgui stuff here
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	// dark color
	ImGui::StyleColorsDark();

	ImGui_ImplGlfw_InitForOpenGL(m_appWin.GetNativeWindow(), true);

	const char* glsl_version = "#version 130";
	ImGui_ImplOpenGL3_Init(glsl_version);
}

GUIManager::~GUIManager()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

void GUIManager::Update(float dt)
{
}

void GUIManager::BegFrame()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

void GUIManager::EndFrame()
{
	ImGui::Render();

	// rendering here
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	// programming tip: always reset state - application will be easier to debug ...
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glUseProgram(0);
}

void GUIManager::Render()
{
	BegFrame();

	if (!m_isConfigSet)
		RenderGameConfig();
	else
		RenderGameInfo();

	EndFrame();
}

void GUIManager::RenderOverlay(float alpha)
{
	const ImGuiWindowFlags flags =	ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar |
									ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
									ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoTitleBar;

	// set up sizes
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, alpha));

	// set top left corner
	ImGui::SetNextWindowPos(ImVec2());

	// set full window size
	ImVec2 displaysize = ImGui::GetIO().DisplaySize;
	ImGui::SetNextWindowSize(ImVec2(displaysize.x, displaysize.y));

	// generate the overlay
	if (ImGui::Begin("Overlay", nullptr, flags))
	{
		if (ImGui::IsWindowFocused())
			ImGui::SetNextWindowFocus();
		ImGui::End();
	}

	// pop out style
	ImGui::PopStyleColor();
}

void GUIManager::RenderGameInfo()
{
	// rendering here
	ImGui::Begin("Statistics");

	if (ImGui::PlusButton("Add Scene Spd"))
	{
		m_sceneMgr.SetSceneSpeed(m_sceneMgr.GetSceneSpeed() << 1);
	}
	
	ImGui::SameLine();

	if (ImGui::MinusButton("Minus Scene Spd"))
	{
		m_sceneMgr.SetSceneSpeed(m_sceneMgr.GetSceneSpeed() >> 1);
	}

	ImGui::Text("Current Score:\t%d",	m_sceneMgr.GetTrainingScene()->GetCurrentScore());
	ImGui::Text("Max Score:\t%d",		m_sceneMgr.GetTrainingScene()->GetMaxScore());
	ImGui::Text("Current Gen:\t%d",		m_sceneMgr.GetTrainingScene()->GetCurrentGeneration());
	ImGui::Text("Birds Alive:\t%d",		m_sceneMgr.GetTrainingScene()->GetLiveBirdCount());

	ImGui::End();
}

void GUIManager::RenderGameConfig()
{
	RenderOverlay(1.f);

	ImGui::Begin("Training Configuration");
	int sample = static_cast<int>(m_scenConfig.m_agentCount);
	if (ImGui::InputInt("Agents Count", &sample, 1, 100))
		m_scenConfig.m_agentCount = static_cast<unsigned>(max(1, sample));

	if (ImGui::Button("Start Training"))
	{
		m_scenConfig.m_discreteDT = static_cast<float>(DISCRETE_DT);
		m_sceneMgr.Init(m_scenConfig);
		m_isConfigSet = true;
	}
	ImGui::End();
}