#include <iostream>

// GLEW
#define GLEW_STATIC
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

// Other Libs
#include <SOIL.h>

#include <thread>

// Other includes
#include "Shader.h"

float max(float a, float b) {return (a > b) ? a:b; }

// Function prototypes
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

// Window dimensions
const GLuint WIDTH = 1000, HEIGHT = 600;
GLint id = 2; GLfloat value = 1.;
// The MAIN function, from here we start the application and run the game loop
void console_input(){
	std::string description =
		R"(0:  saturation
1:  gamma_correction
2:  brightness
3:  contrast
4:  color_only
5:  hue
6: color separation
7: tritanopia)";
	std::cout << description << "\n";
	while (true) {
		std::cout << "Enter id: ";
		std::cin >> id;
		std::cout << "Enter value: ";
		std::cin >> value;
		std::cout << "\n";
	}

};


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
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Shader Colour Correction", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	// Set the required callback functions
	glfwSetKeyCallback(window, key_callback);

	// Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
	glewExperimental = GL_TRUE;
	// Initialize GLEW to setup the OpenGL Function pointers
	glewInit();

	// Define the viewport dimensions
	glViewport(0, 0, WIDTH, HEIGHT);


	// Build and compile our shader program
	Shader imageProcessShader("D:\\0_visual_studio_projects\\OpenGL_2020\\Shader_colour_correction\\res\\empty.vs", "D:\\0_visual_studio_projects\\OpenGL_2020\\Shader_colour_correction\\res\\color_correction.fs");
	int width, height;
	const char* pathToImage = "D:\\_my_TEMP\\images\\images_jpg\\IMG_20190608_135136.jpg";
	//const char* pathToImage = "F:\\downloads\\1280px-Hawaii_turtle_2.jpg";
	unsigned char* image = SOIL_load_image(pathToImage, &width, &height, 0, SOIL_LOAD_RGB);

	float posX=1.f, posY=1.f;
	float aspectRatio = ((float)WIDTH / HEIGHT) / ((float)width / height); //співвідношення співвідношень сторін вікна та зображення
	
	if (aspectRatio < 1.f) {posY = aspectRatio; }
	else {posX = 1.f/aspectRatio; };

	// Set up vertex data (and buffer(s)) and attribute pointers


	GLfloat vertices[] = {
		// Positions            // Texture Coords
		 -posX, -posY, 		0.f, 1.f,
		 posX, -posY,  		1.0f, 1.f,
		 posX, posY,   		1.f, 0.f,
		-posX,  posY,  		0.f, 0.f
	};

	GLuint indices[] = {  // Note that we start from 0!
		0, 1, 3, // First Triangle
		1, 2, 3  // Second Triangle
	};
	GLuint VBO, VAO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// Position attribute
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	
	// TexCoord attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)(2 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0); // Unbind VAO


	// Load and create a texture 
	GLuint texture;
	// ====================
	// Texture 1
	// ====================
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture); // All upcoming GL_TEXTURE_2D operations now have effect on our texture object
	// Set our texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// Set texture wrapping to GL_REPEAT
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// Set texture filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// Load, create texture and generate mipmaps
	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture when done, so we won't accidentily mess up our texture.
	
	// Game loop
	std::thread thr(console_input);
	while (!glfwWindowShouldClose(window))
	{
		// Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
		glfwPollEvents();

		// Render
		// Clear the colorbuffer
		glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// Activate shader
		imageProcessShader.Use();

		// Bind Textures using texture units
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);
		glUniform1i(glGetUniformLocation(imageProcessShader.Program, "Texture"), 0);
		
		glUniform1i(glGetUniformLocation(imageProcessShader.Program, "id"), id);
		glUniform1f(glGetUniformLocation(imageProcessShader.Program, "value"), value);
		

		// Draw
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		// Swap the screen buffers
		glfwSwapBuffers(window);

		/*std::cout << "Enter id: ";
		std::cin >> id;
		std::cout << "Enter value: ";
		std::cin >> value;
		std::cout << "\n";*/
	}
	// Properly de-allocate all resources once they've outlived their purpose
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	// Terminate GLFW, clearing any resources allocated by GLFW.
	glfwTerminate();
	return 0;
}

// Is called whenever a key is pressed/released via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
}