#ifndef NUMBER_GENERATOR_H
#define NUMBER_GENERATOR_H
#include <sstream>
#include <fstream>
#include <iostream>
#include <vector>

using namespace std;

class Number_Generator {
protected:
    static int number_counter;
public:
    static vector<int> random_numbers;
    static void populate_rand_numbers(const string& file_location);
    static int get_next_rand_frm_file();
    static int my_random(int burst);
};

#endif //NUMBER_GENERATOR_H
