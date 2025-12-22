#include <iostream>
#include <random>
#include "ret_struct.hpp"
#include "json.hpp" // from nlohman btw
using json = nlohmann::json;

using namespace nlohmann::literals;

// Class what handles slot parameters
class Params
{
public:
    // Each slot element/thing (idk, how to call it) has it's modyfier
    std::vector<float> elem_modyfier;
    int number_of_elem;
    // There are always at least 3 columns
    // Modyfier starts from the 4th column (modyfier of the 3rd column is always 1)
    // and if there's more values, when adds more columns. Can be empty
    std::vector<float> column_modyfier;
    int number_of_columns;

    int number_of_rows;

    int luck_value;
    int bonus_chance; // has element id of -1
    int freespins_chance; // has element id of -2
    
    // good thing for reference https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#rc-zero
    Params (const std::vector<float>& em, const std::vector<float>& cm, const int nor
          , const int lv, const int bc, const int fc)
                : elem_modyfier(em), number_of_elem(em.size())
                , column_modyfier(cm), number_of_columns(cm.size() + 3), number_of_rows(nor)
                , luck_value(lv), bonus_chance(bc), freespins_chance(fc) {}

    bool isBetween0and100(int num)
    {
        return (num >= 0) & (num <= 100) ? true : false;
    }

    // Function to verify that input params are corrct
    bool verify()
    {
        if (isBetween0and100(luck_value)
          & isBetween0and100(bonus_chance)
          & isBetween0and100(freespins_chance)
          & isBetween0and100(bonus_chance + freespins_chance)
          & (number_of_elem > 2)
          & (number_of_rows > 0))
        {
            std::cout << "Number parameters are correct\n";

            for (float modyfier : elem_modyfier)
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

    float getColumnModifier(int match_count)
    {
        return match_count > 3 ? column_modyfier[match_count - 4] : 1;
    }
};



int countMoney(int bet, float elem_modyfier, float column_modyfier)
{
    return float(bet) * elem_modyfier * column_modyfier;
}

Params parseParamsFromJson(std::string str_params)
{
    json parsed_params = json::parse(str_params);

    Params params(parsed_params["elem modyfier"].get<std::vector<float>>()
                , parsed_params["column modyfier"].get<std::vector<float>>()
                , parsed_params["number of rows"].get<int>(), parsed_params["luck value"].get<int>()
                , parsed_params["bonus chance"].get<int>(), parsed_params["freespins chance"].get<int>());

    return params;
}


std::random_device dev;
std::mt19937 rnd(dev());

// TODO: figure out with luck, freespins and bonus
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

    std::uniform_int_distribution<int> elem_dist(0, params.number_of_elem - 1);
    std::uniform_int_distribution<int> random_chance_dist(1, 100);
    int money_earn = -bet;
    int freespins_left = 0;
    bool bonus_game = false;
    for (int i = 0; i < params.number_of_rows; ++i)
    {
        int match_count = 1;
        int elem_idx;
        bool skip = false;
        for (int j = 0; j < params.number_of_columns; ++j)
        {
            int random_chance = random_chance_dist(rnd);
            int cur_elem_idx;
            if (random_chance < params.bonus_chance)
            {
                cur_elem_idx = -1;
            }
            else if (random_chance < params.bonus_chance + params.freespins_chance)
            {
                cur_elem_idx = -2;
            }
            else
            {
                cur_elem_idx = elem_dist(rnd);
            }

            spin_result[i * params.number_of_columns + j] = cur_elem_idx;

            std::cout << char(zero_char + cur_elem_idx) << ' ';

            // Slot logic
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
                if (cur_elem_idx == -1)
                {
                    bonus_game = true;
                }
                else if (cur_elem_idx == -2)
                {
                    freespins_left += params.getColumnModifier(match_count); 
                }
                else
                {
                    money_earn += countMoney(bet, params.elem_modyfier[elem_idx]
                                           , params.getColumnModifier(match_count));
                }
            }
            else
            {
                skip = true;
            }
        }
        std::cout << '\n';
    }
    std::cout << std::endl;
    struct RetStruct ret(money_earn, spin_result, freespins_left, bonus_game);
    return ret;
}
