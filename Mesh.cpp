#include "Mesh.h"

/**
 * The Mesh constructor initializes the VAO, VBO, and IBO variables to 0 and sets the indexCount to 0.
 */
Mesh::Mesh()
{
	VAO = 0;
	VBO = 0;
	IBO = 0;
	indexCount = 0;
}

/**
 * This function creates a mesh by generating and binding vertex arrays and buffers, and setting vertex
 * attribute pointers.
 * 
 * @param vertices An array of GLfloat values representing the vertices of the mesh. Each vertex is
 * represented by three consecutive values in the array, corresponding to its x, y, and z coordinates.
 * @param indices An array of unsigned integers representing the indices of the vertices that make up
 * the triangles in the mesh. Each group of three indices represents a single triangle.
 * @param numOfVertices The number of vertices in the mesh.
 * @param numOfIndices The number of indices in the indices array.
 */
void Mesh::CreateMesh(GLfloat* vertices, unsigned int* indices, unsigned int numOfVertices, unsigned int numOfIndices)
{
	indexCount = numOfIndices;

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0]) * numOfIndices, indices, GL_STATIC_DRAW);
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices[0]) * numOfVertices, vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

/**
 * This function renders a mesh using OpenGL by binding the vertex array and index buffer objects and
 * calling the appropriate draw function.
 */
void Mesh::RenderMesh()
{
	glBindVertexArray(VAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

/**
 * The function clears the mesh by deleting the index buffer object, vertex buffer object, and vertex
 * array object.
 */
void Mesh::ClearMesh()
{
	if (IBO != 0)
	{
		glDeleteBuffers(1, &IBO);
		IBO = 0;
	}

	if (VBO != 0)
	{
		glDeleteBuffers(1, &VBO);
		VBO = 0;
	}

	if (VAO != 0)
	{
		glDeleteVertexArrays(1, &VAO);
		VAO = 0;
	}

	indexCount = 0;
}

/**
 * The destructor function for the Mesh class that clears the mesh.
 */
Mesh::~Mesh()
{
	ClearMesh();
}
