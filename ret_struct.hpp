#pragma once

struct RetStruct
{
    int win_ammount = 0;
    int* spin_result = nullptr;
    int freespins_left = 0;

    RetStruct(const int wa, int* sr
            , const int fl)
            : win_ammount(wa), spin_result(sr)
            , freespins_left(fl) {}

    RetStruct() = default;
};
