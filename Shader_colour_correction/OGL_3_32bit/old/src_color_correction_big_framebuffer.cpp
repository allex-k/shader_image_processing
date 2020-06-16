//öâåòîêîððåêöèÿ
//ðîçì³ð framebuffer = ðîçì³ð çîáðàæåííÿ
//êîðåêö³ÿ íàêîïëþºòüñÿ - íà íàñòóïí³é ³òåðàö³¿ 
//âèêîðèñòîâóºòüñÿ îáðîáëåíå çîáðàæåííÿ
//ç ïîïåðåäíüî¿ ³òåðàö³¿
#include <iostream>
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <SOIL.h>

#include "Shader.h"
//#include"generate_blur_matrix.h"
#include "other_functions.h"
#include "BMPfile.h"

float max(float a, float b) { return (a > b) ? a : b; }
// Function prototypes
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void generate_frame_buffer_and_texture(GLint width, GLint height, GLuint* frameBuffer, GLuint* frameBuffTexture);
GLuint load_texture(std::string path, GLint* width, GLint* height);

// Window dimensions
const GLint SCR_WIDTH = 1200, SCR_HEIGHT = 1000;

// The MAIN function, from here we start the application and run the game loop
int main()
{
	// Init GLFW
	glfwInit();
	// Set all the required options for GLFW
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	// Create a GLFWwindow object that we can use for GLFW's functions
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Shader Colour Correction", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	// Set the required callback functions
	glfwSetKeyCallback(window, key_callback);

	// Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
	glewExperimental = GL_TRUE;
	// Initialize GLEW to setup the OpenGL Function pointers
	glewInit();

	// Define the viewport dimensions
	//glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);


	std::string pathToRes = "D:\\0_visual_studio_projects\\OpenGL_2020\\Shader_colour_correction\\res\\";
	Shader emptyShader(pathToRes + "empty.vs", pathToRes + "empty.fs");
	Shader imageProcessShader(pathToRes + "empty.vs", pathToRes + "color_correction.fs");

	std::string pathToImage = "D:\\_my_TEMP\\images\\images_bmp\\image_800_600_1.bmp";
	const char* pathToImage1 = "F:\\downloads\\1280px-Hawaii_turtle_2.jpg";

	int width, height;
	GLuint imageTextureID = load_texture(pathToImage, &width, &height);
	glViewport(0, 0, width, height);

	//std::string pathToBMPsrc = "D:\\_my_TEMP\\images\\images_bmp\\image_12MP_1.bmp";
	//std::string pathToBMPdest = "D:\\_my_TEMP\\images\\OGL_result.bmp";
	//BMPfile bmpFile(pathToBMPsrc, pathToBMPdest);
	//char* imagePixelsArr = new char[width*height*3];

	float posX = 1.f, posY = 1.f;
	float aspectRatio = ((float)SCR_WIDTH / SCR_HEIGHT) / ((float)width / height); //ñï³ââ³äíîøåííÿ ñï³ââ³äíîøåíü ñòîð³í â³êíà òà çîáðàæåííÿ

	if (aspectRatio < 1.f) { posY = aspectRatio; }
	else { posX = 1.f / aspectRatio; };

	float scaleX = (float)SCR_WIDTH / width;
	float scaleY = (float)SCR_HEIGHT / height;
	// vertices2 äëÿ â³äîáðàæåííÿ íà åêðàí (ïîâîðîò + ìàñøòàá)
	GLfloat vertices2[] = {
		// Positions            // Texture Coords
		 -posX, -posY, 		0.f, 1.f,
		 posX, -posY, 		1.0f, 1.f,
		posX, posY,  		1.f, 0.f,
		-posX,  posY, 		0.f, 0.f
	};
	//äëÿ îáðîáêè çîáðàæåíÿ  áåç ïîâîðîòó, áåç ìàñøòàáóâàííÿ (1 - ç òåêñòóðè â ºêðàííèé áóôåð)
	//float scale=1.0;
	GLfloat vertices[] = {
		// Positions            // Texture Coords
		 -1, -1, 		0.f, 0.f,
		 1, -1,  		1.f, 0.f,
		 1, 1,   		1.f, 1.f,
		-1,  1,  		0.f, 1.f
	};

	GLuint indices[] = {  // Note that we start from 0!
		0, 1, 3, // First Triangle
		1, 2, 3  // Second Triangle
	};

	GLuint VAO, VBO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

	glBindVertexArray(0);

	//-------------------------------------------------------------------------
	GLuint VAO2, VBO2;
	glGenVertexArrays(1, &VAO2);
	glGenBuffers(1, &VBO2);
	//glGenBuffers(1, &EBO1);

	glBindVertexArray(VAO2);
	glBindBuffer(GL_ARRAY_BUFFER, VBO2);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices2), &vertices2, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

	glBindVertexArray(0);


	//################################ generate FrameBuffer and texture ##############################
	// framebuffer configuration
	GLuint frameBuffer1, frameBuffTexture1, frameBuffer2, frameBuffTexture2;
	generate_frame_buffer_and_texture(width, height, &frameBuffer1, &frameBuffTexture1);
	generate_frame_buffer_and_texture(width, height, &frameBuffer2, &frameBuffTexture2);

	GLint id = 2; GLfloat value = 1;
	// Game loop

	std::string description =
		R"(0:  saturation
