#pragma once

#include "common.h"
#include "states/state.h"

class Game
{
    public:
        Game(int argc, char* argv[]);
        ~Game();

        void update();
    private:
        State* state;

        u32 old_time_limit;
};
