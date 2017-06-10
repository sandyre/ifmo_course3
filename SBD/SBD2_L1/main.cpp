#include <fstream>
#include <iostream>
#include <random>
#include <string>
using namespace std;

int main()
{
	ofstream out("Isbns.txt");
	std::string phone;

	mt19937 gen(0);
	uniform_int_distribution<int> distr(0, 9);
	for(int i = 0; i < 10000; ++i)
	{
		for(int j = 0; j < 10; ++j)
		{
			phone += to_string(distr(gen));		
		}

		out << phone << "\n";
		phone.clear();
	}

	out.close();
}
