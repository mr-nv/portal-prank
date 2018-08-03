#pragma once
#include "main.hpp"
class CBaseEntity
{
public:
	Vector& GetAbsOrigin();
	IClientCollideable* GetCollideable();
	ClientClass* GetClientClass();
	matrix3x4& GetCoordinationFrame();
};