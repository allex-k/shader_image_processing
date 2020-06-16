#pragma once
#include <iostream>
#include <string>

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Shader.h"

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image.h"
#include "stb_image_write.h"
#include "other_functions.h"
#include "generate_blur_matrix.h"

class ImageProcessing {
public:
	ImageProcessing(GLFWwindow* window, int windowWidth, int windowHeight, std::string srcImagePath, std::string destImagePath) :
		window(window), windowWidth(windowWidth), windowHeight(windowHeight),
		srcImagePath(srcImagePath), destImagePath(destImagePath),
		emptyShader(resPath + "empty.vs", resPath + "empty.fs"),
		colorCorrectionShader(resPath + "empty.vs", resPath + "color_correction.fs"),
		blur2StepsShader(resPath + "empty.vs", resPath + "blur_2_steps.fs"),
		convolution3x3Shader(resPath + "empty.vs", resPath + "convolution3x3.fs"),
		convolution_n_mShader(resPath + "empty.vs", resPath + "convolution_n_m.fs"),
		convolution_n_m_textureShader(resPath + "empty.vs", resPath+"convolution_n_m_kern_texture.fs")
	{
		imageTextureID = load_texture(srcImagePath, &imageWidth, &imageHeight, &nrComponents);
		glViewport(0, 0, imageWidth, imageHeight);
		imagePixelsArr = new uint8_t[imageWidth*imageHeight*nrComponents];

		create_VAO_VBO_EBO_1_2();

		create_frame_buffer_and_texture(imageWidth, imageHeight, &frameBuffer1, &frameBuffTexture1);
		create_frame_buffer_and_texture(imageWidth, imageHeight, &frameBuffer2, &frameBuffTexture2);
		create_frame_buffer_and_texture(imageWidth, imageHeight, &frameBuffer3, &frameBuffTexture3);

		//================= Рендер початкового зображення з текстури у frameBuffer1 ============================
		glViewport(0, 0, imageWidth, imageHeight);//!!! зміна разміру Viewport
		//адже розмір frameFuffer1,2 відрізняється від розміру вікна (стандартного кадрового буфера)
		glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer1);
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		emptyShader.Use();
		glBindVertexArray(VAO1);
		glBindTexture(GL_TEXTURE_2D, imageTextureID);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	}

	void color_correction(GLint id, GLfloat value) {
		value = scale(value, -1, 1, ranges[id][0], ranges[id][1]);
		//############################# з frameFuffer1 у frameFuffer2 ##################################
		glViewport(0, 0, imageWidth, imageHeight);
		glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer2);

		colorCorrectionShader.Use();
		glBindTexture(GL_TEXTURE_2D, frameBuffTexture1);
		glUniform1i(glGetUniformLocation(colorCorrectionShader.Program, "id"), id);
		glUniform1f(glGetUniformLocation(colorCorrectionShader.Program, "value"), value);
		glBindVertexArray(VAO1);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	}

	void blur_2_steps(GLint id, GLfloat value, GLfloat blurScale = 1.f, GLfloat gamma = 2.2) {
		const int MAX_SIZE = 512;
		float coeffVector[MAX_SIZE];
		GLint SIZE = clip(scale(value, -1, 1, 2, 200), 2, 512);
		
		switch (id)
		{
		case 10: genenate_gaussian_blur_vector(coeffVector, SIZE); break;
		case 11: genenate_blur_vector_parabola(coeffVector, SIZE, 2.f); break;
		case 12: genenate_blur_vector_square(coeffVector, SIZE); gamma = 4.0;  break;
		default:
			break;
		}
	
		glViewport(0, 0, imageWidth, imageHeight);
		glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer3);

		blur2StepsShader.Use();
		glBindTexture(GL_TEXTURE_2D, frameBuffTexture1);

		glUniform2f(glGetUniformLocation(blur2StepsShader.Program, "direction"), 1.f/imageWidth*blurScale, 0.f);
		glUniform1i(glGetUniformLocation(blur2StepsShader.Program, "SIZE"), SIZE);
		glUniform1fv(glGetUniformLocation(blur2StepsShader.Program, "coeffVector"), SIZE, coeffVector);
		glUniform1f(glGetUniformLocation(blur2StepsShader.Program, "gamma"), gamma);
		glBindVertexArray(VAO1);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
