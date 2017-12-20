#include <glad/glad.h>
#include <glfw3.h>
#include <stdio.h>
#include <iostream>
#include <string>

#include "shader.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void DrawAcrossTriangle(GLFWwindow* window);
void DrawTwoTriangle(GLFWwindow* window);
void DrawCustomShader();
void DrawTexture(GLFWwindow* window);
void DrawTransformation(GLFWwindow* window);

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);


	GLFWwindow* window = glfwCreateWindow(800, 600, "Eden", NULL, NULL);

	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	glViewport(0, 0, 800, 600);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// render loop
	//while (!glfwWindowShouldClose(window))
	//{
	//	// input
	//	processInput(window);

	//	// render commands
	//	//DrawAcrossTriangle(window);
	//	//DrawTwoTriangle(window);
	//	//DrawCustomShader();
	//	//DrawTexture();

	//	glfwSwapBuffers(window);
	//	glfwPollEvents();
	//}

	DrawTexture(window);

	glfwTerminate();

	return 0;
}

void DrawAcrossTriangle(GLFWwindow* window)
{
	glClearColor(0.5f, 0.75f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

#pragma region Declare Vertices and Indices
	// create a triangle within OpenGL's visible region (-1,1) 
	float triangleVertices[] = {
		-0.25f, -0.25f, 0.0f,
		0.25f, -0.25f, 0.0f,
		0.0f,  0.25f, 0.0f
	};
	float doubleTriangleVertices[] = 
	{
		0.0f, 0.25f, 0.0f,
		-0.25f, 0.0f, 0,
		0, 0, 0,
		0, 0, 0,
		0, -0.25f, 0,
		0.25f, 0, 0
	};
	for (int i = 0; i < 18; i++)
	{
		doubleTriangleVertices[i] *= 2;
	}
	// create a square using element buffer and indices
	float squareVertices[] = {
		0.5f,  0.5f, 0.0f,  // top right
		0.5f, -0.5f, 0.0f,  // bottom right
		-0.5f, -0.5f, 0.0f,  // bottom left
		-0.5f,  0.5f, 0.0f   // top left 
	};
	unsigned int indices[] = {  // note that we start from 0!
		0, 1, 3,				// first triangle
		1, 2, 3					// second triangle
	};
#pragma endregion

#pragma region Generate Buffer Object and Array Objects
	// Create vertex buffer objects, used for the vertex shader
	unsigned int VBO;
	glGenBuffers(1, &VBO);
	// Create vertex array objects, used for vertex attribute pointers
	unsigned int VAO;
	glGenVertexArrays(1, &VAO);
	// Create element buffers
	unsigned int EBO;
	glGenBuffers(1, &EBO);
#pragma endregion

#pragma region Create, compile, and link vertex and fragment shaders

	const char* vertexShaderSource;
	unsigned int vertexShader;
	const char* fragmentShaderSource;
	unsigned int fragmentShader;

	unsigned int shaderProgram;
	shaderProgram = glCreateProgram();

	vertexShaderSource = "#version 330 core\n"
		"layout(location = 0) in vec3 aPos;"
		""
		"void main()"
		"{"
		"gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);"
		"}";
	vertexShader = glCreateShader(GL_VERTEX_SHADER);

	fragmentShaderSource = "#version 330 core\n"
		"out vec4 FragColor;"
		""
		"void main()"
		"{"
		"FragColor = vec4(0.25f, 0.5f, 0.2f, 1.0f);"
		"}";
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);

	// attach the created shaders above to the shader program and link it 
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	// Debug for the shader program
	int  success;
	char infoLog[512];
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

#pragma endregion

#pragma region Bind Buffers and Draw
	// 0. bind vertex array object (VAO)
	glBindVertexArray(VAO);
	// 1a. copy our vertices array in a buffer for OpenGL to use
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(doubleTriangleVertices), doubleTriangleVertices, GL_STATIC_DRAW);
	// 1b. copy our index array in an element buffer for OpenGL to use
	// glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	// glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	// 2. then set the vertex attributes pointers
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// 3. use our shader program when we want to render an object
	glUseProgram(shaderProgram);
	glBindVertexArray(VAO);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);	// unbind current vertex array object (VAO)

