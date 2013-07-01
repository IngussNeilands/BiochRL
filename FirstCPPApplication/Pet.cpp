#include "stdafx.h"
#include "Person.h"
#include <iostream>
#include <sstream>
#include <typeinfo>
#include <cstdlib>
#include <algorithm>

#include "Pet.h"

using namespace std;


void Pet::printout(){

    cout << "\nPET PRINTOUT" << endl;
    cout << "NAME: " << name << endl;
    cout << "MAX HP: " << max_hp << endl;

};

Pet::Pet(){
    name = "Nameless Pet";
    max_hp = 100;
    cur_hp = max_hp;

    representation = 'p';

    master = NULL;
    is_dead = false;
};

Pet::Pet(string name, int max_hp, char representation ){
    name = name;
    cur_hp = max_hp;
    max_hp = max_hp;

    representation = representation;

    master = NULL;
    is_dead = false;


};

void Pet::Attack(Pet* ptr_target, int dmg){
    //attacks another pet for dmg damage
    // cout << ptr_target << endl;

    cout << "About to attack " << (*ptr_target).name << endl;
    ptr_target->TakeDamage(dmg);

    bool boo = ptr_target->CheckDeath();
    if (boo){
        printf("AHAAHAHA, you're dead bro, that's got to feel bad.\n");
    };

};

void Pet::TakeDamage(int dmg){
    printf("About to take %d damage! ", dmg);
    printf("With %d hp.\n", cur_hp);
    cur_hp-=dmg;
    cout << name << " at " << cur_hp << " health left!" << endl;

    CheckDeath();

};

bool Pet::CheckDeath(){
    if (cur_hp <= 0  && !is_dead){
	printf("OH NO I\"M DEAD\n");
	is_dead = true;
    }
    return is_dead;

};
