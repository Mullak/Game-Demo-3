#ifndef _SHOOTER
#define _SHOOTER
#include <TL-Engine.h>	// TL-Engine include file and namespace
#include "Player.h"
using namespace tle;
//the BulletData structure contians the variables needed for the bullets to work correctly
struct BulletData
{
	IModel* model;
	float bulletSpeed;
	float life;//how long the bullet has till it is deleted
};
//this class controls the shooter NPC
//the shooter does not move and will shoot the player
//if within a certain range close to the NPC
//it will only fire one bullet at a time
class CShooter
{
private:
	IMesh* mesh;
	IModel* model;
	bool shot;// Makes sure only one bullet can be fired at a time 
				//by toggling to true when one has fired and false when it is deleted
	BulletData* bullet;
public:
	CShooter();
	bool createModel(float x, float y, float z);
	void updateShooter(CPlayer* player, float updateTime);
	float getX(){return model->GetX();};
	float getY(){return model->GetY();};
	~CShooter();
};
#endif