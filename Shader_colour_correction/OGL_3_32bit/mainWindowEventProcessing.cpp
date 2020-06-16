//#define ENTER_PATH_TO_IMAGE
//#define ENTER_PATH_TO_RESULT

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string>

#include "ImageProcessing.hpp"
#include "ConsoleEngine.hpp"

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

GLint id = 0;
GLfloat value = 0.f;

int nVertices = 5;
GLfloat blurScale = 1.f;

const int NUM_EFFECTS = 16;

ImageProcessing* imageProcessing;
ConsoleEngine* consoleEngine;

int main() {
	const int windowWidth = 800, windowHeight = 600;

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, "Shader Image Processing", nullptr, nullptr);
	glfwMakeContextCurrent(window);
	glfwSetKeyCallback(window, key_callback);
	glewExperimental = GL_TRUE;
	glewInit();

	std::string resPath = "res\\";
	//std::string srcImagePath = resPath + "image.bmp";
	std::string srcImagePath = resPath + "image.bmp";
	std::string destImagePath = resPath + "result.bmp";

#ifdef ENTER_PATH_TO_IMAGE
	std::cout << "Enter path to image: \n";
	std::getline(std::cin, srcImagePath);
	std::cout << srcImagePath << std::endl;
#endif

#ifdef ENTER_PATH_TO_RESULT 
	std::cout << "Enter path to result(*.bmp): \n";
	std::getline(std::cin, destImagePath);
	std::cout << destImagePath << std::endl;
#endif

	imageProcessing = new ImageProcessing(window, windowWidth, windowHeight, srcImagePath, destImagePath);
	//imageProcessing->print();
	imageProcessing->color_correction(id, value);
	imageProcessing->show();

	
	//Sleep(3000); //щоб побачити повідомлення про помилки
	consoleEngine = new ConsoleEngine;
	consoleEngine->ConstructConsole(60, NUM_EFFECTS*2, 8, 16);
	std::wstring description[NUM_EFFECTS] = {
		L"saturation",  //0
		L"gamma correction",   //1
		L"brightness",   //2
		L"contrast",  //3
		L"temperature",  //4
		L"hue",  //5
		L"chromatic abberation (dispersion)",  //6
		L"color_only", // 7
		L"rgg", //8
		L"rbb", //9
		L"gaussion blur", //10
		L"blur (parabola)", //11
		L"blur (square)", //12
		L"sharpness", //13
		L"blur regular polygon" ,//14
		L"blur regular polygon texture", //15
	};
	consoleEngine->add_description(description, 0, NUM_EFFECTS);
	consoleEngine->update(id, value);
	consoleEngine->show();

	while (!glfwWindowShouldClose(window))
	{
		glfwWaitEvents();
		//imageProcessing.draw_color_correction(id, value);
		//std::cout << "id = " << id << std::endl << "value = " << value << std::endl;
	}
	glfwTerminate();
	return 0;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	/*if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (key == GLFW_KEY_ENTER && action == GLFW_PRESS) {
		imageProcessing->apply_changes(); return;
	}
	if (key == GLFW_KEY_S && action == GLFW_PRESS) {
		imageProcessing->save_to_file(); return;
	}*/

	if (action == GLFW_PRESS || action == GLFW_REPEAT)
	{
		switch (key)
		{
		case GLFW_KEY_ESCAPE: glfwSetWindowShouldClose(window, GL_TRUE); break;
		case GLFW_KEY_ENTER: imageProcessing->apply_changes(); return; break;
		case GLFW_KEY_S: imageProcessing->save_to_file(); return;  break;

		case GLFW_KEY_UP: value = 0; if(id>0) --id;  break;
		case GLFW_KEY_DOWN: value = 0; if(id<NUM_EFFECTS-1) ++id; break;
		case GLFW_KEY_LEFT: value -= 0.1; break;
		case GLFW_KEY_RIGHT: value += 0.1; break;
		case GLFW_KEY_A: value -= 0.01; break;
		case GLFW_KEY_D: value += 0.01; break;

		case GLFW_KEY_Q: --nVertices; break;
		case GLFW_KEY_E: ++nVertices; break;
		case GLFW_KEY_Z: blurScale -= 0.5f; break;
		case GLFW_KEY_C: blurScale += 0.5f; break;
		case GLFW_KEY_R: blurScale = 1.f; break;

		default:
			break;
		}
		consoleEngine->update(id, value);
		consoleEngine->show();

		if (id <= 13) imageProcessing->color_correction(id, value);
		if (id >= 10 && id <= 12) imageProcessing->blur_2_steps(id, value, blurScale);
		if (id == 13) imageProcessing->sharpness(id, value);
		if(id == 14) imageProcessing->regular_polygon_blur(id, value, nVertices, blurScale);
		if (id == 15) imageProcessing->regular_polygon_blur_texture(id, value, nVertices, blurScale);

		imageProcessing->show();
	}
}
