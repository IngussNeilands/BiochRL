#include "stdafx.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <typeinfo>
#include <cstdlib>
#include <algorithm>
#include <windows.h>

// #include "libtcod_cpp_hpp\libtcod.hpp"

#include "FirstCPPApplication.h"

#include "utils.h"
#include <actors\Person.h>
#include "combat.h"
#include "map.h"
#include "game.h"
#include "input.h"

using namespace std;

std::string random_msg()
{
    static const std::string msg_array[] = {
        "Nhthfg Oheaf Erq vf zhfvp",
        "Fyrrcvat va gur Genva Lneq vf n terng fbat",
        "Vs lbhe tnzr unf tencuvpf, punaprf ner vg'f abg n ebthryvxr",
        "Crbcyr ner zrna",
        "Vs lbh'er rnfvyl bssraqrq fgrre pyrne bs guvf tnzr",
        "fb rqtl",
        "Guvf tnzr znl pbagnva napubivrf",
        "Gryy zr V'z gnyy, bar zber gvzr",
        "Fnsrgl, V snegrq",
        "Cerggl fher lbhe onfrzrag vf unhagrq",
        "OFGQLB",
        "Fbzr gvzrf, V trg fnq",
        "Jura jvyy gur sberfg fcrnx?",
        "V jnf znqr gb jnvg sbe lbh",
        "Yrg zl jbeqf or lbhe jbeqf",
        "Qebc vg naq cbc vg",
        "Purpx lbhe cevivyrtr"


            "Cercner gb qvr!"
    };
    // std::vector<std::string> msg_vector;
    TCODRandom *rng=TCODRandom::getInstance();
    int index = rng->getInt(0, sizeof(msg_array)/sizeof(*msg_array)-1);
    return msg_array[index];
};

std::string ROT13(std::string source)
{
    std::string transformed;
    for (size_t i = 0; i < source.size(); ++i) {
        if (isalpha(source[i])) {
            if ((tolower(source[i]) - 'a') < 13)
                transformed.append(1, source[i] + 13);
            else
                transformed.append(1, source[i] - 13);
        } else {
            transformed.append(1, source[i]);
        }
    }
    return transformed;
}

void WelcomeMessage(){

    printf("__________.__              .__   __________.____                             \n");
    printf("\\______   \\__| ____   ____ |  |__\\______   \\    |        .__         .__     \n");
    printf(" |    |  _/  |/  _ \\_/ ___\\|  |  \\|       _/    |      __|  |___   __|  |___ \n");
    printf(" |    |   \\  (  <_> )  \\___|   Y  \\    |   \\    |___  /__    __/  /__    __/ \n");
    printf(" |______  /__|\\____/ \\___  >___|  /____|_  /_______ \\    |__|        |__|    \n");
    printf("        \\/               \\/     \\/       \\/        \\/                        \n");
    // printf("\n\n");
    // printf("\t\t************************\n");
    // printf("\t\t WELCOME TO BiochRL++!\n");
    // printf("\t\t************************\n\n\n");
    printf("\n\t\t%s\n\n", ROT13(random_msg()).c_str());

    //printf("Most importantly, ignore the instructions that aren't the right ones. \n");
    printf("Num pad to move\n");
    printf("Move into someone to attack\n");
    //printf("Move into a door then press o to open or close it. It's dependant on the direction you're facing\n");

    printf("\n\nPress i to open inventory\n");
    printf("\nIn the inventory:");
    printf("\nAlphabetically, a is your first item, b is your second, z is your 26th or whatever\n");
    printf("You need to choose and then activate the item by pressing the appropriate button\n and then the button again to activate it\n");
    printf("Then you can press e to use the item\n");
    //printf("If you eat corpses you get one health per turn\n");
    //printf("You should have an item that you can eat in your inventory right now that gives 1k hp\n");
    printf("\n");
    printf("You'll get there, big guy\n");
};

int* getConsoleSize(){

    CONSOLE_SCREEN_BUFFER_INFO csbi;
    int columns, rows;

    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    columns = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    rows = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;

    // printf("columns: %d\n", columns);
    // printf("rows: %d\n", rows);

    int *info = new int[2];
    info[0] = columns;
    info[1] = rows;
    return info;
};

void clearScreen()
{
    system("cls");
}
