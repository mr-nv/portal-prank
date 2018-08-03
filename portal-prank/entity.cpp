#include "entity.hpp"

Vector& CBaseEntity::GetAbsOrigin() { return GetVFunc<Vector&(__thiscall*)(void*)>(this, 10)(this); }
IClientCollideable* CBaseEntity::GetCollideable() { return GetVFunc<IClientCollideable*(__thiscall*)(void*)>(this, 3)(this); }
matrix3x4& CBaseEntity::GetCoordinationFrame() { return *(matrix3x4*)((DWORD)this + 0x538); }
ClientClass* CBaseEntity::GetClientClass()
{
	auto networkable = (void*)(this + 0x8);
	return GetVFunc<ClientClass*(__thiscall*)(void*)>(networkable, 2)(networkable);
}