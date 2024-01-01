#include "shader.h"

// Constructor reads and builds the shader
Shader::Shader(const char* vertexPath, const char* fragmentPath) {
	unsigned int vertex, fragment;
	int success;
	char infoLog[512];
	
	vertex = loadShaderSource(vertexPath);
	fragment = loadShaderSource(fragmentPath);

	// shader Program
	ID = glCreateProgram();
	glAttachShader(ID, vertex);
	glAttachShader(ID, fragment);
	glLinkProgram(ID);
	// print linking errors if any
	glGetProgramiv(ID, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(ID, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}

	// delete the shaders as they're linked into our program now and no longer necessary
	glDeleteShader(vertex);
	glDeleteShader(fragment);
}

// Activate the shader
void Shader::use() {
	glUseProgram(ID);
}

void Shader::setBool(const std::string& name, bool value) const {
	glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}
void Shader::setInt(const std::string& name, int value) const {
	glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}
void Shader::setFloat(const std::string& name, float value) const {
	glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

// Load shaders	from file
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
		return -1;
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