#pragma endregion

}

void DrawTwoTriangle(GLFWwindow* window)
{
	glClearColor(0.5f, 0.75f, 0.45f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	float leftTriangleVertex[] =
	{
		-0.25f, 0.5f, 0.0f,
		-0.5f, 0.25f, 0,
		-0.25f, 0.25f, 0,
	};
	float rightTriangleVertex[] =
	{
		0.25f, 0.5f, 0,
		0.5f, 0.25f, 0,
		0.25f, 0.25f, 0
	};

	unsigned int VBOLeft;
	unsigned int VBORight;
	glGenBuffers(1, &VBOLeft);
	glGenBuffers(1, &VBORight);

	unsigned int VAOLeft;
	unsigned int VAORight;
	glGenVertexArrays(1, &VAOLeft);
	glGenVertexArrays(1, &VAORight);

	const char* vertexShaderSource;
	unsigned int vertexShader;
	const char* fragmentShaderSource;
	unsigned int fragmentShader;

	vertexShaderSource = "#version 330 core\n"
		"layout(location = 0) in vec3 aPos;"
		""
		"void main()"
		"{"
		"gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);"
		"}";
	vertexShader = glCreateShader(GL_VERTEX_SHADER);

	fragmentShaderSource = "#version 330 core\n"
		"out vec4 FragColor;"
		""
		"void main()"
		"{"
		"FragColor = vec4(0.65f, 0.2f, 0.75f, 1.0f);"
		"}";
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);

	unsigned int shaderProgram;
	shaderProgram = glCreateProgram();

	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	
	glUseProgram(shaderProgram);
	glBindVertexArray(VAOLeft);
	glBindBuffer(GL_ARRAY_BUFFER, VBOLeft);
	glBufferData(GL_ARRAY_BUFFER, sizeof(leftTriangleVertex), leftTriangleVertex, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);

	glBindVertexArray(VAORight);
	glBindBuffer(GL_ARRAY_BUFFER, VBORight);
	glBufferData(GL_ARRAY_BUFFER, sizeof(rightTriangleVertex), rightTriangleVertex, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glBindVertexArray(VAOLeft);
	glDrawArrays(GL_TRIANGLES, 0, 3);
	
	glBindVertexArray(VAORight);
	glDrawArrays(GL_TRIANGLES, 0, 3);

}

void DrawCustomShader()
{
	glClearColor(0.5f, 0.75f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

#pragma region Declare vertices and indices
	float triangleVertices[] = {
		-0.33f, -0.33f, 0.0f,
		0.33f, -0.33f, 0.0f,
		0.0f,  0.33f, 0.0f
	};

	float verticesWithColor[] = {
		// positions         // colors
		0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,   // bottom right, red
		-0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,   // bottom left, green
		0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f    // top, blue
	};
#pragma endregion

#pragma region Generate Buffer Object and Array Objects
	// Create vertex buffer objects, used for the vertex shader
	unsigned int VBO;
	glGenBuffers(1, &VBO);
	// Create vertex array objects, used for vertex attribute pointers
	unsigned int VAO;
	glGenVertexArrays(1, &VAO);
	// Create element buffers
	unsigned int EBO;
	glGenBuffers(1, &EBO);
#pragma endregion

#pragma region Create, compile, and link vertex and fragment shaders in custom shader class

	Shader customShader("vertex.vert", "fragment.frag");

#pragma endregion

	float timeValue = glfwGetTime();
	float greenValue = (sin(timeValue) / 2.0f) + 0.5f;
	verticesWithColor[3] = greenValue;
	verticesWithColor[10] = greenValue;
	verticesWithColor[17] = greenValue * 2;

#pragma region Bind Buffers and Draw
	// 0. bind vertex array object (VAO)
	glBindVertexArray(VAO);
	// 1. copy our vertices array in a buffer for OpenGL to use
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verticesWithColor), verticesWithColor, GL_STATIC_DRAW);
	// 2a. then set the vertex attributes pointers for position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// 2b. set the vertex attributes pointers for color
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	// 3. use our shader program when we want to render an object
	//glUseProgram(shaderProgram);
	//glUniform4f(vertexColorLocation, greenValue, 0.0f, 0.0f, 1.0f);
	customShader.use();
	customShader.setFloat("horizontalOffset", 0.0f);
	glBindVertexArray(VAO);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDrawArrays(GL_TRIANGLES, 0, 3);
	//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);	// unbind current vertex array object (VAO)
#pragma endregion
}

