//блюр в два проходи (по горизонталі, по вертикалі)
//розмір framebuffer = розмір вікна
#include <iostream>

// GLEW
#define GLEW_STATIC
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

// Other Libs
#include <SOIL.h>

// Other includes
#include "Shader.h"

float max(float a, float b) { return (a > b) ? a : b; }
// Function prototypes
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void generate_frame_buffer_and_texture(GLint width, GLint height, GLuint* frameBuffer, GLuint* frameBuffTexture);
GLuint load_texture(std::string path, GLint* width, GLint* height);

// Window dimensions
const GLint SCR_WIDTH = 1200, SCR_HEIGHT = 800;

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
	glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

	std::string pathToRes = "D:\\0_visual_studio_projects\\OpenGL_2020\\Shader_colour_correction\\res\\";
	Shader emptyShader(pathToRes+"empty.vs", pathToRes+"empty.fs");
	Shader imageProcessShader(pathToRes+"empty.vs", pathToRes+"blur2.fs");

	std::string pathToImage = "D:\\_my_TEMP\\images\\images_jpg\\IMG_20190608_135242.jpg";
	const char* pathToImage1 = "F:\\downloads\\1280px-Hawaii_turtle_2.jpg";
	
	int width, height;	
	GLuint imageTextureID = load_texture(pathToImage, &width, &height);

	float posX = 1.f, posY = 1.f;
	float aspectRatio = ((float)SCR_WIDTH / SCR_HEIGHT) / ((float)width / height); //співвідношення співвідношень сторін вікна та зображення

	if (aspectRatio < 1.f) { posY = aspectRatio; }
	else { posX = 1.f / aspectRatio; };

	// vertices1 для завантаження зображення в екранний буфер (поворот + масштаб)
	GLfloat vertices1[] = {
		// Positions            // Texture Coords
		 -posX, -posY, 		0.f, 1.f,
		 posX, -posY, 		1.0f, 1.f,
		 posX, posY,  		1.f, 0.f,
		-posX,  posY, 		0.f, 0.f
	};
	//для обробки зображеня  без повороту, без масштабування
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
	//----------------------------------------------------------------------
	GLuint VAO1, VBO1;
	glGenVertexArrays(1, &VAO1);
	glGenBuffers(1, &VBO1);
	//glGenBuffers(1, &EBO1);

	glBindVertexArray(VAO1);
	glBindBuffer(GL_ARRAY_BUFFER, VBO1);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices1), &vertices1, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

	glBindVertexArray(0);
	
	
	//################################ generate FrameBuffer and texture ##############################
	// framebuffer configuration
	GLuint frameBuffer, frameBuffTexture;
	generate_frame_buffer_and_texture(SCR_WIDTH, SCR_HEIGHT, &frameBuffer, &frameBuffTexture);

	GLint SIZE = 10;

	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
		glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
		glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		imageProcessShader.Use();

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, imageTextureID);
		glUniform1i(glGetUniformLocation(imageProcessShader.Program, "Texture"), 0);
		glUniform2f(glGetUniformLocation(imageProcessShader.Program, "direction"), 1.0 / SCR_WIDTH, 0.0);
		glUniform1i(glGetUniformLocation(imageProcessShader.Program, "SIZE"), SIZE);

		// Draw
		glBindVertexArray(VAO1);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		//----------------------------------------------------------------------------
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		//glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // set clear color to white (not really necessery actually, since we won't be able to see behind the quad anyways)
		glClear(GL_COLOR_BUFFER_BIT);

		imageProcessShader.Use();
		glBindVertexArray(VAO);
		//glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, frameBuffTexture);
		glUniform1i(glGetUniformLocation(imageProcessShader.Program, "Texture"), 0);
		glUniform2f(glGetUniformLocation(imageProcessShader.Program, "direction"), 0.0, 1.0/SCR_HEIGHT);
		glUniform1i(glGetUniformLocation(imageProcessShader.Program, "SIZE"), SIZE);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		// Swap the screen buffers
		glfwSwapBuffers(window);


		std::cout << "Enter SIZE: ";
		std::cin >> SIZE;

		std::cout << "\n";
	}
	// Properly de-allocate all resources once they've outlived their purpose
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	// Terminate GLFW, clearing any resources allocated by GLFW.
	glfwTerminate();
	return 0;
}

//вспомогательные функции
void generate_frame_buffer_and_texture(GLint width, GLint height, GLuint* frameBuffer, GLuint* frameBuffTexture) {
	glGenFramebuffers(1, frameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, *frameBuffer);
	// create a color attachment texture

	glGenTextures(1, frameBuffTexture);
	glBindTexture(GL_TEXTURE_2D, *frameBuffTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);	// Set texture wrapping to GL_REPEAT
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
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
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);	// Set texture wrapping to GL_REPEAT
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
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