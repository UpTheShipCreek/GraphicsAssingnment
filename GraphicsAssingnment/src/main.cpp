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
#include <map>
#include <vector>

#include "shader.h"
#include "stb/stb_image.h"
#include "window.h"
#include "texture.h"

void processInput(GLFWwindow* window, glm::vec3& playerPosition, glm::vec3& playerEulers) {

	int wasdState{ 0 };
	float walk_direction{ playerEulers.z };
	bool walking{ false };

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		wasdState += 1;
	}

	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		wasdState += 2;
	}

	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		wasdState += 4;
	}

	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		wasdState += 8;
	}

	switch (wasdState) {
	case 1:
	case 11:
		//forwards
		walking = true;
		break;
	case 3:
		//left-forwards
		walking = true;
		walk_direction += 45;
		break;
	case 2:
	case 7:
		//left
		walking = true;
		walk_direction += 90;
		break;
	case 6:
		//left-backwards
		walking = true;
		walk_direction += 135;
		break;
	case 4:
	case 14:
		//backwards
		walking = true;
		walk_direction += 180;
		break;
	case 12:
		//right-backwards
		walking = true;
		walk_direction += 225;
		break;
	case 8:
	case 13:
		//right
		walking = true;
		walk_direction += 270;
		break;
	case 9:
		//right-forwards
		walking = true;
		walk_direction += 315;
	}

	if (walking) {
		playerPosition += 0.1f * glm::vec3{
			glm::cos(glm::radians(walk_direction)),
			glm::sin(glm::radians(walk_direction)),
			0.0f
		};
	}

	double mouse_x, mouse_y;
	glfwGetCursorPos(window, &mouse_x, &mouse_y);
	glfwSetCursorPos(window, static_cast<double>(640 / 2), static_cast<double>(480 / 2));

	float delta_x{ static_cast<float>(mouse_x - static_cast<double>(640 / 2)) };
	playerEulers.z -= delta_x;

	float delta_y{ static_cast<float>(mouse_y - static_cast<double>(480 / 2)) };
	playerEulers.y = std::max(std::min(playerEulers.y + delta_y, 180.0f), 0.0f);

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
}


int main(void) {

	glm::vec3 playerPosition = { 0.0f, 0.0f, 1.0f };
	glm::vec3 playerEulers = { 0.0f, 90.0f,0.0f };

	int width = 800;
	int height = 600;
	float aspectRatio = (float)width / float(height);

	GLFWwindow* window = windowInitializations(width, height, "Graphics Assignment");
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

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

	// Transformation matrix for our shaders
	glm::mat4 trans = glm::mat4(1.0f);
	float angle = 0.0;
	glm::vec3 translation = glm::vec3(0.0f, 0.0f, -3.0f);

	// Z-buffer
	glEnable(GL_DEPTH_TEST);

	// Main while loop
	while (!glfwWindowShouldClose(window)) {

		processInput(window, playerPosition, playerEulers);

		glfwPollEvents();

		// Clear the colour buffer
		glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Set the mode
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		// Call the shader program
		shader.use();
		shader.setInt("texture1", 0);
		shader.setInt("texture2", 1);

		// View matrix
		glm::vec3 forwards{
			glm::sin(glm::radians(playerEulers.y)) * glm::cos(glm::radians(playerEulers.z)),
			glm::sin(glm::radians(playerEulers.y)) * glm::sin(glm::radians(playerEulers.z)),
			glm::cos(glm::radians(playerEulers.y))
		};
		glm::vec3 globalUp{ 0.0f, 0.0f, 1.0f };
		glm::vec3 right{ glm::cross(forwards, globalUp) };
		glm::vec3 up{ glm::cross(right,forwards) };
		glm::mat4 view = glm::lookAt(playerPosition, playerPosition + forwards, up);
		shader.setMat4("view", view);	
		glUniformMatrix4fv(glGetUniformLocation(shader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));

		int projectionLoc = glGetUniformLocation(shader.ID, "projection");
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

		// Transform 
		float angle{ glm::radians(static_cast<float>(10 * glfwGetTime())) };
		glm::mat4 model_transform{ glm::mat4(1.0f) };
		model_transform = glm::translate(model_transform, { 1.0f, -3.0f, 0.5f });
		model_transform = glm::rotate(model_transform, angle, { 1.0f, 0.0f, 0.0f });
		model_transform = glm::rotate(model_transform, 2 * angle, { 0.0f, 1.0f, 0.0f });
		glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model_transform));

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

	}
	glfwTerminate();
	return 0;
}