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
using namespace tle;

#include <al.h>      // Main OpenAL functions
#include <alc.h>     // OpenAL "context" functions (also part of main OpenAL API)
#include <AL/alut.h> // OpenAL utility library - helper functions

/*Sound Variables*/
ALuint buffer;
ALuint bufferBack;
ALuint bufferMenu;

ALuint source;
ALuint sourceBack;
ALuint Menu;

// Each source has several properties, see the code for examples. Here we store position and velocity of
// the sound source above (x, y & z)
ALfloat sourcePos[3] = { 0.0, 5.0, 100.0 };
ALfloat sourceVel[3] = { 0.0, 0.0, 0.0 };
ALfloat sourceMusicPos[3] = { 0.0, 0.0, 0.0 };
ALfloat sourceMusicVel[3] = { 0.0, 0.0, 0.0 };
// There is always assumed to be a listener in an OpenAL application. We don't need a specific listener
// variable. However, listeners also have properties (examples in code). Here we store the position and
// velocity of the listener
ALfloat listenerPos[3] = { 0.0, 5.0, 100.0 };
ALfloat listenerVel[3] = { 0.0, 0.0, 0.0 };

// The listener may be at an angle (which may affect the perception of sound). Here we store the 
// orientation of the listener. The first three values are the facing direction (x, y, z) of the
// listener - called "at" in the documentation. The next three values are the upward direction
// of the listener, called "up". These vectors can be extracted from a world or view matrix
// NOTE: OpenAL (like OpenGL) uses a right-handed system for 3D coordinates. To convert from the
// left-handed system  we have used, we must negate all Z values (facing direction has -ve Z below)
ALfloat listenerOri[6] = { 0.0, 0.0, -1.0,
                           0.0, 1.0, 0.0 };
float volume = 0.1f;
const float maxVolume = 2.0f;
const float minVolume = 0.0f;
//Constant Variables
const float baseHeight = 0.0f;// Height the main floor is at
const float speed = 55.0f; // speed of player and jumping
const float jumpSpeed = 0.10f;

//Engine
I3DEngine* myEngine = New3DEngine( kTLX );

//Bullet variables
const float bulletSpeed = 80.0f; // Movement speed of a bullet
int numBullets = 0;
const int maxBullets = 5;

//Mesh/Model Variables
ICamera* camera = NULL;
IMesh* floorMesh = NULL;
IMesh* bulletMesh = NULL;
IMesh* runnerMesh = NULL;
IMesh* platformMesh = NULL;
IMesh* platformWideMesh = NULL;
IMesh* crateMesh = NULL;

IModel* bullet = NULL;
IModel* ground = NULL;
deque <IModel*> crates; 
deque <CShooter*> shooters;
deque <IModel*> runners;
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
ISprite* textBackColour = NULL; //background square for front end.

//Control Variables
enum EKeyCode quitKey = Key_Escape;
enum EKeyCode jumpKey = Key_Up;
enum EKeyCode leftKey = Key_Left;
enum EKeyCode rightKey = Key_Right;
enum EKeyCode downKey = Key_Down;
enum EKeyCode fireKey = Key_Space;
enum EKeyCode enterKey = Key_Return;
enum EKeyCode pauseKey = Key_P;
float gravity = 2.5f;
float updateTime = 0.0f; // calculating the updatetime every frame
const float playerY = 5.0f;
const float playerX = 0.0f;

int playerHealth;
int playerLives;

bool isPaused = false;
bool isQuiting = false;
bool isBegining = false;
bool collision = false;

bool faceRight = true;
bool faceLeft = false;
bool isDead = false;
/************Structures/Classes****************/

deque <BulletData*> bullets;
/*Classes*/
CPlayer* player = NULL;

void frontEndSetUp()
{	
	soundLoader(volume);
	
	alSourcePlay( Menu );
	frontEndFont = myEngine->LoadFont("Poplar Std", 85);
	textBackColour = myEngine->CreateSprite("BackTextColour.png",145,100,0);
	backdrop = myEngine->CreateSprite( "lab.jpg", 0, 0, 1);
}

void frontEndUpdate()
{
	frontEndFont->Draw("Start Game",155,100);
	frontEndFont->Draw("Quit",155,200);

	if(myEngine->KeyHit(downKey))
	{
		textBackColour->SetY(200);
	}
	else if(myEngine->KeyHit(jumpKey) )
	{
		textBackColour->SetY(100);
	}
	if(myEngine->KeyHit(enterKey) && textBackColour->GetY() == 100)
	{
		isBegining = !isBegining;
	}

	if(myEngine->KeyHit(quitKey)  && textBackColour->GetY() == 200)
	{
		isQuiting = !isQuiting;
	}
}

