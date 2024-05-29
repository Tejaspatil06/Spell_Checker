#include <iostream>
#include <chrono>
#include <fstream>
#include <unordered_map>
#include <vector>
#include <string>
#include <map>
#include <algorithm>

using namespace std;
using namespace chrono;

class SpellCorrector {
private:
    unordered_map<string, long long int> wordList;
    unordered_map<string, string> corrections;

    int editDistance(const string& s1, const string& s2) {
        int n = s1.length(), m = s2.length();
        vector<vector<int>> dp(n + 1, vector<int>(m + 1, 0));
        string s1_temp = "." + s1;
        string s2_temp = "." + s2;

        for (int i = n; i >= 0; i--) {
            for (int j = m; j >= 0; j--) {
                if (i == n || j == m) {
                    dp[i][j] = max(n - i, m - j);
                } else {
                    if (s1_temp[i] != s2_temp[j]) {
                        dp[i][j] = 1 + min(dp[i][j + 1], min(dp[i + 1][j + 1], dp[i + 1][j]));
                    } else {
                        dp[i][j] = dp[i + 1][j + 1];
                    }
                }
            }
        }
        return dp[0][0];
    }

    string spellCorrect(const string& word) {
        // If word exists in dictionary return the word
        if (wordList.count(word)) {
            return word;
        }
        // if word is already corrected
        if (corrections.count(word)) {
            return corrections[word];
        }

        // Calculate edit distances for current word and words in dictionary
        map<int, vector<string>> distances;
        for (const auto& word_freq : wordList) {
            int ed = editDistance(word_freq.first, word);
            distances[ed].push_back(word_freq.first);
        }

        // Take the words with lowest edit distance, these are assumed to be the nearest correct words.
        auto lowestDistance = distances.begin();
        pair<string, long long int> mostProbableWord = make_pair("", -1);

        // Among the nearest correct words find the most probable word. The most probable word is the most frequently used word in English
        for (const auto& s : lowestDistance->second) {
            long long int s_freq = wordList[s];
            if (s_freq > mostProbableWord.second) {
                mostProbableWord = make_pair(s, s_freq);
            }
        }

        corrections[word] = mostProbableWord.first;
        return mostProbableWord.first;
    }

public:
    void loadDictionary(const string& dictFile, const string& freqFile) {
        // Reading all words and hashing them
        ifstream dict(dictFile), freq(freqFile);
        if (dict.is_open() && freq.is_open()) {
            string word, freq_str;
            while (getline(dict, word) && getline(freq, freq_str)) {
                long long int currentFreq = stoll(freq_str);
                wordList[word] = currentFreq;
            }
            dict.close();
            freq.close();
        } else {
            cout << "Cannot find required files" << endl;
        }
    }

    string correctParagraph(const string& inputString) {
        // Collecting individual words and correct
        string word = "";
        string correctedParagraph = "";
        for (char c : inputString) {
            if (c == ' ' || c == ',' || c == '.') {
                if (!word.empty()) {
                    string correctWord = spellCorrect(word);
                    correctedParagraph += correctWord + " ";
                    word.clear();
                }
            } else {
                if (isupper(c)) {
                    word += tolower(c);
                } else {
                    word += c;
                }
            }
        }
        if (!word.empty()) {
            string correctWord = spellCorrect(word);
            correctedParagraph += correctWord + " ";
        }
        return correctedParagraph;
    }
};

int main() {
    // Reading input sentence
    cout << "Please enter a paragraph:" << endl;
    cout << "---------------------------" << endl;

    string inputString;
    getline(cin, inputString);

    SpellCorrector corrector;
    corrector.loadDictionary("dict.txt", "freq.txt");

    auto start1 = high_resolution_clock::now();  // starting the timer
    string correctedParagraph = corrector.correctParagraph(inputString);
    auto stop1 = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop1 - start1);

    cout << endl;
    cout << "Corrected paragraph is: " << endl;
    cout << "---------------------------" << endl;
    cout << correctedParagraph << endl;
    cout << endl;
    cout << "--------------------------------" << endl;
    cout << "Execution Time: " << duration.count() / 1000.0 << " milliseconds" << endl;
    return 0;
}
