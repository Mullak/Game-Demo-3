#include "Runner.h"

extern I3DEngine* myEngine;
extern IModel* ground;

CRunner::CRunner()
{
	mesh = myEngine->LoadMesh("sopwith-camel.x");
	model = NULL;
	timer = 300;
	isRight = true;
}

bool CRunner::createModel(float x, float y, float z)
{
	model = mesh->CreateModel(x,y,z);// coordinates obtained for map.txt
	model->AttachToParent(ground);// attached to the main ground model so they move as one
	model->Scale(3.0f);
	model->RotateLocalY(180);
	return true;
}

void CRunner::updateRunner(CPlayer* player, float updateTime)
{
	//determines whether the player has entered the Runners attack range
	bool hasDetected = false;
	if(player->GetX() > model->GetX() - 75 && player->GetX() < model->GetX() + 75)
	{
		if(player->GetY() > model->GetY() - 50 && player->GetY() < model->GetY() + 50)
		{
			hasDetected = true;
		}
	}
	// the runners heads towards the player once in attack range
	if(hasDetected)
	{
		timer--;
		if(model->GetX() < player->GetX()  && timer <= 0)
		{
			isRight = false;
			timer = 300;
		}
		else if (model->GetX() > player->GetX() && timer <= 0)
		{
			isRight = true;
			timer = 300;
		}

		if(isRight)
		{
			model->Move(0.0f, 0.0f, -75.0f * updateTime);
		}
		else
		{
			model->Move(0.0f, 0.0f, 75.0f * updateTime);
		}
		//if there is a collision between player and runner decrease player health
		if(player->GetX() > model->GetX() - 10 && player->GetX() < model->GetX() + 10)
		{
			if(player->GetY() > model->GetY() - 10 && player->GetY() < model->GetY() + 10)
			{
				player->SetHealth (10);
			}
		}
	}

}
CRunner::~CRunner()
{
	mesh->RemoveModel(model);
	myEngine->RemoveMesh(mesh);
}