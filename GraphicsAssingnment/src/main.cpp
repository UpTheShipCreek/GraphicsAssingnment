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

class KeyInput {

	enum KeyState : std::uint8_t {
		KEY_UP,
		KEY_DOWN,
		KEY_HOLD
	};

	bool _isEnabled = true;
	std::map<int, KeyState> _keys;
	GLFWwindow* _window;

	void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {

		std::map<int, KeyState>::iterator it = _keys.find(key);
		if (it == _keys.end()) return;

		switch (action) {
			case GLFW_PRESS:
				_keys[key] = KEY_DOWN;
				break;
			case GLFW_REPEAT:
				_keys[key] = KEY_HOLD;
				break;
			case GLFW_RELEASE:
				_keys[key] = KEY_UP;
				break;
			default: 
				_keys[key] = KEY_UP; // Just in case? I hope this is not a bug let's see
				break;
		}
	}
	
	public:
	KeyInput(GLFWwindow* window, std::vector<int> keysToMonitor) {
		_window = window;
		for (int key : keysToMonitor) {
			_keys[key] = KEY_UP;
		}
	}
	~KeyInput(){}

	bool is_enabled() {
		return _isEnabled;
	}

	void enable() {
		_isEnabled = true;
	}

	void disable() {
		_isEnabled = false;
	}

	bool key_pressed(int key) {
		if (!_isEnabled) return false;

		std::map<int, KeyState>::iterator it = _keys.find(key);

		if (it != _keys.end()) {
			if (_keys[key] == KEY_DOWN) return true;
			else return false;
		}
	}
	bool key_held(int key) {
		if (!_isEnabled) return false;

		std::map<int, KeyState>::iterator it = _keys.find(key);

		if (it != _keys.end()) {
			if (_keys[key] == KEY_HOLD) return true;
			else return false;
		}
	}
};

class MouseInput {
	bool firstMouse = true;
	float yaw = -90.0f;	// yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
	float pitch = 0.0f;
	float lastX = 800.0f / 2.0;
	float lastY = 600.0 / 2.0;
	float fov = 45.0f;
};

class Camera {
	/*glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
	glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);*/

	glm::vec3 _position;
	glm::vec3 _front;
	glm::vec3 _up;
	
	public:
	Camera(glm::vec3 position, glm::vec3 front, glm::vec3 up) {
		_position = position;
		_front = front;
		_up = up;
	}

	glm::vec3 get_position() {
		return _position;
	}
	glm::vec3 get_front() {
		return _front;
	}
	glm::vec3 get_up() {
		return _up;
	}

	void update_position(glm::vec3 position) {
		_position = position;
	}
	void update_front(glm::vec3 front) {
		_front = front;
	}
	void update_up(glm::vec3 up) {
		_up = up;
	}
};

class Movement {
	float _speed;
	Camera& _camera;
	KeyInput& _keyInput;

	public:
	Movement(float speed, Camera& camera, KeyInput& keyInput) : _speed(speed), _camera(camera), _keyInput(keyInput) {
	}

	void update() {
		if (_keyInput.key_held(GLFW_KEY_W)) {
			_camera.update_position(_camera.get_position() + _camera.get_front() * _speed);
		}
		if (_keyInput.key_held(GLFW_KEY_S)) {
			_camera.update_position(_camera.get_position() - _camera.get_front() * _speed);
		}
		if (_keyInput.key_held(GLFW_KEY_A)) {
			_camera.update_position(_camera.get_position() - glm::normalize(glm::cross(_camera.get_front(), _camera.get_up())) * _speed);
		}
		if (_keyInput.key_held(GLFW_KEY_D)) {
			_camera.update_position(_camera.get_position() + glm::normalize(glm::cross(_camera.get_front(), _camera.get_up())) * _speed);
		}
	}
};

// camera
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

bool firstMouse = true;
float yaw = -90.0f;	// yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
float pitch = 0.0f;
float lastX = 800.0f / 2.0;
float lastY = 600.0 / 2.0;
float fov = 45.0f;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

int main(void) {

	GLFWwindow* window = windowInitializations(800, 600, "Graphics Assignment");

	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

	// Delay for input processing
	double lastPressTime = 0.0;
	double delay = 0.2;

	
	// Main while loop
	while (!glfwWindowShouldClose(window)) { // Checks if the window has been instructed to close
		processInput(window, translation, lastPressTime, delay);

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
		glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
		shader.setMat4("view", view);
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
	// Left/Right
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		translation += glm::vec3(-0.1, 0.0, 0.0);
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
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