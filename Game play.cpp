// Game play.cpp: A program using the TL-Engine
#include <TL-Engine.h>	// TL-Engine include file and namespace
#include <fstream>
#include <sstream>
#include <iostream>
#include <time.h>
#include <vector>
#include <deque>
#include <stdlib.h> // General console window includes
#include <conio.h>
#include <ctype.h>
#include "map.h"
#include "Player.h"
#include "Main Controls.h"
#include "Shooter.h"
#include "Crate.h"
#include "Runner.h"
#include "Goal.h"
using namespace tle;

//Constant Variables
const float baseHeight = 0.0f;// Height the main floor is at
const float speed = 175.0f; // speed of player and jumping
const float jumpSpeed = 0.07f;

//Engine
I3DEngine* myEngine = New3DEngine( kTLX );

//Bullet variables
const float bulletSpeed = 105.0f; // Movement speed of a bullet
int numBullets = 0;
const int maxBullets = 5;

//Mesh/Model Variables
ICamera* camera = NULL;
IMesh* floorMesh = NULL;
IMesh* bulletMesh = NULL;
IMesh* runnerMesh = NULL;
IMesh* platformMesh = NULL;
IMesh* platformWideMesh = NULL;

IModel* bullet = NULL;
IModel* ground = NULL;
deque <CCrate*> crates; 
deque <CShooter*> shooters;
deque <CRunner*> runners;
deque <IModel*> platforms;
deque <IModel*> platformsWide;

//Text Variables
int fontY = 20;
int fontX = 20;
int livesfontY = 40;
int livesfontX = 40;
IFont* FPSDisplay = NULL;
stringstream outText;
ISprite* backdrop = NULL;
IFont* frontEndFont = NULL; // the font for the front end.
IFont* RenewelFont = NULL;
IFont* loadingFont = NULL;

//Control Variables
enum EKeyCode quitKey = Key_Escape;
enum EKeyCode jumpKey = Key_W;
enum EKeyCode leftKey = Key_A;
enum EKeyCode rightKey = Key_D;
enum EKeyCode downKey = Key_S;
enum EKeyCode fireKey = Key_Space;
enum EKeyCode enterKey = Key_Return;
enum EKeyCode pauseKey = Key_P;
float gravity = 3.5f;
float updateTime = 0.0009f; // calculating the updatetime every frame
const float playerY = 5.0f;
const float playerX = 0.0f;

int playerHealth;
int playerLives;

bool isPaused = false;
bool isQuiting = false;
bool isBegining = false;
bool isFinished = false;
bool collision = false;

bool faceRight = true;
bool faceLeft = false;
bool isDead = false;
/************Structures/Classes****************/

deque <BulletData*> bullets;
/*Classes*/
CPlayer* player = NULL;
CGoal* goal = NULL;

void loading()
{
	loadingFont = myEngine->LoadFont("Poplar Std", 85);
	loadingFont->Draw("Loading...",155,100);
}

void frontEndSetUp()
{	
	frontEndFont = myEngine->LoadFont("Poplar Std", 65);
	backdrop = myEngine->CreateSprite( "lab.jpg", 0, 0, 1);
}

void frontEndUpdate()
{
	frontEndFont->Draw("Press enter to Start Game",40,100);
	frontEndFont->Draw("Press Esc to Quit",40,200);

	if(myEngine->KeyHit(enterKey)) 
	{
		isBegining = !isBegining;
	}

	if(myEngine->KeyHit(quitKey))
	{
		isQuiting = !isQuiting;
	}
}

void frontEndRemovel()
{
	myEngine->RemoveFont(frontEndFont);
	myEngine->RemoveSprite(backdrop);
}
	
void gameSetUp()
{
	player = new CPlayer();
	goal = new CGoal();
	/*Model Setup*/
	floorMesh = myEngine->LoadMesh("Ground.x");
	bulletMesh = myEngine->LoadMesh("Bullet.x");
	platformMesh = myEngine->LoadMesh("platform.x");
	platformWideMesh = myEngine->LoadMesh("PlatformWide.x");
	ground = floorMesh->CreateModel(-15.0f, baseHeight, 60.0f);
	ground->RotateY(90);
	camera = myEngine->CreateCamera(kManual,0.0f,40.0f,-10.0f);
	numBullets = 0;
	mapLoader();
	/*UI Setup*/
	FPSDisplay = myEngine->LoadFont( "Comic Sans MS", 36);
	myEngine->RemoveFont(loadingFont);
}

void gameRenewal();

