// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>


#include <string>
#include <vector>
#include <algorithm>
#include <iostream>

#include <libtcod.hpp>

#include <utils.h>
#include "color_utils.h"
#include "statistics.h"

// TODO: reference additional headers your program requires here

class Actor;
class Tile;
class CustomKey;

typedef std::vector<Actor*> actor_vec_t;
typedef std::vector<Actor*>::iterator actor_vec_t_it;

typedef std::vector<Tile*> tile_vec_t;
typedef std::vector<Tile*>::iterator tile_vec_t_it;

typedef std::vector<CustomKey*> ckey_vec_t;
typedef std::vector<CustomKey*>::iterator ckey_vec_t_it;

//[assembly:AssemblyKeyFileAttribute("keyPair.snk")];
//debugging the heap that didn't work
// #include "crtdbg.h"
// #ifdef _DEBUG
//    #define DEBUG_CLIENTBLOCK   new( _CLIENT_BLOCK, __FILE__, __LINE__)
// #else
//    #define DEBUG_CLIENTBLOCK
// #endif // _DEBUG
// 
// #ifdef _DEBUG
// #define new DEBUG_CLIENTBLOCK
// #endif
