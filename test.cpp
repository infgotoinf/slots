#include <iostream>
#include <vector>
#include <Windows.h>
#include "ret_struct.hpp"
#include "json.hpp" // from nlohman btw
using json = nlohmann::json;

using namespace nlohmann::literals;
int main()
{
    json j;

    j["elem modyfier"] = {1.5, 3, 5, 10};
    std::vector<float> column_modyfier = {2, 5};
    j["column modyfier"] = column_modyfier;

    int number_of_rows = 3;
    j["number of rows"] = number_of_rows;
    j["luck value"] = 50;
    j["wild chance"] = 5;
    j["freespins chance"] = 5;

    std::string str_params = j.dump();
    
    int bet = 100;

// loading DLL part
    typedef RetStruct (*casino_func)(int bet, std::string str_params, int freespins_left);

    const char* path = "./slots.dll";

    void* handle = nullptr;
    casino_func function = nullptr;

    handle = LoadLibraryA(path);
    if (handle == nullptr)
    {
        std::cerr << "Error: Could not load library. Error code: " << GetLastError() << std::endl;
        return 1;
    }

    const char* main_func_name = "spinSlot";
    function = (casino_func)GetProcAddress((HMODULE)handle, main_func_name);

    if (function == nullptr)
    {
        std::cerr << "Error: Could not find the " << main_func_name << " function." << std::endl;
        FreeLibrary((HMODULE)handle);
        return 1;
    }

// function test
    int number_of_columns = column_modyfier.size() + 3;
    int freespins_left = 0;
    while (true)
    {
        RetStruct win = function(bet, str_params, freespins_left);
        freespins_left = win.freespins_left;
        std::cout << "you won: " << win.win_ammount << '\n';
        std::cout << "number of freespins left: " << win.freespins_left << '\n';

        /*
        std::cout << "lines were:\n";
        for (int i = 0; i < number_of_rows; ++i)
        {
            for (int j = 0; j < number_of_columns; ++j)
            {
                std::cout << win.spin_result[i * number_of_columns + j] << ' ';
            }
            std::cout << '\n';
        }
        */
        std::cin.get();
        std::cout << '\n' << std::endl;
    }
    return 0;
}
