#ifndef _MAP
#define _MAP
#include <TL-Engine.h>	// TL-Engine include file and namespace
#include <fstream>
#include <sstream>
#include <iostream>
#include <deque>
#include "Shooter.h"
#include "Crate.h"
#include "Runner.h"
#include "Goal.h"
using namespace tle;

bool mapLoader();
bool playerCollisionDectection(deque <IModel*> models, float maxXModifier, float minXModifier, float maxYModifier, float minYModifier);

#endif