#include <iostream>
#include <vector>
#include <chrono>
#include <thread>
#include <queue>
#include <mutex>

using namespace std;
using namespace std::chrono_literals;

mutex mxin, mxout;

const unsigned long npthreads = thread::hardware_concurrency(); // num of threads available

int intlen(uint64_t n, std::vector<int> &digits){
    unsigned int number_of_digits = 0;

    do {
        ++number_of_digits;
        digits.push_back(n%10);
        n /= 10;
    } while (n);
    return number_of_digits;
}

uint64_t analyze(uint64_t num, uint64_t &depth){
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

class num{
public:
    uint64_t n;
    uint64_t d;
};

void worker(queue<uint64_t> &qin, queue<num> &qout, bool &work){
    using namespace std::chrono_literals;

    num next;
    uint64_t n;
    uint64_t depth;

    while(work){
        while(qin.size() < npthreads+1){
            std::this_thread::sleep_for(100ns);
            // wait until queue is not empty
        }
        mxin.lock();
        n = qin.front();
        qin.pop();
        mxin.unlock();

        depth = 0;
        analyze(n, depth);
        next.d = depth;
        next.n = n;

        mxout.lock();
        qout.push(next);
        mxout.unlock();
    }
}

int main4(){
    queue<uint64_t> in;
    queue<num> out;
    thread tpool[npthreads];
    bool work = true;
    for (unsigned long i=0; i<npthreads; i++){
        tpool[i] = thread(worker, ref(in), ref(out), ref(work));
    }

    uint64_t n = 1;
    uint64_t record = 0;
    num cand;

    while(record<8){
        if(in.size() < npthreads*2){
            for(unsigned long i=0; i < npthreads*2; i++){
                mxin.lock();
                in.push(n++);
                mxin.unlock();
            }
        }
        cout << "\rProcessing: " << n;

        while(!out.empty()){
            mxout.lock();
            cand = out.front();
            out.pop();
            mxout.unlock();
            if(record<cand.d){
                record = cand.d;
                cout << "\rNumber: " << cand.n << ", depth: " << cand.d << "\n";
            }
        }
    }

       return 0;

    work = false;
    for (unsigned long i=0; i<npthreads; i++){
        tpool[i].join();
    }

}

int main3(){
    vector<thread> tpool;
    // unsigned char record(0);
    uint64_t candidates[npthreads];
    uint64_t depths[npthreads];
    uint64_t n(1);
    uint64_t record = 0;

    while(1){
        for(unsigned long i=0; i<npthreads; i++){
            candidates[i] = n++;
            depths[i]= 0;
            tpool.push_back(thread(analyze, candidates[i], ref(depths[i])));
        }
        cout << "\rProcessing: " << n;
        for(auto &t:tpool){
            t.join();
        }
        tpool.clear();

        for(unsigned long i=0; i<npthreads; i++){
            if(depths[i] > record){
                record = depths[i];
                cout << "\rNumber: " << candidates[i] << ", depth: " << depths[i] << "\n";
                if(depths[i] == 8){
                    return 0;
                }
            }
        }
    }

    return 0;
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
        if(n%4096 == 0){
            cout << "\rProcessing: " << n;
        }
        if(semi > record){
            record = semi;
            cout << "\rNumber: " << n << ", depth: " << depth << "\n";
            if(record == 12){
                return 0;
            }
        }
    }

    return 0;
}
