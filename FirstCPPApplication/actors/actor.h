#ifndef ACTOR_H
#define ACTOR_H

#include "object.h"
#include <attr_effect.h>
#include <enums/champions_t.h>

class Representation;

class Tile;
class Thinker;
class Game;
class Inventory;
class Equipment;
class Item;
class AttributeContainer;
class Combat;
class Background;

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

        Background* background;
        AttributeContainer* attrs;
        bool has_attributes();
        bool is_active;

        unsigned long long total_gold;

        TCODPath* l_path;

        int x,y;
        int dest_x, dest_y;
        Tile * my_tile;

        int xp;
        int xp_this_level;
        int xp_required_to_lvlup;

        int level;
        int xp_value;

        unsigned int target_queue_tick;

        std::string img_path;
        TCODImage* get_image();

        bool is_champion;
        champions_t champion_type;
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

        void resetIsMoving();
        void get_direction_heading(int& dir_x, int& dir_y);

        actor_vec_t * actors_in_sight;

        Actor* soullinked_to;
        Actor* soullinked_from;
        void soullink_to(Actor* target);
        void unsoullink();
        void unsoullink_to();
        void unsoullink_from();

        bool is_sneaking;
        bool is_defending;
        bool is_fighter;
        Combat * combat;
        bool has_live_combat;
        bool has_attacked;

        bool try_detect(Actor* target);

        Actor();
        virtual ~Actor();
        Actor(Actor& other);
        std::string get_name();
        const char* get_name_c();

        virtual void update() = 0;

        Item* Actor::create_corpse();
        virtual void die();

        void printout_score();

        bool is_actor_in_sight(Actor * actor);
        void mark_as_unseen(Actor* actor);
        void mark_as_seen(int x, int y, Actor * actor);

        void put_person( Tile * next_tile, int new_x, int new_y);

        void pick_up_item(Item* item);

        virtual void championize() = 0;

};

#endif
