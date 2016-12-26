#define GLEW_STATIC
#include <glew/glew.h>
#include <gl/gl.h>
#include <glm/gtc/matrix_transform.hpp>

#include "StringFunctions.h"
#include "CWindowManager.h"
#include "CRenderer.h"
#include "Skybox.h"

glm::mat4 CRenderer::m_DefaultPerspective;
glm::mat4 CRenderer::m_DefaultOrtho;
CShader* CRenderer::m_Shader;
CMesh* CRenderer::m_mshFloor;
GLuint CRenderer::m_uiFloorTexture;
GLuint CRenderer::m_uiNormalMap;
CMesh* CRenderer::m_mshSkybox;
GLuint CRenderer::m_uiSkyboxTexture;
GLuint CRenderer::m_uiBlockImageTextures[7];
CMesh* CRenderer::m_mshGrid;
CMesh* CRenderer::m_mshBlock;
CMesh* CRenderer::m_mshMarker;
CMesh* CRenderer::m_Quad;
CGrid* CRenderer::m_pGrid;
CFont* CRenderer::m_Font;
GLuint CRenderer::m_uiBackgroundImage;
GLuint CRenderer::m_uiHelpImage;
GLuint CRenderer::m_uiTitleImage;
std::vector<CUIText*> CRenderer::m_UITexts;

void CRenderer::Initialize(CGrid* grid) {
    glEnable(GL_DEPTH_TEST);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glViewport(0, 0, (GLsizei) CWindowManager::GetClientWidth(), (GLsizei) CWindowManager::GetClientHeight());
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);
	glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	glewExperimental = GL_TRUE;
	glewInit();

	m_pGrid = grid;

    float FOV = 60.0f, Near = 0.5f, Far = 400.0f;
	m_DefaultPerspective = glm::perspective(
        glm::radians(FOV),
        float(CWindowManager::GetClientWidth()) / float(CWindowManager::GetClientHeight()),
        Near,
        Far
    );
    m_DefaultOrtho = glm::ortho(0.0f, (float) CWindowManager::GetClientWidth(), 0.0f, (float) CWindowManager::GetClientHeight(), -1.0f, 1.0f);


    float QuadVertices[] = {
        0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f
    };
    m_Quad = new CMesh(QuadVertices, sizeof(QuadVertices), false, false, GL_STATIC_DRAW);
}

void CRenderer::LoadResources(GLuint FloorTexture, GLuint NormalMap, GLuint SkyboxTexture, CMesh* GridMesh, CMesh* BlockMesh, CMesh* MarkerMesh, char* MemGSF, GLuint BackgroundImage, GLuint HelpImage, GLuint TitleImage, std::vector<CUIText*> UITexts) {
    m_uiFloorTexture = FloorTexture;
    m_uiNormalMap = NormalMap;
    m_uiSkyboxTexture = SkyboxTexture;
    m_uiBackgroundImage = BackgroundImage;
    m_uiHelpImage = HelpImage;
    m_uiTitleImage = TitleImage;
    m_UITexts = UITexts;

    m_mshGrid = GridMesh;
    m_mshBlock = BlockMesh;
    m_mshMarker = MarkerMesh;

    m_Shader = new CShader(VertexSource, FragmentSource);
	glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_uiNormalMap);
    glActiveTexture(GL_TEXTURE0);

    float FLOOR_SIZE = 300.0f;
    float FLOOR_HEIGHT = 0.0f;
    float FLOOR_TILE = 90.0f;
    float FloorVertices[] = {
		-FLOOR_SIZE, FLOOR_HEIGHT, -FLOOR_SIZE,		0.0f, FLOOR_TILE,
		-FLOOR_SIZE, FLOOR_HEIGHT,  FLOOR_SIZE,		0.0f, 0.0f,
		 FLOOR_SIZE, FLOOR_HEIGHT, -FLOOR_SIZE,		FLOOR_TILE, FLOOR_TILE,
		-FLOOR_SIZE, FLOOR_HEIGHT,  FLOOR_SIZE,		0.0f, 0.0f,
		 FLOOR_SIZE, FLOOR_HEIGHT,  FLOOR_SIZE,		FLOOR_TILE, 0.0f,
		 FLOOR_SIZE, FLOOR_HEIGHT, -FLOOR_SIZE,		FLOOR_TILE, FLOOR_TILE
	};
	m_mshFloor = new CMesh(FloorVertices, sizeof(FloorVertices), false, true);

	float SKYBOX_SIZE = 230.0f;
	m_mshSkybox = CreateSkyboxMesh(SKYBOX_SIZE, m_uiSkyboxTexture);

	m_Font = new CFont(MemGSF);
}

