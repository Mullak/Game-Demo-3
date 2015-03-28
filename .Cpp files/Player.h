#ifndef _PLAYER
#define _PLAYER
#include <TL-Engine.h>
#include <windows.h>
using namespace tle;
extern I3DEngine* myEngine;
//the player class the controls the player
class CPlayer
{
private:
	int health;
	int lives;
	IMesh* playerMesh;
	IModel* model;
	int legLimit;// the height limit each leg can go
	int lCounter;// the current height of the left leg
	int rCounter;// the current height of the right leg
	bool nextLeg;// triggers the swicth in the direction that each leg moves in once the limit has been reached
public:
	CPlayer();
	~CPlayer();
	void SetHealth( int damage);
	void SetLives(int life);
	IModel* GetModel() const;
	void GetHealth( int &playerHealth) const;
	void GetLives(int & playerLife) const;
	void leftLeg();// moves trhe left leg of the player model
	void rightLeg();// moves trhe right leg of the player model
	float GetX(){return model->GetX();};
	float GetY(){return model->GetY();};
};
#endif