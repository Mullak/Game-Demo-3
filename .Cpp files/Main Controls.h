#ifndef _MAIN
#define _MAIN
bool collisionDetection(float modelOneX, float modelOneY, float modelTwoX, float modelTwoY, float xMin, float xMax, float yMin, float yMax);
void bulletMovement(float bulletSpeed, int maxBullets, float playerY, float playerX);
void playerMovement();
#endif