#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <fstream>
#include <sstream>
#include <streambuf>
#include <string>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
int processInput(GLFWwindow* window, int modeOscillator, double& lastPressTime, double delay);
unsigned int loadShaderSource(const char* filename);

int main(void) {

	// Initialize GLFW
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); 
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Create a window
	GLFWwindow* window = glfwCreateWindow(800, 600, "Graphics Assingment", NULL, NULL);
	if (window == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Load GLAD
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// Viewport
	glViewport(0, 0, 800, 600); 

	// Run our resize function whenever the window is resized
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);


	// Triangle vertices
	float vertices[] = {
		-0.5f, -0.5f, 0.0f,
		 0.5f, -0.5f, 0.0f,
		 0.0f,  0.5f, 0.0f
	};

	unsigned int VBO;  // Vertex buffer object
	glGenBuffers(1, &VBO); // Generate a buffer object
	glBindBuffer(GL_ARRAY_BUFFER, VBO);	// Bind the buffer object to the GL_ARRAY_BUFFER target
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); // Copy the vertices data into the buffer's memory

	// Vertex shader and Fragment shader
	unsigned int vertexShader = loadShaderSource("./assets/vertex_core.glsl");
	unsigned int fragmentShader = loadShaderSource("./assets/fragment_core.glsl");

	// Create a shader program
	unsigned int shaderProgram = glCreateProgram();
	
	glAttachShader(shaderProgram, vertexShader);
	glDeleteShader(vertexShader);
	
	glAttachShader(shaderProgram,fragmentShader);
	glDeleteShader(fragmentShader);
	
	// Link the program
	glLinkProgram(shaderProgram);

	// Check if the linking was successful
	int  success;
	char infoLog[512];
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "Linking shaders failed\n" << infoLog << std::endl;
		return -1;
	}
	else {
		std::cout << "Successfully linked shaders" << std::endl;
	}

	// Vertex array object
	unsigned int VAO;
	glGenVertexArrays(1, &VAO);

	// Bind the vertex array object
	glBindVertexArray(VAO);

	// Copy the vertices array into a buffer for OpenGL to use
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Set the vertex attributes pointers
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// Setting up the different modes of rendering
	unsigned int modes[] = { GL_TRIANGLES, GL_POINTS, GL_LINES};
	size_t modesNumber = sizeof(modes) / sizeof(modes[0]);
	int mode_oscilator = 0;

	// Delay for input processing
	double lastPressTime = 0.0;
	double delay = 0.2;
	
	// Main while loop
	while (!glfwWindowShouldClose(window)) { // Checks if the window has been instructed to close
		mode_oscilator = processInput(window, mode_oscilator, lastPressTime, delay); // Checks if the escape key is pressed

		// Rendering commands here
		glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// Call the shader program
		glUseProgram(shaderProgram);
		glBindVertexArray(VAO);
		glDrawArrays(modes[mode_oscilator % modesNumber], 0, 3);

		// Check the events and swap the buffers
		glfwSwapBuffers(window); // Swaps the color buffer (whatever is being drawn) to the front
		glfwPollEvents(); // Checks if any events are triggered (like keyboard input or mouse movement)
	}
	glfwTerminate();
	return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

int processInput(GLFWwindow* window, int modeOscillator, double& lastPressTime, double delay) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}

	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		double currentTime = glfwGetTime();
		if (currentTime - lastPressTime > delay) {
			modeOscillator++;
			lastPressTime = currentTime;
		}
	}
	return modeOscillator;
}

unsigned int loadShaderSource(const char* filename) {
	enum ShaderType { VERTEX, FRAGMENT, GEOMETRY };
	ShaderType shaderType;
	unsigned int shaderCode;

	// Read file
	std::ifstream file(filename);
	if (!file.is_open()) {
		std::cout << "Failed to open file: " << filename << std::endl;
		return -1;
	}
	std::string str((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

	// Determine shader type
	if (std::strcmp(filename, "./assets/fragment_core.glsl") == 0) {
		shaderType = FRAGMENT;
		shaderCode = GL_FRAGMENT_SHADER;
	}
	else if (std::strcmp(filename, "./assets/vertex_core.glsl") == 0) {
		shaderType = VERTEX;
		shaderCode = GL_VERTEX_SHADER;
	}
	else if (std::strcmp(filename, "./assets/geometry_core.glsl") == 0) {
		shaderType = GEOMETRY;
		shaderCode = GL_GEOMETRY_SHADER;
	}
	else {
		std::cout << "Failed to determine shader type" << std::endl;
		return - 1;
	}

	// Load shader source
	GLchar* const stringGL = (GLchar*)str.c_str();

	unsigned int shaderValue = glCreateShader(shaderCode);

	glShaderSource(shaderValue, 1, &stringGL, NULL);
	glCompileShader(shaderValue);

	int  success;
	char infoLog[512];
	glGetShaderiv(shaderValue, GL_COMPILE_STATUS, &success);

	if (!success) {
		glGetShaderInfoLog(shaderValue, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
		return -1;
	}
	else {
		std::cout << "Sucessfully compiled shader: " << filename << std::endl;
		return shaderValue;
	}
}	

