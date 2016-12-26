#define GLEW_STATIC
#include <glew/glew.h>
#include <gl/gl.h>

#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <iostream>

#include "CWindowManager.h"
#include "CResourceManager.h"

GLuint CResourceManager::LoadTGAFromFile(const char* Filename) {
	char* Bytes;
	std::ifstream File(Filename, std::ios::in | std::ios::binary | std::ios::ate);
	if (File.is_open()) {
		std::streampos size = File.tellg();
		Bytes = new char[size];
		File.seekg(0, std::ios::beg);
		File.read(Bytes, size);
		File.close();
	} else {
		CWindowManager::Error("TGA Image Error", "Cannot open TGA file.");
		return 0;
	}

	GLuint TextureID = LoadTGAFromMemory(Bytes);
	delete[] Bytes;
	return TextureID;
}

GLuint CResourceManager::LoadTGAFromMemory(char* Bytes) {
	char Header[12];
	char ImageInfo[6];
	char *Data;
	int Width, Height, BitDepth;
	char UncompressedTGA[12] = { 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

	for (int i = 0; i < 12; i++) Header[i] = Bytes[i];
	for (int i = 0; i < 6; i++) ImageInfo[i] = Bytes[12 + i];
	Width = ImageInfo[1] * 256 + ImageInfo[0];
	Height = ImageInfo[3] * 256 + ImageInfo[2];
	BitDepth = ImageInfo[4];
	int TGASize = Width * Height * BitDepth / 8;
	Data = new char[TGASize];

	if (BitDepth != 24 && BitDepth != 32) {
		CWindowManager::Error("TGA Image Error", "TGA is not 24/32-bit.");
		return 0;
	}

	if (memcmp(Header, UncompressedTGA, 12) == 0) {
		for (int i = 0; i < TGASize; i++) Data[i] = Bytes[18 + i];
	} else {
		CWindowManager::Error("TGA Image Error", "Not a valid TGA file.");
		return 0;
	}

	GLuint TextureID;
	glGenTextures(1, &TextureID);
	glBindTexture(GL_TEXTURE_2D, TextureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, BitDepth == 24 ? GL_RGB : GL_RGBA, Width, Height, 0, BitDepth == 24 ? GL_BGR_EXT : GL_BGRA_EXT, GL_UNSIGNED_BYTE, Data);
	delete Data;
	return TextureID;
}

CMesh* CResourceManager::LoadTModelFromFile(const char* Filename) {
    char* Bytes;
    std::streampos Size;
	std::ifstream File(Filename, std::ios::in | std::ios::binary | std::ios::ate);
	if (File.is_open()) {
		Size = File.tellg();
		Bytes = new char[Size];
		File.seekg(0, std::ios::beg);
		File.read(Bytes, Size);
		File.close();
	} else {
		CWindowManager::Error("TModel File Error", "Cannot open TModel file.");
		return 0;
	}

	CMesh* mesh = LoadTModelFromMemory(Bytes, Size);
	delete[] Bytes;
	return mesh;
}

std::vector<std::string> SplitString(std::string str, char delim) {
    std::stringstream ss(str);
    std::string temp;
    std::vector<std::string> strings;
    while (std::getline(ss, temp, delim)) strings.push_back(temp);
    return strings;
}

float StringToFloat(std::string str) {
    float result;
    std::stringstream ss(str);
    ss >> result;
    return result;
}

CMesh* CResourceManager::LoadTModelFromMemory(char* Bytes, int Size) {
    char* cString = new char[Size+1];
    for (int i = 0; i < Size; i++) cString[i] = Bytes[i];
    cString[Size] = '\0';
    std::stringstream ss(cString);
    std::string line;

    std::vector<float> fVertices;
    std::vector<std::string> tempStrVertices;

    bool ReadingVertices = false;
    while (std::getline(ss, line, '\n')) {
        line.erase(std::remove(line.begin(), line.end(), '\t'), line.end());
        if (line.compare("EndMeshData") == 0) ReadingVertices = false;
        if (ReadingVertices) {
            tempStrVertices = SplitString(line, ' ');
            for (int i = 0; i < 6; i++) fVertices.push_back(StringToFloat(tempStrVertices[i]));
        }
        if (line.compare("MeshData") == 0) ReadingVertices = true;
    }

    return new CMesh((float*) &fVertices[0], fVertices.size() * 4, true, false);
}

char* CResourceManager::LoadFileIntoMemory(const char* Filename) {
    char* Bytes;
    std::streampos Size;
	std::ifstream File(Filename, std::ios::in | std::ios::binary | std::ios::ate);
	if (File.is_open()) {
		Size = File.tellg();
		Bytes = new char[Size];
		File.seekg(0, std::ios::beg);
		File.read(Bytes, Size);
		File.close();
	} else {
		CWindowManager::Error("File error", "Cannot open file.");
		return NULL;
	}
	return Bytes;
}
