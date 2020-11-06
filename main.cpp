#include <iostream>
#include <vector>
#include <chrono>

using namespace std;

int intlen(uint64_t n, std::vector<int> &digits){
    unsigned int number_of_digits = 0;

    do {
        ++number_of_digits;
        digits.push_back(n%10);
        n /= 10;
    } while (n);
    return number_of_digits;
}

int analyze(uint64_t num, uint64_t &depth){
    std::vector<int> digits;
    int len = intlen(num, digits);
    if(len == 1){
        return depth;
    }else {
        uint64_t combination = 1;
        depth += 1;
        for(auto d:digits){
            combination *= d;
        }
        return analyze(combination, depth);
    }
}


int main()
{
    uint64_t record(0);
    uint64_t depth(0);
    uint64_t n(0);
    uint64_t semi;

    while(1){
        n += 1;
        depth = 0;
        semi = analyze(n, depth);
        if(n%1000 == 0){
            cout << "\rProcessing: " << n;
        }
        if(semi > record){
            record = semi;
            cout << "\rNumber: " << n << ", depth: " << depth << "\n";
            if(record == 20){
                return 0;
            }
        }
    }

    return 0;
}
