#include <cstdio>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <string>
#include <vector>
#include "timer.h"

using namespace std;

const string WORD_FILE_NAME = "words.txt";
const string RESULT_FILE_NAME = "result_vec_serial.txt";

int main(void)
{
    ifstream ifs;
    ifs.open(WORD_FILE_NAME.c_str(), ios::in);

    ofstream ofs;
    ofs.open(RESULT_FILE_NAME.c_str(), ios::trunc);

    vector<string> words;

    int palin_cnt = 0;

    double start, finish;

    GET_TIME(start);

    string word;
    ifs >> word;
    if (!ifs.eof()) {
        do {
            transform(word.begin(), word.end(), word.begin(), ::tolower);
            words.push_back(word);
            ifs >> word;
        } while (!ifs.eof());
    }

    for (int i=0; i<words.size(); ++i) {
        string reverse_word(words[i]);
        reverse(reverse_word.begin(), reverse_word.end());

        vector<string>::iterator iter = find(words.begin(), words.begin() + i + 1, reverse_word);
        if (iter != words.begin() + i + 1) {
            ofs << reverse_word << " " << words[i] << endl;
            ++palin_cnt;
        }
    }

    GET_TIME(finish);

    cout << "# of words (include duplication) : " << words.size() << endl;
    cout << "# of palindrome pairs : " << palin_cnt << endl;
    cout << "Elapsed Time : " << finish - start << "(s)." << endl;

    return 0;
}