void frontEndRemovel()
{
	myEngine->RemoveSprite(textBackColour);
	myEngine->RemoveFont(frontEndFont);
	myEngine->RemoveSprite(backdrop);
	alSourceStop( Menu );
}

void gameShutDown()
{
	myEngine->Stop();
}
	
void gameSetUp()
{
	alSourcePlay( sourceBack );
	player = new CPlayer();
	/*Model Setup*/
	floorMesh = myEngine->LoadMesh("Ground.x");
	bulletMesh = myEngine->LoadMesh("Bullet.x");
	platformMesh = myEngine->LoadMesh("platform.x");
	platformWideMesh = myEngine->LoadMesh("PlatformWide.x");
	crateMesh = myEngine->LoadMesh("Crate.x");
	ground = floorMesh->CreateModel(-15.0f, baseHeight, 60.0f);
	ground->RotateY(90);
	camera = myEngine->CreateCamera(kManual,0.0f,40.0f,-10.0f);
	numBullets = 0;
	mapLoader();
	/*UI Setup*/
	FPSDisplay = myEngine->LoadFont( "Comic Sans MS", 36);
}

void gameUpdate()
{
	player->GetHealth(playerHealth);
	player->GetLives(playerLives);
	updateTime = myEngine->Timer();
	outText << "Health: " << playerHealth;
	FPSDisplay ->Draw( outText.str(), fontX, fontY, kWhite );
	outText.str("");
	outText << "Lives remaining: " << playerLives ;
	FPSDisplay ->Draw( outText.str(), livesfontX, livesfontY, kWhite );
	outText.str("");
	float floorY = ground->GetY();
	if(myEngine->KeyHit(Key_Period) )
	{
		if( volume > minVolume)
		{
		volume -= 0.1; 
		alListenerf ( AL_GAIN, volume ); 
		}
	}
	else if(myEngine->KeyHit(Key_Comma))
	{
		if( volume < maxVolume)
		{
			volume += 0.1;
			alListenerf ( AL_GAIN, volume );
		}
	}

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
		}
	}

	collision = playerCollisionDectection(platforms,20.0f,0.0f,15.0f,-14.75);
	if(!collision)
	{
		collision = playerCollisionDectection(platformsWide,30.0f,25.0f,15.0f,-14.75);
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
			gravity = 2.5f;
		}
			ground->MoveY(-(jumpSpeed * gravity));
			gravity -= 0.0105f;
	}
	else
	{
	//checks to see if players is in the air moves ground up if so
		if(floorY < baseHeight && !collision)
		{
			gravity -= 0.0105f;
			ground->MoveY(-(jumpSpeed * gravity));
		}
		if(floorY < baseHeight && collision)
		{
			gravity = 0.0f;
		}
	//sets gravity to start value if thier is a collision or on ground floor
		if(floorY > baseHeight)
		{
			gravity = 2.5f;
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

	bulletMovement(bulletSpeed, maxBullets, playerY, playerX, false);

	if(shooters.size())
	{
		for(int i = 0; i < shooters.size(); i++)
		{
			shooters[i]->updateShooter(player, updateTime);
		}
	}
}
	
void gameRemovel()
{
	alutExit();
	delete(player);
	floorMesh->RemoveModel(ground);
	myEngine->RemoveMesh(floorMesh);
	myEngine->RemoveMesh(bulletMesh);
	myEngine->RemoveCamera(camera);

	for(int i = 0; i < platforms.size(); i++)
	{
		platformMesh->RemoveModel(platforms[i]);
	}
	myEngine->RemoveMesh(platformMesh);

	for(int i = 0; i < crates.size(); i++)
	{
		crateMesh->RemoveModel(crates[i]);
	}
	myEngine->RemoveMesh(crateMesh);

	for(int i = 0; i < platformsWide.size(); i++)
	{
		platformWideMesh->RemoveModel(platformsWide[i]);
	}
	myEngine->RemoveMesh(platformWideMesh);
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
			gameShutDown();
		}
	}

	frontEndRemovel();

	if(myEngine->IsRunning())
	{
		gameSetUp();
	}
	
	while (myEngine->IsRunning())
	{
		// Draw the scene
		myEngine->DrawScene();
		if(!isPaused)
		{
			gameUpdate();
		}
		else if(isDead)
		{
			FPSDisplay->Draw("Game Over", 500, 500, kWhite);
			if(myEngine->KeyHit(quitKey))
			{
				break;
			}
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
			break;
		}
	}
	gameRemovel();
	
	// Delete the 3D engine now we are finished with it
	myEngine->Delete();
	
}