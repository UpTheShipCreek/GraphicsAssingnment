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
#include "window.h"
#include "texture.h"

void processInput(GLFWwindow* window, glm::vec3& translation, float& angle, int& modeOscillator, double& lastPressTime, double delay);

int main(void) {

	GLFWwindow* window = windowInitializations(800, 600, "Graphics Assignment");

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

	// Triangle vertices
	float vertices[] = {
	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
	 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
	-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

	-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f
	};


	glm::vec3 cubePositions[] = {
		glm::vec3(0.0f,  0.0f,  0.0f),
		glm::vec3(2.0f,  5.0f, -15.0f),
		glm::vec3(-1.5f, -2.2f, -2.5f),
		glm::vec3(-3.8f, -2.0f, -12.3f),
		glm::vec3(2.4f, -0.4f, -3.5f),
		glm::vec3(-1.7f,  3.0f, -7.5f),
		glm::vec3(1.3f, -2.0f, -2.5f),
		glm::vec3(1.5f,  2.0f, -2.5f),
		glm::vec3(1.5f,  0.2f, -1.5f),
		glm::vec3(-1.3f,  1.0f, -1.5f)
	};

	// Model matrix
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));

	// Projection matrix
	glm::mat4 projection;
	projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);

	// Element buffer object
	unsigned int EBO;
	glGenBuffers(1, &EBO);
	
	unsigned int VBO;  // Vertex buffer object
	glGenBuffers(1, &VBO); // Generate a buffer object

	// Vertex array object
	unsigned int VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	// Copy the vertices array into a buffer for OpenGL to use
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Position Attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// Texture Attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(2);

	Shader shader("./assets/vertex_core.glsl", "./assets/fragment_core.glsl");

	// Load textures
	unsigned int texture1 = loadTexture("./assets/textures/b_prisoner.jpg");
	unsigned int texture2 = loadTexture("./assets/textures/themediterranean.png");

	// Delay for input processing
	double lastPressTime = 0.0;
	double delay = 0.2;

	// Setting up the different modes of rendering
	unsigned int modes[] = {GL_FILL, GL_LINE};
	int modesNumber = sizeof(modes) / sizeof(modes[0]);
	int modeOscillator = 0;

	// Transformation matrix for our shaders
	glm::mat4 trans = glm::mat4(1.0f);
	float angle = 0.0;
	glm::vec3 translation = glm::vec3(0.0f, 0.0f, -3.0f);

	// Z-buffer
	glEnable(GL_DEPTH_TEST);
	
	// Main while loop
	while (!glfwWindowShouldClose(window)) { // Checks if the window has been instructed to close
		processInput(window, translation, angle, modeOscillator, lastPressTime, delay);

		// Modular arithmetic so we have the correct index even if the modeOscillator is negative
		while (modeOscillator < 0) {
			modeOscillator += modesNumber;
		}

		// Clear the colour buffer
		glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Set the mode
		glPolygonMode(GL_FRONT_AND_BACK, modes[modeOscillator % modesNumber]);

		// Call the shader program
		shader.use();
		shader.setInt("texture1", 0);
		shader.setInt("texture2", 1);

		int modelLoc = glGetUniformLocation(shader.ID, "model");
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

		// View matrix
		glm::mat4 view = glm::mat4(1.0f);
		view = glm::translate(view, translation);
		int viewLoc = glGetUniformLocation(shader.ID, "view");
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

		int projectionLoc = glGetUniformLocation(shader.ID, "projection");
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

		// Transform 
		trans = glm::rotate(trans, glm::radians(angle), glm::vec3(0.0, 0.0, 1.0));
		// trans = glm::translate(trans, translation);
		unsigned int transformLoc = glGetUniformLocation(shader.ID, "transform");
		glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));

		// Resetting the angles 
		angle = 0.0;
		// translation = glm::vec3(0.0f, 0.0f, -3.0f);

		// Texture binding
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture1);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture2);

		// Draw the triangle
		glBindVertexArray(VAO);
		for (unsigned int i = 0; i < 10; i++) {
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, cubePositions[i]);
			float angle = 20.0f * i;
			model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
			shader.setMat4("model", model);

			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		glBindVertexArray(0);

		// Check the events and swap the buffers
		glfwSwapBuffers(window); // Swaps the color buffer (whatever is being drawn) to the front
		glfwPollEvents(); // Checks if any events are triggered (like keyboard input or mouse movement)
	}
	glfwTerminate();
	return 0;
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

	// Object movement
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
		if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
			angle -= 5.0f;
		}
		if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
			angle += 5.0f;
		}
	}
	// Camera movement
	else {
		// Up/Down
		if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
			translation += glm::vec3(0.0, -0.1, 0.0);
		}
		if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
			translation += glm::vec3(0.0, 0.1, 0.0);
		}
		// Left/Right
		if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
			translation += glm::vec3(-0.1, 0.0, 0.0);
		}
		if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
			translation += glm::vec3(0.1, 0.0, 0.0);
		}
		// Forward/Backward
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
			translation += glm::vec3(0.0, 0.0, 0.1);
		}
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
			translation += glm::vec3(0.0, 0.0, -0.1);
		}
	}
}