//блюр в два проходи (по горизонталі, по вертикалі)
//розмір framebuffer = розмір зображення

#include <iostream>
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <SOIL.h>

#include "Shader.h"
#include"generate_blur_matrix.h"
#include "other_functions.h"

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
	Shader imageProcessShader(pathToRes + "empty.vs", pathToRes + "blur2.fs");

	std::string pathToImage = "D:\\_my_TEMP\\images\\images_jpg\\IMG_20190608_135242.jpg";
	const char* pathToImage1 = "F:\\downloads\\1280px-Hawaii_turtle_2.jpg";

	int width, height;
	GLuint imageTextureID = load_texture(pathToImage, &width, &height);
	glViewport(0, 0, width, height);

	float posX = 1.f, posY = 1.f;
	float aspectRatio = ((float)SCR_WIDTH / SCR_HEIGHT) / ((float)width / height); //співвідношення співвідношень сторін вікна та зображення

	if (aspectRatio < 1.f) { posY = aspectRatio; }
	else { posX = 1.f / aspectRatio; };

	float scaleX = (float)SCR_WIDTH / width;
	float scaleY = (float)SCR_HEIGHT / height;
	// vertices2 для відображення на екран (поворот + масштаб)
	GLfloat vertices2[] = {
		// Positions            // Texture Coords
		 -posX, -posY, 		0.f, 1.f,
		 posX, -posY, 		1.0f, 1.f,
		posX, posY,  		1.f, 0.f,
		-posX,  posY, 		0.f, 0.f
	};
	//для обробки зображеня  без повороту, без масштабування (1 - з текстури в єкранний буфер)
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

	const int MAX_SIZE = 512;
	float coeffVector[MAX_SIZE];
	GLint SIZE = 10;
	while (!glfwWindowShouldClose(window))
	{
		
		glViewport(0, 0, width, height); //!!! зміна разміру Viewport
		//адже розмір frameFuffer1 відрізняється від розміру вікна (стандартного кадрового буфера)
		//frameFuffer1,2- розмір зображення, станлартний - розмір вікна
		// Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
		genenate_gaussian_blur_vector(coeffVector, SIZE);
		print_array(coeffVector, SIZE);

		// З imageTexture у FrameFuffer1
		glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer1);

		// Render
		glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Activate shader
		imageProcessShader.Use();

		// З imageTexture у FrameFuffer1
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, imageTextureID);
		glUniform1i(glGetUniformLocation(imageProcessShader.Program, "Texture"), 0);
		glUniform2f(glGetUniformLocation(imageProcessShader.Program, "direction"), 1.0 / width, 0.0);
		glUniform1i(glGetUniformLocation(imageProcessShader.Program, "SIZE"), SIZE);
		glUniform1fv(glGetUniformLocation(imageProcessShader.Program, "coeffVector"), SIZE, coeffVector);
		// Draw
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		//----------------------------------------------------------------------------
		// З frameFuffer1 у frameFuffer2
		glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer2);

		// Render
		glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Activate shader
		imageProcessShader.Use();

		// З imageTexture у FrameFuffer1
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, frameBuffTexture1);
		glUniform1i(glGetUniformLocation(imageProcessShader.Program, "Texture"), 0);
		glUniform2f(glGetUniformLocation(imageProcessShader.Program, "direction"), 0.0, 1.0/height);
		glUniform1i(glGetUniformLocation(imageProcessShader.Program, "SIZE"), SIZE);
		glUniform1fv(glGetUniformLocation(imageProcessShader.Program, "coeffVector"), SIZE, coeffVector);
		// Draw
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		//з frameFuffer2 у вікно
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

		std::cout << "Enter SIZE: ";
		std::cin >> SIZE;

		std::cout << "\n";
		glfwPollEvents();
		
	}
	// Properly de-allocate all resources once they've outlived their purpose
	glDeleteVertexArrays(1, &VAO); glDeleteVertexArrays(1, &VAO2);
	glDeleteBuffers(1, &VBO); glDeleteBuffers(1, &VBO2);
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