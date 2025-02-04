#include "ComponentMesh.h"
#include "ComponentMaterial.h"
#include "ComponentTransform.h"

#include "Application.h"
#include "ComponentCamera.h"

#include "imGui/imgui.h"
#include "imGui/backends/imgui_impl_opengl3.h"
#include "imGui/backends/imgui_impl_SDL2.h"


ComponentMesh::ComponentMesh()
{
	wireMode = false;
	drawVertexNormals = drawFaceNormals = false;
	drawAABB = true;
	resourceMesh = nullptr;
	type = ComponentType::MESH;
	Enable();

	if (resourceMesh != nullptr) { GetGlobalAABB(); }
}

ComponentMesh::ComponentMesh(GameObject* own)
{
	owner = own;
	UUID = App->resources->GenerateNewUID();
	wireMode = false;
	drawVertexNormals = drawFaceNormals = false;
	drawAABB = true;
	resourceMesh = nullptr;

	type = ComponentType::MESH;
	Enable();

	if (resourceMesh != nullptr) { GetGlobalAABB(); }
}

ComponentMesh::~ComponentMesh()
{
	if (resourceMesh != nullptr)
	{
		resourceMesh = nullptr;
		//delete resourceMesh;

	}
}

AABB ComponentMesh::GenerateLocalAABB()
{
	localAABB.SetNegativeInfinity();
	localAABB.Enclose((float3*)resourceMesh->rMesh.vertex, resourceMesh->rMesh.num_vertex);

	return localAABB;
}

AABB ComponentMesh::GetGlobalAABB()
{
	mOBB = GenerateLocalAABB();
	mOBB.Transform(owner->objTransform->globalTransform);

	globalAABB.SetNegativeInfinity();
	globalAABB.Enclose(mOBB);

	return globalAABB;
}

bool ComponentMesh::Update()
{
	//Draw mesh
	// To DO: Transformation
	if(resourceMesh!=nullptr && active)
	{		
		//owner->objTransform->GenerateGlobalMatrix();
		float4x4 m = owner->objTransform->globalTransform.Transposed();
		
		glPushMatrix();
		glMultMatrixf(m.ptr());

		//Va debajo de la matrix para intentar que se mueva a donde debe estar
		if (drawVertexNormals) {	DrawVertexNormals();}
		if (drawFaceNormals) { DrawFaceNormals(); }
		if (drawAABB) { DrawGlobalAABB(); }
		if (drawOBB) { DrawOBB(); }

		Color col = { 1,1,1,1 };
		uint texID = 0;
		if (owner->HasComponent(ComponentType::MATERIAL))
		{
			ComponentMaterial* cMate = (ComponentMaterial*)owner->GetComponentByType(ComponentType::MATERIAL);
			if (cMate->active)
			{
				
				col = cMate->color;
				if (cMate->resource != nullptr && cMate->active) {
					//Enable texture
					texID = cMate->resource->tex.textureBuffer;
				}
				if(cMate->checkers)
				{
					texID = App->renderer3D->checkersID;
				}
			}
		}
		if (App->scene->selectedGO->HasComponent(CAMERA))
		{
			ComponentCamera* camera = (ComponentCamera*)App->scene->selectedGO->GetComponentByType(CAMERA);
			if (camera->ContainsAaBox(localAABB))
			{
				App->renderer3D->DrawMesh(resourceMesh->rMesh, wireMode, col, texID);
			}
		}
		else
		{
			App->renderer3D->DrawMesh(resourceMesh->rMesh, wireMode, col, texID);
		}
		glPopMatrix();
	}
	
	return true;
}

bool ComponentMesh::GetActive()
{
	return active;
}

bool ComponentMesh::DrawVertexNormals()
{
	if(resourceMesh != nullptr)
	{
		//glLineWidth(2.0f); //Not very good, not supported in a lot of systems values different from 1
		glBegin(GL_LINES);

		//glColor3b();
		GLfloat const color[3] = { (1.0 / 255), (240.0 / 255), (30.0 / 255) };
		glColor3fv(color); //Uses values from 1 to 0 no 255
		for (int i = 0; i <= resourceMesh->rMesh.num_vertex; i++)
		{
			glVertex3f(resourceMesh->rMesh.vertex[i*3], resourceMesh->rMesh.vertex[i*3 + 1], resourceMesh->rMesh.vertex[i*3 + 2]);
			glVertex3f(resourceMesh->rMesh.vertex[i*3] + resourceMesh->rMesh.normals[i*3], resourceMesh->rMesh.vertex[i*3 + 1] + resourceMesh->rMesh.normals[i*3 + 1], resourceMesh->rMesh.vertex[i*3 + 2] + resourceMesh->rMesh.normals[i*3 + 2]);
		}
		glEnd();
		GLfloat const color2[3] = { (1.0f), (1.0f), (1.0f) };
		glColor3fv(color2); //Return to White as normal
	}
	else
	{
		return false;
	}
	
	return true;
}

