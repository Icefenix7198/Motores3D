#include "aasimp.h"
#include "GameObject.h"

#include "Component.h"
#include "ComponentTransform.h"
#include "ComponentMesh.h"

#include "Application.h"
#include "ModuleScene.h"

//#include <vector>

Mesh* aasimp::ExtractMeshInfo(aiScene* scene)
{
	return nullptr;
}

void aasimp::Load(const char* file_path)
{
	Mesh* ourMesh = new Mesh();
	//Creo que hay que iterar sobre una iaMesh a la cual assignemows algo
	const aiScene* scene = aiImportFile(file_path, aiProcessPreset_TargetRealtime_MaxQuality);
	if (scene != nullptr && scene-> HasMeshes())
	{
		// Use scene->mNumMeshes to iterate on scene->mMeshes array
		for (int i = 0; i < scene->mNumMeshes; i++) 
		{
			//Copy Vertex
			ourMesh->num_vertex = scene->mMeshes[i]->mNumVertices; //Copy the num of Vertex to our mesh from the imported mesh
			
			ourMesh->vertex = new float[ourMesh->num_vertex * 3]; //Create array of vertex of size equal to the imported mesh
			memcpy(ourMesh->vertex, scene->mMeshes[i]->mVertices, sizeof(float) * ourMesh->num_vertex * 3); //Copy the vertices array into mesh Vertex array 
			
			LOG("New mesh with %d vertices", ourMesh->num_vertex);
		
			//Copy faces/indexes
			if (scene->mMeshes[i]->HasFaces())
			{
				ourMesh->num_index = scene->mMeshes[i]->mNumFaces * 3; //Get the number of indixes (it will be 3 for each triangle/face)

				ourMesh->index = new uint[ourMesh->num_index]; // assume each face is a triangle
				for (uint j = 0; j < scene->mMeshes[i]->mNumFaces; ++j)
				{
					if (scene->mMeshes[i]->mFaces[j].mNumIndices != 3)
					{
						LOG("WARNING, geometry face with != 3 indices!");
					}	
					else
					{
						memcpy(&ourMesh->index[j * 3], scene->mMeshes[i]->mFaces[j].mIndices, 3 * sizeof(uint)); //Copy in index array 
					}
						
				}
			}

			//Copy normals
			if (scene->mMeshes[i]->HasNormals())
			{
				ourMesh->num_normals = scene->mMeshes[i]->mNumVertices; //Get the number of normals (it will be 3 for each triangle/face)

				ourMesh->normals = new uint[ourMesh->num_normals]; // assume each face is a triangle (so it will have 3 normals)
				memcpy(ourMesh->normals, scene->mMeshes[i]->mNormals, sizeof(float) * ourMesh->num_normals); //Copy the vertices array into mesh Vertex array 

				LOG("New mesh with %d normals", ourMesh->num_normals);
				
			}
			
			// Copy UV
			//uint UV_index = 0;
			//if (scene->mMeshes[i]->HasTextureCoords(UV_index))
			//{
			//	ourMesh->num_UVs = ourMesh->num_vertex;
			//	//ourMesh->UVs = new float2[ourMesh->num_UVs];
			//	for (int k = 0; k < scene->mMeshes[i]->mNumVertices; ++k) //There is one UV per vertex
			//	{
			//		ourMesh->UVs[k].x = scene->mMeshes[i]->mTextureCoords[UV_index][k].x;
			//		ourMesh->UVs[k].y = scene->mMeshes[i]->mTextureCoords[UV_index][k].y;
			//	}
			//	LOG("New mesh with %d texture coordinates",ourMesh->num_UVs);
			//}


			//BUFFERS
			ourMesh->VBO = 0; //Buffer de vertices
			ourMesh->VN = 0;
			ourMesh->EBO = 0;
			
			//ourMesh->VAO = 0;

			//Generate buffers.If after this any of them is 0 there is an error
			glGenBuffers(1, &ourMesh->VBO);
			glGenBuffers(1, &ourMesh->VN);
			glGenBuffers(1, &ourMesh->EBO);

			

			glBindBuffer(GL_ARRAY_BUFFER, ourMesh->VBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(float)*ourMesh->num_vertex*3, ourMesh->vertex, GL_STATIC_DRAW); 
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glBindBuffer(GL_ARRAY_BUFFER, ourMesh->VN);
			glBufferData(GL_ARRAY_BUFFER, sizeof(float) * ourMesh->num_normals, ourMesh->normals, GL_STATIC_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ourMesh->EBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(uint) * ourMesh->num_index, ourMesh->index, GL_STATIC_DRAW); 
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	


			//Aqui habria que crear un GameObject y asignarle la mesh
			//scene->mMeshes[i].
			//GameObject* go = new GameObject();
			//scene->mRootNode->mTransformation.Decompose();

			//Copiar OurMesh a un vector de meshes
			vecMeshes.push_back(ourMesh);
		}
		
		//Get name
		std::string fPathFull; //String with name path
		fPathFull.assign(file_path);
		int posSlash=fPathFull.find_last_of("/"); //Find the last / of the file, will have the name of the fbx as the others are folders. (returns int position)
		int nameLength = fPathFull.find_last_of(".") - posSlash -1; //Size of the word, the -1 is to not take the "."
		
		std::string name;
		name.assign(fPathFull, posSlash +1, nameLength); //We add a +1 to not write the /, 
		HierarcyGameObject(scene,scene->mRootNode,name.c_str(),App->scene->root);
		aiReleaseImport(scene);
	}
	else
	{
		LOG("Error loading scene %s", file_path);
	}
		
}

