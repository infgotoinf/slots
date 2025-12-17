#include <iostream>
#include <Windows.h>
#include "json.hpp" // from nlohman btw
using json = nlohmann::json;

using namespace nlohmann::literals;
int main()
{
    json j;

    j["elem money modyfier"] = {1.5, 3, 5, 10};
    j["column money modyfier"] = {2, 5};
    
    j["number of rows"] = 3;
    j["luck value"] = 50;
    j["bonus chance"] = 2;
    j["freespins chance"] = 5;

    std::string str_params = j.dump();
    
    int bet = 100;

// loading DLL part
    typedef (*casino_func)(int bet, std::string str_params);

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
    while (true)
    {
        int win = function(100, str_params);
        std::cout << "you won: " << win << '\n';
        std::cin.get();
    }

    return 0;
}
