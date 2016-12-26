#define GLEW_STATIC
#include <glew/glew.h>
#include <gl/gl.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>

#include "CWindowManager.h"
#include "CRenderer.h"
#include "CShader.h"

CShader::CShader(const char* VertexSource, const char* FragmentSource) {
	LoadShader(&m_uiVertexShader, GL_VERTEX_SHADER, VertexSource);
	LoadShader(&m_uiFragmentShader, GL_FRAGMENT_SHADER, FragmentSource);
	m_uiShaderProgram = glCreateProgram();
	glAttachShader(m_uiShaderProgram, m_uiVertexShader);
	glAttachShader(m_uiShaderProgram, m_uiFragmentShader);

	// Bind default attributes
	glBindAttribLocation(m_uiShaderProgram, 0, "Vertex");
	glBindAttribLocation(m_uiShaderProgram, 1, "TexCoord");
	glBindAttribLocation(m_uiShaderProgram, 2, "Normal");

	// Link
	GLint Status;
	glLinkProgram(m_uiShaderProgram);
	glGetProgramiv(m_uiShaderProgram, GL_LINK_STATUS, &Status);
	if (Status == GL_FALSE) {
		GLchar* ErrorLog = new GLchar[1024];
		glGetProgramInfoLog(m_uiShaderProgram, 1024, NULL, ErrorLog);
		CWindowManager::Error("Shader Link Error", ErrorLog);
	}

	// Validate
	glValidateProgram(m_uiShaderProgram);
	glGetProgramiv(m_uiShaderProgram, GL_VALIDATE_STATUS, &Status);
	if (Status == GL_FALSE) {
		GLchar* ErrorLog = new GLchar[1024];
		glGetProgramInfoLog(m_uiShaderProgram, 1024, NULL, ErrorLog);
		CWindowManager::Error("Shader Validate Error", ErrorLog);
	}

	Use();
	m_iModelLocation = glGetUniformLocation(GetProgramID(), "Model");
	m_iViewLocation = glGetUniformLocation(GetProgramID(), "View");
	m_iProjectionLocation = glGetUniformLocation(GetProgramID(), "Projection");
	m_iNormalMatrixLocation = glGetUniformLocation(GetProgramID(), "NormalMatrix");
	m_iTextUVLocation = glGetUniformLocation(GetProgramID(), "TextUV");
	m_iRenderingStageLocation = glGetUniformLocation(GetProgramID(), "RenderingStage");
	m_iLightPositionLocation = glGetUniformLocation(GetProgramID(), "LightPosition");
	m_iColourLocation = glGetUniformLocation(GetProgramID(), "Colour");
	m_iPausedLocation = glGetUniformLocation(GetProgramID(), "Paused");
	SetModel(glm::mat4(1.0f));
	SetView(glm::mat4(1.0f));
	SetRenderingStage(0);
	SetLightPosition(glm::vec3(0.0f));
	SetColour(glm::vec3(1.0f));
	SetPaused(false);

    glUniform1i(glGetUniformLocation(GetProgramID(), "Texture"), 0);
    glUniform1i(glGetUniformLocation(GetProgramID(), "NormalMap"), 1);

	SetProjection(CRenderer::GetDefaultPerspective());
}

CShader::~CShader() {
	glUseProgram(0);
	glDetachShader(m_uiShaderProgram, m_uiVertexShader);
	glDetachShader(m_uiShaderProgram, m_uiFragmentShader);
	glDeleteShader(m_uiVertexShader);
	glDeleteShader(m_uiFragmentShader);
	glDeleteProgram(m_uiShaderProgram);
}

void CShader::Use() {
	glUseProgram(m_uiShaderProgram);
}

bool CShader::LoadShader(GLuint* Shader, GLenum ShaderType, const char* Source) {
	*Shader = glCreateShader(ShaderType);
	glShaderSource(*Shader, 1, &Source, NULL);
	glCompileShader(*Shader);

	GLint ShaderStatus;
	glGetShaderiv(*Shader, GL_COMPILE_STATUS, &ShaderStatus);
	if (ShaderStatus == GL_TRUE) return true;
	else {
		GLchar* ErrorLog = new GLchar[1024];
		glGetShaderInfoLog(*Shader, 1024, NULL, ErrorLog);
		CWindowManager::Error((ShaderType == GL_VERTEX_SHADER ? "Vertex Shader Error" : "Fragment Shader Error"), ErrorLog);
		return false;
	}
}

GLuint CShader::GetProgramID() {
	return m_uiShaderProgram;
}

void CShader::SetModel(glm::mat4 Model) {
    m_mat4Model = Model;
    glUniformMatrix4fv(m_iModelLocation, 1, GL_FALSE, glm::value_ptr(Model));
    glUniformMatrix3fv(m_iNormalMatrixLocation, 1, GL_FALSE, glm::value_ptr(glm::inverseTranspose(glm::mat3(m_mat4View * m_mat4Model))));
}

void CShader::SetView(glm::mat4 View) {
    m_mat4View = View;
    glUniformMatrix4fv(m_iViewLocation, 1, GL_FALSE, glm::value_ptr(View));
    glUniformMatrix3fv(m_iNormalMatrixLocation, 1, GL_FALSE, glm::value_ptr(glm::inverseTranspose(glm::mat3(m_mat4View * m_mat4Model))));
}

void CShader::SetLightPosition(glm::vec3 position) {
    glm::vec3 transformedLightPosition = glm::vec3(m_mat4View * m_mat4Model * glm::vec4(position, 1.0f));
    glUniform3fv(m_iLightPositionLocation, 1, glm::value_ptr(transformedLightPosition));
}
