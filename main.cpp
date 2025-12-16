#include <iostream>
#include <random>

struct Params
{
    // Each slot element/thing (idk, how to call it) has it's money_modyfier
    float* elem_money_modyfier;
    int number_of_elem;
    // There are always at least 3 collumns
    // Modyfier starts from the 4th collumn (modyfier of the 3rd collumn is always 1)
    // and if there's more values, when adds more collumns. Can be nullptr if noc == 3
    float* column_money_modyfier;
    int number_of_collumns;

    int number_of_rows;

    int luck_value;
    int bonus_chance;
    int freespins_chance;

    // good thing https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#rc-zero
    Params (float* emm, const int noe, float* cmm, const int noc, const int nor
          , const int lv, const int bc,const int fc)
                : elem_money_modyfier(emm), number_of_elem(noe)
                , column_money_modyfier(cmm), number_of_collumns(noc), number_of_rows(nor)
                , luck_value(lv), bonus_chance(bc), freespins_chance(fc) {}

    bool isBetween0and100(int num)
    {
        return num >= 0 & num <= 100 ? true : false;
    }

    bool verify()
    {
        // We believe what number_of_elem and number_of_collumns are correct
        // (cause we can't check if it's true)
        if (isBetween0and100(luck_value)
          & isBetween0and100(bonus_chance)
          & isBetween0and100(freespins_chance)
          & number_of_elem > 2
          & number_of_collumns > 2
          & number_of_rows > 0)
            return true;
        return false;
    }
};

std::random_device dev;
std::mt19937 rnd(dev());

int countMoney(int bet, float elem_money_modyfier, float column_money_modyfier)
{
    return float(bet) * elem_money_modyfier * column_money_modyfier;
}

// TODO: figure out with luck, freespeens and bonus game
int spinSlot(int bet, Params params)
{
    std::uniform_int_distribution<int> dist(0, params.number_of_elem - 1);
    int zero_char = 65;

    int** spin_result = new int*[params.number_of_rows];

    int money_earn = -bet;
    for (int i = 0; i < params.number_of_rows; ++i)
    {
        spin_result[i] = new int[params.number_of_collumns];
        int match_count = 1;
        int elem_idx;
        bool skip = false;
        for (int j = 0; j < params.number_of_collumns; ++j)
        {
            int cur_elem_idx = dist(rnd);
            spin_result[i][j] = cur_elem_idx;

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
    return money_earn;
}

int main()
{
    int number_of_elements = 4;
    float* elem_money_modyfier = new float[number_of_elements]{1.5, 3, 5, 10};
    int number_of_collumns = 5;
    float* column_money_modyfier = new float[number_of_collumns]{2, 5};

    int number_of_rows = 3;
    int luck_value = 50;
    int bonus_chance = 2;
    int freespins_chance = 5;
    struct Params params(elem_money_modyfier, number_of_elements
                       , column_money_modyfier, number_of_collumns, number_of_rows
                       , luck_value, bonus_chance, freespins_chance);

    while (true)
    {
        int win = spinSlot(100, params);
        std::cout << "you won: " << win << '\n';
        std::cin.get();
    }

    return 0;
}
