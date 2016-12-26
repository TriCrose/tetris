#define GLEW_STATIC
#include <glew/glew.h>
#include <gl/gl.h>

#include <limits.h>
#include <math.h>
#include <fstream>

#include "CFont.h"

CFont::CFont(const char* FontName, int Height) {
    m_szFontName = FontName;
    m_iHeight = Height;
    m_TexCoords = new glm::vec4[256];

    m_Sheet = CreateBlankGlyphSheet(LoadFont());
    DrawCharsToGlyphSheet(m_Sheet);
    m_iGlyphSheet = UploadGlyphSheetToCPU(m_Sheet);
}

CFont::CFont(char* FileInMemory) {
    int offset = 0;

    m_iSheetWidth = *((int*)&FileInMemory[0]);
    offset += sizeof(m_iSheetWidth);
    m_iSheetHeight = *((int*)&FileInMemory[offset]);
    offset += sizeof(m_iSheetHeight);
    m_iHeight = *((int*)&FileInMemory[offset]);
    offset += sizeof(m_iHeight);
    unsigned char* Data = (unsigned char *) &FileInMemory[offset];
    offset += m_iSheetWidth * m_iSheetHeight * 4;
    m_TexCoords = (glm::vec4*) &FileInMemory[offset];

	glGenTextures(1, &m_iGlyphSheet);
	glBindTexture(GL_TEXTURE_2D, m_iGlyphSheet);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_iSheetWidth, m_iSheetHeight, 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, Data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	delete[] Data;
}

CFont::~CFont() {
    delete[] m_TexCoords;
    DestroyGlyphSheet(m_Sheet);
    glDeleteTextures(1, &m_iGlyphSheet);
}

bool CFont::SaveGlyphSheet(const char* Filename) {
    GlyphSheetFile gsf = GetGSF(m_Sheet, m_TexCoords);
    std::ofstream file(Filename, std::ios::out | std::ios::binary);
    if (!file.is_open()) return false;
    file.write((char*) &gsf.width, sizeof(gsf.width));
    file.write((char*) &gsf.height, sizeof(gsf.height));
    file.write((char*) &m_iHeight, sizeof(m_iHeight));
    file.write((char*) gsf.Data, gsf.width * gsf.height * 4);
    file.write((char*) gsf.TexCoords, 256 * sizeof(glm::vec4));
    delete[] gsf.Data;
    return true;
}

GlyphSheetFile CFont::GetGSF(GlyphSheet sheet, glm::vec4* TexCoords) {
    GlyphSheetFile gsf = {};
    BITMAPINFO bmi = {};
    bmi.bmiHeader.biSize = sizeof(bmi.bmiHeader);
    GetDIBits(sheet.MemoryDC, sheet.MemoryBMP, 0, 1, NULL, &bmi, DIB_RGB_COLORS);
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;
    bmi.bmiHeader.biHeight = abs(bmi.bmiHeader.biHeight);
    unsigned char* Data = new unsigned char[bmi.bmiHeader.biSizeImage];

    GetDIBits(sheet.MemoryDC, sheet.MemoryBMP, 0, sheet.Height, Data, &bmi, DIB_RGB_COLORS);

    gsf.width = sheet.Width;
    gsf.height = sheet.Height;
    gsf.Data = Data;
    gsf.TexCoords = TexCoords;
    return gsf;
}

HFONT CFont::LoadFont() {
    HFONT font = CreateFont(m_iHeight, 0, 0, 0, FW_NORMAL, false, false, false, ANSI_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, FF_DONTCARE|DEFAULT_PITCH, m_szFontName);
    HFONT OldFont = (HFONT) SelectObject(CWindowManager::GetWindowDC(), font);
    TEXTMETRIC tm;
    GetTextMetrics(CWindowManager::GetWindowDC(), &tm);
    SIZE CharSize;
    int TotalArea = 0;
    for (char ch = CHAR_MIN; ch <= CHAR_MAX - 1; ch++) {
        GetTextExtentPoint32(CWindowManager::GetWindowDC(), &ch, 1, &CharSize);
        TotalArea += CharSize.cx * CharSize.cy;
    }
    TotalArea += 16 * m_iHeight * tm.tmMaxCharWidth;
    int TotalAreaSideLength = ceil(sqrt((double) TotalArea));
    m_iSheetWidth = TotalAreaSideLength;
    m_iSheetHeight = TotalAreaSideLength + 2 * m_iHeight;
    SelectObject(CWindowManager::GetWindowDC(), OldFont);
    return font;
}