Mesh* AiMeshtoMesh(aiMesh* mesh)
{
	Mesh* nMesh = new Mesh;

	//Copy Vertex
	nMesh->num_vertex = mesh->mNumVertices; //Copy the num of Vertex to our mesh from the imported mesh

	nMesh->vertex = new float[nMesh->num_vertex * 3]; //Create array of vertex of size equal to the imported mesh
	memcpy(nMesh->vertex, mesh->mVertices, sizeof(float) * nMesh->num_vertex * 3); //Copy the vertices array into mesh Vertex array 

	//Copy faces/indexes
	if (mesh->HasFaces())
	{
		nMesh->num_index = mesh->mNumFaces * 3; //Get the number of indixes (it will be 3 for each triangle/face)

		nMesh->index = new uint[nMesh->num_index]; // assume each face is a triangle
		for (uint j = 0; j < mesh->mNumFaces; ++j)
		{
			if (mesh->mFaces[j].mNumIndices != 3)
			{
				LOG("WARNING, geometry face with != 3 indices!");
			}
			else
			{
				memcpy(&nMesh->index[j * 3], mesh->mFaces[j].mIndices, 3 * sizeof(uint)); //Copy in index array 
			}

		}
	}

	//Copy normals
	if (mesh->HasNormals())
	{
		nMesh->num_normals = mesh->mNumVertices; //Get the number of normals (it will be 3 for each triangle/face)

		nMesh->normals = new uint[nMesh->num_normals]; // assume each face is a triangle (so it will have 3 normals)
		memcpy(nMesh->normals, mesh->mNormals, sizeof(float) * nMesh->num_normals); //Copy the vertices array into mesh Vertex array 
	}

	return nMesh;
}

void HierarcyGameObject(const aiScene* scene,aiNode* root,const char* name,GameObject* parent)
{

	if(root->mNumChildren>1)
	{
			
			if (root->mNumMeshes > 0)
			{
				for (int i = 0; i < root->mNumMeshes; i++)
				{
					aiVector3D translation, scaling;
					aiQuaternion rotation;
					root->mTransformation.Decompose(scaling, rotation, translation);
					GameObject* gm = App->scene->CreateGameObject(parent, name);
					gm->objTransform->SetValues(translation, scaling, rotation);
					ComponentMesh* cMesh; //Create reference to modify compoent mesh
					cMesh = (ComponentMesh*)gm->CreateComponent(ComponentType::MESH); //Create component mesh
					uint* numMesh = root->mMeshes; //mMeshes is a number to to the scene mMeshes array
					cMesh->mesh = AiMeshtoMesh(scene->mMeshes[numMesh[i]]); //TODO ERIC: Es un aiMesh, hay que convertirlo a Mesh normal
				}

			}
			else
			{
				parent = App->scene->CreateGameObject(parent, name);
			}
		

		for (int i = 0; i < root->mNumChildren; i++)
		{
			HierarcyGameObject(scene,root->mChildren[i],root->mChildren[i]->mName.C_Str(), parent);
		}
	}
	else
	{
		if (root->mNumChildren>0)
		{
			if (root->mNumMeshes > 0)
			{
				for (int i = 0; i < root->mNumMeshes; i++)
				{
					aiVector3D translation, scaling;
					aiQuaternion rotation;
					root->mTransformation.Decompose(scaling, rotation, translation);
					LOG("Create GamoObject for mesh %s", name)
						GameObject* gm = App->scene->CreateGameObject(parent, name);
					gm->objTransform->SetValues(translation, scaling, rotation);
					ComponentMesh* cMesh; //Create reference to modify compoent mesh
					cMesh = (ComponentMesh*)gm->CreateComponent(ComponentType::MESH); //Create component mesh
					uint* numMesh = root->mMeshes; //mMeshes is a number to to the scene mMeshes array
					cMesh->mesh = AiMeshtoMesh(scene->mMeshes[numMesh[i]]); //TODO ERIC: Es un aiMesh, hay que convertirlo a Mesh normal
				}

			}
		}
		else
		{
			
			if (root->mNumMeshes>0) 
			{ 
				for (int i = 0; i < root->mNumMeshes; i++)
				{
					aiVector3D translation, scaling;
					aiQuaternion rotation;
					root->mTransformation.Decompose(scaling, rotation, translation);
					LOG("Create GamoObject for mesh %s",name)
					GameObject* gm=App->scene->CreateGameObject(parent, name);
					gm->objTransform->SetValues(translation,scaling,rotation);
					ComponentMesh* cMesh; //Create reference to modify compoent mesh
					cMesh=(ComponentMesh*)gm->CreateComponent(ComponentType::MESH); //Create component mesh
					uint* numMesh = root->mMeshes; //mMeshes is a number to to the scene mMeshes array
					cMesh->mesh = AiMeshtoMesh(scene->mMeshes[numMesh[i]]); //TODO ERIC: Es un aiMesh, hay que convertirlo a Mesh normal
				}
				
			}

		}
	}
}
