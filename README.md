**AnagramFinder**

C++ program that can take in a word and look through 400,000+ words for anagrams of it in under \<1μs, made for my CS32 class to explore hash tables and algorithm efficiency and time complexities.

**Data Storage**

My DictionaryImpl class had a hash table of pointers to vectors of vectors of strings. Each "bucket" of the hash table was a pointer to a vector\<vector\<string\>\>, and each vector\<string\> only contained words that were anagrams to each other.

All of the pointers in the hash table were initially nullptr.

My hash function worked such that words that were anagrams of each other produced the same hash number. As you can see here, I used the sort function to achieve this:

unsigned int getHash(string s)

{

sort(s.begin(), s.end());

return std::hash\<string\>()(s);

}

When a new word was inserted into the dictionary, it followed these general rules:

If the corresponding bucket had a nullptr, then that pointer was set to a new dynamically allocated vector\<vector\<string\>\>. A vector\<string\> with our word was then inserted into the 0th spot of the vector\<vector\<string\>\>.

Otherwise, the word was either added to the vector\<string\> that had anagrams of the word, or a new vector\<string\> was added if no such vector\<string\> was already in our vector\<vector\<string\>\>

![image1.jpg](RackMultipart20230615-1-mplxl9_html_aeefb21495c4fe43.gif)
I chose this data structure because it made lookups very fast, as all you had to do was go to the corresponding bucket, and then do a small handful of calls to checkIfAnagram() (which was the slowest part of my lookups previously). This is because you only had to check if the word you were looking up was an anagram of the front() of each vector\<string\>, because all the words of the vector\<string\> are anagrams of each other. Then you could just callback() the entire vector\<string\> and you've found all of your anagrams in one go.

**Pseudocode:**

**void DictionaryImpl::insert(string word)**

_removeNonLetters(word)_

_If the word is not empty_

_bucket = getHash(word) % numBuckets_

_If m\_buckets[bucket] is nullptr_

_Dynamically allocate new vector\<vector\<string\>\>_

_Make a vector\<string\> with our word in it_

_Insert as first element of our vector\<vector\<string\>\>_

_Else_

_Visit each vector\<string\> in our bucket_

_If we find a non-empty vector\<string\> with an anagram of our word_

_Insert our word there and stop looping_

_If none of the vector\<string\>'s had an anagram of our word_

_Make a vector\<string\> with our word in it_

_Insert as last element of our vector\<vector\<string\>\>_

**void DictionaryImpl::lookup(string letters, void callback(string)) const**

_if (callback == nullptr)_

_return;_

_removeNonLetters(letters);_

_if (letters.empty())_

_return;_

_bucket = getHash(word) % numBuckets_

_If m\_buckets[bucket] is not nullptr_

_Visit each vector\<string\> in our bucket_

_If checkIfAnagram(letters, first element of current vector\<string\>) == true_

_callback() everything in this vector\<string\>_

_Stop looking at vector\<string\>'s_

**bool checkIfAnagram(const string& a, string& b)**

_If a and b are different sizes_

_return false_

_//These count how often each possible char (from 0-255) appears in each string_

_count1[256] = 0_

_count2[256] = 0_

_For each char in the strings_

_Increment count1 and count 2's index that corresponds to the int value of that char_

_Loop through the arrays_

_If any differences_

_Return false_

_Return true_

**Challenges:**

One of my biggest challenges was figuring out what data structure and lookup algorithm would be even remotely efficient. My first attempt at this project was a hash table where each bucket was a binary search tree, and that was very slow. One of the main reasons is that this attempt did not ditch the generateNextPermutation() algorithm, and had lookup() try to find every single permutation of the given word within the hash table. This meant words that were long took extremely long to search for.

I first realized that I had to ditch the generateNextPermutation() approach when I tried out the professor's given fastanagrams.exe, and realized that he was able to look up a really long string like "Veronica Snot" in 0.0004 ms. I knew there was no way any function could perform anything trillions of times in such a short amount of time, so I knew the correct solution was not going through all permutations of veronica snot every time it called lookup().

I then messed around with a few different data structures until I settled on doing a hash table of pointers to vector\<vector\<string\>\>. This was nice because I could have all of the empty buckets be nullptr, so it would be very easy to skip over empty buckets when searching.

The reason I did vector\<vector\<string\>\> instead of vector\<string\> alone was because putting all words that were anagrams of each other into a vector together fixed the issue of long look up times due to many calls to checkIfAnagram().