void CRenderer::LoadBlockTextureImages(GLuint cyan, GLuint blue, GLuint orange, GLuint yellow, GLuint green, GLuint magenta, GLuint red) {
    m_uiBlockImageTextures[0] = cyan;
    m_uiBlockImageTextures[1] = blue;
    m_uiBlockImageTextures[2] = orange;
    m_uiBlockImageTextures[3] = yellow;
    m_uiBlockImageTextures[4] = green;
    m_uiBlockImageTextures[5] = magenta;
    m_uiBlockImageTextures[6] = red;
}

void CRenderer::CleanUp() {
    glDeleteTextures(1, &m_uiFloorTexture);
    glDeleteTextures(1, &m_uiNormalMap);
    glDeleteTextures(1, &m_uiSkyboxTexture);
    for (int i = 0; i < 7; i++) glDeleteTextures(1, &m_uiBlockImageTextures[i]);
    delete m_Font;
    delete m_Quad;
    delete m_mshMarker;
    delete m_mshBlock;
    delete m_mshGrid;
    delete m_mshSkybox;
    delete m_mshFloor;
    delete m_Shader;
}

void CRenderer::RenderAll(CCamera* camera) {
    m_Shader->Use();
    m_Shader->SetModel(glm::mat4());
    m_Shader->SetView(camera->GetMatrix());
    m_Shader->SetProjection(m_DefaultPerspective);
    m_Shader->SetLightPosition(camera->GetPosition() + glm::vec3(0.0f, 2.0f, 0.0f));

    // Draw grid and blocks
    static glm::mat4 Model = glm::translate(glm::mat4(), glm::vec3(-4.0f, 0.0f, 4.0f));
    m_Shader->SetModel(Model);
    m_Shader->SetRenderingStage(1);
    glDisable(GL_CULL_FACE);
    m_Shader->SetColour(glm::vec3(1.0f));
    m_mshGrid->Draw();
    glEnable(GL_CULL_FACE);

    if (m_pGrid->GetCurrentAnimation().type == ANIMATION_PLANE) {
        float AnimationProportion = float(m_pGrid->GetCurrentAnimation().counter) / float(m_pGrid->GetCurrentAnimation().duration);
        int FullPlanesBelow = 0;
        for (int j = 0; j < GRID_HEIGHT; j++) {
            for (int i = 0; i < GRID_WIDTH; i++) for (int k = 0; k < GRID_DEPTH; k++) {
                if (m_pGrid->GetInvis(i, j, k)) continue;
                if (CheckValueInVector(j, m_pGrid->GetFullPlanes())) {
                    m_Shader->SetModel(
                        glm::translate(Model, glm::vec3(i, float(j) - float(FullPlanesBelow) * AnimationProportion, -k)) * glm::scale(glm::mat4(), glm::vec3(1.0f, 1.0f - AnimationProportion, 1.0f))
                    );
                } else m_Shader->SetModel(glm::translate(Model, glm::vec3(i, float(j) - float(FullPlanesBelow) * AnimationProportion, -k)));
                m_Shader->SetColour(m_pGrid->GetColour(i, j, k));
                m_mshBlock->Draw();
            }
            if (CheckValueInVector(j, m_pGrid->GetFullPlanes())) FullPlanesBelow++;
        }
    } else ITERATE_THROUGH_BLOCKS {
        if (m_pGrid->GetInvis(i, j, k)) continue;
        m_Shader->SetModel(glm::translate(Model, glm::vec3(i, j, -k)));
        m_Shader->SetColour(m_pGrid->GetColour(i, j, k));
        m_mshBlock->Draw();
    }

    // Draw grid marker
    m_Shader->SetRenderingStage(2);
    m_Shader->SetColour(glm::vec3(glm::sin(glm::radians(float(m_pGrid->GetMarkerCounter())/20.0f)) / 2.0f + 0.5f, 0.1f, 0.1f));
    ITERATE_THROUGH_BLOCKS if (m_pGrid->GetMoving(i, j, k)) {
        if (j != 0 && m_pGrid->GetInvis(i, j-1, k)) {
            int LowestY = j - 1;
            while (m_pGrid->GetInvis(i, LowestY-1, k) && LowestY > 0) LowestY--;
            m_Shader->SetModel(glm::translate(glm::translate(glm::mat4(1.0f), glm::vec3(-4.0f, 0.0f, 4.0f)), glm::vec3(GLfloat(i), GLfloat(LowestY), GLfloat(-k))));
            m_mshMarker->Draw();
        }
    }
    m_Shader->SetColour(glm::vec3(1.0f));

    // Draw skybox
    m_Shader->SetRenderingStage(3);
    m_Shader->SetModel(glm::translate(glm::mat4(), camera->GetPosition()));
    glBindTexture(GL_TEXTURE_2D, m_uiSkyboxTexture);
    m_mshSkybox->Draw();

    // Draw floor
    glEnable(GL_BLEND);
    m_Shader->SetModel(glm::mat4());
    m_Shader->SetRenderingStage(0);
    glBindTexture(GL_TEXTURE_2D, m_uiFloorTexture);
    m_mshFloor->Draw();

    // Draw UI
    m_Shader->SetRenderingStage(4);
    m_Shader->SetProjection(m_DefaultOrtho);
    m_Shader->SetView(glm::mat4());

    static float TextScale = float(CWindowManager::GetClientHeight())/float(1052);
    static int TextOffset = int(20.0f * TextScale);
    RenderText(m_Font, (std::string("Score: ") + ToString(m_pGrid->GetScore())).c_str(), TextOffset, 0, BLEFT, TextScale);
    RenderText(m_Font, (std::string("Selected Axis: ") + std::string(m_pGrid->m_iSelectedAxis==0?"X":(m_pGrid->m_iSelectedAxis==1?"Y":"Z"))).c_str(), TextOffset, CWindowManager::GetClientHeight(), TLEFT, TextScale);
    RenderText(m_Font, "Next Shape:", CWindowManager::GetClientWidth() - TextOffset, CWindowManager::GetClientHeight(), TRIGHT, TextScale);
    RenderText(m_Font, m_pGrid->GetTimerAsString().c_str(), CWindowManager::GetClientWidth() - TextOffset, 0, BRIGHT, TextScale);

    static int BlockImageWidth = int(float(256)*TextScale);
    RenderTexture(
        m_uiBlockImageTextures[(int) m_pGrid->GetNextShape() - 1],
        BlockImageWidth, int(float(256)*TextScale),
        CWindowManager::GetClientWidth() - TextOffset - (int(float(m_Font->GetTextWidth("Next Shape:"))*TextScale) - BlockImageWidth)/2, CWindowManager::GetClientHeight() - int(float(m_Font->GetGlyphHeight())*TextScale),
        glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), TRIGHT
    );
    glDisable(GL_BLEND);
}

