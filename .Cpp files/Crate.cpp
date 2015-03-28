#include "Crate.h"

extern I3DEngine* myEngine;
extern IModel* ground;

CCrate::CCrate()
{
	mesh = myEngine->LoadMesh("Crate.x");
	model = NULL;
	isHit = false;
}

bool CCrate::createModel(float x, float y, float z)
{
	model = mesh->CreateModel(x,y,z);
	model->AttachToParent(ground);
	model->Scale(0.5f);
	return true;
}

void CCrate::updateCrate()
{

}

CCrate::~CCrate()
{
	mesh->RemoveModel(model);
	myEngine->RemoveMesh(mesh);
}