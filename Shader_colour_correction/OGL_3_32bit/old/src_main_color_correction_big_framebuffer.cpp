//öâåòîêîððåêöèÿ
//ðîçì³ð framebuffer = ðîçì³ð çîáðàæåííÿ
//êîðåêö³ÿ íàêîïëþºòüñÿ - íà íàñòóïí³é ³òåðàö³¿ 
//âèêîðèñòîâóºòüñÿ îáðîáëåíå çîáðàæåííÿ
//ç ïîïåðåäíüî¿ ³òåðàö³¿
//#define ENTER_PATH_TO_IMAGE
//#define ENTER_PATH_TO_RESULT

#include <iostream>
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
//#include <SOIL.h>
//#include <windows.h>
#include "Shader.h"
//#include"generate_blur_matrix.h"
#include "other_functions.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

float max(float a, float b) { return (a > b) ? a : b; }
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void generate_frame_buffer_and_texture(GLint imageWidth, GLint imageHeight, GLuint* frameBuffer, GLuint* frameBuffTexture);
GLuint load_texture(const std::string path, GLint* imageWidth, GLint* imageHeight, int* nrComponents);
void save_image_to_file(std::string path, int width, int height, int nrComponents, const void *data);

GLenum nComponentsToGLConst[5] = { 0, GL_RED , GL_RG, GL_RGB, GL_RGBA };

// The MAIN function, from here we start the application and run the game loop
int main()
{
	const GLint WINDOW_WIDTH = 800, WINDOW_HEIGHT = 500;

	std::string resPath = "res\\";
	std::string srcImagePath = resPath + "image.jpg";
	std::string destImagePath = resPath + "result.bmp";

#ifdef ENTER_PATH_TO_IMAGE
	std::cout << "Enter path to image: \n";
	std::getline(std::cin, srcImagePath);
	std::cout << srcImagePath<<std::endl;
#endif
	
#ifdef ENTER_PATH_TO_RESULT 
	std::cout << "Enter path to result(*.bmp): \n";
	std::getline(std::cin, destImagePath);
	std::cout << destImagePath<<std::endl;
#endif
	
	/*TCHAR NPath[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, NPath);
	std::cout << NPath;*/

	// Init GLFW
	glfwInit();
	// Set all the required options for GLFW
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	// Create a GLFWwindow object that we can use for GLFW's functions
	GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Shader Colour Correction", nullptr, nullptr);
	glfwMakeContextCurrent(window);
	glfwSetKeyCallback(window, key_callback);
	glewExperimental = GL_TRUE;
	glewInit();

	Shader emptyShader(resPath + "empty.vs", resPath + "empty.fs");
	Shader imageProcessShader(resPath + "empty.vs", resPath + "color_correction.fs");

	int imageWidth, imageHeight, nrComponents;
	GLuint imageTextureID = load_texture(srcImagePath, &imageWidth, &imageHeight, &nrComponents);
	std::cout << "imageWidth = " << imageWidth << "\nimageHeight = " << imageHeight << std::endl;
	glViewport(0, 0, imageWidth, imageHeight);

	uint8_t* imagePixelsArr = new uint8_t[imageWidth*imageHeight*nrComponents];

	float posX = 1.f, posY = 1.f;
	float aspectRatio = ((float)WINDOW_WIDTH / WINDOW_HEIGHT) / ((float)imageWidth / imageHeight); //ñï³ââ³äíîøåííÿ ñï³ââ³äíîøåíü ñòîð³í â³êíà òà çîáðàæåííÿ

	if (aspectRatio < 1.f) { posY = aspectRatio; }
	else { posX = 1.f / aspectRatio; };

	float scaleX = (float)WINDOW_WIDTH / imageWidth;
	float scaleY = (float)WINDOW_HEIGHT / imageHeight;
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

	GLuint VAO1, VBO1, EBO1;
	glGenVertexArrays(1, &VAO1);
	glGenBuffers(1, &VBO1);
	glGenBuffers(1, &EBO1);

	glBindVertexArray(VAO1);
	glBindBuffer(GL_ARRAY_BUFFER, VBO1);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO1);
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

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO1);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

	glBindVertexArray(0);


	//################################ generate FrameBuffer and texture ##############################
	GLuint frameBuffer1, frameBuffer2, frameBuffTexture1, frameBuffTexture2;
	generate_frame_buffer_and_texture(imageWidth, imageHeight, &frameBuffer1, &frameBuffTexture1);
	generate_frame_buffer_and_texture(imageWidth, imageHeight, &frameBuffer2, &frameBuffTexture2);

	GLint id = 1; GLfloat value = 1;

	std::string description =
		R"(0: saturation
