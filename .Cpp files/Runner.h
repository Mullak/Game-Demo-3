#ifndef _RUNNER
#define _RUNNER
#include <TL-Engine.h>	// TL-Engine include file and namespace
#include "Player.h"
using namespace tle;
//the class for the runner NPC, run towards the player when they are within attack range
class CRunner
{
private:
	IMesh* mesh;
	IModel* model;
	bool isRight;// used to check in what direction the NPC is facing
	int timer;// //used to avoid the NPC from staying at the same coordinates as the player consantly 
public:
	CRunner();
	bool createModel(float x, float y, float z);
	void updateRunner(CPlayer* player, float updateTime);
	float getX(){return model->GetX();};
	float getY(){return model->GetY();};
	~CRunner();
};
#endif