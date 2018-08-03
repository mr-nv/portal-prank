#pragma once

typedef void* (*CreateInterfaceFn)(const char *pName, int *pReturnCode);

inline void**& GetVTable(void* inst, size_t offset = 0)
{
	return *reinterpret_cast<void***>((size_t)inst + offset);
}
inline const void** GetVTable(const void* inst, size_t offset = 0)
{
	return *reinterpret_cast<const void***>((size_t)inst + offset);
}
template< typename Fn >
inline Fn GetVFunc(const void* inst, size_t index, size_t offset = 0)
{
	return (Fn)(GetVTable(inst, offset)[index]);
}

class CBaseEntity;

typedef unsigned long long VPANEL;
enum VGuiPanel_t
{
	PANEL_ROOT = 0,
	PANEL_GAMEUIDLL,
	PANEL_CLIENTDLL,
	PANEL_TOOLS,
	PANEL_INGAMESCREENS,
	PANEL_GAMEDLL,
	PANEL_CLIENTDLL_TOOLS
};

// In-game panels are cropped to the current engine viewport size
enum PaintMode_t
{
	PAINT_UIPANELS = (1 << 0),
	PAINT_INGAMEPANELS = (1 << 1),
	PAINT_CURSOR = (1 << 2), // software cursor, if appropriate
};

typedef float matrix3x4[3][4];

#define square( x ) ( x * x )
#define CHECK_VALID( _v ) 0

class Vector
{
public:
	Vector()
	{
		this->x = 0.0f;
		this->y = 0.0f;
		this->z = 0.0f;
	}
	Vector(float x, float y, float z)
	{
		this->x = x;
		this->y = y;
		this->z = z;
	}
	Vector(const Vector& v)
	{
		this->x = v.x;
		this->y = v.y;
		this->z = v.z;
	}
	Vector(const float* v)
	{
		this->x = v[0];
		this->y = v[1];
		this->z = v[2];
	}
	void Set(float x = 0.0f, float y = 0.0f, float z = 0.0f)
	{
		this->x = x;
		this->y = y;
		this->z = z;
	}
	bool operator!() const
	{
		return IsZero();
	}
	bool operator==(const Vector& v) const
	{
		return (this->x == v.x &&
			this->y == v.y &&
			this->z == v.z);
	}
	bool operator!=(const Vector& v) const
	{
		return (this->x != v.x ||
			this->y != v.y ||
			this->z != v.z);
	}
	float& operator[ ](unsigned int i)
	{
		return ((float*)this)[i];
	}
	float operator[ ](unsigned int i) const
	{
		return ((float*)this)[i];
	}
	Vector operator-() const
	{
		return Vector(-x, -y, -z);
	}
	Vector& operator=(const Vector& v)
	{
		CHECK_VALID(v);
		this->x = v.x;
		this->y = v.y;
		this->z = v.z;

		return *this;
	}
	Vector&	operator=(const float* v)
	{
		this->x = v[0];
		this->y = v[1];
		this->z = v[2];

		return *this;
	}
	Vector&	operator+=(const Vector& v)
	{
		this->x += v.x;
		this->y += v.y;
		this->z += v.z;

		return *this;
	}
	Vector&	operator-=(const Vector& v)
	{
		this->x -= v.x;
		this->y -= v.y;
		this->z -= v.z;

		return *this;
	}
	Vector&	operator*=(const Vector& v)
	{
		this->x *= v.x;
		this->y *= v.y;
		this->z *= v.z;

		return *this;
	}
	Vector&	operator/=(const Vector& v)
	{
		this->x /= v.x;
		this->y /= v.y;
		this->z /= v.z;

		return *this;
	}
	Vector&	operator+=(float v)
	{
		this->x += v;
		this->y += v;
		this->z += v;

		return *this;
	}
	Vector&	operator-=(float v)
	{
		this->x -= v;
		this->y -= v;
		this->z -= v;

		return *this;
	}
	Vector&	operator*=(float v)
	{
		this->x *= v;
		this->y *= v;
		this->z *= v;

		return *this;
	}
	Vector&	operator/=(float v)
	{
		this->x /= v;
		this->y /= v;
		this->z /= v;

		return *this;
	}
	Vector operator+(const Vector& v) const
	{
		return Vector(this->x + v.x, this->y + v.y, this->z + v.z);
	}
	Vector operator-(const Vector& v) const
	{
		return Vector(this->x - v.x, this->y - v.y, this->z - v.z);
	}
	Vector operator*(const Vector& v) const
	{
		return Vector(this->x * v.x, this->y * v.y, this->z * v.z);
	}
	Vector operator/(const Vector& v) const
	{
		return Vector(this->x / v.x, this->y / v.y, this->z / v.z);
	}
	Vector operator+(float v) const
	{
		return Vector(this->x + v, this->y + v, this->z + v);
	}
	Vector operator-(float v) const
	{
		return Vector(this->x - v, this->y - v, this->z - v);
	}
	Vector operator*(float v) const
	{
		return Vector(this->x * v, this->y * v, this->z * v);
	}
	Vector operator/(float v) const
	{
		return Vector(this->x / v, this->y / v, this->z / v);
	}
	float Dot(const Vector& v) const
	{
		return (this->x * v.x + this->y * v.y + this->z * v.z);
	}
	float Length() const
	{
		return sqrt(square(this->x) + square(this->y) + square(this->z));
	}
	float LengthSqr() const
	{
		return (square(this->x) + square(this->y) + square(this->z));
	}
	float Length2D() const
	{
		return sqrt(square(this->x) + square(this->y));
	}
	float Length2DSqr() const
	{
		return (square(this->x) + square(this->y));
	}
	float DistTo(const Vector& v) const
	{
		return (*this - v).Length();
	}
	float DistToSqr(const Vector& v) const
	{
		return (*this - v).LengthSqr();
	}
	float Normalize()
	{
		float flLength = Length();
		float flLengthNormal = 1.f / (FLT_EPSILON + flLength);

		x = x * flLengthNormal;
		y = y * flLengthNormal;
		z = z * flLengthNormal;

		return flLength;
	}
	bool IsZero() const
	{
		return (x == 0.0f && y == 0.0f && z == 0.0f);
	}
public:
	float		x;
	float		y;
	float		z;
};