bool ComponentMesh::DrawFaceNormals()
{
	//float* facesCenter = new float[resourceMesh->rMesh.num_index/3]; //Num faces

	glBegin(GL_LINES);
	GLfloat const color[3] = { (200.0 / 255), (10.0 / 255), (200.0 / 255) };
	glColor3fv(color); //Uses values from 1 to 0 no 255
	for (uint i = 0; i < resourceMesh->rMesh.num_index; i += 3)
	{
		//Becose each vertex has 3 components we must take steps of three to select each vertex of the array
		uint index1 = resourceMesh->rMesh.index[i]*3 ; 
		uint index2 = resourceMesh->rMesh.index[i + 1]*3 ;
		uint index3 = resourceMesh->rMesh.index[i + 2]*3 ;

		vec3 vertex0(resourceMesh->rMesh.vertex[index1], resourceMesh->rMesh.vertex[index1 + 1], resourceMesh->rMesh.vertex[index1 + 2]); //First vertex of the face
		vec3 vertex1(resourceMesh->rMesh.vertex[index2], resourceMesh->rMesh.vertex[index2 + 1], resourceMesh->rMesh.vertex[index2 + 2]); //Second vertex of the face
		vec3 vertex2(resourceMesh->rMesh.vertex[index3], resourceMesh->rMesh.vertex[index3 + 1], resourceMesh->rMesh.vertex[index3 + 2]); //Third vertex of the face

		//We take 2 vector of the dots of the mesh becose cross product gives us the perpendicular direction AKA the normal of the face
		vec3 vec0 = vertex0 - vertex2;
		vec3 vec1 = vertex1 - vertex2;
		vec3 normalVec = cross(vec0, vec1);

		vec3 normalizedVec = normalize(normalVec); //We normalize to reduce the size

		//vec3 faceCenter = { (((vertex0.x + vertex1.x + vertex2.x) / 3),((vertex0.y + vertex1.y + vertex2.y) / 3),((vertex0.z + vertex1.z + vertex2.z) / 3)) }; //X,Y,Z of the center of face
		float faceCenters[3] ; //X,Y,Z of the center of face
		faceCenters[0] = (vertex0.x + vertex1.x + vertex2.x)/3;
		faceCenters[1] = (vertex0.y + vertex1.y + vertex2.y)/3;
		faceCenters[2] = (vertex0.z + vertex1.z + vertex2.z)/3;
		
		glVertex3f(faceCenters[0],faceCenters[1],faceCenters[2]);
		glVertex3f(faceCenters[0] + normalVec.x, faceCenters[1] + normalVec.y, faceCenters[2] + normalVec.z);
		
	}

	glEnd();
	GLfloat const color2[3] = { (1.0f), (1.0f), (1.0f) };
	glColor3fv(color2); //Return to White as normal

	return true;
}

