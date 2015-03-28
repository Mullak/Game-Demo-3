#include "Goal.h"
extern I3DEngine* myEngine;
extern IModel* ground;

CGoal::CGoal()
{
	mesh =  myEngine->LoadMesh("PlatformWide.x");
	model = NULL;
}

bool CGoal::createModel(float x, float y, float z)
{
	model = mesh->CreateModel(x,y,z);// coordinates obtained for map.txt
	model->AttachToParent(ground); // attached to the main ground model so they move as one
	return true;
}

CGoal::~CGoal()
{
	mesh->RemoveModel(model);
	myEngine->RemoveMesh(mesh);
}
