#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <iostream>
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
#include "camera.h"
#include "model.h"


#define SIMULATION_SPEED 4.0f

void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window, bool& motion, double& motionStartTime, double& motionStopTime, double& lastPressTime, double delay);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(40.0f, 0.0f, 50.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// lighting
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

enum PlanetType{
    EARTH=0, MOON, SUN
};

struct Object {
    Model model;
    glm::vec3 position;
    glm::vec3 scale;
    PlanetType type;
};

int main()
{
    GLFWwindow* window = windowInitializations(SCR_WIDTH, SCR_HEIGHT, "Graphics Assignment");

    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    Shader ourShader("./assets/vertex_model.glsl", "./assets/fragment_model.glsl");
    // build and compile our shader zprogram
    // ------------------------------------
    Shader lightingShader("./assets/vertex_core.glsl", "./assets/fragment_core.glsl");
    Shader lightCubeShader("./assets/vertex_light.glsl", "./assets/fragment_light.glsl");

    // load models
    // -----------
    std::string sunPath = "./assets/objects/sun/scene.gltf";
    std::string moonPath = "./assets/objects/moon/Moon.obj";
    std::string earthPath = "./assets/objects/earth/Earth.obj";

    Model sunModel(sunPath);
    Model moonModel(moonPath);
    Model earthModel(earthPath);

    // Test
    float earthOrbitRadius = 5.0f;  // adjust based on your scene setup
    float earthOrbitalPeriod = 10.0f;

    // Positions 
    glm::vec3 sunPosition = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 moonPosition = glm::vec3(0.0f, 0.0f, 5.0f);
    glm::vec3 earthPosition = glm::vec3(0.0f, 0.0f, 5.0f);


    // Create objects that handle the model, position, scale and the movement
    Object sun = { sunModel, sunPosition, glm::vec3(30.0), SUN };
    Object moon = { moonModel, moonPosition, glm::vec3(0.1), MOON };
    Object earth = { earthModel, earthPosition, glm::vec3(0.5), EARTH};

    std::vector<Object> objects = { sun, earth, moon };


    bool motion = true;
    double lastPressTime = 0.0;
    double delay = 0.2;

    double motionStartTime = (double)glfwGetTime();
    double motionStopTime = (double)glfwGetTime();
    float elapsedTime;

    // Earth's orbit parametric formula
    // vec3 position = (r cos(w t), r sin(w t), 0) 
    // where r is the radius of the orbit, w is the angular speed, and t is the time

    // Initialize positions
    glm::mat4 sunFramePosition = glm::translate(glm::mat4(1.0f), sunPosition);
    glm::mat4 earthFramePosition = glm::translate(glm::mat4(1.0f), earthPosition);
    glm::mat4 moonFramePosition = glm::translate(glm::mat4(1.0f), moonPosition);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window, motion, motionStartTime, motionStopTime, lastPressTime, delay);

        // render
        // ------
        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // don't forget to enable shader before setting uniforms
        ourShader.use();

        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);

        // render the loaded model
        for (int i = 0; i < (int)objects.size(); i++) {
            glm::mat4 model = glm::mat4(1.0f);
            
            if (motion == true) {
                elapsedTime = currentFrame - (motionStartTime - motionStopTime);

                elapsedTime *= SIMULATION_SPEED;

                if (objects[i].type == MOON) {
                    // Rotate around the sun (20 days to complete one spin around the sun)
                    model = glm::rotate(model, elapsedTime/20, glm::vec3(0.0f, 1.0f, 0.0f));
                    // Translate to its orbit around the sun
                    model = glm::translate(model, glm::vec3(47, 0.0f, 0.0f));

                    // Rotate around the Earth (27 days to complete one spin around the Earth)
                    model = glm::rotate(model, elapsedTime/27, glm::vec3(0.0f, 1.0f, 0.0f));
                    // Translate to its orbit around the Earth
                    model = glm::translate(model, glm::vec3(3.0f, 0.0f, 0.0f));

                    // Rotate around itself (4 days to complete one spin around itself)
                    model = glm::rotate(model, elapsedTime/4, glm::vec3(0.0f, 1.0f, 0.0f));

                    moonFramePosition = model;
                }
                else if (objects[i].type == EARTH) {
                    if (motion == true) {

                        // 1. Rotate around the sun (20 days to complete one spin around the sun)
                        model = glm::rotate(model, elapsedTime/20, glm::vec3(0.0f, 1.0f, 0.0f));

                        // 2. Translate to its orbit around the sun
                        model = glm::translate(model, glm::vec3(47, 0.0f, 0.0f));

                        // 3. Rotate around its own axis (spin)
                        model = glm::rotate(model, elapsedTime, glm::vec3(0.0f, 1.0f, 0.0f));

                        // Save the position
                        earthFramePosition = model;
                    }
                }
                else if (objects[i].type == SUN) {
                    model = glm::rotate(model, elapsedTime/100, glm::vec3(0.0f, 1.0f, 0.0f));
                    sunFramePosition = model;
                }
            }
            else {
                if (objects[i].type == MOON) {
                    model = moonFramePosition;
                }
                else if (objects[i].type == EARTH) {
                    model = earthFramePosition;
                }
                else if (objects[i].type == SUN) {
                    model = sunFramePosition;
                }
            }
  
            model = glm::scale(model, objects[i].scale);
            ourShader.setMat4("model", model);
			objects[i].model.Draw(ourShader);
		}

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window, bool& motion, double& motionStartTime, double& motionStopTime, double& lastPressTime, double delay)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        double currentTime = glfwGetTime();
        if (currentTime - lastPressTime > delay) {
            if (motion == true) {
                motion = false;
                motionStopTime = currentTime;
            }
            else {
                motion = true;
                motionStartTime = currentTime;
            }
            lastPressTime = currentTime;
        }
    }

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}