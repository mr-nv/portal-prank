#pragma once
#include "main.hpp"

class CUtils
{
public:
	DWORD PatternSearch(const char* modulename, std::string pattern);
	bool WorldToScreen(const Vector& world, Vector& screen);
};

extern CUtils* g_pUtils;