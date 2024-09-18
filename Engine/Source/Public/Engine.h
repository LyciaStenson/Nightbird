#pragma once

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <flecs/flecs.h>

#include <Shader.h>

//#include <Camera.h>

//#include <Scene.h>

#include <TransformComponent.h>
#include <MeshComponent.h>
#include <CameraComponent.h>

#include <RenderTarget.h>

#include <vector>
#include <filesystem>

class Engine
{
public:
	//Camera camera;

	float lastX = 0.0f;
	float lastY = 0.0f;
	bool mouseDown = false;

	double deltaTime = 0.0;
	double lastFrameTime = 0.0;
	double fps = 0.0;

	Engine(GLFWwindow* aWindow, RenderTarget* aRenderTarget);
	~Engine();

	bool Init();
	void Terminate();
	void MainLoop();

private:
	flecs::world world;

	flecs::system renderSystem;

	flecs::system renderShutdownSystem;
	
	GLFWwindow* m_Window;

	RenderTarget* renderTarget;

	unsigned int framebuffer = 0;

	static void FramebufferSizeCallback(GLFWwindow* window, int width, int height);
	static void CursorEnterCallback(GLFWwindow* window, int entered);
	static void MouseCallback(GLFWwindow* window, double xPos, double yPos);
	static void ScrollCallback(GLFWwindow* window, double xOffset, double yOffset);

	void HandleFramebuffer(int width, int height);
	void HandleCursorEnter();
	void HandleMouse(GLFWwindow* window, double xPos, double yPos);
	void HandleScroll();

	void ProcessInput(GLFWwindow* window);
};