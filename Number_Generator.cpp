#include <sstream>
#include "Number_Generator.h"

using namespace std;

int Number_Generator::number_counter = 0;
vector<int> Number_Generator::random_numbers;

// Generates the vector of random numbers for us to retrieve from
void Number_Generator::populate_rand_numbers(const string& file_location) {
    ifstream file;
    string file_line;
    string rand_num;

    file.open(file_location);
    if (file.is_open()) {
        while (getline(file, rand_num)) {
            Number_Generator::random_numbers.push_back(stoi(rand_num));
        }
    } else {
        cout << "Unable to open rfile." << endl;
    }
    file.close();
}

// Returns the next random number from the vector
int Number_Generator::get_next_rand_frm_file() {
    int rand_num;
    Number_Generator::number_counter++;
    if (Number_Generator::number_counter > Number_Generator::random_numbers.size()-1)
        Number_Generator::number_counter = 1;

    rand_num = Number_Generator::random_numbers.at(number_counter);
    return rand_num;
}

// Generates the random number to use from the rand number from the file/vector
int Number_Generator::my_random(int burst){
    return 1 + (Number_Generator::get_next_rand_frm_file() % burst);
}