void gameUpdate()
{
	player->GetHealth(playerHealth);
	player->GetLives(playerLives);
	outText << "Health: " << playerHealth;
	FPSDisplay ->Draw( outText.str(), fontX, fontY, kWhite );
	outText.str("");
	outText << "Lives remaining: " << playerLives;
	FPSDisplay ->Draw( outText.str(), livesfontX, livesfontY, kWhite );
	outText.str("");
	float floorY = ground->GetY();

	if(playerHealth <= 0)
	{
		player->SetLives(1);
		if(playerLives > 0)
		{
			player->SetHealth(-100);
		}
		else
		{
			isDead = true;
			RenewelFont = myEngine->LoadFont("Poplar Std", 65);
			return;
		}
	}

	collision = playerCollisionDectection(platforms, 20.0f,0.0f,15.0f,-14.75);
	if(!collision)
	{
		collision = playerCollisionDectection(platformsWide, 30.0f,25.0f,15.0f,-14.75);
	}
	if(collisionDetection(playerX, playerY, goal->getX(), goal->getY(), 30.0f,25.0f,15.0f,-14.75))
	{
		RenewelFont = myEngine->LoadFont("Poplar Std", 65);
		isFinished = true;
	}

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

	if(myEngine->KeyHit(pauseKey))
	{
		isPaused = !isPaused;
	}

	bulletMovement(bulletSpeed, maxBullets, playerY, playerX);

	for(int i = 0; i < shooters.size(); i++)
	{
		shooters[i]->updateShooter(player, updateTime);
	}
	

	for(int i = 0; i < runners.size(); i++)
	{
		runners[i]->updateRunner(player, updateTime);
	}
	
}

void gameRemovel()
{
	delete(player);
	delete(goal);
	floorMesh->RemoveModel(ground);
	myEngine->RemoveMesh(floorMesh);
	myEngine->RemoveMesh(bulletMesh);
	myEngine->RemoveCamera(camera);
	for(int i = 0; i < platforms.size(); i++)
	{
		platformMesh->RemoveModel(platforms[i]);
	}
	myEngine->RemoveMesh(platformMesh);
	for(int i = 0; i < platformsWide.size(); i++)
	{
		platformWideMesh->RemoveModel(platformsWide[i]);
	}
	myEngine->RemoveMesh(platformWideMesh);
}

void gameRenewal()
{
		RenewelFont->Draw("Press Enter to Retry",40,100);
		RenewelFont->Draw("Press Esc to Quit",40,200);

		if(myEngine->KeyHit(enterKey)) 
		{
			myEngine->RemoveFont(RenewelFont);
			loading();
			myEngine->DrawScene();
			player->~CPlayer();
			goal ->~CGoal();
			floorMesh->RemoveModel(ground);
			myEngine->RemoveMesh(floorMesh);
			myEngine->RemoveMesh(bulletMesh);
			myEngine->RemoveCamera(camera);
			for(int i = 0; i < platforms.size(); i++)
			{
				platformMesh->RemoveModel(platforms[i]);
			}
			for(int i = 0; i < platformsWide.size(); i++)
			{
				platformWideMesh->RemoveModel(platformsWide[i]);
			}
			for(int i = 0; i < shooters.size(); i++)
			{
				shooters[i]->~CShooter();
			}
			for(int i = 0; i < runners.size(); i++)
			{
				runners[i]->~CRunner();
			}
			for(int i = 0; i < crates.size(); i++)
			{
				crates[i]->~CCrate();
			}
			myEngine->RemoveMesh(platformMesh);
			myEngine->RemoveMesh(platformWideMesh);
			shooters.clear();
			runners.clear();
			crates.clear();
			platformsWide.clear();
			platforms.clear();
			gameSetUp();
			isFinished = false;
			isDead = false;
		}
		else if(myEngine->KeyHit(quitKey))
		{
			isQuiting = !isQuiting;
			myEngine->RemoveFont(RenewelFont);
		}
	
}

void main()
{
	// Create a 3D engine (using TLX engine here) and open a window for it
	myEngine->StartWindowed(800, 500);

	// Add default folder for meshes and other media
	myEngine->AddMediaFolder( "Media" );

	frontEndSetUp();

	while (myEngine->IsRunning())
	{
		// Draw the scene
		myEngine->DrawScene();

		frontEndUpdate();

		if(isBegining)
		{
			break;
		}
		if(isQuiting)
		{
			myEngine->Stop();
		}
	}

	frontEndRemovel();
	loading();
	myEngine->DrawScene();

	if(!isQuiting)
	{
		gameSetUp();
	}
	
	while (myEngine->IsRunning())
	{
		if(isQuiting)
		{
			break;
		}
		// Draw the scene
		myEngine->DrawScene();
		if(isFinished)
		{
			gameRenewal();
		}
		else if(isDead)
		{
			gameRenewal();
		}
		else if(!isPaused)
		{
			gameUpdate();
		}
		else
		{ 
			if(myEngine->KeyHit(pauseKey))
			{
				isPaused = !isPaused;
			}
			FPSDisplay->Draw("Paused", 500, 500, kWhite);
		}
		
		if(myEngine->KeyHit(quitKey))
		{
			gameRemovel();
			break;
		}
	}
	// Delete the 3D engine now we are finished with it
	myEngine->Delete();
	
}