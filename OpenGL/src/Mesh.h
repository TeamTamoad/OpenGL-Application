#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <vector>

#include "Renderer.h"
#include "Shader.h"
#include "VertexArray.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"

struct Vertex
{
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoords;
};

struct Texture
{
	GLuint id;
	std::string type;
	std::string path;
};

class Mesh
{
private:
	VertexArray VAO;
	VertexBuffer VBO;
	IndexBuffer IBO;

public:
	// mesh data
	std::vector<Vertex>  mVertices;
	std::vector<GLuint>  mIndices;
	std::vector<Texture> mTextures;

	Mesh(const std::vector<Vertex>& vertices, const std::vector<GLuint>& indices, const std::vector<Texture>& textures);
	void Draw(const Shader& shader) const;
	void DrawTextureless(const Shader& shader) const;
	VertexArray GetVAO() const;

};