void DrawTexture(GLFWwindow* window)
{
	

	// contains position, color and texture values
	float triangleVertices[] = 
	{
		-0.33f, -0.33f, 0.0f,  0.65f, 0.0, 0.0f,	0.0f, 0.0f, // bottom left
		 0.33f, -0.33f, 0.0f,  0.0f, 0.65f, 0.0f,	1.0f, 0.0f, // bottom right
		 0.0f,   0.33f, 0.0f,  0.0f, 0.0f, 0.65f,	0.5f, 1.0f	// middle top
	};
	// texture coordinates range from (0,0) of lower left to (1,1) of upper right 
	float textureCoordinates[] =
	{
		0.0f, 0.0f,  // lower-left corner  
		1.0f, 0.0f,  // lower-right corner
		0.5f, 1.0f   // top-center corner
	};
	float blendAmount = 0.1f;

	Shader shader("texVert.vert", "texFrag.frag");
	shader.setFloat("blendAmount", blendAmount);
	unsigned int VAO;
	glGenVertexArrays(1, &VAO);
	unsigned int VBO;
	glGenBuffers(1, &VBO);

	stbi_set_flip_vertically_on_load(true);

	int width, height, nrChannels;
	unsigned char *data = stbi_load("moon-texture.png", &width, &height, &nrChannels, 0);
	std::cout << "Channels: " << nrChannels;
	int width2, height2, nrChannels2;
	unsigned char *data2 = stbi_load("strawberry.png", &width2, &height2, &nrChannels2, 0);


	unsigned int texture;
	glGenTextures(1, &texture);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);

	// set the texture wrapping/filtering options (on the currently bound texture object)
	// s and t since it's part of the 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	float bordercolor[] = { 0, 0, 0, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, bordercolor);

	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}

	stbi_image_free(data);

	unsigned int texture2;
	glGenTextures(1, &texture2);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texture2);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	if (data2)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width2, height2, 0, GL_RGBA, GL_UNSIGNED_BYTE, data2);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}

	stbi_image_free(data2);

	shader.use();
	shader.setInt("ourTexture", 0);
	shader.setInt("ourTexture2", 1);
	while (!glfwWindowShouldClose(window))
	{
		// input
		processInput(window);

		if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
		{
			if (blendAmount < 0.9f)
			{
				blendAmount += 0.1f;
			}
		}
		else if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
		{
			if (blendAmount > 0.1f)
			{
				blendAmount -= 0.1f;
			}
		}
		shader.setFloat("blendAmount", blendAmount);

		glClearColor(0.5f, 0.75f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		// render loop
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(triangleVertices), triangleVertices, GL_STATIC_DRAW);
		// set pointer for position
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		// set pointer for color
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
		// set pointer for texture
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(2);


		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture2);

		glBindVertexArray(VAO);
		//glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);
		glDrawArrays(GL_TRIANGLES, 0, 3);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	

	
}

void DrawTransformation(GLFWwindow* window)
{

}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

