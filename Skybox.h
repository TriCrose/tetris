#ifndef SOURCE_SKYBOXCREATOR_H_
#define SOURCE_SKYBOXCREATOR_H_

CMesh* CreateSkyboxMesh(float Size, GLuint TextureID) {
	float Vertices[] = {
		// Front
		-Size, -Size, -Size,	0.250f, 0.333f,
		 Size,  Size, -Size,	0.500f, 0.667f,
		-Size,  Size, -Size,	0.250f, 0.667f,
		 Size, -Size, -Size,	0.500f, 0.333f,
		 Size,  Size, -Size,	0.500f, 0.667f,
		-Size, -Size, -Size,	0.250f, 0.333f,

		// Left
		-Size,  Size,  Size,	0.000f, 0.6662f,
		-Size, -Size, -Size,	0.250f, 0.334f,
		-Size,  Size, -Size,	0.250f, 0.6662f,
		-Size, -Size,  Size,	0.000f, 0.334f,
		-Size, -Size, -Size,	0.250f, 0.334f,
		-Size,  Size,  Size,	0.000f, 0.6662f,

		// Right
		 Size, -Size, -Size,	0.500f, 0.334f,
		 Size,  Size,  Size,	0.750f, 0.6665f,
		 Size,  Size, -Size,	0.500f, 0.6665f,
		 Size, -Size,  Size,	0.750f, 0.334f,
		 Size,  Size,  Size,	0.750f, 0.6665f,
		 Size, -Size, -Size,	0.500f, 0.334f,

		// Top
		 Size,  Size,  Size,	0.4997f, 1.000f,
		-Size,  Size,  Size,	0.251f, 1.000f,
		-Size,  Size, -Size,	0.251f, 0.667f,
		-Size,  Size, -Size,	0.251f, 0.667f,
		 Size,  Size, -Size,	0.4997f, 0.667f,
		 Size,  Size,  Size,	0.4997f, 1.000f,

		// Back
		-Size,  Size,  Size,	1.000f, 0.6665f,
		 Size,  Size,  Size,	0.750f, 0.6665f,
		 Size, -Size,  Size,	0.750f, 0.334f,
		 Size, -Size,  Size,	0.750f, 0.334f,
		-Size, -Size,  Size,	1.000f, 0.334f,
		-Size,  Size,  Size,	1.000f, 0.6665f,

		// Bottom
		-Size, -Size,  Size,	0.251f, 0.000f,
		 Size, -Size, -Size,	0.4995f, 0.333f,
		 -Size, -Size, -Size,	0.251f, 0.333f,
		-Size, -Size,  Size,	0.251f, 0.000f,
		 Size, -Size,  Size,	0.4995f, 0.000f,
		 Size, -Size, -Size,	0.4995f, 0.333f
	};
	CMesh* mesh = new CMesh(Vertices, sizeof(Vertices), false, true);
	return mesh;
};

#endif /* SOURCE_SKYBOXCREATOR_H_ */
