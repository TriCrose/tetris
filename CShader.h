#ifndef CSHADER_H
#define CSHADER_H

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

class CShader {
private:
	GLuint m_uiVertexShader;
	GLuint m_uiFragmentShader;
	GLuint m_uiShaderProgram;

	GLint m_iModelLocation;
	GLint m_iViewLocation;
	GLint m_iProjectionLocation;
	GLint m_iNormalMatrixLocation;
	GLint m_iTextUVLocation;

	GLint m_iRenderingStageLocation;
	GLint m_iLightPositionLocation;
	GLint m_iColourLocation;
	GLint m_iPausedLocation;

	glm::mat4 m_mat4Model, m_mat4View;
protected:
	static bool LoadShader(GLuint *Shader, GLenum ShaderType, const char *Source);
public:
	CShader(const char* VertexSource, const char* FragmentSource);
	virtual ~CShader();
	void Use();
	GLuint GetProgramID();

    void SetModel(glm::mat4 Model);
    void SetView(glm::mat4 View);
	void SetProjection(glm::mat4 Projection) { glUniformMatrix4fv(m_iProjectionLocation, 1, GL_FALSE, glm::value_ptr(Projection)); }
	void SetTextUV(glm::vec4 TextUV) { glUniform4fv(m_iTextUVLocation, 1, glm::value_ptr(TextUV)); }
    void SetLightPosition(glm::vec3 position);
	void SetRenderingStage(int stage) { glUniform1i(m_iRenderingStageLocation, stage); }
	void SetPaused(bool paused) { glUniform1i(m_iPausedLocation, paused?1:0); }
	void SetColour(glm::vec3 Colour) { glUniform3fv(m_iColourLocation, 1, glm::value_ptr(Colour)); }
};

#endif // CSHADER_H
