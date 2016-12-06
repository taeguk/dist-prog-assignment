#include <set>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <string>
#include "timer.h"

using namespace std;

const string WORD_FILE_NAME = "words.txt";
const string RESULT_FILE_NAME = "result_bst_serial.txt";

int main(void)
{
    ifstream ifs;
    ifs.open(WORD_FILE_NAME.c_str(), ios::in);

    ofstream ofs;
    ofs.open(RESULT_FILE_NAME.c_str(), ios::trunc);

    set<string> word_set;

    int palin_cnt = 0;

    double start, finish;

    GET_TIME(start);

    string word;
    ifs >> word;
    if (!ifs.eof()) {
        do {
            transform(word.begin(), word.end(), word.begin(), ::tolower);
            word_set.insert(word);

            string reverse_word(word);
            reverse(reverse_word.begin(), reverse_word.end());

            set<string>::iterator iter = word_set.find(reverse_word);
            if (iter != word_set.end()) {
                ofs << reverse_word << " " << word << endl;
                ++palin_cnt;
            }

            ifs >> word;
        } while (!ifs.eof());
    }

    GET_TIME(finish);

    cout << "# of words (except duplication) : " << word_set.size() << endl;
    cout << "# of palindrome pairs : " << palin_cnt << endl;
    cout << "Elapsed Time : " << finish - start << "(s)." << endl;

    return 0;
}