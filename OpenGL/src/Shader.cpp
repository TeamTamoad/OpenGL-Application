#include "Shader.h"

Shader::Shader(const char* vertexPath, const char* fragmentPath)
{
	// retrieve the vertex/fragment source code form filepath
	std::string vertexCode, fragmentCode;
	std::ifstream vShaderFile, fShaderFile;
	
	// ensure ifstream objects can throw exceptions:
	vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try
	{
		vShaderFile.open(vertexPath);
		fShaderFile.open(fragmentPath);

		std::stringstream vShaderStream, fShaderStream;
		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();

		vShaderFile.close();
		fShaderFile.close();

		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();
	}
	catch (std::ifstream::failure e)
	{
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ" << std::endl;
		ASSERT(false);
	}

	const char* vShaderCode = vertexCode.c_str();
	const char* fShaderCode = fragmentCode.c_str();

	GLuint vertexShader = CompileShader(GL_VERTEX_SHADER, vShaderCode);
	GLuint fragmentShader = CompileShader(GL_FRAGMENT_SHADER, fShaderCode);

	mID = CreateProgram(vertexShader, fragmentShader);
}

void Shader::Use() const
{
	glUseProgram(mID);
}

void Shader::Unuse() const
{
	glUseProgram(0);
}

// Setter Methods
void Shader::SetUniform1i(const std::string& name, int value) const
{
	GLint location = glGetUniformLocation(mID, name.c_str());
	if (location >= 0)
		glUniform1i(location, value);
	else
	{
		std::cout << "ERROR::SHADER::SETTING_UNIFORM_FAILED:: " << name << " do not exist." << std::endl;
		ASSERT(false);
	}
}

void Shader::SetUniform1f(const std::string& name, float value) const
{
	GLint location = glGetUniformLocation(mID, name.c_str());
	if (location >= 0)
		glUniform1f(glGetUniformLocation(mID, name.c_str()), value);
	else 
	{
		std::cout << "ERROR::SHADER::SETTING_UNIFORM_FAILED:: " << name << " do not exist." << std::endl;
		ASSERT(false);
	}
		
}

void Shader::SetUniformMat4(const std::string& name, const glm::mat4& matrix) const
{
	GLint location = glGetUniformLocation(mID, name.c_str());
	if (location >= 0)
		glUniformMatrix4fv(glGetUniformLocation(mID, name.c_str()), 1, GL_FALSE, glm::value_ptr(matrix));
	else
	{
		std::cout << "ERROR::SHADER::SETTING_UNIFORM_FAILED:: " << name << " do not exist." << std::endl;
		ASSERT(false);
	}
}

void Shader::SetUniformVec3(const std::string& name, const float& x, const float& y, const float& z) const
{
	GLint location = glGetUniformLocation(mID, name.c_str());
	if (location >= 0)
		glUniform3f(location, x, y, z);
	else
	{
		std::cout << "ERROR::SHADER::SETTING_UNIFORM_FAILED:: " << name << " do not exist." << std::endl;
		ASSERT(false);
	}
}

void Shader::SetUniformVec3(const std::string& name, const glm::vec3& vector) const
{
	GLint location = glGetUniformLocation(mID, name.c_str());
	if (location >= 0)
		glUniform3f(location, vector.x, vector.y, vector.z);
	else
	{
		std::cout << "ERROR::SHADER::SETTING_UNIFORM_FAILED:: " << name << " do not exist." << std::endl;
		ASSERT(false);
	}
}

GLuint Shader::GetID() const {
	return mID;
}

GLuint Shader::CompileShader(const GLenum& type, const char* source)
{
	GLuint shader = glCreateShader(type);
	glShaderSource(shader, 1, &source, NULL);
	glCompileShader(shader);

	GLint success;
	char infoLog[512];
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	glGetShaderInfoLog(shader, 512, NULL, infoLog);

	if (!success) {
		if (type == GL_VERTEX_SHADER)
			std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
		else if (type == GL_FRAGMENT_SHADER)
			std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
		ASSERT(false);
	}

	return shader;
}

GLuint Shader::CreateProgram(const GLuint& vertexShader, const GLuint& fragmentShader)
{
	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	GLint success;
	char infoLog[512];
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);

	if (!success)
	{
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	return shaderProgram;
}