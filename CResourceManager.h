#ifndef CRESOURCEMANAGER_H
#define CRESOURCEMANAGER_H

#include "CMesh.h"

class CResourceManager {
public:
	// Only 24/32-bit uncompressed TGAs supported
	static GLuint LoadTGAFromFile(const char* Filename);
	static GLuint LoadTGAFromMemory(char* Bytes);
	static CMesh* LoadTModelFromFile(const char* Filename);
	static CMesh* LoadTModelFromMemory(char* Bytes, int Size);
	static char* LoadFileIntoMemory(const char* Filename);
};

#endif // CRESOURCEMANAGER_H
