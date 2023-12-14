#include <iostream>
#include <Windows.h>
#include <thread>
#include <random>
#include <future>
#include <algorithm>
#include <vector>
#include <chrono>

using namespace std;
using namespace std::chrono_literals;
using namespace std::chrono;

constexpr int size_v = 1000;

//#define ACCUM

#ifndef ACCUM

template<typename It, typename T, typename F, size_t size = 25>
F para_for_each(It begin, It end, T init, F func)
{
    size_t curr_size = std::distance(begin, end);
    if (curr_size <= size) return for_each(begin, end, func);
    auto mid = begin;
    advance(mid, curr_size/2);
    auto ft_res = async(para_for_each<It, T, F>, begin, mid, init, func);
    auto l_res = para_for_each(mid, end, init, func);
    return ft_res.get() + l_res;
}

/*
template<typename It, typename F>
void para_for_each(It begin, It end, F func)
{
    unique_lock<mutex> lg(m);
    const auto length = std::distance(begin, end);
    if (0 == length) return;
    int numSegments = thread::hardware_concurrency() - 1;
    if (numSegments == 1)
    {
        return for_each(func.begin(), func.end(), [&](int& i) {sum += i; });
    }
    int segmentSize = length / numSegments;
    auto next = begin;
    advance(next, segmentSize);

    vector<future<void>> futures(numSegments);
    int segment = 0;
    lg.unlock();
    for (auto& future : futures)
    {
        lg.lock();
        future = async([=, &func]() {
            auto segmentStart = begin + segment * segmentSize;
            auto segmentEnd = segmentStart + segmentSize;

            for (auto i = segmentStart; i != segmentEnd; ++i) func(*i);
            });
        ++segment;
        lg.unlock();
    }

    for (auto& future : futures) future.wait();
}
*/

/*
template<typename It, typename F>
void para_for_each(It range, F func, int numSegments)
{
    para_for_each(std::begin(range), std::end(range), func, numSegments);
}
*/


int main()
{
    vector<int> v(size_v);
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<int> rnd(0, 100);
    for (auto i = 0; i < size_v; ++i)
    {
        v[i] = rnd(rd);
//        cout << v[i] << ' ';
    }
    cout << endl;

    //int cores = thread::hardware_concurrency();

    int sum = 0;
    auto start = chrono::high_resolution_clock::now();
    for_each(v.begin(), v.end(), [&](int& i) {sum += i; });
    auto end = chrono::high_resolution_clock::now();
    double time = duration_cast<nanoseconds>(end - start).count() / 1'000'000'000.0;
    cout << sum << endl << "time, s: " << time << endl;
 
    sum = 0;
    start = chrono::high_resolution_clock::now();
    para_for_each(v.begin(), v.end(), 0, [&](int& i) {sum += i; });
    end = chrono::high_resolution_clock::now();
    time = duration_cast<nanoseconds>(end - start).count() / 1'000'000'000.0;
    cout << sum << endl << "time, s: " << time << endl;

    system("pause");
}

#else

template<typename It, typename T, size_t size = 25>
T para_accum(It begin, It end, T init)
{
    size_t curr_size = distance(begin, end);
    if (curr_size <= size)
    {
        return accumulate(begin, end, init);
    }
    auto mid = begin;
    advance(mid, curr_size / 2);
    auto ft_res = async(para_accum<It, T>, begin, mid, init);
    auto l_res = para_accum(mid, end, init);
    return ft_res.get() + l_res;
}

int main()
{
    vector<int> v(100, 2);
    int cores = thread::hardware_concurrency();
    cout << "rez: " << para_accum(v.begin(), v.end(), 0) << endl;
    system("pause");
}
#endif

/*
For_each
Реализуйте параллельный вариант оператора for_each.
Функция должна принимать итераторы на начало и конец контейнера и ссылку, обрабатывающую функцию.
При реализации нужно рекурсивно разделить контейнер на несколько блоков и для каждого запустить отдельную задачу, применяющую обрабатывающую функцию к части контейнера.

*/