__forceinline Vector operator*(float f, const Vector& v)
{
	return v * f;
}

class IVDebugOverlay
{
public:
	int ScreenPosition(const Vector& in, Vector& out) { return GetVFunc<int(__thiscall*)(void*, const Vector&, Vector&)>(this, 12)(this, in, out); }
};

class ClientClass
{
public:
	const char* GetName(void)
	{
		return *(char**)(this + 0x8);
	}
};

class IClientCollideable
{
public:
	const Vector& OBBMins() { return GetVFunc<const Vector&(__thiscall*)(void*)>(this, 1)(this); }
	const Vector& OBBMaxs() { return GetVFunc<const Vector&(__thiscall*)(void*)>(this, 2)(this); }
};

class IVEngineClient
{
public:
	bool IsInGame() { return GetVFunc<bool(__thiscall*)(void*)>(this, 25)(this); }
	bool IsConnected() { return GetVFunc<bool(__thiscall*)(void*)>(this, 26)(this); }
	bool Con_IsVisible() { return GetVFunc<bool(__thiscall*)(void*)>(this, 11)(this); }
	int GetLocalPlayer() { return GetVFunc<int(__thiscall*)(void*)>(this, 12)(this); }
};

class ISurface
{
public:
	void GetFontSize(unsigned long font, const wchar_t* text, int& wide, int& tall) { GetVFunc<void(__thiscall*)(void*, unsigned long, const wchar_t*, int&, int&)>(this, 78)(this, font, text, wide, tall); }
	void DrawOutlinedRect(int x0, int y0, int x1, int y1) { GetVFunc<void(__thiscall*)(void*, int, int, int, int)>(this, 17)(this, x0, y0, x1, y1); }
	void DrawSetColor(int r, int g, int b, int a) { GetVFunc<void(__thiscall*)(void*, int, int, int, int)>(this, 14)(this, r, g, b, a); }
	void DrawLine(int x0, int y0, int x1, int y1) { GetVFunc<void(__thiscall*)(void*, int, int, int, int)>(this, 18)(this, x0, y0, x1, y1); }
	unsigned long CreateFont_() { return GetVFunc<unsigned long(__thiscall*)(void*)>(this, 69)(this); }
	void SetFontGlyphSet(unsigned long &font, const char *windowsFontName, int tall, int weight, int blur, int scanlines, int flags) { GetVFunc<void(__thiscall*)(void*, unsigned long, const char*, int, int, int, int, int, int, int)>(this, 70)(this, font, windowsFontName, tall, weight, blur, scanlines, flags, 0, 0); }
	void DrawSetTextFont(unsigned long font) { GetVFunc<void(__thiscall*)(void*, unsigned long)>(this, 22)(this, font); }
	void DrawSetTextColor(int r, int g, int b, int a) { GetVFunc<void(__thiscall*)(void*, int, int, int, int)>(this, 24)(this, r, g, b, a); }
	void DrawSetTextPos(int x, int y) { GetVFunc<void(__thiscall*)(void*, int, int)>(this, 25)(this, x, y); }
	void DrawPrintText(const wchar_t* text, int length) { GetVFunc<void(__thiscall*)(void*, const wchar_t*, int, int)>(this, 27)(this, text, length, 0); }
};

class CEntityList
{
public:
	CBaseEntity* GetBaseEntity(int i) { return GetVFunc<CBaseEntity*(__thiscall*)(void*, int)>(this, 3)(this, i); }
	int GetHighestEntityIndex() { return GetVFunc<int(__thiscall*)(void*)>(this, 6)(this); }
};

class IBaseClientDLL
{
public:
};

class IEngineVGui
{
public:
	virtual ~IEngineVGui(void) {}
	virtual VPANEL GetPanel(VGuiPanel_t type) = 0;
	virtual bool IsGameUIVisible() = 0;
};

extern IVEngineClient* engine;
extern CEntityList* entitylist;
extern IBaseClientDLL* client;
extern IEngineVGui* enginevgui;
extern ISurface* surface;
extern IVDebugOverlay* debugoverlay;