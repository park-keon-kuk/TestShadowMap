#include <gl/glew.h>
#include <GLFW/glfw3.h>
#include <glm/gtx/transform.hpp>
#include <cstdio>
#include "GLObject.h"

#ifdef _DEBUG
#include <cstdlib>
#include <crtdbg.h>
#endif

int g_width = 1200;
int g_height = 800;
float g_aspect = (float)g_width / (float)g_height;
bool g_pause = false;

void initContext(bool useDefault, int major = 3, int minor = 3, bool useCompatibility = false);
void framebufferSizeCallback(GLFWwindow*, int w, int h);
void mousebuttonCallback(GLFWwindow*, int btn, int act, int);

// for shadow map

int main()
{
#ifdef _DEBUG
	_CrtDumpMemoryLeaks();
	_CrtSetDbgFlag(0x21);
#endif

	/* 초기화, 에러 핸들링 등록, 이벤트 콜백 등록, OpenGL 초기화 */
	/* -------------------------------------------------------------------------------------- */
	glfwInit();
	glfwSetErrorCallback([](int err, const char* desc) { puts(desc); });
	initContext(/*use dafault = */ true);
	GLFWwindow *window = glfwCreateWindow(g_width, g_height, "Order Independent Transparency Rendering!", nullptr, nullptr);
	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
	glfwSetMouseButtonCallback(window, mousebuttonCallback);
	glfwMakeContextCurrent(window);
	glewInit();

	/* 객체 생성 및 초기화 */
	/* -------------------------------------------------------------------------------------- */
	QuadRenderer * qr = new QuadRenderer(2, 3);

	/* 객체 생성 및 초기화 검사 */
	/* -------------------------------------------------------------------------------------- */
	printAllErrors("객체 생성 및 초기화 검사");

	/* 메인 루프 */
	/* -------------------------------------------------------------------------------------- */
	while (!glfwWindowShouldClose(window))
	{
		if (g_pause) {
			// 이벤트 폴
			glfwPollEvents();
			continue;
		}

		glViewport(0, 0, g_width, g_height);
		qr->use();
		qr->setBorderColor(1, 0, 0);
		for(int r = 0; r < 2; r++)
			for(int c = 0; c < 3; c++)
				qr->render(r, c, 0);
		qr->unuse();

		// 버퍼 스왑, 이벤트 폴
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	/* 루프 종료 검사 */
	/* -------------------------------------------------------------------------------------- */
	printAllErrors("루프 종료 검사");

	/* 객체 제거 */
	/* -------------------------------------------------------------------------------------- */
	delete qr;

	/* 객체 제거 검사 */
	/* -------------------------------------------------------------------------------------- */
	printAllErrors("객체 제거 검사");

	/* 종료 */
	/* -------------------------------------------------------------------------------------- */
	glfwTerminate();
}

void initContext(bool useDefault, int major, int minor, bool useCompatibility)
{
	if (useDefault)
	{
		// 기본적으로  Default로 설정해주면
		// 가능한 최신의 OpenGL Vesion을 선택하며 (연구실 컴퓨터는 4.5)
		// Profile은 Legacy Function도 사용할 수 있게 해줍니다.(Compatibility 사용)
		glfwDefaultWindowHints();
	}
	else
	{
		// Major 와 Minor 결정
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, major);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, minor);

		// Core 또는 Compatibility 선택
		if (useCompatibility)
		{
			glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
		}
		else
		{
			glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		}
	}
}

void framebufferSizeCallback(GLFWwindow*, int w, int h)
{
	g_width = w;
	g_height = h;
	g_aspect = (float)g_width / (float)g_height;
	printf("%d, %d\n", w, h);

	g_pause = false;
	puts("start !");
}

void mousebuttonCallback(GLFWwindow*, int button, int action, int mods)
{
	if (action == GLFW_PRESS) {
		if (button == GLFW_MOUSE_BUTTON_LEFT) {
		}
		else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
		}
		else {
			g_pause = !g_pause;
			puts(g_pause ? "pause !" : "start !");
		}

	}
}
