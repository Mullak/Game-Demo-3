#include <TL-Engine.h>
#include <deque>
#include "Shooter.h"
#include "Crate.h"
#include "Runner.h"
using namespace tle;

//the main controls contain the functions to control the player and those that would effect the encapsulation of the classes
//also having them here makes it so only one copy of the functions are stored rather than multiple if they where in a class

//these are the global varibles that are needed for these functions 
#ifndef _EXTERNS
#define _EXTERNS

extern I3DEngine* myEngine;
extern IModel* ground;
extern IMesh* bulletMesh;
extern deque <BulletData*> bullets;
extern deque <CShooter*> shooters;
extern deque <CCrate*> crates; 
extern deque <CRunner*> runners; 
extern float updateTime;
extern int numBullets;
extern bool faceLeft;
extern const float bulletSpeed;
extern int numBullets;
extern const int maxBullets;
extern const float baseHeight;
extern const float speed;
extern const float jumpSpeed;
extern enum EKeyCode jumpKey;
extern enum EKeyCode leftKey;
extern enum EKeyCode rightKey;
extern enum EKeyCode downKey;
extern enum EKeyCode fireKey;
extern bool faceRight;
extern bool faceLeft;
extern CPlayer* player;
extern float gravity;
extern bool collision;
extern const float playerY;
extern const float playerX;

#endif

bool collisionDetection(float modelOneX, float modelOneY, float modelTwoX, float modelTwoY, float xMin, float xMax, float yMin, float yMax)
{
		if(modelOneX > modelTwoX - xMin && modelOneX < modelTwoX + xMax)
		{
			if(modelOneY > modelTwoY - yMin && modelOneY < modelTwoY  + yMax)
			{
				return true;
			}

		}
		return false;
}   

//Template function that allows any deque array of models to be passed over to check for a collision with the players bullets 
template <class T> 
int colisionRoutine(deque <T*> models, float xMin, float xMax, float yMin, float yMax)
{
	for(int j = 0; j < models.size(); j++)
	{
		for (int i = 0; i < numBullets; i++)
		{
			if( collisionDetection( bullets[i]->model->GetX(), bullets[i]->model->GetY(), 
				 models[j]->getX(),  models[j]->getY(), xMin, xMax, yMin, yMax))
			{
 				return j;	
			}
		}
	}
	return -1;
}

void bulletMovement(float bulletSpeed, int maxBullets, float playerY, float playerX)
{
	//
	if (myEngine->KeyHit( fireKey ) && numBullets < maxBullets)
	{
		BulletData* tmpB = new BulletData;
		tmpB->bulletSpeed = bulletSpeed;
		tmpB->model = bulletMesh->CreateModel( 0.0f, 0.0f, 0.0f );
		tmpB->life = 1.0f;
		tmpB->model->Scale(0.5f);
        bullets.push_back( tmpB );

		bullets[numBullets]->model->AttachToParent(ground);
		bullets[numBullets]->model->SetLocalPosition(playerX, -ground->GetY() + playerY + 15.0f ,  -ground->GetX() + playerX);
		numBullets++;
		if(faceLeft)
		{
			tmpB->bulletSpeed = -bulletSpeed;
		}
	}
	
	for (int i = 0; i < numBullets; i++)
	{
		bullets[i]->model->Move( 0 , 0, bullets[i]->bulletSpeed * updateTime);
	}

	int in = colisionRoutine(shooters, 20, 20, 10, 20);
	if ( in > -1)
	{
		if( shooters[in] ==  shooters.back())
		{
			delete( shooters[in]);
			shooters.pop_back();
		}
		else
		{
			delete( shooters[in]);
			shooters[in] =  shooters.back();
 			shooters.pop_back();
		}	
	}

	in = colisionRoutine(runners, 20, 20, 10, 10);
	if ( in > -1)
	{
		if( runners[in] ==  runners.back())
		{
			delete( runners[in]);
			runners.pop_back();
		}
		else
		{
			delete( runners[in]);
			runners[in] =  runners.back();
 			runners.pop_back();
		}	
	}

	in = colisionRoutine(crates, 10, 10, 0, 20);
	if ( in > -1)
	{
		if( crates[in] ==  crates.back())
		{
			delete( crates[in]);
			crates.pop_back();
		}
		else
		{
			delete( crates[in]);
			crates[in] =  crates.back();
 			crates.pop_back();
		}	
	}

	for (int i = 0; i < numBullets; i++)
	{
		// Decrease life and see if bullet is dead
		bullets[i]->life -= updateTime;
		if (bullets[i]->life <= 0) //|| hit == true)
		{
			// Destroy bullet
  			bulletMesh->RemoveModel(bullets[i]->model);
			delete(bullets[i]);
			bullets.pop_front();
			// Decrease number of bullets
			numBullets--;
		}
	}
}

void playerMovement()
{
	float floorY = ground->GetY();

	//Movement Controls
	if(myEngine->KeyHeld(leftKey) && ground->GetX() < 0)
	{
		ground->MoveX(speed * updateTime);
		player->leftLeg();
		if(faceLeft == false)
		{
			faceRight = !faceRight;
			faceLeft = !faceLeft;
			player->GetModel()->RotateY(180);
		}
	}
	if(myEngine->KeyHeld(rightKey) && ground->GetX() < 1000)
	{
		ground->MoveX(-speed * updateTime);
		player->rightLeg();
		if(faceRight == false)
		{
			faceRight = !faceRight;
			faceLeft = !faceLeft;
			player->GetModel()->RotateY(180);
		}
	}
//Code to make the player look like they are jumping but the ground moves down
	if(myEngine->KeyHit(jumpKey))
	{
		//code to apply gravity and move the ground and platforms attached to it
		if(floorY > baseHeight || collision)
		{
			gravity = 3.5f;
		}
			ground->MoveY(-(jumpSpeed * gravity));
			gravity -= 0.0155f;
	}
	else
	{
	//checks to see if players is in the air moves ground up if so
		if(floorY < baseHeight && !collision)
		{
			gravity -= 0.0155f;
			ground->MoveY(-(jumpSpeed * gravity));
		}
		if(floorY < baseHeight && collision)
		{
			gravity = 0.0f;
		}
		if(floorY > baseHeight)
		{
			gravity = 3.5f;
		}
	//sets gravity to start value if thier is a collision or on ground floor
		if(floorY > baseHeight)
		{
			gravity = 3.5f;
		}
	}

	if(myEngine->KeyHeld(downKey) && collision)
	{
		ground->MoveY(jumpSpeed * updateTime + 5.0f);
	}

	bulletMovement(bulletSpeed, maxBullets, playerY, playerX);

}