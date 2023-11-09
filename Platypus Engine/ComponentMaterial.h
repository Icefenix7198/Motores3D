#ifndef __COMPONENT_MATERIAL_H__
#define __COMPONENT_MATERIAL_H__

#include "Globals.h"
#include "GameObject.h"
#include "Component.h"

#include <vector>
#include <string>

class ComponentMaterial : public Component
{
public:
	ComponentMaterial();
	ComponentMaterial(GameObject* own);
	~ComponentMaterial();

	void OnEditor();

	bool Update();

	bool checkers;
	bool activeCheck;

	uint textureBuffer;

private:

};

#endif //!__COMPONENT_MATERIAL_H__
