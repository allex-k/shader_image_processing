//#define ENTER_PATH_TO_IMAGE
//#define ENTER_PATH_TO_RESULT

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string>

#include "ImageProcessing.hpp"
#include "ConsoleEngine.hpp"

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);

GLint id = 0;
GLfloat value = 0.f;

int nVertices = 5;
GLfloat blurScale = 1.f;
GLfloat gamma = 2.2f;

const int NUM_EFFECTS = 19;

ImageProcessing* imageProcessing;
ConsoleEngine* consoleEngine;

int main() {
	const int windowWidth = 800, windowHeight = 600;

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

	GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, "Shader Image Processing", nullptr, nullptr);
	glfwMakeContextCurrent(window);
	glfwSetKeyCallback(window, key_callback);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glewExperimental = GL_TRUE;
	glewInit();

	std::string resPath = "res\\";
	//std::string srcImagePath = resPath + "image.bmp";
	std::string srcImagePath = resPath + "image1.jpg";
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
	
	std::wstring description[NUM_EFFECTS] = {
		L"saturation",  //0
		L"gamma correction",   //1
		L"brightness",   //2
		L"contrast",  //3
		L"white balance (temperature)",  //4
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
		L"heart blur", //16
		L"bloom (fog glow)", //17
		L"show heart textrure",//18
	};
	const int nRowsConsole = NUM_EFFECTS * 2 + 6;
	const int nColumnsConsole = 68;
	consoleEngine = new ConsoleEngine;
	consoleEngine->ConstructConsole(nRowsConsole, nColumnsConsole, 8, 16);
	
	consoleEngine->DrawString(0, 15, L"Shader Image Processing");
	consoleEngine->set_color(0, 2, 0, nColumnsConsole, 0x003f);
	
	consoleEngine->add_description(description, 2, NUM_EFFECTS);

	consoleEngine->DrawString(NUM_EFFECTS * 2+2, 0, L"Left/A, Right/D, Up, Down, Enter-apply changes, S-save to file", 0x0003);
	consoleEngine->DrawString(NUM_EFFECTS * 2 + 3, 0, L"Z/C-blur scale, Q/E-change namber of vertices in polygon, R-reset", 0x0003);
	consoleEngine->set_title(L"Shader Image Processing");
	consoleEngine->update(id, value, nVertices, blurScale, gamma);
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
		case GLFW_KEY_R: blurScale = 1.f; gamma = 2.2f; break;
		case GLFW_KEY_1: gamma -=0.2f; break;
		case GLFW_KEY_2: gamma += 0.2f; break;

		default:
			break;
		}
		consoleEngine->update(id, value, nVertices, blurScale, gamma);
		consoleEngine->show();

		if (id <= 13) imageProcessing->color_correction(id, value);
		if (id >= 10 && id <= 12) imageProcessing->blur_2_steps(id, value, blurScale, gamma);
		if (id == 13) imageProcessing->sharpness(id, value, gamma);
		if(id == 14) imageProcessing->regular_polygon_blur(id, value, nVertices, blurScale, gamma);
		if (id == 15 || id == 16)imageProcessing->blur_texture_kernel(id, value, nVertices, blurScale, gamma);
		if (id == 17) imageProcessing->bloom(id, value, blurScale, gamma);
		if (id == 18) imageProcessing->draw_blur_texture(id, value, nVertices, blurScale, gamma);

		imageProcessing->show();
	}
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	imageProcessing->resize(width, height);
	imageProcessing->show();
}