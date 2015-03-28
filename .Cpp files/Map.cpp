#include "map.h"
//
struct SMap
{
	char type;
	float x,y,z;
};

extern IMesh* platformMesh;
extern IMesh* platformWideMesh;

extern deque <CCrate*> crates; 
extern deque <CShooter*> shooters;
extern deque <CRunner*> runners;
extern deque <IModel*> platforms;
extern deque <IModel*> platformsWide;
extern IModel* ground;
extern CGoal* goal;
deque <SMap> map;



bool mapLoader()
{
	ifstream inp;
	inp.open("map.txt");
	SMap tmp;
	char tmpChar;
	while(!inp.eof())
	{
		inp >> tmp.type;
		inp >> tmp.y;
		inp >> tmpChar;
		inp >> tmp.z;
		inp >> tmpChar;
		tmp.x = -30;
		map.push_back(tmp);
	}
	inp.close();
	for(int i = 0; i < map.size(); i++)
	{
		IModel* tmp;
		CShooter* tmpS = new CShooter();
		CCrate* tmpC = new CCrate();
		CRunner* tmpR = new CRunner();
		if(map[i].type == 'p')
		{    
			tmp = platformMesh->CreateModel(map[i].x, map[i].y, map[i].z);
			tmp->AttachToParent(ground);
			tmp->Scale(0.75);
			platforms.push_back(tmp);
		}
		else if (map[i].type == 'w')
		{
			tmp = platformWideMesh->CreateModel(map[i].x, map[i].y, map[i].z);
			tmp->AttachToParent(ground);
			tmp->Scale(1.5);
			platformsWide.push_back(tmp);
		}

		else if (map[i].type == 's')
		{
			tmpS->createModel(map[i].x, map[i].y, map[i].z);
			shooters.push_back(tmpS);
		}
		else if (map[i].type == 'c')
		{
			tmpC->createModel(map[i].x, map[i].y, map[i].z);
			crates.push_back(tmpC);
		}
		else if (map[i].type == 'r')
		{
			tmpR->createModel(map[i].x, map[i].y, map[i].z);
			runners.push_back(tmpR);
		}
		else if(map[i].type == 'g')
		{
			goal->createModel(map[i].x, map[i].y, map[i].z);
		}
	}
	return true;
}

bool playerCollisionDectection(deque <IModel*> models, float maxXModifier, float minXModifier, float maxYModifier, float minYModifier)
{
	float playerY = 15.0f;
	float playerX = 0.0f;
	for(int i = 0; i < models.size(); i++) 
	{
		if(playerX > models[i]->GetX() - minXModifier && playerX < models[i]->GetX() + maxXModifier)
		{
			if(playerY > models[i]->GetY() - minYModifier && playerY < models[i]->GetY() + maxYModifier)
			{
				if(models[i]->GetY() + maxYModifier - playerY  < playerY - models[i]->GetY() +14.55)
				{
					return true;
				}
			}
		}
	}
	return false;
}