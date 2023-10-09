#include "Globals.h"
#include "Application.h"
#include "ModuleRenderer3D.h"
#include "SDL\include\SDL_opengl.h"
#include <gl/GL.h>
#include <gl/GLU.h>

#include "ImGui/imgui.h"
#include "ImGui/backends/imgui_impl_sdl2.h"
#include "ImGui/backends/imgui_impl_opengl3.h"


#pragma comment (lib, "opengl32.lib") /* link Microsoft OpenGL lib   */
#pragma comment (lib, "glu32.lib") /* link Microsoft OpenGL lib   */
#pragma comment (lib, "Glew/libx86/glew32.lib")

#ifdef _DEBUG
#pragma comment (lib, "MathGeoLib/libx86/Debug/MathGeoLib.lib") /* link Microsoft OpenGL lib   */
#else
#pragma comment (lib, "MathGeoLib/libx86/Release/MathGeoLib.lib") /* link Microsoft OpenGL lib   */
#endif // _DEBUG


ModuleRenderer3D::ModuleRenderer3D(Application* app, bool start_enabled) : Module(app, start_enabled)
{
}

// Destructor
ModuleRenderer3D::~ModuleRenderer3D()
{}

// Called before render is available
bool ModuleRenderer3D::Init()
{
	LOG("Creating 3D Renderer context");
	bool ret = true;
	
	//Create context
	context = SDL_GL_CreateContext(App->window->window);
	if(context == NULL)
	{
		LOG("OpenGL context could not be created! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}
	
	if(ret == true)
	{
		//Use Vsync
		if(App->window->GetVSYNC() && SDL_GL_SetSwapInterval(1) < 0)
			LOG("Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError());

		//Initialize Projection Matrix
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();

		//Check for error
		GLenum error = glGetError();
		if(error != GL_NO_ERROR)
		{
			LOG("Error initializing OpenGL! %s\n", gluErrorString(error));
			ret = false;
		}

		//Initialize Modelview Matrix
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		//Check for error
		error = glGetError();
		if(error != GL_NO_ERROR)
		{
			LOG("Error initializing OpenGL! %s\n", gluErrorString(error));
			ret = false;
		}
		
		glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
		glClearDepth(1.0f);
		
		//Initialize clear color
		glClearColor(0.f, 0.f, 0.f, 1.f);

		//Check for error
		error = glGetError();
		if(error != GL_NO_ERROR)
		{
			LOG("Error initializing OpenGL! %s\n", gluErrorString(error));
			ret = false;
		}
		
		GLfloat LightModelAmbient[] = {0.0f, 0.0f, 0.0f, 1.0f};
		glLightModelfv(GL_LIGHT_MODEL_AMBIENT, LightModelAmbient);
		
		lights[0].ref = GL_LIGHT0;
		lights[0].ambient.Set(0.25f, 0.25f, 0.25f, 1.0f);
		lights[0].diffuse.Set(0.75f, 0.75f, 0.75f, 1.0f);
		lights[0].SetPos(0.0f, 0.0f, 2.5f);
		lights[0].Init();
		
		GLfloat MaterialAmbient[] = {1.0f, 1.0f, 1.0f, 1.0f};
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, MaterialAmbient);

		GLfloat MaterialDiffuse[] = {1.0f, 1.0f, 1.0f, 1.0f};
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, MaterialDiffuse);
		
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		lights[0].Active(true);
		glEnable(GL_LIGHTING);
		glEnable(GL_COLOR_MATERIAL);
	}

	// Projection matrix for
	OnResize(App->window->GetScreenWidth(), App->window->GetScreenHeigth());

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable; //@ANDREU This flag causes errors with the coment: 
	// Forgot to call UpdatePlatformWindows() in main loop after EndFrame()? Check examples/ applications for reference."

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsClassic();

	// Setup Platform/Renderer backends
	ImGui_ImplSDL2_InitForOpenGL(App->window->window, context);
	ImGui_ImplOpenGL3_Init("#version 130");
	//iNICIAR GL GLEW (ESTA EN EL POWER)

	Grid.axis = true;

	
	
	//VBO = 0; //Buffer de vertices
	//EBO = 0;
	//VAO = 0;

	////Generate buffers.If after this any of them is 0 there is an error
	//glGenBuffers(1, &VBO); 
	//glGenBuffers(1, &EBO);
	//glGenVertexArrays(1, &VAO);

	//std::vector<float> vecVertices;
	//std::vector<GLuint> vecIndices;

	//glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//glBufferData(GL_ARRAY_BUFFER, vecVertices.capacity(), vecVertices.data(), GL_STATIC_DRAW); //Bien declarada pero hay que darle un arrayVertices que exista
	//glBindBuffer(GL_ARRAY_BUFFER, 0);

	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	//glBufferData(GL_ARRAY_BUFFER, vecIndices.capacity(), vecIndices.data(), GL_STATIC_DRAW); //Bien declarada pero hay que darle un arrayIndices que exista
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	//
	////VAO declaration is special/different
	//glBindVertexArray(VAO);
	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0); //???,num elements del tipo, tipo VAR, ???, tama�o bite elementos, offset en bites (por si hay elementos anteriores)
	//glEnableVertexAttribArray(0);
	//glBindVertexArray(0);

	return ret;
}

