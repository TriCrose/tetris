#ifndef CFONT_H
#define CFONT_H

#include <glm/glm.hpp>
#include "CWindowManager.h"

struct GlyphSheet {
    int Width, Height;
    HDC MemoryDC;
    HBITMAP MemoryBMP;
    HBITMAP OldBMP;
    HFONT OldFont;
};

struct GlyphSheetFile {
    int width, height;
    unsigned char* Data;
    glm::vec4* TexCoords;
};

class CFont {
private:
    const char* m_szFontName;
    int m_iHeight;
    GLuint m_iGlyphSheet;
    int m_iSheetWidth, m_iSheetHeight;
    glm::vec4* m_TexCoords;     // 256-element array
    GlyphSheet m_Sheet;

    HFONT LoadFont();
    GlyphSheet CreateBlankGlyphSheet(HFONT font);
    void DestroyGlyphSheet(GlyphSheet sheet);
    void DrawCharsToGlyphSheet(GlyphSheet sheet);
    GLuint UploadGlyphSheetToCPU(GlyphSheet sheet);

    static GlyphSheetFile GetGSF(GlyphSheet sheet, glm::vec4* TexCoords);
public:
    CFont(const char* FontName, int Height);
    CFont(char* FileInMemory);
    bool SaveGlyphSheet(const char* Filename);

    GLuint GetGlyphSheet() { return m_iGlyphSheet; }
    int GetSheetWidth() { return m_iSheetWidth; }
    int GetSheetHeight() { return m_iSheetHeight; }
    int GetTextWidth(const char* text);
    int GetGlyphHeight() { return m_iHeight; }
    glm::vec4* GetTexCoords() { return m_TexCoords; }
    virtual ~CFont();
};

#endif // CFONT_H
