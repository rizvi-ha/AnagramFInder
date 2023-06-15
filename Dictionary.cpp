// Dictionary.cpp

#include "Dictionary.h"
#include <string>
#include <list>
#include <algorithm>
#include <iostream>
#include <cctype>
#include <vector>
#include <utility>  // for swap
using namespace std;

void removeNonLetters(string& s);
bool checkIfAnagram(const string& a, string& b); //Checks if two strings are anagrams of eachother
unsigned int getHash(string s); //creates a hash number from a string that is independent of the order of the letters in the string

class DictionaryImpl
{ //Basically I implemented dictionary as a Hash Table (array) of pointers to vectors of vectors of strings
public: 
    DictionaryImpl(int maxBuckets) 
    {
        numBuckets = maxBuckets;
        m_buckets = new vector<vector<string>>*[maxBuckets]; 
        //Making all the empty buckets be a null pointer so that it's easy to skip empty buckets later on
        for (int i = 0; i < numBuckets; i++)
        {
            m_buckets[i] = nullptr;
        }
    }
    ~DictionaryImpl() 
    {
        for (int i = 0; i < numBuckets; i++) // Deleting all the non-empty buckets
        {
            if (m_buckets[i] != nullptr)
                delete m_buckets[i];
        }
        delete[] m_buckets; //Deleting the array of buckets
    }
    void insert(string word);
    void lookup(string letters, void callback(string)) const;

private:
    int numBuckets;
    vector<vector<string>>** m_buckets; //points to the first element of our array of pointers to vectors of vectors of strings
};

void DictionaryImpl::insert(string word) 
{
    // The basic idea is to make it so each vector<string> contains only words that are anagrams of eachother, 
    // and to add a new vector<string> to our vector<vector<string>> if the 
    // current vector<string>'s available are all non-anagrams of what we need to add

    removeNonLetters(word);
    if (!word.empty())
    {
        int bucket = getHash(word) % numBuckets; 
        // since getHash() returns the same value for anagrams, we know our vector<string> of anagrams that we want to 
        // add this word to is going to be in this bucket if it exists already

        if (m_buckets[bucket] == nullptr) //If the vector<vector<string>> for this bucket doesn't exist already
        {
            m_buckets[bucket] = new vector<vector<string>>; //dynamically allocate a new vector<vector<string>>
            vector<string> s;
            s.push_back(word); //make new vector<string> with our word in it and insert it as the first element of our vector<vector<string>>
            m_buckets[bucket]->insert(m_buckets[bucket]->begin(), s);
        }
        else //if there already is a vector<vector<string>> in this bucket
        {
            int i; 
            bool flag = false;
            for (i = 0; i < m_buckets[bucket]->size(); i++) // Visit each vector<string> in our bucket
            {
                //If the current vector<string> is not empty and it's top item is an anagram of what we want to insert
                if (!m_buckets[bucket]->at(i).empty() && checkIfAnagram(m_buckets[bucket]->at(i).back(), word)) 
                { 
                    m_buckets[bucket]->at(i).push_back(word);
                    flag = true;
                    break; //Then we put our word in this vector<string> and stop loop
                }
            }
            if (!flag) //If none of the vector<strings> were what we were looking for
            { // Make a new vector<string> with our word
                vector<string> s;
                s.push_back(word);
                m_buckets[bucket]->insert((m_buckets[bucket]->begin() + i), s);
            }


        }
    }
        
}



void DictionaryImpl::lookup(string letters, void callback(string)) const
{
    if (callback == nullptr)
        return;

    removeNonLetters(letters);
    if (letters.empty())
        return;

    //Find the bucket # that all anagrams of this word would be in (since getHash does not care about character order)
    int bucket = getHash(letters) % numBuckets; 

    //If that bucket is not empty
    if (m_buckets[bucket] != nullptr)
    {
        bool flag = false;
        //Visit each vector<string>
        for (int i = 0; (i < m_buckets[bucket]->size()) && flag == false; i++)
        {
            // And see if it's front element is an anagram of the word we want to insert 
            // (we only need to check 1 element sicne all the words in the vector are anagrams of eachother)
            if (checkIfAnagram(letters, m_buckets[bucket]->operator[](i).front()))
            {
                //If it is we can callback() all of the strings in that vector and stop searching
                flag = true;
                for (int z = 0; z < m_buckets[bucket]->operator[](i).size(); z++)
                {
                    callback(m_buckets[bucket]->operator[](i).operator[](z));
                }
            }

            if (flag)
                return;
        }
    }
}

void removeNonLetters(string& s)
{
    string::iterator to = s.begin();
    for (string::const_iterator from = s.begin(); from != s.end(); from++)
    {
        if (isalpha(*from))
        {
            *to = tolower(*from);
            to++;
        }
    }
    s.erase(to, s.end());  // chop everything off from "to" to end.
}

bool checkIfAnagram(const string& a, string& b)
{

    if (a.size() != b.size()) //Low cost-check first
        return false;

    int count1[256] = { 0 }; //These arrays hold a count of how often each character possibility appears in each string
    int count2[256] = { 0 }; // For example, count1[57] holds a count of how often character 57 appears in string 1

    int i;
    for (i = 0; a[i]; i++) 
    { //Once we get past the length of the string, a[i] will return the null char which is 
      // the only char that returns false when converted to boolean, so our loop will stop.

        count1[a[i]]++;
        count2[b[i]]++;
    }

    for (i = 0; i < 256; i++)
    {
        if (count1[i] != count2[i])
        {
            return false; //If any of the counts are different, the two words are not anagrams of eachother

        }
    }
        
            
    return true; //If no discrepancies found, they are anagrams
}

unsigned int getHash(string s)
{
    sort(s.begin(), s.end()); //By getting the hash of the sorted string, all anagrams of s will return the same hash number.
    return std::hash<string>()(s);
}

//******************** Dictionary functions ******************************

// These functions simply delegate to DictionaryImpl's functions
// You probably don't want to change any of this code

Dictionary::Dictionary(int maxBuckets)
{
    m_impl = new DictionaryImpl(maxBuckets);
}

Dictionary::~Dictionary()
{
    delete m_impl;
}

void Dictionary::insert(string word)
{
    m_impl->insert(word);
}

void Dictionary::lookup(string letters, void callback(string)) const
{
    m_impl->lookup(letters, callback);
}
