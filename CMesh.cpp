#define GLEW_STATIC
#include <glew/glew.h>
#include <gl/gl.h>

#include "CMesh.h"

CMesh::CMesh(float* Vertices, int Size, bool Normals, bool TexCoords, GLenum Usage) {
	m_bNormals = Normals;
	m_bTexCoords = TexCoords;
	m_iVertexCount = Size / (4 * (3 + (Normals ? 3 : 0) + (TexCoords ? 2 : 0)));
	glGenBuffers(1, &m_uiVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_uiVBO);
	glBufferData(GL_ARRAY_BUFFER, Size, Vertices, Usage);
}

CMesh::~CMesh() {
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &m_uiVBO);
}

void CMesh::Draw() {
	Draw(0, m_iVertexCount);
}

void CMesh::Draw(int Offset, int Count) {
	glBindBuffer(GL_ARRAY_BUFFER, m_uiVBO);
	glEnableVertexAttribArray(0);
	if (m_bTexCoords) glEnableVertexAttribArray(1);
	if (m_bNormals) glEnableVertexAttribArray(2);

	if (m_bTexCoords && m_bNormals) {
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(0 * sizeof(float)));
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(3 * sizeof(float)));
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(5 * sizeof(float)));
	} else if (m_bTexCoords) {
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(0 * sizeof(float)));
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));
	} else if (m_bNormals) {
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(0 * sizeof(float)));
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));
	} else {
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0 * sizeof(float), (void *)(0 * sizeof(float)));
	}

	glDrawArrays(GL_TRIANGLES, Offset, Count);

	glDisableVertexAttribArray(0);
	if (m_bTexCoords) glDisableVertexAttribArray(1);
	if (m_bNormals) glDisableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}
