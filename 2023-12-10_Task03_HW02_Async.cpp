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

constexpr int size_v = 1000000;

template<typename It, typename T, typename F, size_t size = 100000>
void para_for_each(It begin, It end, T init, F func)
{
    size_t curr_size = std::distance(begin, end);
    if (curr_size <= size)
    {
        for_each(begin, end, func);
        return;
    }
    auto mid = begin;
    advance(mid, curr_size/2);
    async(para_for_each<It, T, F>, begin, mid, init, func);
    para_for_each(mid, end, init, func);

}

int main()
{
    vector<int> v(size_v);
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<int> rnd(0, 100);
    for (auto i = 0; i < size_v; ++i) v[i] = rnd(rd);
    cout << endl;

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

/*
For_each
Реализуйте параллельный вариант оператора for_each.
Функция должна принимать итераторы на начало и конец контейнера и ссылку, обрабатывающую функцию.
При реализации нужно рекурсивно разделить контейнер на несколько блоков и для каждого запустить отдельную задачу, применяющую обрабатывающую функцию к части контейнера.

*/