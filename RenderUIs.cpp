#define GLEW_STATIC
#include <glew/glew.h>
#include <gl/gl.h>

#include "CRenderer.h"
#include "CUIText.h"

void CRenderer::RenderUIText(CUIText* text) {
    RenderText(text->m_Font, text->m_szText, text->m_iX, text->m_iY, text->m_Origin, text->m_fScale);
}

void CRenderer::RenderTitleScreen(bool HelpImage) {
    glEnable(GL_BLEND);
    m_Shader->Use();
    m_Shader->SetModel(glm::mat4());
    m_Shader->SetView(glm::mat4());
    m_Shader->SetColour(glm::vec3(1.0f));
    m_Shader->SetProjection(m_DefaultOrtho);
    m_Shader->SetRenderingStage(4);

    static float TextScale = float(CWindowManager::GetClientHeight())/float(1052);
    static float SmallTextScale = TextScale/1.5f;
    static float aspect = 16.0f/9.0f;
    static int width = CWindowManager::GetClientWidth();
    static int height = CWindowManager::GetClientHeight();
    int image_width = width;
    int image_height = image_width / aspect;
    if (image_height < height) {
        image_height = height;
        image_width = image_height * aspect;
    }
    RenderTexture(m_uiBackgroundImage, image_width, image_height, width/2, height/2, glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), CENTRE);

    glClear(GL_DEPTH_BUFFER_BIT);
    if (HelpImage) {
        if (m_UITexts[6]->m_bSelected) m_Shader->SetColour(glm::vec3(1.0f, 1.0f, 0.4f));
        RenderUIText(m_UITexts[6]);
        m_Shader->SetColour(glm::vec3(1.0f));
        static float help_aspect = 1.3671875f;
        static int help_width = height * help_aspect;
        RenderTexture(m_uiHelpImage, help_width, height, width/2, height/2, glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), CENTRE);
    } else {
        RenderText(m_Font, "A game by TriCrose", CWindowManager::GetClientWidth(), 0, BRIGHT, SmallTextScale);
        RenderText(m_Font, "(C) 2015", 0, 0, BLEFT, SmallTextScale);
        RenderTexture(m_uiTitleImage, 512 * TextScale * 0.9f, 256 * TextScale * 0.9f, width/2 + TextScale*17, height/2 + TextScale*260, glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), CENTRE);
        for (unsigned i = 0; i < 3; i++) {
            if (m_UITexts[i]->m_bSelected) m_Shader->SetColour(glm::vec3(1.0f, 1.0f, 0.4f));
            RenderUIText(m_UITexts[i]);
            m_Shader->SetColour(glm::vec3(1.0f));
        }
    }

    glDisable(GL_BLEND);
}

void CRenderer::RenderOverlayScreen(bool GameOver) {
    glEnable(GL_BLEND);
    m_Shader->Use();
    m_Shader->SetModel(glm::mat4());
    m_Shader->SetView(glm::mat4());
    m_Shader->SetColour(glm::vec3(1.0f));
    m_Shader->SetProjection(m_DefaultOrtho);
    m_Shader->SetRenderingStage(4);
    m_Shader->SetPaused(false);

    static float TextScale = 1.5f * float(CWindowManager::GetClientHeight())/float(1052);
    if (GameOver) RenderText(m_Font, "Game Over", CWindowManager::GetClientWidth()/2, CWindowManager::GetClientHeight()/2 + TextScale*200, CENTRE, TextScale);
    else RenderText(m_Font, "Game Paused", CWindowManager::GetClientWidth()/2, CWindowManager::GetClientHeight()/2 + TextScale*200, CENTRE, TextScale);
    for (unsigned i = 3; i < 6; i++) {
        if (m_UITexts[i]->m_bSelected) m_Shader->SetColour(glm::vec3(1.0f, 1.0f, 0.4f));
        RenderUIText(m_UITexts[i]);
        m_Shader->SetColour(glm::vec3(1.0f));
    }

    m_Shader->SetPaused(true);
    glDisable(GL_BLEND);
}

void CRenderer::RenderCountdown(int Timer) {
    glEnable(GL_BLEND);
    m_Shader->Use();
    m_Shader->SetModel(glm::mat4());
    m_Shader->SetView(glm::mat4());
    m_Shader->SetColour(glm::vec3(1.0f));
    m_Shader->SetProjection(m_DefaultOrtho);
    m_Shader->SetRenderingStage(4);
    m_Shader->SetPaused(false);

    static float TextScale = 1.5f * float(CWindowManager::GetClientHeight())/float(1052);
    RenderText(m_Font, Timer<=1500/3?"1":(Timer<=1000?"2":"3"), CWindowManager::GetClientWidth()/2, CWindowManager::GetClientHeight()/2, CENTRE, TextScale);

    m_Shader->SetPaused(true);
    glDisable(GL_BLEND);
}
