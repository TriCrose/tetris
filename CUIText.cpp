#define GLEW_STATIC
#include <glew/glew.h>
#include <gl/gl.h>

#include "CWindowManager.h"
#include "CRenderer.h"
#include "CUIText.h"

CUIText::CUIText(CFont* font, const char* text, int x, int y, int origin, float scale) {
    m_Font = font;
    m_szText = text;
    m_fScale = scale;
    m_iX = x;
    m_iY = y;
    m_Origin = (CRenderer::ORIGIN) origin;
    m_bSelected = false;
    int width = font->GetTextWidth(text) * scale;
    int height = font->GetGlyphHeight() * scale;

    switch (m_Origin) {
    case CRenderer::BLEFT:
        m_Bounds.left = x;
        m_Bounds.right = x + width;
        m_Bounds.top = CWindowManager::GetClientHeight() - (y + height);
        m_Bounds.bottom = CWindowManager::GetClientHeight() - y;
        break;
    case CRenderer::TLEFT:
        m_Bounds.left = x;
        m_Bounds.right = x + width;
        m_Bounds.top = CWindowManager::GetClientHeight() - y;
        m_Bounds.bottom = CWindowManager::GetClientHeight() - (y - height);
        break;
    case CRenderer::TRIGHT:
        m_Bounds.left = x - width;
        m_Bounds.right = x;
        m_Bounds.top = CWindowManager::GetClientHeight() - y;
        m_Bounds.bottom = CWindowManager::GetClientHeight() - (y - height);
        break;
    case CRenderer::BRIGHT:
        m_Bounds.left = x - width;
        m_Bounds.right = x;
        m_Bounds.top = CWindowManager::GetClientHeight() - (y + height);
        m_Bounds.bottom = CWindowManager::GetClientHeight() - y;
        break;
    case CRenderer::CENTRE:
        m_Bounds.left = x - width/2;
        m_Bounds.right = x + width/2;
        m_Bounds.top = CWindowManager::GetClientHeight() - (y + height/2);
        m_Bounds.bottom = CWindowManager::GetClientHeight() - (y - height/2);
        break;
    }
}

void CUIText::Update(int mouseX, int mouseY) {
    if (mouseX >= m_Bounds.left && mouseX <= m_Bounds.right && mouseY >= m_Bounds.top && mouseY <= m_Bounds.bottom)
        m_bSelected = true;
    else
        m_bSelected = false;
}
