#include <iostream>
#include <vector>
#include <chrono>
#include <thread>
#include <queue>
#include <mutex>
#include <stdlib.h>

#include <boost/multiprecision/cpp_int.hpp>

using namespace boost::multiprecision;

using namespace std;
using namespace std::chrono_literals;

mutex mxout, mxrec;

const unsigned long npthreads = thread::hardware_concurrency(); // num of threads available

int intlen(uint1024_t n, vector<int> &digits){
    unsigned int number_of_digits = 0;

    do {
        ++number_of_digits;
        digits.push_back(static_cast<int>(n%10));
        n /= 10;
    } while (n);
    return number_of_digits;
}

uint64_t analyze(uint1024_t num, uint64_t &depth){
    vector<int> digits;
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

struct num{
    uint1024_t n;
    uint64_t d;
};

void worker(const ulong offset, num &record, chrono::time_point<std::chrono::steady_clock> start){
    uint1024_t n = offset;
    uint64_t depth;
    uint64_t i10 = static_cast<uint64_t>(pow(10,10));
    /* uint128_t i20 = static_cast<uint64_t>(pow(10,20));
    uint256_t i30 = static_cast<uint64_t>(pow(10,30)); */
    chrono::time_point<std::chrono::steady_clock> end;
    chrono::duration<double> diff;

    while(1){
        depth = 0;
        analyze(n, depth);

        mxrec.lock();
        if(depth > record.d){
            record.d = depth;
            record.n = n;
            mxrec.unlock();

            end = std::chrono::steady_clock::now();
            diff = end-start;
            cout << "\rNumber: " << n << ", depth: " << depth << ", in: " << diff.count() <<"s\n";
        }else if(depth == record.d){// && n < record.n){
            record.n = n;
            mxrec.unlock();

            end = std::chrono::steady_clock::now();
            diff = end-start;
            cout << "\rNumber: " << n << ", depth: " << depth << ", in: " << diff.count() <<"s\n";
        }else{
            mxrec.unlock();
        }


        n = static_cast<uint1024_t>((rand()+i10)) +
                static_cast<uint1024_t>((rand()+i10)*i10);

        // 26888999
        // 3778888999

        /*if(n%16384 == 0){
            cout << "\rProcessing: " << n;
        }*/
    }
}

int main()
{
    num record;
    record.d = 0;
    record.n = 1;
    thread tpool[npthreads];

    auto start = std::chrono::steady_clock::now();

    for(ulong i=0; i<npthreads; i++){
        tpool[i] = thread(worker, i, ref(record), start);
    }
    tpool[npthreads-1].join();

    return 0;
}
