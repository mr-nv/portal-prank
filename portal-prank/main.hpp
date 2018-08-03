#pragma once
#include <windows.h>
#include <cstdint>
#include <iostream>
#include <string>
#include <Psapi.h>

// fuck that shit
#pragma warning(disable : 4244)

#define INRANGE(x,a,b)    (x >= a && x <= b)
#define getBits( x )    (INRANGE((x&(~0x20)),'A','F') ? ((x&(~0x20)) - 'A' + 0xa) : (INRANGE(x,'0','9') ? x - '0' : 0))
#define getByte( x )    (getBits(x[0]) << 4 | getBits(x[1]))

#include "crc32.hpp"
#include "sdk.hpp"
#include "entity.hpp"
#include "vmt.hpp"
#include "utils.hpp"

extern unsigned long g_Font;