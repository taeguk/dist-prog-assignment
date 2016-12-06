#include <cstdio>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <string>
#include <vector>
#include <omp.h>
#include "timer.h"

using namespace std;

const string WORD_FILE_NAME = "words.txt";
const string RESULT_FILE_NAME = "result_vec_parallel.txt";

int main(int argc, char **argv)
{
    if (argc != 2) {
        cerr << "[Usage] " << argv[0] << " <the number of threads>" << endl;
        return 1;
    }

    int thread_cnt = atoi(argv[1]);
    cout << "The number of threads : " << thread_cnt << endl;

    ifstream ifs;
    ifs.open(WORD_FILE_NAME.c_str(), ios::in);

    ofstream ofs;
    ofs.open(RESULT_FILE_NAME.c_str(), ios::trunc);

    vector<string> words;

    int palin_cnt = 0;

    double start, finish;

    start = omp_get_wtime();

    string word;
    ifs >> word;
    if (!ifs.eof()) {
        do {
            transform(word.begin(), word.end(), word.begin(), ::tolower);
            words.push_back(word);
            ifs >> word;
        } while (!ifs.eof());
    }

    #pragma omp parallel for reduction(+:palin_cnt) schedule(dynamic) num_threads(thread_cnt)
    for (int i=0; i<words.size(); ++i) {
        string reverse_word(words[i]);
        reverse(reverse_word.begin(), reverse_word.end());

        vector<string>::iterator iter = find(words.begin(), words.begin() + i + 1, reverse_word);
        if (iter != words.begin() + i + 1) {
            #pragma omp critical(output)
            ofs << reverse_word << " " << words[i] << endl;
            ++palin_cnt;
        }
    }

    finish = omp_get_wtime();

    cout << "# of words (include duplication) : " << words.size() << endl;
    cout << "# of palindrome pairs : " << palin_cnt << endl;
    cout << "Elapsed Time : " << finish - start << "(s)." << endl;

    return 0;
}