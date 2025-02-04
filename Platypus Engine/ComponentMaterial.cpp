#include "ComponentMaterial.h"

#include "Application.h"

#include "imGui/imgui.h"
#include "imGui/backends/imgui_impl_opengl3.h"
#include "imGui/backends/imgui_impl_SDL2.h"

ComponentMaterial::ComponentMaterial()
{
	checkers = true;
	type = ComponentType::MATERIAL;
}

ComponentMaterial::ComponentMaterial(GameObject* own)
{
	owner = own;
	UUID = App->resources->GenerateNewUID();
	name = "Null";
	type = ComponentType::MATERIAL;
	color = { 1,1,1,1 };
}

ComponentMaterial::~ComponentMaterial()
{
}

void ComponentMaterial::OnEditor()
{
	std::string butonChar;

	//Give an ID to each colapsing header to be able to have more than one of the same time
	//This must be done due to ImGui using the names as the ids of all menus and things
	int myPosInComponents = owner->GetComponentPosition(this);
	std::string idComponent;
	idComponent.append("##");
	idComponent.append(std::to_string(myPosInComponents).c_str());

	butonChar.clear();
	butonChar.append("Material");
	if (ImGui::CollapsingHeader(butonChar.append(idComponent).c_str()))
	{
		butonChar.clear();
		butonChar.append("##MaterialActive");
		ImGui::Checkbox(butonChar.append(idComponent).c_str(), &active); //El doble ## hace que no se muestre el texto. Es necesario poner un nombre distinto a cada checkbox y boton ya que ImGui usa el nombre como la ID

		butonChar.clear();
		butonChar.append("Colors");
		if (ImGui::BeginTable(butonChar.append(idComponent).c_str(), 4, 0))
		{
			
			//Color
			ImGui::TableNextRow();
			butonChar.clear(); butonChar.append("R");
			ImGui::TableNextColumn(); ImGui::SliderFloat(butonChar.append(idComponent).c_str(), &color.r, 0, 1);
			butonChar.clear(); butonChar.append("G");
			ImGui::TableNextColumn(); ImGui::SliderFloat(butonChar.append(idComponent).c_str(), &color.g, 0, 1);
			butonChar.clear(); butonChar.append("B");
			ImGui::TableNextColumn(); ImGui::SliderFloat(butonChar.append(idComponent).c_str(), &color.b, 0, 1);
			butonChar.clear(); butonChar.append("A");
			ImGui::TableNextColumn(); ImGui::SliderFloat(butonChar.append(idComponent).c_str(), &color.a, 0, 1);

			ImGui::EndTable();
		}

		if (name != "Null")
		{
			ImGui::Text("Width: %i", resource->tex.width);
			ImGui::Text("Height: %i", resource->tex.heigth);
			ImGui::Text("Path: %s", resource->tex.name);
			ImGui::Image((ImTextureID)resource->tex.textureBuffer, ImVec2(128, 128));
		}
		else
		{
			ImGui::Text("Texture: Dropt it on the scene");
		}

		butonChar.clear();
		butonChar.append("Checkers");
		if(ImGui::Checkbox(butonChar.append(idComponent).c_str(), &checkers))
		{
			activeCheck = true;
		}


	}
}

bool ComponentMaterial::Update()
{
	return false;
}

bool ComponentMaterial::GetActive()
{
	return active;
}




