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

#include "shader.h"
#include "stb/stb_image.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window, glm::vec3& translation, float& angle, int& modeOscillator, double& lastPressTime, double delay);

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
		// positions          // colors           // texture coords
		 0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   0.5f, 0.8f,   // top right
		 0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   0.5f, 0.0f,   // bottom right
		-0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // bottom left
		-0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 0.8f    // top left 
	};

	// Indices for the EBO
	unsigned int indices[] = {  // note that we start from 0!
		0, 1, 2,
		0, 2, 3,
	};

	// Element buffer object
	unsigned int EBO;
	glGenBuffers(1, &EBO);
	
	unsigned int VBO;  // Vertex buffer object
	glGenBuffers(1, &VBO); // Generate a buffer object

	Shader shader("./assets/vertex_core.glsl", "./assets/fragment_core.glsl");

	// Vertex array object
	unsigned int VAO;
	glGenVertexArrays(1, &VAO);

	// Bind the vertex array object
	glBindVertexArray(VAO);

	// Copy the vertices array into a buffer for OpenGL to use
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Copy the indices array into an element buffer for OpenGL to use
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// Position Attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// Color Attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// Texture Attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	// Load image 
	unsigned int texture1;
	glGenTextures(1, &texture1);
	glBindTexture(GL_TEXTURE_2D, texture1);

	// set the texture wrapping/filtering options (on the currently bound texture object)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load and generate the texture
	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true);
	unsigned char* data = stbi_load("./assets/textures/b_prisoner.jpg", &width, &height, &nrChannels, 0);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);

	// Load image 
	unsigned int texture2;
	glGenTextures(1, &texture2);
	glBindTexture(GL_TEXTURE_2D, texture2);

	// Set the texture wrapping/filtering options (on the currently bound texture object)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// load and generate the texture
	stbi_set_flip_vertically_on_load(true);
	data = stbi_load("./assets/textures/themediterranean.png", &width, &height, &nrChannels, 0);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);

	// Delay for input processing
	double lastPressTime = 0.0;
	double delay = 0.2;

	// Setting up the different modes of rendering
	unsigned int modes[] = {GL_FILL, GL_POINT, GL_LINE};
	int modesNumber = sizeof(modes) / sizeof(modes[0]);
	int modeOscillator = 0;

	// Transformation matrix for our shaders
	glm::mat4 trans = glm::mat4(1.0f);
	float angle = 0.0;
	glm::vec3 translation = glm::vec3(0.0, 0.0, 0.0);
	// 

	// Main while loop
	while (!glfwWindowShouldClose(window)) { // Checks if the window has been instructed to close
		processInput(window, translation, angle, modeOscillator, lastPressTime, delay);

		// Modular arithmetic so we have the correct index even if the modeOscillator is negative
		while (modeOscillator < 0) {
			modeOscillator += modesNumber;
		}

		// Clear the colour buffer
		glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// Set the mode
		glPolygonMode(GL_FRONT_AND_BACK, modes[modeOscillator % modesNumber]);

		// Call the shader program
		shader.use();
		shader.setInt("texture1", 0);
		shader.setInt("texture2", 1);

		// Transform 
		trans = glm::rotate(trans, glm::radians(angle), glm::vec3(0.0, 0.0, 1.0));
		trans = glm::translate(trans, translation);
		unsigned int transformLoc = glGetUniformLocation(shader.ID, "transform");
		glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));

		// Resetting the angles 
		angle = 0.0;
		translation = glm::vec3(0.0, 0.0, 0.0);

		// Texture binding
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture1);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture2);

		// Draw the triangle
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

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

void processInput(GLFWwindow* window, glm::vec3& translation, float& angle, int& modeOscillator, double& lastPressTime, double delay) {
	// Exit
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}

	// Change rendering modes
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		double currentTime = glfwGetTime();
		if (currentTime - lastPressTime > delay) {
			modeOscillator++;
			lastPressTime = currentTime;
		}
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		double currentTime = glfwGetTime();
		if (currentTime - lastPressTime > delay) {
			modeOscillator--;
			lastPressTime = currentTime;
		}
	}
	// Rotate
	if(glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS){
		if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
			angle -= 5.0f;
		}
		if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
			angle += 5.0f;
		}
	}
	// Translations
	else {
		// Up/Down
		if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
			translation += glm::vec3(0.0, 0.1, 0.0);
		}
		if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
			translation += glm::vec3(0.0, -0.1, 0.0);
		}
		// Left/Right
		if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
			translation += glm::vec3(0.1, 0.0, 0.0);
		}
		if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
			translation += glm::vec3(-0.1, 0.0, 0.0);
		}
	}
}
