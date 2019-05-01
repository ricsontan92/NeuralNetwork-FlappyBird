#include "GUIManager.h"

#include <GL/glew.h>

#include "AppWindow.h"
#include "GameScene.h"
#include "ANNWrapper.h"
#include "ANNTrainer.h"
#include "SceneManager.h"
#include "TrainingScene.h"

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
	{
		RenderGameInfo();
		RenderHistogram();
	}
	EndFrame();
}

void GUIManager::RenderOverlay(float alpha)
{
	const ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar |
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

	const std::vector<std::shared_ptr<TrainingScene>> & scenes = m_sceneMgr.GetTrainingScenes();

	for (unsigned i = 0; i < scenes.size(); ++i)
	{
		bool isRendering = m_sceneMgr.GetRenderingSceneIdx() == i;
		if (ImGui::Checkbox(("##Render" + std::to_string(i)).c_str(), &isRendering))
		{
			if (isRendering)
			{
				m_sceneMgr.SetRenderingSceneIdx(i);
			}
			else
			{
				m_sceneMgr.SetRenderingSceneIdx(-1);
			}
		}

		ImGui::SameLine();

		ImGui::Text("Scene %d: %d data", i + 1, scenes[i]->GetANNTrainer().GetDataCount());
		ImGui::SameLine();

		bool isNormalSpd = scenes[i]->GetNormalSpeed();
		ImGui::Checkbox(("Normal Speed##" + std::to_string(i)).c_str(), &isNormalSpd);
		scenes[i]->SetNormalSpeed(isNormalSpd);
	}

	if (m_sceneMgr.GetGameScene())
	{
		if (!m_sceneMgr.GetGameScene()->IsTraining())
		{
			ImGui::Text("Current Score: %d", m_sceneMgr.GetGameScene()->GetCurrentScore());
			ImGui::Text("Max Score: %d", m_sceneMgr.GetGameScene()->GetMaxScore());
			ImGui::Text("Avg Score: %2f", m_sceneMgr.GetGameScene()->GetAverageScore());
			ImGui::Separator();
		}
		
		ImGui::Text("Epochs: %d/%d", m_sceneMgr.GetGameScene()->GetANN().GetCurrentEpoch(), m_sceneMgr.GetGameScene()->GetANN().GetMaxEpoch());
		ImGui::Text("MSE: %2f (Desired: <= %2f)", m_sceneMgr.GetGameScene()->GetANN().GetCurrentMSE(), m_sceneMgr.GetGameScene()->GetANN().GetDesiredMSE());

		ImGui::Separator();

		ImGui::Text("Scene Speed");
		ImGui::SameLine();
		if (ImGui::PlusButton("Fast Forward"))
		{
			m_sceneMgr.GetGameScene()->SetSceneSpeed(m_sceneMgr.GetGameScene()->GetSceneSpeed() << 1);
		}
		ImGui::SameLine();
		if (ImGui::MinusButton("Slow Down"))
		{
			m_sceneMgr.GetGameScene()->SetSceneSpeed(m_sceneMgr.GetGameScene()->GetSceneSpeed() >> 1);
		}
		ImGui::SameLine();
		ImGui::Text("(X%d)", m_sceneMgr.GetGameScene()->GetSceneSpeed());

		if (ImGui::Button("Reset"))
		{
			m_isConfigSet = false;
			m_sceneMgr.Unload();
		}

		ImGui::SameLine();

		if (m_sceneMgr.GetGameScene() && m_sceneMgr.GetGameScene()->IsTraining() && ImGui::Button("Stop Training"))
		{
			m_sceneMgr.GetGameScene()->PrematureEndTraining();
		}
	}

	ImGui::End();
}

void GUIManager::RenderHistogram()
{
	if (m_sceneMgr.GetGameScene())
	{
		ImGui::Begin("Points Statistics");
		ImVec2 size = ImGui::GetContentRegionAvail();
		const auto & scoreStats = m_sceneMgr.GetGameScene()->GetScoreStatistics();
		if (!scoreStats.empty())
		{
			std::vector<float> fScores(scoreStats.rbegin()->first + 1, 0.f );
			for (auto & score : m_sceneMgr.GetGameScene()->GetScoreStatistics())
				fScores[score.first] = static_cast<float>(score.second);
			ImGui::PlotHistogram("Points", &fScores[0], fScores.size(), 0, NULL, FLT_MIN, FLT_MAX, size, 4);
		}

		ImGui::End();
	}
}

void GUIManager::RenderGameConfig()
{
	RenderOverlay(1.f);

	ImGui::Begin("Training Configuration");
	int sample = static_cast<int>(m_scenConfig.m_samplesCount);
	if (ImGui::InputInt("Sample Count", &sample, 1, 100))
		m_scenConfig.m_samplesCount = static_cast<unsigned>(max(0, sample));
	ImGui::Checkbox("Multi-core training", &m_scenConfig.m_threaded);

	if (ImGui::Button("Start Training"))
	{
		m_scenConfig.m_discreteDT = static_cast<float>(DISCRETE_DT);
		m_sceneMgr.Init(m_scenConfig);
		m_isConfigSet = true;
	}
	ImGui::End();
}