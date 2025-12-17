#include <iostream>
#include <random>
#include "ret_struct.hpp"
#include "json.hpp" // from nlohman btw
using json = nlohmann::json;

using namespace nlohmann::literals;

class Params
{
public:
    // Each slot element/thing (idk, how to call it) has it's money_modyfier
    std::vector<float> elem_money_modyfier;
    int number_of_elem;
    // There are always at least 3 columns
    // Modyfier starts from the 4th column (modyfier of the 3rd column is always 1)
    // and if there's more values, when adds more columns. Can be empty
    std::vector<float> column_money_modyfier;
    int number_of_columns;

    int number_of_rows;

    int luck_value;
    int bonus_chance;
    int freespins_chance;
    
    // good thing for reference https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#rc-zero
    Params (const std::vector<float>& emm, const std::vector<float>& cmm, const int nor
          , const int lv, const int bc, const int fc)
                : elem_money_modyfier(emm), number_of_elem(emm.size())
                , column_money_modyfier(cmm), number_of_columns(cmm.size() + 3), number_of_rows(nor)
                , luck_value(lv), bonus_chance(bc), freespins_chance(fc) {}

    bool isBetween0and100(int num)
    {
        return num >= 0 & num <= 100 ? true : false;
    }

    bool verify()
    {
        if (isBetween0and100(luck_value)
          & isBetween0and100(bonus_chance)
          & isBetween0and100(freespins_chance)
          & number_of_elem > 2
          & number_of_rows > 0)
        {
            std::cout << "Number parameters are correct\n";

            for (float modyfier : elem_money_modyfier)
            {
                if (modyfier < 0)
                {
                    std::cout << "Negative modyfier value!\n";
                    return false;
                }
            }
            std::cout << "Modyfiers values are correct\n";

            return true;
        }

        std::cout << "Number parameters are wrong!";
        return false;
    }

};



int countMoney(int bet, float elem_money_modyfier, float column_money_modyfier)
{
    return float(bet) * elem_money_modyfier * column_money_modyfier;
}

Params parseParamsFromJson(std::string str_params)
{
    json parsed_params = json::parse(str_params);

    Params params(parsed_params["elem money modyfier"].get<std::vector<float>>()
                , parsed_params["column money modyfier"].get<std::vector<float>>()
                , parsed_params["number of rows"].get<int>(), parsed_params["luck value"].get<int>()
                , parsed_params["bonus chance"].get<int>(), parsed_params["freespins chance"].get<int>());

    return params;
}


std::random_device dev;
std::mt19937 rnd(dev());

// TODO: figure out with luck, freespeens and bonus game
extern "C" RetStruct spinSlot(int bet, std::string str_params)
{
    std::cout << str_params << std::endl;
    Params params {parseParamsFromJson(str_params)};

    if (params.verify() == false)
    {
        std::cout << "Input parameter verification error!" << std::endl;
        RetStruct bad_ret;
        return bad_ret;
    }
    int zero_char = 65;
    int* spin_result = new int[params.number_of_columns * params.number_of_rows]{0};

    std::uniform_int_distribution<int> dist(0, params.number_of_elem - 1);
    int money_earn = -bet;
    for (int i = 0; i < params.number_of_rows; ++i)
    {
        int match_count = 1;
        int elem_idx;
        bool skip = false;
        for (int j = 0; j < params.number_of_columns; ++j)
        {
            int cur_elem_idx = dist(rnd);
            spin_result[i * params.number_of_columns + j] = cur_elem_idx;

            std::cout << char(zero_char + cur_elem_idx) << ' ';

            if (skip)
                continue;

            if (j == 0)
            {
                elem_idx = cur_elem_idx;
            }
            else if (elem_idx == cur_elem_idx)
            {
                ++match_count;
            }
            else if (match_count > 2)
            {
                money_earn +=
                    countMoney(bet, params.elem_money_modyfier[elem_idx],
                               match_count > 3 ? params.column_money_modyfier[match_count - 4] : 1);
            }
            else
            {
                skip = true;
            }
        }
        std::cout << '\n';
    }
    std::cout << std::endl;
    struct RetStruct ret(money_earn, spin_result);
    return ret;
}
