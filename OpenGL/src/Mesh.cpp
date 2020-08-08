#include "Mesh.h"

Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<GLuint>& indices, const std::vector<Texture>& textures)
	: mVertices(vertices), mIndices(indices), mTextures(textures), VAO()
{
	VBO = VertexBuffer(mVertices);
	IBO = IndexBuffer(mIndices);

	// vertex position
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	// vertex normals
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
	// vertex texture coords
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
}

void Mesh::Draw(const Shader& shader) const
{
	GLuint diffuseNr = 1;
	GLuint specularNr = 1;
	shader.Use();
	for (int i = 0; i < mTextures.size(); i++)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		std::string number;
		std::string name = mTextures[i].type;

		if (name == "texture_diffuse")
			number = std::to_string(diffuseNr++);
		else if (name == "texture_specular")
			number = std::to_string(specularNr++);

		shader.SetUniform1i("material." + name + number, i);
		glBindTexture(GL_TEXTURE_2D, mTextures[i].id);
	}
	glActiveTexture(GL_TEXTURE0);

	// draw mesh
	VAO.Bind();
	glDrawElements(GL_TRIANGLES, mIndices.size(), GL_UNSIGNED_INT, 0);
}

void Mesh::DrawTextureless(const Shader& shader) const
{
	VAO.Bind();
	glDrawElements(GL_TRIANGLES, mIndices.size(), GL_UNSIGNED_INT, 0);
}

void Mesh::DrawInstance(const Shader& shader, unsigned int amount) const
{
	GLuint diffuseNr = 1;
	GLuint specularNr = 1;
	shader.Use();
	for (int i = 0; i < mTextures.size(); i++)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		std::string number;
		std::string name = mTextures[i].type;

		if (name == "texture_diffuse")
			number = std::to_string(diffuseNr++);
		else if (name == "texture_specular")
			number = std::to_string(specularNr++);

		shader.SetUniform1i("material." + name + number, i);
		glBindTexture(GL_TEXTURE_2D, mTextures[i].id);
	}
	glActiveTexture(GL_TEXTURE0);

	// draw mesh
	VAO.Bind();
	glDrawElementsInstanced(GL_TRIANGLES, mIndices.size(), GL_UNSIGNED_INT, 0, amount);
}

VertexArray Mesh::GetVAO() const
{
	return VAO;
}
