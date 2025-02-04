#ifndef __COMPONENT_TRANSFORM_H__
#define __COMPONENT_TRANSFORM_H__

#include "Globals.h"
#include "GameObject.h"
#include "Component.h"
#include "MathGeoLib/include/Math/float3.h"
#include "Assimp/include/quaternion.h"
#include "Assimp/include/vector3.h"
#include "MathGeoLib/include/Math/Quat.h"
#include "MathGeoLib/include/Math/float4x4.h"


#include <vector>
#include <string>


class ComponentTransform : public Component
{
public:

	bool active;

	ComponentTransform();
	ComponentTransform(GameObject* own);
	ComponentTransform(aiVector3D vecPos, aiVector3D vecScale, aiQuaternion quatRot); //All parameters

	~ComponentTransform();

	bool GetActive();

	//Matrix Values
	aiVector3D translation, scaling;
	aiQuaternion rotation;

	void SetValues(aiVector3D translation, aiVector3D scaling, aiQuaternion rotation);
	
	float3 pos;
	float3 scale;
	math::Quat rot;
	float3 angles; //Only for display

	float4x4 localTransform;
	float4x4 globalTransform = float4x4::zero;
	
	//Sets the local transform in function of the values of the Component
	void GenerateLocalMatrix();
	void GenerateGlobalMatrix(bool regenerateLocal = true);
	float4x4 CreateMatrix(float3 translation, float3 scaling, Quat rotation);
	
	//Funcion recursiva actualizar all transforms
	bool RecalculateMatrix(bool regenerateLocal = true);
	//Transforma la local para quedarse en el sitio
	float4x4 ReparentLocalMatrix(GameObject* newParent);
	float4x4 GetGlobalMatrix() { return globalTransform; }
	//ImGui
	void OnEditor();

private:
	//Values are inicialized to 0,0,0-0,0,0,0-1,1,1, so the first time in the constructor when we calculate the 
	bool firstRecalculate;
};

#endif //!__COMPONENT_TRANSFORM_H__
