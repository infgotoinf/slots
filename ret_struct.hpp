#pragma once

struct RetStruct
{
    int win_ammount = 0;
    int* spin_result = nullptr;

    RetStruct(const int wa, int* sr)
            : win_ammount(wa), spin_result(sr) {}

    RetStruct() = default;
};
