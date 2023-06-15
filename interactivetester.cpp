#include "Dictionary.h"
#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <vector>
#include <numeric>
#include <algorithm>

#if defined(_MSC_VER)  &&  !defined(_DEBUG)
#include <iostream>
#include <windows.h>
#include <conio.h>

struct KeepWindowOpenUntilDismissed
{
	~KeepWindowOpenUntilDismissed()
	{
		DWORD pids[1];
		if (GetConsoleProcessList(pids, 1) == 1)
		{
			std::cout << "Press any key to close this window . . . ";
			_getch();
		}
	}
} keepWindowOpenUntilDismissed;
#endif

using namespace std;

template<typename T>
double getAverage(std::vector<T> const& v) {
	if (v.empty()) {
		return 0;
	}
	return std::accumulate(v.begin(), v.end(), 0.0) / v.size();
}

class Timer
{
public:
	Timer()
	{
		start();
	}
	void start()
	{
		m_time = std::chrono::high_resolution_clock::now();
	}
	double elapsed() const
	{
		std::chrono::duration<double, std::milli> diff =
			std::chrono::high_resolution_clock::now() - m_time;
		return diff.count();
	}
private:
	std::chrono::high_resolution_clock::time_point m_time;
};

void outputCallback(string anagram);
void emptyCallback(string anagram);

int main()
{
	Dictionary dict(500000);

	// Build dictionary
	{
		//cout << "Enter name of dictionary file: ";
		string filename = "words.txt";
		//getline(cin, filename);
		ifstream wordFile(filename);
		if (!wordFile)
		{
			cout << "Cannot open " << filename << endl;
			return 1;
		}

		string word;
		while (getline(wordFile, word))
			dict.insert(word);
	}
	

	// Find and print anagrams
	for (;;)
	{
		cout << "Enter a set of letters: ";
		string letters;
		getline(cin, letters);
		if (!cin || letters.empty())
			break;

		if (letters == "xxx")
			return 0;

		int trials = 10;

		cout << "Anagrams of " << letters << ":\n";

		dict.lookup(letters, outputCallback);

		cout << endl;

		Timer s;
		vector<double> results;

		for (int i = 0; i < trials; i++)
		{
			Timer t;
			dict.lookup(letters, emptyCallback);
			double result = t.elapsed();
			results.push_back(result);
		}

		double total = s.elapsed();

		double avg = getAverage(results);

		cout << "The lookup of anagrams takes " << avg << " milliseconds, on average. In total we took " << total / 1000 << " seconds." << endl;
	}
}

void outputCallback(string anagram)
{
	cout << "  " << anagram;
}

void emptyCallback(string)
{
	// For timing purposes, we use a callback function that does nothing,
	// so that the cost of doing output or whatever else a callback function
	// might do doesn't enter into the timing of the lookup functionality.
}
