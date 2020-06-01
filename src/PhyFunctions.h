#pragma once
#ifdef WIN32
PhysicsBody WIN_GetBody(int type, float x, float y, float first, float second, float third);
void WIN_InitPhysics();
void WIN_SetPhysicsGravity(float x, float y);
void WIN_RunPhysicsStep();
void WIN_ClosePhysics();
#else
PhysicsBody LNX_GetBody(int type, float x, float y, float first, float second, float third);
#endif