bool ComponentMesh::DrawGlobalAABB()
{
	//GetGlobalAABB();
	glBegin(GL_LINES);
	GLfloat const color[3] = { (220.0 / 255), (10.0 / 255), (10.0 / 255) };
	glColor3fv(color); //Uses values from 1 to 0 no 255
	glLineWidth(2.0f);
	

	//FRONT FACE
	// 0,0,0 - 1,0,0
	glVertex3f(globalAABB.minPoint.x, globalAABB.minPoint.y, globalAABB.minPoint.z);
	glVertex3f(globalAABB.maxPoint.x, globalAABB.minPoint.y, globalAABB.minPoint.z);

	//0,0,0 - 0,1,0
	glVertex3f(globalAABB.minPoint.x, globalAABB.minPoint.y, globalAABB.minPoint.z);
	glVertex3f(globalAABB.minPoint.x, globalAABB.maxPoint.y, globalAABB.minPoint.z);

	//1,0,0 - 1,1,0
	glVertex3f(globalAABB.maxPoint.x, globalAABB.minPoint.y, globalAABB.minPoint.z);
	glVertex3f(globalAABB.maxPoint.x, globalAABB.maxPoint.y, globalAABB.minPoint.z);

	//0,1,0 - 1,1,0
	glVertex3f(globalAABB.minPoint.x, globalAABB.maxPoint.y, globalAABB.minPoint.z);
	glVertex3f(globalAABB.maxPoint.x, globalAABB.maxPoint.y, globalAABB.minPoint.z);

	//RIGTH FACE (1,0,0 - 1,1,0 IN FRONT)
	//1,0,0 - 1,0,1
	glVertex3f(globalAABB.maxPoint.x, globalAABB.minPoint.y, globalAABB.minPoint.z);
	glVertex3f(globalAABB.maxPoint.x, globalAABB.minPoint.y, globalAABB.maxPoint.z);

	//1,1,0 - 1,1,1
	glVertex3f(globalAABB.maxPoint.x, globalAABB.maxPoint.y, globalAABB.minPoint.z);
	glVertex3f(globalAABB.maxPoint.x, globalAABB.maxPoint.y, globalAABB.maxPoint.z);

	//1,0,1 - 1,1,1
	glVertex3f(globalAABB.maxPoint.x, globalAABB.minPoint.y, globalAABB.maxPoint.z);
	glVertex3f(globalAABB.maxPoint.x, globalAABB.maxPoint.y, globalAABB.maxPoint.z);

	//LEFT FACE (0,0,1 - 0,1,0 IN FRONT)
	//0,0,1 - 0,1,1
	glVertex3f(globalAABB.minPoint.x, globalAABB.minPoint.y, globalAABB.maxPoint.z);
	glVertex3f(globalAABB.minPoint.x, globalAABB.maxPoint.y, globalAABB.maxPoint.z);

	//0,0,0 - 0,0,1
	glVertex3f(globalAABB.minPoint.x, globalAABB.minPoint.y, globalAABB.minPoint.z);
	glVertex3f(globalAABB.minPoint.x, globalAABB.minPoint.y, globalAABB.maxPoint.z);

	//0,1,0 - 0,1,1
	glVertex3f(globalAABB.minPoint.x, globalAABB.maxPoint.y, globalAABB.minPoint.z);
	glVertex3f(globalAABB.minPoint.x, globalAABB.maxPoint.y, globalAABB.maxPoint.z);

	//BACK FACE (0,0,1 - 0,1,1 IN LEFT AND 1,0,1 - 1,1,1 IN RIGTH)
	//0,0,1 - 1,0,1
	glVertex3f(globalAABB.minPoint.x, globalAABB.minPoint.y, globalAABB.maxPoint.z);
	glVertex3f(globalAABB.maxPoint.x, globalAABB.minPoint.y, globalAABB.maxPoint.z);

	//0,1,1 - 1,1,1
	glVertex3f(globalAABB.minPoint.x, globalAABB.maxPoint.y, globalAABB.maxPoint.z);
	glVertex3f(globalAABB.maxPoint.x, globalAABB.maxPoint.y, globalAABB.maxPoint.z);
	

	glLineWidth(1.0f);
	GLfloat const color2[3] = { (1.0f), (1.0f), (1.0f) };
	glColor3fv(color2); //Return to White as normal
	glEnd();

	return true;
}

