#pragma once

#include <chrono>

struct GLFWwindow;

class AppWindow
{
public:
	AppWindow();

	bool Init(unsigned winWd, unsigned winHt);
	void Update();
	void Shutdown() const;
	void PollEvent() const;
	void SwapBuffers() const;

	void BeginFrame();
	void EndFrame();
	int GetFPS() const;
	float GetDeltaTime() const;

	// checkers
	bool ShouldClose() const;

	// gettors
	GLFWwindow* GetNativeWindow() const;
	void GetFrameBufferSize(int & width, int & height) const;

private:
	static void WindowSizeChangeCallback(GLFWwindow*, int, int);

	std::chrono::steady_clock::time_point m_timePt;
	int m_fps, m_prevFPS;
	GLFWwindow* m_window;
};