// PreUpdate: clear buffer
update_status ModuleRenderer3D::PreUpdate(float dt)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(App->camera->GetViewMatrix());

	// light 0 on cam pos
	lights[0].SetPos(App->camera->Position.x, App->camera->Position.y, App->camera->Position.z);

	for(uint i = 0; i < MAX_LIGHTS; ++i)
		lights[i].Render();

	return UPDATE_CONTINUE;
}

// PostUpdate present buffer to screen
update_status ModuleRenderer3D::PostUpdate(float dt)
{
	// Start the Dear ImGui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();

	//Functions to draw in direct mode here (it will need to go away)
	DrawCubeDirectMode();

	ImGuiIO& io = ImGui::GetIO(); (void)io;

	App->editor->DrawEditor();
	

	// Rendering
	ImGui::Render();
	glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
	//glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
	glClear(GL_COLOR_BUFFER_BIT);
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	// Update and Render additional Platform Windows
	// (Platform functions may change the current OpenGL context, so we save/restore it to make it easier to paste this code elsewhere.
	//  For this specific demo app we could also call SDL_GL_MakeCurrent(window, gl_context) directly)
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		SDL_Window* backup_current_window = SDL_GL_GetCurrentWindow();
		SDL_GLContext backup_current_context = SDL_GL_GetCurrentContext();
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
		SDL_GL_MakeCurrent(backup_current_window, backup_current_context);
	}

	Grid.Render();
	SDL_GL_SwapWindow(App->window->window);
	return UPDATE_CONTINUE;
}

// Called before quitting
bool ModuleRenderer3D::CleanUp()
{
	LOG("Destroying 3D Renderer");

	//if (VBO!=0)
	/*{
		glDeleteBuffers(1, &VBO);
	}*/
	
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	SDL_GL_DeleteContext(context);
	SDL_DestroyWindow(App->window->window);
	SDL_Quit();

	return true;
}


void ModuleRenderer3D::OnResize(int width, int height)
{
	glViewport(0, 0, width, height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	//todo: USE MATHGEOLIB here BEFORE 1st delivery! (TIP: Use MathGeoLib/Geometry/Frustum.h, view and projection matrices are managed internally.)
	ProjectionMatrix = perspective(60.0f, (float)width / (float)height, 0.125f, 512.0f);
	glLoadMatrixf(ProjectionMatrix.M);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void ModuleRenderer3D::DrawCubeDirectMode(float originX, float originY, float originZ,float size)
{

	//Arrays to draw cube
	//Vertices of the cube
	//GLfloat vertices[] =
	//{
	// originX + 0 * size ,originY + 0 * size ,originZ + 0 * size, //v0
	// originX + 1 * size ,originY + 0 * size ,originZ + 0 * size, //v1
	// originX + 1 * size ,originY + 1 * size ,originZ + 0 * size, //v2
	// originX + 0 * size ,originY + 1 * size ,originZ + 0 * size, //v3
	// originX + 0 * size ,originY + 1 * size ,originZ + 1 * size, //v4
	// originX + 0 * size ,originY + 0 * size ,originZ + 1 * size, //v5
	// originX + 1 * size ,originY + 0 * size ,originZ + 1 * size, //v6
	// originX + 1 * size ,originY + 1 * size ,originZ + 1 * size, //v7

	//};
	////Indices to draw the cube 
	//GLubyte indices[] = 
	//{ 
	//0,1,2,	2,3,0,  //front face 
	//0,3,4,	4,5,0,	//rigth face
	//0,5,6,	6,1,0,	//top face
	//
	//1,6,7,	7,2,1,	//left face
	//7,4,3,	3,2,7,	//bottom face
	//4,7,6,	6,5,4,	//back face
	//};

	//Try 2
	GLfloat vertices[] =
	{
	 originX + 0 * size ,originY + 0 * size ,originZ + 0 * size, //v0
	 originX + 0 * size ,originY + 1 * size ,originZ + 0 * size, //v1
	 originX + 1 * size ,originY + 1 * size ,originZ + 0 * size, //v2
	 originX + 1 * size ,originY + 0 * size ,originZ + 0 * size, //v3
	 originX + 1 * size ,originY + 0 * size ,originZ + 1 * size, //v4
	 originX + 0 * size ,originY + 0 * size ,originZ + 1 * size, //v5
	 originX + 0 * size ,originY + 1 * size ,originZ + 1 * size, //v6
	 originX + 1 * size ,originY + 1 * size ,originZ + 1 * size, //v7

	};
	//Indices to draw the cube 
	GLubyte indices[] =
	{
	7,6,5,	5,4,7,  //front face 
	2,7,3,	7,4,3,	//rigth face
	1,6,7,	2,1,7,	//top face

	1,0,5,	5,6,1,	//left face
	0,3,5,	5,3,4,	//bottom face
	0,1,2,	2,3,0,	//back face
	};

	// activate and specify pointer to vertex array
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, vertices);

	// draw a cube
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_BYTE, indices);

	// deactivate vertex arrays after drawing
	glDisableClientState(GL_VERTEX_ARRAY);
}
