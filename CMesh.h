#ifndef CMESH_H
#define CMESH_H

class CMesh {
	friend class CRenderer;
private:
	GLuint m_uiVBO;
	int m_iVertexCount;
	bool m_bTexCoords;
	bool m_bNormals;

    void Draw();
	void Draw(int Offset, int Count);
public:
	// Size is in bytes
	CMesh(float* Vertices, int Size, bool Normals, bool TexCoords, GLenum Usage = GL_STATIC_DRAW);		// Size in bytes
	virtual ~CMesh();
	bool HasTexCoords() { return m_bTexCoords; }
	bool HasNormals() { return m_bNormals; }
};

#endif // CMESH_H