//===============================================================================================
		glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer2);
		glBindTexture(GL_TEXTURE_2D, frameBuffTexture3);

		glUniform2f(glGetUniformLocation(blur2StepsShader.Program, "direction"), 0.f, 1.f/imageHeight*blurScale);
		glUniform1i(glGetUniformLocation(blur2StepsShader.Program, "SIZE"), SIZE);
		glUniform1fv(glGetUniformLocation(blur2StepsShader.Program, "coeffVector"), SIZE, coeffVector);
		glUniform1f(glGetUniformLocation(blur2StepsShader.Program, "gamma"), gamma);
		glBindVertexArray(VAO1);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	}

	void sharpness(GLint id, GLfloat value, float gamma = 2.2) {
		float coeffVector[9];
		generate_sharpness_matrix(coeffVector, scale(value, -1, 1, -9, 11));
	
		glViewport(0, 0, imageWidth, imageHeight);
		glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer2);

		convolution3x3Shader.Use();
		glBindTexture(GL_TEXTURE_2D, frameBuffTexture1);

		glUniform2f(glGetUniformLocation(convolution3x3Shader.Program, "offset"), 1.0/imageWidth , 1.0 / imageHeight);
		glUniform1fv(glGetUniformLocation(convolution3x3Shader.Program, "coeffVector"), 9, coeffVector);
		glUniform1f(glGetUniformLocation(convolution3x3Shader.Program, "gamma"), gamma);
		glBindVertexArray(VAO1);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	}

	void regular_polygon_blur(GLint id, GLfloat value, int nVertices=5, GLfloat blurScale=1.f, GLfloat gamma = 4.0) {
		int n = scale(value, -1, 1, 2, 22);
		int n2 = n * n;
		float* kernel = new float[n2]();

		generate_regular_polygon_matrix(kernel, n, n, nVertices);

		glViewport(0, 0, imageWidth, imageHeight);
		glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer2);

		convolution_n_mShader.Use();
		glBindTexture(GL_TEXTURE_2D, frameBuffTexture1);

		glUniform2f(glGetUniformLocation(convolution_n_mShader.Program, "offset"), 1.f / imageWidth * blurScale, 1.f / imageHeight * blurScale);
		glUniform1f(glGetUniformLocation(convolution_n_mShader.Program, "gamma"), gamma);
		glUniform1i(glGetUniformLocation(convolution_n_mShader.Program, "nrows"), n);
		glUniform1i(glGetUniformLocation(convolution_n_mShader.Program, "ncols"), n);

		glUniform1fv(glGetUniformLocation(convolution_n_mShader.Program, "kernel"), n2, kernel);
		glBindVertexArray(VAO1);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		delete[] kernel;
	}

	void regular_polygon_blur_texture(GLint id, GLfloat value, int nVertices=5, GLfloat blurScale=1.f, GLfloat gamma = 4.0) {
		int n = scale(value, -1, 1, 2, 50);
		int n2 = n * n;
		//std::cout << "n=" << n << "  n2=" << n2 << std::endl;
		float* kernel = new float[n2]();

		generate_regular_polygon_matrix(kernel, n, n, nVertices);
		//Matrix mtrx(kernel, n, n);
		//draw_regular_polygon(&mtrx, 5);
		//fill_convex_polygon(&mtrx);
		//normalize_array(kernel, n2);
		
		//====================== створення текстури ==============================	
		unsigned int kernelTextureID;
			glGenTextures(1, &kernelTextureID);

		
				glBindTexture(GL_TEXTURE_2D, kernelTextureID);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, n, n, 0, GL_RED, GL_FLOAT, kernel);
				glGenerateMipmap(GL_TEXTURE_2D);

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        //========================================================================

		glViewport(0, 0, imageWidth, imageHeight);
		glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer2);

		convolution_n_m_textureShader.Use();

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, frameBuffTexture1);
		glUniform1i(glGetUniformLocation(convolution_n_m_textureShader.Program, "imageTexture"), 0);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, kernelTextureID);
		glUniform1i(glGetUniformLocation(convolution_n_m_textureShader.Program, "kernelTexture"), 1);


		glUniform2f(glGetUniformLocation(convolution_n_m_textureShader.Program, "offsetImage"), 1.f/imageWidth* blurScale, 1.f/imageHeight* blurScale);
		glUniform2f(glGetUniformLocation(convolution_n_m_textureShader.Program, "offsetKernel"), 1.f/n, 1.f/n);
		glUniform1f(glGetUniformLocation(convolution_n_m_textureShader.Program, "gamma"), gamma);
		
		glBindVertexArray(VAO1);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		
		delete[] kernel;
		glActiveTexture(GL_TEXTURE0);
	}

	void show() {
		//############################# з frameFuffer2  на екран #######################################
		glViewport(0, 0, windowWidth, windowHeight);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		emptyShader.Use();
		glBindTexture(GL_TEXTURE_2D, frameBuffTexture2);
		glBindVertexArray(VAO2);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		GLsync fence = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
		glClientWaitSync(fence, GL_SYNC_FLUSH_COMMANDS_BIT, 500 * 1000000);

		glfwSwapBuffers(window);
	}
	void apply_changes() {
		swap_buffers();
	}
	void save_to_file() {
		//################################## з frameFuffer2 у файл ####################################################
		glViewport(0, 0, imageWidth, imageHeight);
		glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer2);
		glReadPixels(0, 0, imageWidth, imageHeight, nComponentsToGLConst[nrComponents], GL_UNSIGNED_BYTE, imagePixelsArr);
		save_image_to_file(destImagePath.c_str(), imageWidth, imageHeight, nrComponents, imagePixelsArr);
	}
	void print() {
		std::cout << "windowWidth = " << windowWidth << std::endl << "windowHeight = " << windowHeight << std::endl;
		std::cout << "srcImagePath = " << srcImagePath << std::endl << "destImagePath = " << destImagePath << std::endl;
	}

