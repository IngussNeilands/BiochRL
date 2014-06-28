#pragma once
#ifndef THINKER_H
#define THINKER_H


class Game;
class Actor;
class Civilian;

//class to handle the ai, from combat to movement
class Thinker
{
    private:
        int consecutive_fails;
        bool path_possible;
        int skipped_tries;
        int consecutive_skips;
        int skip_threshold;
        int fail_threshold;


    public:
        static int visibility_threshold;
        long turn_last_seen_by_player;
        double tracking_memory;

        bool is_ally;

        Actor* master;
        Civilian* civilian;

        Actor* target;

        int is_dumb;
        int is_civ;
        int is_shopkeep;

        bool is_aware; //passive mode, not looking for hero

        void update();
        void smart_update();
        void update_path();
        void build_path();
        void set_target();

        void dumb_update();
        Thinker();
        ~Thinker();
        void walk_towards_target();
        void walk_dumbly();
        void try_attacking_player();

};
#endif
