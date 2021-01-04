#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include "Renderer.h"

class Shader
{
private:
    // the program ID
	GLuint mID;

public:
    // constructor reads and builds the shader
    Shader(const char* vertexPath, const char* fragmentPath);

    // Use/activate the shader
    void Use() const;
    void Unuse() const;

    // Add more shaders
    void AddShader(GLenum type, const std::string& sourcePath);

    // utility uniform functions
    void SetUniform1i(const std::string& name, int value) const;
    void SetUniform1f(const std::string& name, float value) const;
    void SetUniformMat4(const std::string& name, const glm::mat4& matrix) const;
    void SetUniformVec3(const std::string& name, float x, float y, float z) const;
    void SetUniformVec3(const std::string& name, const glm::vec3& vector) const;
    void SetUniformVec2(const std::string& name, float x, float y) const;
    void SetUniformVec2(const std::string& name, const glm::vec2& vector) const;
    GLuint GetID() const;

private:
    GLuint CompileShader(GLenum type, const char* source);
    
    GLuint CreateProgram(GLuint vertexShader, GLuint fragmentShader);
};