private:
	const int windowWidth, windowHeight;
	GLFWwindow* window;
	std::string resPath = "res\\";
	std::string srcImagePath, destImagePath;
	Shader emptyShader, colorCorrectionShader, blur2StepsShader, convolution3x3Shader, convolution_n_mShader, convolution_n_m_textureShader;
	int imageWidth, imageHeight, nrComponents;
	GLuint imageTextureID;
	GLenum nComponentsToGLConst[5] = { 0, GL_RED , GL_RG, GL_RGB, GL_RGBA };
	uint8_t* imagePixelsArr;
	GLuint VAO1, VAO2;
	GLuint frameBuffer1, frameBuffer2, frameBuffer3, frameBuffTexture1, frameBuffTexture2, frameBuffTexture3;
	float ranges[10][2] = { {0,2}, {2,0} ,{0.5, 1.5}, {3,0}, {-0.25,0.25}, {-0.1,0.1}, {-0.01, 0.01}, {0,0}, {0,0}, {0,0} };
	//Shader colorCorrectionShader;

	void create_frame_buffer_and_texture(GLint imageWidth, GLint imageHeight, GLuint* frameBuffer, GLuint* frameBuffTexture) {
		glGenFramebuffers(1, frameBuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, *frameBuffer);
		// create a color attachment texture

		glGenTextures(1, frameBuffTexture);
		glBindTexture(GL_TEXTURE_2D, *frameBuffTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imageWidth, imageHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);	// Set texture wrapping to GL_REPEAT
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
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
		//print_array(&data[2000], 400);
		if (data)
		{
			GLenum format = nComponentsToGLConst[*nrComponents];
			glBindTexture(GL_TEXTURE_2D, textureID);
			glTexImage2D(GL_TEXTURE_2D, 0, format, *imageWidth, *imageHeight, 0, format, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
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
	void swap_buffers() {
		GLuint temp = frameBuffer1;
		frameBuffer1 = frameBuffer2;
		frameBuffer2 = temp;

		temp = frameBuffTexture1;
		frameBuffTexture1 = frameBuffTexture2;
		frameBuffTexture2 = temp;
	}
	void create_VAO_VBO_EBO_1_2() {
		float posX = 1.f, posY = 1.f;
		float aspectRatio = ((float)windowWidth / windowHeight) / ((float)imageWidth / imageHeight); //співвідношення співвідношень сторін вікна та зображення

		if (aspectRatio < 1.f) { posY = aspectRatio; }
		else { posX = 1.f / aspectRatio; };

		float scaleX = (float)windowWidth / imageWidth;
		float scaleY = (float)windowHeight / imageHeight;
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

		GLuint VBO1, EBO1;
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
		GLuint VBO2;
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

	}
	void save_image_to_file(std::string path, int width, int height, int nrComponents, const void *data) {
		size_t index = path.rfind(".");
		std::string ext = path.substr(index + 1); //ext - file extension

		if (ext == "png" || ext == "PNG")
			stbi_write_png(path.c_str(), width, height, nrComponents, data, 0);
		else
			if (ext == "jpg" || ext == "jpeg" || ext == "JPG" || ext == "JPEG")
				stbi_write_jpg(path.c_str(), width, height, nrComponents, data, 90);
			else
				stbi_write_bmp(path.c_str(), width, height, nrComponents, data);
	}

};