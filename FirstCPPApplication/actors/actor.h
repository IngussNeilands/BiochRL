#ifndef ACTOR_H
#define ACTOR_H



#include "object.h"
#include <attr_effect.h>

class Representation;

class Tile;
class Thinker;
class Game;
class Inventory;
class Equipment;
class Item;
class AttributeContainer;
class Combat;

class TCODPath;
class Townsmen;
class Spell;
class IClass;

class TimedEffect;



class Actor : public Object 
{

    public:
        std::string name;
        Representation* representation;
        Thinker* thinker;
        Inventory* inventory;
        Equipment* equipment;
        std::vector<Spell*>* spells;

        AttributeContainer* attrs;
        bool has_attributes();
        bool is_active;

        TCODPath* l_path;

        int x,y;
        int dest_x, dest_y;
        Tile * my_tile;

        int xp;
        int xp_this_level;
        int xp_required;

        int level;
        int xp_value;

        std::string img_path;
        TCODImage* get_image();
        
        bool is_champion;
        bool is_hero;
        static int champ_chance;
        static int pack_size;
        static int preferred_pack_size;

        std::vector<TimedEffect*>* timed_attr_effects;
        std::vector<TimedEffect*>* timed_spell_effects;

        IClass* actor_class;
        static std::vector<IClass*>* actor_class_choices;

        bool is_moving_left;
        bool is_moving_right;
        bool is_moving_up;
        bool is_moving_down;
        bool is_sneaking;
        void resetIsMoving();
        int* get_direction_heading();

        actor_vec_t * actors_in_sight;

        bool is_fighter;
        Combat * combat;
        bool has_live_combat;
        bool has_attacked;

        bool try_detect(Actor* target);

        Actor();
        virtual ~Actor();
        std::string GetName();
        const char* GetNameC();

        virtual void update() = 0;

        Item* Actor::CreateCorpse();
        virtual void Die();

        void ScorePrintout();

        bool IsActorInSight(Actor * actor);
        void mark_as_unseen(Actor* actor);
        void mark_as_seen(int x, int y, Actor * actor);

        void putPerson( Tile * next_tile, int new_x, int new_y);

        void pickUpItem(Item* item);

        virtual void championize() = 0;

};

#endif
