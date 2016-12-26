#ifndef CUITEXT_H
#define CUITEXT_H

class CUIText {
    friend class CRenderer;
private:
    CFont* m_Font;
    const char* m_szText;
    float m_fScale;
    int m_iX, m_iY;
    CRenderer::ORIGIN m_Origin;
    bool m_bSelected;

    RECT m_Bounds;
public:
    CUIText(CFont* font, const char* text, int x, int y, int origin, float scale);
    void Update(int mouseX, int mouseY);

    bool GetSelected() { return m_bSelected; }
};

#endif // CUITEXT_H
