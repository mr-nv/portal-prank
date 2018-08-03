#include "main.hpp"

IVEngineClient* engine;
CEntityList* entitylist;
IBaseClientDLL* client;
IEngineVGui* enginevgui;
ISurface* surface;
IVDebugOverlay* debugoverlay;

CUtils* g_pUtils = nullptr;
unsigned long g_Font;

template<typename T>
T GetBP()
{
	__asm mov eax, ebp
}

typedef void(__thiscall* CreateMoveFn)(void*, int, float, bool);
CreateMoveFn oCreateMove;
void __stdcall hkdCreateMove(int sequence_number, float input_sample_frametime, bool active)
{
	oCreateMove(client, sequence_number, input_sample_frametime, active);

	auto pLocalEntity = entitylist->GetBaseEntity(engine->GetLocalPlayer());

	if (pLocalEntity)
	{
		static auto s = 0;
		if (s > 0)
		{
			s--;
			*(***GetBP<DWORD****>() + 1) -= 5;
		}
		else
		{
			if (GetAsyncKeyState('E'))
				s = 7; // speed amount
		}
	}
}

typedef void(__thiscall* PaintFn)(void*, int);
PaintFn oPaint;
void __stdcall hkdPaint(int type)
{
	oPaint(enginevgui, type);

	if (engine->Con_IsVisible() || enginevgui->IsGameUIVisible()) return;
	if (!(type & PAINT_UIPANELS)) return;

	static auto StartDrawing = (void(__thiscall*)(void*))(g_pUtils->PatternSearch("vguimatsurface.dll", "55 8B EC 83 EC 10 80 3D"));
	static auto FinishDrawing = (void(__thiscall*)(void*))(g_pUtils->PatternSearch("vguimatsurface.dll", "56 6A 00 E8"));

	static wchar_t watermarktext[128];
	static int watermarklength;

	if (!g_Font)
	{
		surface->SetFontGlyphSet(g_Font = surface->CreateFont_(), "Tahoma", 21, 500, 0, 0, 512);
		return;
	}

	static auto init = false;
	if (!init)
	{
		MultiByteToWideChar(CP_UTF8, 0, "portal-prank", -1, watermarktext, 128);
		watermarklength = wcslen(watermarktext);

		init = true;
	}

	StartDrawing(enginevgui);

	// draw text
	surface->DrawSetTextFont(g_Font);
	surface->DrawSetTextColor(255, 255, 255, 255);
	surface->DrawSetTextPos(10, 10);
	surface->DrawPrintText(watermarktext, watermarklength);

	// sexe
	surface->DrawSetTextPos(11, 11);
	surface->DrawPrintText(watermarktext, watermarklength);

	if (engine->IsInGame() && engine->IsConnected())
	{
		for (auto i = 0; i < entitylist->GetHighestEntityIndex(); i++)
		{
			auto pBaseEntity = entitylist->GetBaseEntity(i);
			if (!pBaseEntity) continue;

			auto clientclass = pBaseEntity->GetClientClass();
			if (!clientclass) continue;

			if (!strcmp(clientclass->GetName(), "CNPC_Portal_FloorTurret"))
			{
				const auto& trans = pBaseEntity->GetCoordinationFrame();
				auto collideable = pBaseEntity->GetCollideable();
				if (!trans || !collideable) return;

				auto min = collideable->OBBMins();
				auto max = collideable->OBBMaxs();

				Vector points[] = { Vector(min.x, min.y, min.z),
					Vector(min.x, max.y, min.z),
					Vector(max.x, max.y, min.z),
					Vector(max.x, min.y, min.z),
					Vector(max.x, max.y, max.z),
					Vector(min.x, max.y, max.z),
					Vector(min.x, min.y, max.z),
					Vector(max.x, min.y, max.z) };

				Vector pointsTransformed[8];
				for (int i = 0; i < 8; i++)
				{
					Vector innew = points[i];
					pointsTransformed[i].x = innew.Dot(Vector(trans[0][0], trans[0][1], trans[0][2])) + trans[0][3];
					pointsTransformed[i].y = innew.Dot(Vector(trans[1][0], trans[1][1], trans[1][2])) + trans[1][3];
					pointsTransformed[i].z = innew.Dot(Vector(trans[2][0], trans[2][1], trans[2][2])) + trans[2][3];
				}

				Vector pos = pBaseEntity->GetAbsOrigin();
				Vector flb;
				Vector brt;
				Vector blb;
				Vector frt;
				Vector frb;
				Vector brb;
				Vector blt;
				Vector flt;

				if (g_pUtils->WorldToScreen(pointsTransformed[3], flb) && g_pUtils->WorldToScreen(pointsTransformed[5], brt)
					&& g_pUtils->WorldToScreen(pointsTransformed[0], blb) && g_pUtils->WorldToScreen(pointsTransformed[4], frt)
					&& g_pUtils->WorldToScreen(pointsTransformed[2], frb) && g_pUtils->WorldToScreen(pointsTransformed[1], brb)
					&& g_pUtils->WorldToScreen(pointsTransformed[6], blt) && g_pUtils->WorldToScreen(pointsTransformed[7], flt))
				{
					Vector arr[] = { flb, brt, blb, frt, frb, brb, blt, flt };

					float left = flb.x;		// left
					float top = flb.y;		// top
					float right = flb.x;	// right
					float bottom = flb.y;	// bottom

					for (int i = 1; i < 8; i++)
					{
						if (left > arr[i].x)
							left = arr[i].x;
						if (top < arr[i].y)
							top = arr[i].y;
						if (right < arr[i].x)
							right = arr[i].x;
						if (bottom > arr[i].y)
							bottom = arr[i].y;
					}

					left = (int)std::round(left);
					right = (int)std::round(right);
					top = (int)std::round(top);
					bottom = (int)std::round(bottom);

					surface->DrawSetColor(255, 0, 0, 255);
					surface->DrawOutlinedRect(left, bottom, right, top);
					surface->DrawSetColor(0, 0, 0, 255);
					surface->DrawOutlinedRect(left + 1, bottom + 1, right - 1, top - 1);
					surface->DrawOutlinedRect(left - 1, bottom - 1, right + 1, top + 1);
				}
			}
			//printf("[%i] [%s]\n", i, clientclass->GetName());
		}
	}

	FinishDrawing(enginevgui);
}