GlyphSheet CFont::CreateBlankGlyphSheet(HFONT font) {
    GlyphSheet sheet;
    sheet.Width = m_iSheetWidth;
    sheet.Height = m_iSheetHeight;
    sheet.MemoryDC = CreateCompatibleDC(CWindowManager::GetWindowDC());
    sheet.MemoryBMP = CreateCompatibleBitmap(CWindowManager::GetWindowDC(), sheet.Width, sheet.Height);
    sheet.OldBMP = (HBITMAP) SelectObject(sheet.MemoryDC, sheet.MemoryBMP);
    sheet.OldFont = (HFONT) SelectObject(sheet.MemoryDC, font);

    SetBkColor(sheet.MemoryDC, RGB(255, 0, 0));
    SetTextColor(sheet.MemoryDC, RGB(255, 255, 255));
    return sheet;
}

void CFont::DestroyGlyphSheet(GlyphSheet sheet) {
    SelectObject(sheet.MemoryDC, sheet.OldBMP);
    SelectObject(sheet.MemoryDC, sheet.OldFont);
    DeleteObject(sheet.MemoryBMP);
    DeleteObject(sheet.MemoryDC);
}

void CFont::DrawCharsToGlyphSheet(GlyphSheet sheet) {
    int xOffset = 0, yOffset = 0;
    glm::vec4 texCoord;
    for (char ch = CHAR_MIN; ch <= CHAR_MAX - 1; ch++) {
        TextOut(sheet.MemoryDC, xOffset, yOffset, &ch, 1);
        SIZE CharSize;
        GetTextExtentPoint32(sheet.MemoryDC, &ch, 1, &CharSize);
        texCoord.x = float(xOffset)/float(sheet.Width);
        texCoord.y = 1.0f - float(yOffset + CharSize.cy)/float(sheet.Height);
        texCoord.z = float(xOffset + CharSize.cx)/float(sheet.Width);
        texCoord.w = 1.0f - float(yOffset)/float(sheet.Height);
        m_TexCoords[int(ch) - CHAR_MIN] = texCoord;
        xOffset += CharSize.cx + (ch == 'i'?CharSize.cx/5:0);
        int NextCharWidth;
        if (ch < 255) {
            ch++;
            GetTextExtentPoint32(sheet.MemoryDC, &ch, 1, &CharSize);
            NextCharWidth = CharSize.cx;
            ch--;
        } else NextCharWidth = 0;
        if (xOffset + NextCharWidth > sheet.Width) {
            xOffset = 0;
            yOffset += m_iHeight;
        }
    }
}

GLuint CFont::UploadGlyphSheetToCPU(GlyphSheet sheet) {
    GlyphSheetFile gsf = GetGSF(sheet, NULL);
    GLuint TextureID;
	glGenTextures(1, &TextureID);
	glBindTexture(GL_TEXTURE_2D, TextureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, gsf.width, gsf.height, 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, gsf.Data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	delete[] gsf.Data;
	return TextureID;
}

int CFont::GetTextWidth(const char* text) {
    int width = 0;
    glm::vec4 texCoords;
    for (const char *p = text; *p; p++) {
        texCoords = m_TexCoords[*p - CHAR_MIN];
        int glyphWidth = (texCoords.z - texCoords.x) * m_iSheetWidth;
        width += glyphWidth;
    }
    return width;
}
