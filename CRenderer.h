#ifndef CRENDERER_H
#define CRENDERER_H

#include <vector>

#include "CGrid.h"
#include "CMesh.h"
#include "CShader.h"
#include "CCamera.h"
#include "CFont.h"

class CUIText;
class CRenderer {
    friend class CUIText;
private:
    static glm::mat4 m_DefaultPerspective;
    static glm::mat4 m_DefaultOrtho;

    static CShader* m_Shader;
    static const char *VertexSource, *FragmentSource;

    static CMesh* m_mshFloor;
    static GLuint m_uiFloorTexture, m_uiNormalMap;

    static CMesh* m_mshSkybox;
    static GLuint m_uiSkyboxTexture;

    static GLuint m_uiBlockImageTextures[7];
    static GLuint m_uiBackgroundImage;
    static GLuint m_uiHelpImage;
    static GLuint m_uiTitleImage;

    static CMesh *m_mshGrid, *m_mshBlock, *m_mshMarker;

    static CGrid* m_pGrid;

    enum ORIGIN {
        BLEFT = 0, TLEFT, TRIGHT, BRIGHT, CENTRE
    };

    static CFont* m_Font;
    static CMesh* m_Quad;
    static std::vector<CUIText*> m_UITexts;

    static void RenderTexture(GLuint TexID, int width, int height, int x, int y, glm::vec4 TexCoords = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), ORIGIN origin = BLEFT);
    static void RenderText(CFont* font, const char* text, int x, int y, ORIGIN origin = BLEFT, float scale = 1.0f);
    static void RenderUIText(CUIText* text);

    static bool CheckValueInVector(int i, std::vector<int> v) { for (unsigned ctr = 0; ctr < v.size(); ctr++) if (v[ctr] == i) return true; return false; }
public:
    static void Initialize(CGrid* grid);
    static glm::mat4 GetDefaultPerspective() { return m_DefaultPerspective; }
    static glm::mat4 GetDefaultOrtho() { return m_DefaultOrtho; }
    static void LoadResources(GLuint FloorTexture, GLuint NormalMap, GLuint SkyboxTexture, CMesh* GridMesh, CMesh* BlockMesh, CMesh* MarkerMesh, char* MemGSF, GLuint BackgroundImage, GLuint HelpImage, GLuint TitleImage, std::vector<CUIText*> UITexts);
    static void LoadBlockTextureImages(GLuint cyan, GLuint blue, GLuint orange, GLuint yellow, GLuint green, GLuint magenta, GLuint red);
    static void CleanUp();
    static void RenderAll(CCamera* camera);
    static void SetPaused(bool paused);
    static void RenderCountdown(int Timer);
    static void RenderTitleScreen(bool HelpImage);
    static void RenderOverlayScreen(bool GameOver);
};

#endif // CRENDERER_H