template <typename T> T* GetInterface(const char* modulename, const char* interfacename)
{
	return (T*)((CreateInterfaceFn)(GetProcAddress(GetModuleHandleA(modulename), "CreateInterface")))(interfacename, 0);
}

bool GetInterfaces()
{
	engine = GetInterface<IVEngineClient>("engine.dll", "VEngineClient015");
	entitylist = GetInterface<CEntityList>("client.dll", "VClientEntityList003");
	client = GetInterface<IBaseClientDLL>("client.dll", "VClient016");
	enginevgui = GetInterface<IEngineVGui>("engine.dll", "VEngineVGui001");
	surface = GetInterface<ISurface>("vguimatsurface.dll", "VGUI_Surface031");
	debugoverlay = GetInterface<IVDebugOverlay>("engine.dll", "VDebugOverlay004");

	if (!engine || !entitylist || !client || !enginevgui || !surface || !debugoverlay) return false;

	printf("engine -> 0x%X\n", (DWORD)engine);
	printf("entitylist -> 0x%X\n", (DWORD)entitylist);
	printf("client -> 0x%X\n", (DWORD)client);
	printf("enginevgui -> 0x%X\n", (DWORD)enginevgui);
	printf("surface -> 0x%X\n", (DWORD)surface);
	printf("debugoverlay -> 0x%X\n", (DWORD)debugoverlay);

	return true;
}

bool Hook()
{
	CVMTHookManager* clienthook = new CVMTHookManager((PDWORD*)client);
	CVMTHookManager* enginevguihook = new CVMTHookManager((PDWORD*)enginevgui);

	oCreateMove = (CreateMoveFn)clienthook->HookMethod((DWORD)hkdCreateMove, 21);
	oPaint = (PaintFn)enginevguihook->HookMethod((DWORD)hkdPaint, 14);

	return true;
}

void Start()
{
	g_pUtils = new CUtils();

	AllocConsole();
	freopen("CONIN$", "r", stdin);
	freopen("CONOUT$", "w", stdout);
	freopen("CONOUT$", "w", stderr);
	SetConsoleTitleA("portal-prank");

	if (!GetInterfaces()) return;
	if (!Hook()) return;

	printf("portal-prank initialized, build %s %s\n", __DATE__, __TIME__);
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	if (fdwReason == DLL_PROCESS_ATTACH)
		CreateThread(0, 0, (LPTHREAD_START_ROUTINE)Start, lpvReserved, 0, 0);
		//Start();

	return true;
}