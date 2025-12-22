#pragma once

struct RetStruct
{
    int win_ammount = 0;
    int* spin_result = nullptr;
    int freespins_left = 0;
    bool bonus_game = false;

    RetStruct(const int wa, int* sr
            , const int fl, const bool bg)
            : win_ammount(wa), spin_result(sr)
            , freespins_left(fl), bonus_game(bg){}

    RetStruct() = default;
};
