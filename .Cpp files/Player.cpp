#include "Player.h"

CPlayer::CPlayer()
{
	health = 100;
	lives = 3;
	playerMesh = myEngine->LoadMesh("Robot.x");
	model = playerMesh->CreateModel(0.0f, 15.0f, 60.0f);// player model nevers moves eveything else moves around it
	model->Scale(0.95f);
	model->RotateY(90.0f);
	legLimit = 200;
	lCounter = 0;
	rCounter = 0;
	nextLeg =  true;
}

CPlayer::~CPlayer()
{
	playerMesh->RemoveModel(model);
	myEngine->RemoveMesh(playerMesh);
}

void CPlayer::GetHealth(int &playerHealth) const
{
	playerHealth = health;
}

void CPlayer::GetLives( int &playerLife) const
{
	playerLife = lives;
}

IModel* CPlayer::GetModel() const
{
	return model;
}

void CPlayer::SetHealth( int damage)
{
	health -= damage;
}

void CPlayer::SetLives(int life)
{
	lives -= life;
}

void CPlayer::leftLeg()
{
	if( nextLeg && lCounter < legLimit)
		{
			model->GetNode(2)->RotateY(0.05f);
			model->GetNode(3)->RotateY(-0.05);

			lCounter++;
			rCounter--;
			if(lCounter >= legLimit)
			{
				nextLeg = !nextLeg;
				rCounter = 0;
			}
		}
		else if( !nextLeg && rCounter < legLimit)
		{
			model->GetNode(3)->RotateY(0.05f);
			model->GetNode(2)->RotateY(-0.05);
			
			rCounter++;
			lCounter--;
			if(rCounter >= legLimit)
			{
				nextLeg = !nextLeg;
				lCounter = 0;
			}
		}
}

void CPlayer::rightLeg()
{
	if( nextLeg && rCounter < legLimit)
		{
			model->GetNode(3)->RotateY(0.05f);
			model->GetNode(2)->RotateY(-0.05);

			rCounter++;
			lCounter--;
			if(rCounter >= legLimit)
			{
				nextLeg = !nextLeg;
				lCounter = 0;
			}
		}
		else if( !nextLeg && lCounter < legLimit)
		{
			model->GetNode(2)->RotateY(0.05f);
			model->GetNode(3)->RotateY(-0.05);
			
			lCounter++;
			rCounter--;
			if(lCounter >= legLimit)
			{
				nextLeg = !nextLeg;
				rCounter = 0;
			}
		}
}