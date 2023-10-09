#pragma once
#include "Module.h"
#include "Globals.h"
#include "Light.h"

#include "MathGeoLib/include/Math/float3x3.h"
#include "MathGeoLib/include/Math/float4x4.h"
#include "Primitive.h"
#include "Glew/include/glew.h"

//todo: REMOVE this before 1st delivery!!
#include "glmath.h"

#define MAX_LIGHTS 8

class ModuleRenderer3D : public Module
{
public:
	ModuleRenderer3D(Application* app, bool start_enabled = true);
	~ModuleRenderer3D();

	bool Init();
	update_status PreUpdate(float dt);
	update_status PostUpdate(float dt);
	bool CleanUp();

	void OnResize(int width, int height);

	void DrawCubeDirectMode(float originX=0, float originY=0, float originZ=0, float size=1);
	void DrawSphereDirectMode(float radius, unsigned int rings, unsigned int sectors);
	void DrawPyramidDirectMode(float originX=0, float originY = 0, float originZ = 0, uint numFaces=3, float heigth=1, float width=1);

public:

	Light lights[MAX_LIGHTS];
	SDL_GLContext context;
	CPlane Grid;

	bool second_Window = true;
	
	//You won't need this after using Frustum
	mat4x4 ProjectionMatrix;

	GLuint VBO; //VERTEX BUFFER OBJECT
	GLuint EBO; //ELEMENT BUFFER OBJECT
	GLuint VAO; //VERTEX ATRIBUTE OBJECT

};