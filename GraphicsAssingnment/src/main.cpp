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

// A class that will allow us to create a template for the transformations of each planet
class Transformation {
    float _radius;
    float _period;

    public:
    Transformation(float radius, float period) {
        _radius = radius;
        _period = period;
    }
    glm::mat4 use(glm::mat4 model, float time) {
        model = glm::rotate(model, time / _period, glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::translate(model, glm::vec3(_radius, 0.0f, 0.0f));
        return model;
    }
};

struct Object {
    Model model;
    glm::vec3 scale;
    std::vector<std::shared_ptr<Transformation>> transformations;
    Shader shader;
    glm::mat4 framePosition;
};

int main()
{
    GLFWwindow* window = windowInitializations(SCR_WIDTH, SCR_HEIGHT, "Graphics Assignment");

    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    
    // build and compile our shader zprogram
    // ------------------------------------
    Shader nonLuminousShader("./assets/vertex_model.glsl", "./assets/fragment_model.glsl");
    Shader sunShader("./assets/vertex_light.glsl", "./assets/fragment_light.glsl");

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

    // Create the transformations
    // Rotation we the period of 100, which is really slow 
    std::shared_ptr<Transformation> sunSpin = std::make_shared<Transformation>(0.0f, 100.0f);
    // Rotation around the sun
    std::shared_ptr<Transformation> earthSunRotation = std::make_shared<Transformation>(47.0f, 20.0f);
    std::shared_ptr<Transformation> earthSpin = std::make_shared<Transformation>(0.0f, 1.0f);
    // Rotation around the sun
    std::shared_ptr<Transformation> moonSunRotation = std::make_shared<Transformation>(47.0f, 20.0f);
    std::shared_ptr<Transformation> moonEarthRotation= std::make_shared<Transformation>(3.0f, 3.0f);
    std::shared_ptr<Transformation> moonSpin = std::make_shared<Transformation>(0.0f, 4.0f);

    // Initialize positions
    glm::mat4 sunFramePosition = glm::translate(glm::mat4(1.0f), sunPosition);
    glm::mat4 earthFramePosition = glm::translate(glm::mat4(1.0f), earthPosition);
    glm::mat4 moonFramePosition = glm::translate(glm::mat4(1.0f), moonPosition);

    // Create objects that handle the model, scale and the movement
    Object sun = { 
        sunModel, // Model
        glm::vec3(30.0), // Scaling
        {sunSpin}, //Transformations
        sunShader, // Shader
        sunFramePosition // Frame position
    };

    Object earth = { 
        earthModel, 
        glm::vec3(0.5), 
        {earthSunRotation, earthSpin}, 
        nonLuminousShader, 
        earthFramePosition
    };
    Object moon = { 
        moonModel, 
        glm::vec3(0.1), 
        {moonSunRotation, moonEarthRotation, moonSpin},  
        nonLuminousShader, 
        moonFramePosition
    };

    // Create a vector of objects
    std::vector<Object> objects = { sun, earth, moon };

    // Various variables for the simulation
    bool motion = true;
    double lastPressTime = 0.0;
    double delay = 0.2;

    double motionStartTime = (double)glfwGetTime();
    double motionStopTime = (double)glfwGetTime();
    float elapsedTime;

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

        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();

        for (auto& object : objects) {
            object.shader.use();

            object.shader.setMat4("projection", projection);
            object.shader.setMat4("view", view);

            object.shader.setVec3("light.position", sunPosition);
            object.shader.setVec3("viewPos", camera.Position);
            object.shader.setVec3("light.ambient", 0.2f, 0.2f, 0.2f);
            object.shader.setVec3("light.diffuse", 0.5f, 0.5f, 0.5f);

            glm::mat4 model = glm::mat4(1.0f);
            if (motion == true) {
                elapsedTime = currentFrame - (motionStartTime - motionStopTime);
                elapsedTime *= SIMULATION_SPEED;

                for (auto& transformation : object.transformations) {
					model = transformation->use(model, elapsedTime);
				}
                object.framePosition = model;
            }
            else {
				model = object.framePosition;
            }
            model = glm::scale(model, object.scale);
            object.shader.setMat4("model", model);
            object.model.Draw(object.shader);
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