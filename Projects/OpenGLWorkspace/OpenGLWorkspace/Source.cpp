#include <glad/glad.h>
#include <glfw3.h>
#include <stdio.h>
#include <iostream>
#include <string>


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void DrawAcrossTriangle(GLFWwindow* window);
void DrawTwoTriangle(GLFWwindow* window);
void DrawCustomShader();

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
	while (!glfwWindowShouldClose(window))
	{
		// input
		processInput(window);

		// render commands
		//DrawAcrossTriangle(window);
		//DrawTwoTriangle(window);
		DrawCustomShader();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

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
		"gl_Position = vec4(aPos, 1.0);"
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
	// 1. copy our vertices array in a buffer for OpenGL to use
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(triangleVertices), triangleVertices, GL_STATIC_DRAW);
	// 2. then set the vertex attributes pointers
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// 3. use our shader program when we want to render an object
	glUseProgram(shaderProgram);
	glBindVertexArray(VAO);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDrawArrays(GL_TRIANGLES, 0, 3);
	//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);	// unbind current vertex array object (VAO)
#pragma endregion
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

