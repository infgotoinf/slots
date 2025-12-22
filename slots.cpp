#include <iostream>
#include <random>
#include "ret_struct.hpp"
#include "json.hpp" // from nlohman btw
using json = nlohmann::json;

using namespace nlohmann::literals;

enum class IDX 
{
    wild = -1,
    freespins = -2
};

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
    int wild_chance;
    int freespins_chance;
    
    // good thing for reference https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#rc-zero
    Params (const std::vector<float>& em, const std::vector<float>& cm, const int nor
          , const int lv, const int wc, const int fc)
                : elem_modyfier(em), number_of_elem(em.size())
                , column_modyfier(cm), number_of_columns(cm.size() + 3), number_of_rows(nor)
                , luck_value(lv), wild_chance(wc), freespins_chance(fc) {}

    bool isBetween0and100(int num)
    {
        return (num >= 0) & (num <= 100) ? true : false;
    }

    // Function to verify that input params are corrct
    bool verify()
    {
        if (isBetween0and100(luck_value)
          & isBetween0and100(wild_chance)
          & isBetween0and100(freespins_chance)
          & isBetween0and100(wild_chance + freespins_chance)
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
                , parsed_params["wild chance"].get<int>(), parsed_params["freespins chance"].get<int>());

    return params;
}


std::random_device dev;
std::mt19937 rnd(dev());

extern "C" RetStruct spinSlot(int bet, std::string str_params, bool is_streamer, int freespins_left = 0)
{
    std::cout << str_params << std::endl;
    Params params {parseParamsFromJson(str_params)};
    bool good_luck = false;

    if (is_streamer == true)
    {
        float streamer_multiplier = 1.2;
        params.luck_value *= streamer_multiplier;
        if (params.luck_value > 100)
        {
            params.luck_value = 100;
        }
    }
    int luck_modyfier;
    if (params.luck_value < 50)
    {
        luck_modyfier = 50 - params.luck_value;
    }
    else
    {
        luck_modyfier = params.luck_value - 50;
        good_luck = true;
    }

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
    int freespins_count = 0;
    for (int i = 0; i < params.number_of_rows; ++i)
    {
        int match_count = 1;
        int first_elem_idx;
        bool skip = false;
        for (int j = 0; j < params.number_of_columns; ++j)
        {
        start:
            int random_chance = random_chance_dist(rnd);
            int cur_elem_idx;
            if (random_chance < params.wild_chance)
            {
                cur_elem_idx = static_cast<int>(IDX::wild);
            }
            else if (random_chance < params.wild_chance + params.freespins_chance)
            {
                cur_elem_idx = static_cast<int>(IDX::freespins);
            }
            else
            {
                cur_elem_idx = elem_dist(rnd);
            }

            spin_result[i * params.number_of_columns + j] = cur_elem_idx;

            std::cout << char(zero_char + cur_elem_idx) << ' ';


            if (cur_elem_idx == static_cast<int>(IDX::freespins))
            {
                freespins_count += 1;
            }

            if (skip)
                continue;

            if (first_elem_idx == static_cast<int>(IDX::wild))
            {
                first_elem_idx = cur_elem_idx;
            }

            if (j == 0)
            {
                first_elem_idx = cur_elem_idx;
            }
            else if ((first_elem_idx == cur_elem_idx) | (cur_elem_idx == static_cast<int>(IDX::wild)))
            {
                if (!good_luck)
                {
                    if (float(random_chance_dist(rnd)) / 2 <= luck_modyfier)
                        goto start;
                }
                ++match_count;
            }
            else if ((match_count > 2) & (first_elem_idx != static_cast<int>(IDX::freespins)))
            {
                money_earn += countMoney(bet, params.elem_modyfier[first_elem_idx]
                                       , params.getColumnModifier(match_count));
            }
            else
            {
                if (good_luck)
                {
                    if (float(random_chance_dist(rnd)) / 2 <= luck_modyfier)
                        goto start;
                }
                skip = true;
            }
        }
        // hangling full row of same symbols
        if (match_count == params.number_of_columns)
        {
            switch (first_elem_idx)
            {
                case static_cast<int>(IDX::freespins):
                    continue;
                case static_cast<int>(IDX::wild):
                    for (int i2 = 1; i2 < params.number_of_columns; ++i2)
                    {
                        money_earn += countMoney(bet, params.elem_modyfier[first_elem_idx]
                                               , params.getColumnModifier(i2));
                    }
                    break;
                default:
                    money_earn += countMoney(bet, params.elem_modyfier[first_elem_idx]
                                           , params.getColumnModifier(match_count));
            }
        }
        std::cout << '\n';
    }
    std::cout << std::endl;
    if (freespins_left > 0)
    {
        freespins_left--;
        money_earn += bet;
    }
    if (freespins_count > 2)
    {
        int base_number_of_free_spins = 5;

        // handling freespin count more than number of columns (yes, i did this pretty ugly)
        for (int i = (freespins_count > params.number_of_columns ? params.number_of_columns
                                                                 : freespins_count)
               ; freespins_count > 0
               ; i = (freespins_count > params.number_of_columns ? params.number_of_columns
                                                                 : freespins_count)
               , freespins_count -= i)
        {
            freespins_left += params.getColumnModifier(i) * base_number_of_free_spins;
        }
    }
    struct RetStruct ret(money_earn, spin_result, freespins_left);
    return ret;
}