bool ComponentMesh::DrawOBB()
{
	if (resourceMesh == nullptr)
		return false;

	GLfloat const color[3] = { (0.0 / 255), (240.0 / 255), (240.0 / 255) };
	glColor3fv(color); //Uses values from 1 to 0 no 255
	glLineWidth(2.0f);
	float3* OBB_points = nullptr;
	mOBB.GetCornerPoints(OBB_points);

	glBegin(GL_LINES);

	glVertex3f(mOBB.CornerPoint(0).x, mOBB.CornerPoint(0).y, mOBB.CornerPoint(0).z);
	glVertex3f(mOBB.CornerPoint(1).x, mOBB.CornerPoint(1).y, mOBB.CornerPoint(1).z);

	glVertex3f(mOBB.CornerPoint(0).x, mOBB.CornerPoint(0).y, mOBB.CornerPoint(0).z);
	glVertex3f(mOBB.CornerPoint(4).x, mOBB.CornerPoint(4).y, mOBB.CornerPoint(4).z);

	glVertex3f(mOBB.CornerPoint(0).x, mOBB.CornerPoint(0).y, mOBB.CornerPoint(0).z);
	glVertex3f(mOBB.CornerPoint(2).x, mOBB.CornerPoint(2).y, mOBB.CornerPoint(2).z);

	glVertex3f(mOBB.CornerPoint(2).x, mOBB.CornerPoint(2).y, mOBB.CornerPoint(2).z);
	glVertex3f(mOBB.CornerPoint(3).x, mOBB.CornerPoint(3).y, mOBB.CornerPoint(3).z);

	glVertex3f(mOBB.CornerPoint(1).x, mOBB.CornerPoint(1).y, mOBB.CornerPoint(1).z);
	glVertex3f(mOBB.CornerPoint(5).x, mOBB.CornerPoint(5).y, mOBB.CornerPoint(5).z);

	glVertex3f(mOBB.CornerPoint(1).x, mOBB.CornerPoint(1).y, mOBB.CornerPoint(1).z);
	glVertex3f(mOBB.CornerPoint(3).x, mOBB.CornerPoint(3).y, mOBB.CornerPoint(3).z);

	glVertex3f(mOBB.CornerPoint(4).x, mOBB.CornerPoint(4).y, mOBB.CornerPoint(4).z);
	glVertex3f(mOBB.CornerPoint(5).x, mOBB.CornerPoint(5).y, mOBB.CornerPoint(5).z);

	glVertex3f(mOBB.CornerPoint(4).x, mOBB.CornerPoint(4).y, mOBB.CornerPoint(4).z);
	glVertex3f(mOBB.CornerPoint(6).x, mOBB.CornerPoint(6).y, mOBB.CornerPoint(6).z);

	glVertex3f(mOBB.CornerPoint(2).x, mOBB.CornerPoint(2).y, mOBB.CornerPoint(2).z);
	glVertex3f(mOBB.CornerPoint(6).x, mOBB.CornerPoint(6).y, mOBB.CornerPoint(6).z);

	glVertex3f(mOBB.CornerPoint(5).x, mOBB.CornerPoint(5).y, mOBB.CornerPoint(5).z);
	glVertex3f(mOBB.CornerPoint(7).x, mOBB.CornerPoint(7).y, mOBB.CornerPoint(7).z);

	glVertex3f(mOBB.CornerPoint(6).x, mOBB.CornerPoint(6).y, mOBB.CornerPoint(6).z);
	glVertex3f(mOBB.CornerPoint(7).x, mOBB.CornerPoint(7).y, mOBB.CornerPoint(7).z);

	glVertex3f(mOBB.CornerPoint(3).x, mOBB.CornerPoint(3).y, mOBB.CornerPoint(3).z);
	glVertex3f(mOBB.CornerPoint(7).x, mOBB.CornerPoint(7).y, mOBB.CornerPoint(7).z);

	glLineWidth(1.0f);
	GLfloat const color2[3] = { (1.0f), (1.0f), (1.0f) };
	glColor3fv(color2); //Return to White as normal
	glEnd();

	return true;
}

void ComponentMesh::OnEditor()
{
	std::string butonChar;

	//Give an ID to each colapsing header to be able to have more than one of the same time
	//This must be done due to ImGui using the names as the ids of all menus and things
	int myPosInComponents = owner->GetComponentPosition(this);
	std::string idComponent;
	idComponent.append("##");
	idComponent.append(std::to_string(myPosInComponents).c_str());
	
	butonChar.append("Mesh");
	if (ImGui::CollapsingHeader(butonChar.append(idComponent).c_str()))
	{
		butonChar.clear();
		butonChar.append("##Mesh Active");
		ImGui::Checkbox(butonChar.append(idComponent).c_str(), &active); //El doble ## hace que no se muestre el texto. Es necesario poner un nombre distinto a cada checkbox y boton ya que ImGui usa el nombre como la ID
		ImGui::SameLine();

		butonChar.clear();
		butonChar.append("Delete component");
		if (ImGui::Button(butonChar.append(idComponent).c_str()))
		{
			this->~ComponentMesh();
		}

		if (resourceMesh != nullptr)
		{
			butonChar.clear();
			butonChar.append("Wireframe Mode");
			ImGui::Checkbox(butonChar.append(idComponent).c_str(), &wireMode);
			butonChar.clear();
			butonChar.append("Draw Vertex Normals");
			ImGui::Checkbox(butonChar.append(idComponent).c_str(), &drawVertexNormals);
			butonChar.clear();
			butonChar.append("Draw Face Normals");
			ImGui::Checkbox(butonChar.append(idComponent).c_str(), &drawFaceNormals);
			butonChar.clear();
			butonChar.append("Draw AABB");
			ImGui::Checkbox(butonChar.append(idComponent).c_str(), &drawAABB);
			butonChar.clear();
			butonChar.append("Draw OBB");
			ImGui::Checkbox(butonChar.append(idComponent).c_str(), &drawOBB);
			
			ImGui::Text("Number vertex %d", resourceMesh->rMesh.num_vertex);
			ImGui::Text("Number normals %d", resourceMesh->rMesh.num_normals);
		}
		else
		{
			ImGui::Text("Empty Mesh");
		}
		
		
	}
}