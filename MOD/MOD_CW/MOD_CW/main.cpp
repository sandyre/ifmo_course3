#include <iostream>
#include <random>
using namespace std;

int main()
{
    mt19937 generator(0);
    uniform_int_distribution<int> distr(0, 100);
    
    for(auto i = 0; i < 10; ++i)
    {
        cout << distr(generator) << "\n";
    }
}