1:  gamma_correction
2:  brightness
3:  contrast
4:  color_only
5:  hue
6: color separation)";
	std::cout << description << "\n";

	//ç òåêñòóðè frameBuffer1
	glViewport(0, 0, width, height); 
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer1);
	//glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // set clear color to white (not really necessery actually, since we won't be able to see behind the quad anyways)
	glClear(GL_COLOR_BUFFER_BIT);

	emptyShader.Use();
	glBindVertexArray(VAO);
	//glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, imageTextureID);
	glUniform1i(glGetUniformLocation(emptyShader.Program, "Texture"), 0);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	while (!glfwWindowShouldClose(window))
	{

		glViewport(0, 0, width, height); //!!! çì³íà ðàçì³ðó Viewport
		//àäæå ðîçì³ð frameFuffer1 â³äð³çíÿºòüñÿ â³ä ðîçì³ðó â³êíà (ñòàíäàðòíîãî êàäðîâîãî áóôåðà)
		//frameFuffer1,2- ðîçì³ð çîáðàæåííÿ, ñòàíëàðòíèé - ðîçì³ð â³êíà
		// Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions

		// Ç FrameFuffer1 ó FrameFuffer2
		glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer2);

		glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		imageProcessShader.Use();

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, frameBuffTexture1);
		glUniform1i(glGetUniformLocation(imageProcessShader.Program, "Texture"), 0);
		glUniform1i(glGetUniformLocation(imageProcessShader.Program, "id"), id);
		glUniform1f(glGetUniformLocation(imageProcessShader.Program, "value"), value);	// Draw
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		//----------------------ó ôàéë-------------------------------
		//glReadPixels(0, 0, width, height, GL_BGR_EXT, GL_UNSIGNED_BYTE, imagePixelsArr);
		//bmpFile.save_to_file(imagePixelsArr);

		//ç frameFuffer2  íà åêðàí
		glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		//glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // set clear color to white (not really necessery actually, since we won't be able to see behind the quad anyways)
		glClear(GL_COLOR_BUFFER_BIT);

		emptyShader.Use();
		glBindVertexArray(VAO2);
		//glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, frameBuffTexture2);
		glUniform1i(glGetUniformLocation(emptyShader.Program, "Texture"), 0);

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		// Swap the screen buffers
		glfwSwapBuffers(window);

		std::cout << "Enter id: ";
		std::cin >> id;
		std::cout << "Enter value: ";
		std::cin >> value;
		std::cout << "\n";

		//swap framebuffers
		GLuint temp = frameBuffer1;
		frameBuffer1 = frameBuffer2;
		frameBuffer2 = temp;

		temp = frameBuffTexture1;
		frameBuffTexture1 = frameBuffTexture2;
		frameBuffTexture2 = temp;
	}
	// Properly de-allocate all resources once they've outlived their purpose
	glDeleteVertexArrays(1, &VAO); glDeleteVertexArrays(1, &VAO2);
	glDeleteBuffers(1, &VBO); glDeleteBuffers(1, &VBO2);
	glDeleteBuffers(1, &EBO);
	// Terminate GLFW, clearing any resources allocated by GLFW.
	glfwTerminate();
	return 0;
}

//âñïîìîãàòåëüíûå ôóíêöèè
void generate_frame_buffer_and_texture(GLint width, GLint height, GLuint* frameBuffer, GLuint* frameBuffTexture) {
	glGenFramebuffers(1, frameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, *frameBuffer);
	// create a color attachment texture

	glGenTextures(1, frameBuffTexture);
	glBindTexture(GL_TEXTURE_2D, *frameBuffTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);	// Set texture wrapping to GL_REPEAT
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, *frameBuffTexture, 0);


	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

GLuint load_texture(std::string path, GLint* width, GLint* height) {
	unsigned char* data = SOIL_load_image(path.c_str(), width, height, 0, SOIL_LOAD_RGB);

	GLuint textureID;
	// ====================
	// Texture 1
	// ====================
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID); // All upcoming GL_TEXTURE_2D operations now have effect on our texture object
	// Set our texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);	// Set texture wrapping to GL_REPEAT
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	// Set texture filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// Load, create texture and generate mipmaps

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, *width, *height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);
	SOIL_free_image_data(data);
	glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture when done, so we won't accidentily mess up our texture.
	return textureID;
}

// Is called whenever a key is pressed/released via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
}
