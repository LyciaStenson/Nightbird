#pragma once

#include <RenderTarget.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <iostream>

class EditorRenderTarget : public RenderTarget
{
public:
	EditorRenderTarget(GLFWwindow* aWindow);
	~EditorRenderTarget();
	void Init() override;
	void Bind() override;
	void Unbind() override;
	void Render() override;

	void WindowResize(int aWidth, int aHeight) override;

	void SceneWindowResize(int aWidth, int aHeight);

private:
	bool showAboutWindow = false;

	GLFWwindow* window;

	unsigned int framebuffer;
	unsigned int framebufferTexture;
	unsigned int rbo;
};