void CRenderer::RenderTexture(GLuint TexID, int width, int height, int x, int y, glm::vec4 TexCoords, ORIGIN origin) {
    glBindTexture(GL_TEXTURE_2D, TexID);
    int newX = x, newY = y;
    switch (origin) {
    case BLEFT: break;
    case TLEFT:
        newY = y - height;
        break;
    case TRIGHT:
        newX = x - width;
        newY = y - height;
        break;
    case BRIGHT:
        newX = x - width;
        break;
    case CENTRE:
        newX = x - width/2;
        newY = y - height/2;
        break;
    }
    m_Shader->SetModel(
        glm::translate(glm::mat4(), glm::vec3(newX, newY, 0)) * glm::scale(glm::mat4(), glm::vec3(width, height, 1))
    );
    m_Shader->SetTextUV(TexCoords);
    m_Quad->Draw();
}

void CRenderer::RenderText(CFont* font, const char* text, int x, int y, ORIGIN origin, float scale) {
    int xOffset = 0, newX = x, newY = y;
    switch (origin) {
    case BLEFT: break;
    case TLEFT:
        newY = y - int(scale*float(font->GetGlyphHeight()));
        break;
    case TRIGHT:
        newX = x - int(scale*float(font->GetTextWidth(text)));
        newY = y - int(scale*float(font->GetGlyphHeight()));
        break;
    case BRIGHT:
        newX = x - int(scale*float(font->GetTextWidth(text)));
        break;
    case CENTRE:
        newX = x - int(scale*float(font->GetTextWidth(text)))/2;
        newY = y - int(scale*float(font->GetGlyphHeight()))/2;
        break;
    }

    int glyphHeight = (font->GetTexCoords()[0].w - font->GetTexCoords()[0].y) * font->GetSheetHeight();
    for (const char *p = text; *p; p++) {
        glm::vec4 texCoords = font->GetTexCoords()[*p - CHAR_MIN];
        int glyphWidth = (texCoords.z - texCoords.x) * font->GetSheetWidth();
        RenderTexture(font->GetGlyphSheet(), int(scale*float(glyphWidth)), int(scale*float(glyphHeight)), newX + int(scale*float(xOffset)), newY, texCoords);
        xOffset += glyphWidth;
    }
}

void CRenderer::SetPaused(bool paused) {
    m_Shader->SetPaused(paused);
}
