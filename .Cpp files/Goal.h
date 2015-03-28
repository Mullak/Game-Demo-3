#ifndef _GOAL
#define _GOAL
#include <TL-Engine.h>	// TL-Engine include file and namespace
using namespace tle;
// The goal class is used to indentify when the player has reached the end of a level
// the goal is a wide platform that when a collision is detected triggers the call to the gameRenewal function
class CGoal
{
private:
	IModel* model;
	IMesh* mesh;
public:
	CGoal();
	bool createModel(float x, float y, float z);
	float getX(){return model->GetX();};
	float getY(){return model->GetY();};
	~CGoal();
};
#endif