#include <gl/glew.h>
#include <GLFW/glfw3.h>
#include <glm/gtx/transform.hpp>
#include <cstdio>
#include "GLObject.h"

#ifdef _DEBUG
#include <cstdlib>
#include <crtdbg.h>
#endif

int g_width = 900;
int g_height = 900;
float g_aspect = (float)g_width / (float)g_height;
bool g_pause = false;

void initContext(bool useDefault, int major = 3, int minor = 3, bool useCompatibility = false);
void framebufferSizeCallback(GLFWwindow*, int w, int h);
void mousebuttonCallback(GLFWwindow*, int btn, int act, int);

class Scene
{
	// objects
	VAO cubeVAO;
	FBO shadowFBO;
	Shader shadowShader;
	Shader renderShader;

	// renderer
	QuadRenderer qr;

	// values
	float angle;
	glm::vec3 lightPos;
	glm::mat4 lightPMat; 
	glm::mat4 lightVMat; 

	/********************************************/
	/*											*/
	// main()에서 불리는 함수
	/*											*/
	/********************************************/

public:
	bool create() {
		qr.create(5, 5);

		do {
			if (!cubeVAO.load("C:/users/pkk11/onedrive/objects/cube.obj")) break;
			if (!shadowFBO.create(2048, 2048, 1, true)) break;
			if (!shadowShader.load("resources/shaders/shadow")) break;
			if (!renderShader.load("resources/shaders/render")) break;

			return true;

		} while (false);

		return false;
	}

	void render() {
		// first precess
		angle = (float)glfwGetTime();
		lightPos = glm::vec3(cos(angle), 1, sin(angle));
		lightPMat = glm::ortho(-4.f, 4.f, -4.f, 4.f, -4.f, 4.f);
		lightVMat = glm::lookAt(lightPos, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
		makeShadowMap();

		// second process
		glViewport(0, 0, g_width, g_height);
		glClearColor(1.f, 1.f, 1.f, 1.f);
		glEnable(GL_DEPTH_TEST);
		glClearDepth(1.f);
		glDepthFunc(GL_LESS);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// light에서 본 모습과 shadow map
		qr.use();
		qr.render(0, 3, shadowFBO.getDepthTex());
		qr.render(0, 4, shadowFBO.getColorTex());
		qr.unuse();

		// shadow가 적용된 렌더링
		renderShader.use();
		
		glm::mat4 pmat = glm::perspective(45.f, g_aspect, 0.1f, 100.f);
		glm::mat4 vmat = glm::lookAt(glm::vec3(0, 3, 3), { 0, 0, 0 }, { 0, 1, 0 });
		glUniformMatrix4fv(0, 1, GL_FALSE, &pmat[0][0]);
		glUniformMatrix4fv(1, 1, GL_FALSE, &vmat[0][0]);
		glUniformMatrix4fv(3, 1, GL_FALSE, &lightPMat[0][0]);
		glUniformMatrix4fv(4, 1, GL_FALSE, &lightVMat[0][0]);
		
		shadowFBO.bindDepthTexture();
		
		renderScene();

		renderShader.unuse();
	}

	Scene() = default;
	~Scene() = default;

private:
	/********************************************/
	/*											*/
	// 추가 함수
	/*											*/
	/********************************************/

	void makeShadowMap() {
		shadowFBO.bind();
		glClearColor(0, 0, 0, 1.f);
		glEnable(GL_DEPTH_TEST);
		glClearDepth(1.f);
		glDepthFunc(GL_LESS);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shadowShader.use();
		glUniformMatrix4fv(0, 1, GL_FALSE, &lightPMat[0][0]);
		glUniformMatrix4fv(1, 1, GL_FALSE, &lightVMat[0][0]);

		renderScene();

		shadowShader.unuse();
		shadowFBO.unbind();
	}

	void renderScene() {
		glm::mat4 cubeMMat1 = glm::mat4(1.f);
		cubeMMat1 *= glm::translate(glm::vec3(0.f, 0.25f, 0.f));
		glUniformMatrix4fv(2, 1, GL_FALSE, &cubeMMat1[0][0]);
		cubeVAO.render_once();

		glm::mat4 cubeMMat2 = glm::mat4(1.f);
		cubeMMat2 *= glm::translate(glm::vec3(0.f, -1.f, 0.f));
		cubeMMat2 *= glm::scale(glm::vec3(2.f, 2.f, 2.f));
		glUniformMatrix4fv(2, 1, GL_FALSE, &cubeMMat2[0][0]);
		cubeVAO.render_once();
	}
};

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
	auto scene = new Scene();

	if (scene->create()) {
		puts("객체 생성 성공!");
	}
	else {
		puts("객체 생성 실패");
		delete scene;
		return -1;
	}

	/* 객체 생성 및 초기화 검사 */
	/* -------------------------------------------------------------------------------------- */
	printAllErrors("객체 생성 및 초기화 검사");

	/* 메인 루프 */
	/* -------------------------------------------------------------------------------------- */
	while (!glfwWindowShouldClose(window))
	{
		if (!g_pause) {
			// 렌더링
			scene->render();
			// 버퍼 스왑, 이벤트 폴
			glfwSwapBuffers(window);
		}

		glfwPollEvents();
	}

	/* 루프 종료 검사 */
	/* -------------------------------------------------------------------------------------- */
	printAllErrors("루프 종료 검사");

	/* 객체 제거 */
	/* -------------------------------------------------------------------------------------- */
	delete scene;

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

	if (g_pause) {
		g_pause = false;
		puts("start !");
	}
}

void mousebuttonCallback(GLFWwindow*, int button, int action, int mods)
{
	if (action == GLFW_PRESS) {
		if (button == GLFW_MOUSE_BUTTON_LEFT) {
			g_pause = !g_pause;
			puts(g_pause ? "pause !" : "start !");
		}
		else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
		}
		else {
			g_pause = !g_pause;
			puts(g_pause ? "pause !" : "start !");
		}
	}
}
