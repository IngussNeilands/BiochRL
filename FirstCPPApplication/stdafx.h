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
#include <queue>

#include <algorithm>
#include <iostream>

#include <libtcod.hpp>

#include <utils.h>
#include "color_utils.h"
#include "statistics.h"
#include "vector_helper.h"

// TODO: reference additional headers your program requires here

class Actor;
class Tile;
class Item;
class CustomKey;
class Spell;
class Attribute;
class MenuItem;

typedef std::vector<Actor*> actor_vec_t;
typedef std::vector<Actor*>::iterator actor_vec_t_it;

typedef std::vector<Tile*> tile_vec_t;
typedef std::vector<Tile*>::iterator tile_vec_t_it;

typedef std::vector<Item*> item_vec_t;
typedef std::vector<Item*>::iterator item_vec_t_it;

typedef std::vector<Spell*> spell_vec_t;
typedef std::vector<Spell*>::iterator spell_vec_t_it;

typedef std::vector<Attribute*> attr_vec_t;
typedef std::vector<Attribute*>::iterator attr_vec_t_it;

typedef std::vector<CustomKey*> ckey_vec_t;
typedef std::vector<CustomKey*>::iterator ckey_vec_t_it;

typedef std::vector<std::string*> str_vec_t;
typedef std::vector<std::string*>::iterator str_vec_t_it;

typedef std::vector<MenuItem*> menuitem_vec_t;
typedef std::vector<MenuItem*>::iterator menuitem_vec_t_it;

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
