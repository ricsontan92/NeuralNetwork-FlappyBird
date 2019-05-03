#include "AppWindow.h"

#include <string>
#include <chrono>
#include <iostream>

#include "GL\glew.h"
#include "GLFW\glfw3.h"

static const char * TITLE = "Neural Network(Flappy Bird)";

void AppWindow::WindowSizeChangeCallback(GLFWwindow* window, int newWt, int newHt)
{

}

AppWindow::AppWindow() : m_fps(0), m_prevFPS(0)
{
}

bool AppWindow::Init(unsigned winWd, unsigned winHt)
{
	if (!glfwInit()) // Initialization failed
	{
		return false;
	}

	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

	// our framebuffer will consist of 24-bit depthbuffer and double buffered 24-bit RGB color buffer
	glfwWindowHint(GLFW_DEPTH_BITS, 24);
	glfwWindowHint(GLFW_RED_BITS, 8);
	glfwWindowHint(GLFW_GREEN_BITS, 8);
	glfwWindowHint(GLFW_BLUE_BITS, 8);
	glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);

	glfwWindowHint(GLFW_SAMPLES, 4); // set multisample antialiasing sample count to 4

	m_window = glfwCreateWindow(winWd, winHt, TITLE, NULL, NULL);

	if (!m_window) 		// Window or context creation failed
	{
		return false;
	}

	glfwMakeContextCurrent(m_window);	// window context
	glfwSwapInterval(0);				// set 1 for vsync

	glewExperimental = GL_TRUE;
	glewInit();

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glfwSetWindowSizeCallback(m_window, WindowSizeChangeCallback);

	std::cout << "Graphics: " << glGetString(GL_RENDERER) << std::endl;;

	return true;
}

void AppWindow::Update()
{
	if (m_prevFPS != m_fps)
	{
		std::string title = TITLE + std::string("(") + std::to_string(m_fps) + ")";
		glfwSetWindowTitle(m_window, title.c_str());
		m_prevFPS = m_fps;
	}
}

void AppWindow::Shutdown() const
{
	glfwDestroyWindow(m_window);
	glfwTerminate();
}

void AppWindow::PollEvent() const
{
	glfwPollEvents();
}

void AppWindow::SwapBuffers() const
{
	glfwSwapBuffers(m_window);
}

void AppWindow::BeginFrame()
{
	m_timePt = std::chrono::high_resolution_clock::now();
	glfwMakeContextCurrent(m_window);
}

void AppWindow::EndFrame()
{
	double delta = std::chrono::duration_cast<std::chrono::duration<double>>(std::chrono::high_resolution_clock::now() - m_timePt).count();
	m_fps = static_cast<int>(round(1.0 / delta));
}

int AppWindow::GetFPS() const
{
	return m_fps;
}

float AppWindow::GetDeltaTime() const
{
	return 1.f / static_cast<float>(m_fps);
}

bool AppWindow::ShouldClose() const
{
	return glfwWindowShouldClose(m_window) != 0 || glfwGetKey(m_window, GLFW_KEY_ESCAPE) == GLFW_PRESS;
}

GLFWwindow* AppWindow::GetNativeWindow() const
{
	return m_window;
}

void AppWindow::GetFrameBufferSize(int & width, int & height) const
{
	glfwGetFramebufferSize(m_window, &width, &height);
}