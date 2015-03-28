#include "Shooter.h"

extern I3DEngine* myEngine;//Models won't work unless engine is externally linked
extern IModel* ground;
extern IMesh* bulletMesh;

CShooter::CShooter()
{
	mesh = myEngine->LoadMesh("Sphere.x");
	model = NULL;
	shot = false;
	bullet = new BulletData;
}

bool CShooter::createModel(float x, float y, float z)
{
	model = mesh->CreateModel(x,y,z);// coordinates obtained for map.txt
	model->AttachToParent(ground);// attached to the main ground model so they move as one
	model->SetSkin("plane.jpg");
	model->Scale(0.5f);
	return true;
}

void CShooter::updateShooter(CPlayer* player, float updateTime)
{
	//determines whether the player has entered the Shooters firing range
	bool hasDetected = false;
	if(player->GetX() > model->GetX() - 80 && player->GetX() < model->GetX() + 80)
	{
		if(player->GetY() > model->GetY() - 80 && player->GetY() < model->GetY() + 80)
		{
			hasDetected = true;
		}
	}
	//if the player is in  firing range and the NPC does not have a bullet already on screen it fires a bullet
	if(hasDetected && !shot)
	{
		model->LookAt(player->GetModel());
		bullet->model = bulletMesh->CreateModel( model->GetX() , model->GetY(), model->GetZ());
		bullet->life = 3.0f;
		bullet->model->Scale(0.5f);
		bullet->model->AttachToParent(model);
		bullet->model->LookAt(player->GetModel());
		shot = true;
		bullet->model->SetPosition( model->GetX() , model->GetY(), model->GetZ());
	}
	//if a bullet is on screen it moves, decrease life, checks for colision with player and deletes bullet when condtions are true
	if(shot)
	{
		bullet->model->Move( 0.0f, 0.0f, 75.0f * updateTime);
		// Decrease life and see if bullet is dead
		bullet->life -= updateTime;

		bool hit = false;
		if(player->GetX() > bullet->model->GetX() - 10 && player->GetX() < bullet->model->GetX() + 10)
		{
			if(player->GetY() > bullet->model->GetY() - 10 && player->GetY() < bullet->model->GetY() + 10)
			{
				hit = true;
				player->SetHealth (20);
			}
		}

		if (bullet->life <= 0 || hit == true)
		{
			// Destroy bullet
  			bulletMesh->RemoveModel(bullet->model);
			shot = !shot;
		}
	}
}

CShooter::~CShooter()
{
	mesh->RemoveModel(model);
	myEngine->RemoveMesh(mesh);
}