1: gamma_correction
2: brightness
3: contrast
4: color_only
5: hue
6: dispersion(chromatic aberration)
7: rgg
8: rbb
9: temperature
88: undo)";
	std::cout << description << "\n";

	//Ðåíäåð ïî÷àòêîâîãî çîáðàæåííÿ ç òåêñòóðè ó frameBuffer1 
	glViewport(0, 0, imageWidth, imageHeight);//!!! çì³íà ðàçì³ðó Viewport
	//àäæå ðîçì³ð frameFuffer1,2 â³äð³çíÿºòüñÿ â³ä ðîçì³ðó â³êíà (ñòàíäàðòíîãî êàäðîâîãî áóôåðà)
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer1);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	emptyShader.Use();
	glBindVertexArray(VAO1);
	glBindTexture(GL_TEXTURE_2D, imageTextureID);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	while (!glfwWindowShouldClose(window))
	{
		//############################# ç frameFuffer1 ó frameFuffer2 ##################################
		glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer2);

		imageProcessShader.Use();
		glBindTexture(GL_TEXTURE_2D, frameBuffTexture1);
		glUniform1i(glGetUniformLocation(imageProcessShader.Program, "id"), id);
		glUniform1f(glGetUniformLocation(imageProcessShader.Program, "value"), value);	// Draw
		glBindVertexArray(VAO1);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		//############################# ç frameFuffer2  íà åêðàí #######################################
		glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		emptyShader.Use();
		glBindTexture(GL_TEXTURE_2D, frameBuffTexture2);
		glBindVertexArray(VAO2);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		GLsync fence = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
		glClientWaitSync(fence, GL_SYNC_FLUSH_COMMANDS_BIT, 500 * 1000000);

		glfwSwapBuffers(window);
		
		//################################## ç frameFuffer2 ó ôàéë ####################################################
		glViewport(0, 0, imageWidth, imageHeight); 
		glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer2);
		glReadPixels(0, 0, imageWidth, imageHeight, nComponentsToGLConst[nrComponents], GL_UNSIGNED_BYTE, imagePixelsArr);
		save_image_to_file(destImagePath.c_str(), imageWidth, imageHeight, nrComponents, imagePixelsArr);
		std::cout << "Enter id: ";
		if (std::cin.fail()) // åñëè ïðåäûäóùåå èçâëå÷åíèå îêàçàëîñü íåóäà÷íûì,
		{
			std::cin.clear(); // òî âîçâðàùàåì cin â 'îáû÷íûé' ðåæèì ðàáîòû
			std::cin.ignore(32767, '\n'); // è óäàëÿåì çíà÷åíèÿ ïðåäûäóùåãî ââîäà èç âõîäíîãî áóôåðà
		}
		std::cin >> id;
		if (id == 88)
		{
			id = 1; value = 1;
		}
		else {

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
		glfwPollEvents();
	}
	glDeleteVertexArrays(1, &VAO1); glDeleteVertexArrays(1, &VAO2);
	glDeleteBuffers(1, &VBO1); glDeleteBuffers(1, &VBO2);
	glDeleteBuffers(1, &EBO1);
	glfwTerminate();
	return 0;
}

//äîïîì³æí³ ôóíêö³¿
void generate_frame_buffer_and_texture(GLint imageWidth, GLint imageHeight, GLuint* frameBuffer, GLuint* frameBuffTexture) {
	glGenFramebuffers(1, frameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, *frameBuffer);
	// create a color attachment texture

	glGenTextures(1, frameBuffTexture);
	glBindTexture(GL_TEXTURE_2D, *frameBuffTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imageWidth, imageHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);	// Set texture wrapping to GL_REPEAT
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, *frameBuffTexture, 0);


	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

GLuint load_texture(const std::string path, GLint* imageWidth, GLint* imageHeight, int* nrComponents)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	unsigned char *data = stbi_load(path.c_str(), imageWidth, imageHeight, nrComponents, 0);
	if (data)
	{
		GLenum format = nComponentsToGLConst[*nrComponents];
		/*if (*nrComponents == 1)
			format = GL_RED;
		else if (*nrComponents == 3)
			format = GL_RGB;
		else if (*nrComponents == 4)
			format = GL_RGBA;*/

		glBindTexture(GL_TEXTURE_2D, textureID);
		//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, *imageWidth, *imageHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glTexImage2D(GL_TEXTURE_2D, 0, format, *imageWidth, *imageHeight, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}

// Is called whenever a key is pressed/released via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
}

void save_image_to_file(std::string path, int width, int height, int nrComponents, const void *data) {
	size_t index = path.rfind(".");
	std::string ext = path.substr(index + 1); //ext - file extension

	if (ext == "png" || ext == "PNG")
		stbi_write_png(path.c_str(), width, height, nrComponents, data,  0);
	else 
		if (ext == "jpg" || ext == "jpeg" || ext == "JPG" || ext == "JPEG")
			stbi_write_jpg(path.c_str(), width, height, nrComponents, data, 90);
		else 
			stbi_write_bmp(path.c_str(), width, height, nrComponents, data);
}
