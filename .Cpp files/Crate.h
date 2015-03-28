#ifndef _CRATE
#define _CRATE
#include <TL-Engine.h>	// TL-Engine include file and namespace
#include "Player.h"
using namespace tle;
//This class has not been implemented due to lack of time
class CCrate
{
private:
	IMesh* mesh;
	IModel* model;
	char powerUp;
	bool isHit;
public:
	CCrate();
	bool createModel(float x, float y, float z);
	void updateCrate();
	float getX(){return model->GetX();};
	float getY(){return model->GetY();};
	~CCrate();
};
#endif