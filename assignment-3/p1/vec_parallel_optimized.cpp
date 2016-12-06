#include <cstdio>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <string>
#include <vector>
#include <utility>
#include <omp.h>
#include "timer.h"

using namespace std;

const string WORD_FILE_NAME = "words.txt";
const string RESULT_FILE_NAME = "result_vec_parallel_optimized.txt";

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

    vector<pair<int, int> > * results = new vector<pair<int, int> >[thread_cnt];

    #pragma omp parallel for reduction(+:palin_cnt) schedule(dynamic) num_threads(thread_cnt)
    for (int i=0; i<words.size(); ++i) {
        int tid = omp_get_thread_num();
        string reverse_word(words[i]);
        reverse(reverse_word.begin(), reverse_word.end());

        vector<string>::iterator iter = find(words.begin(), words.begin() + i + 1, reverse_word);
        if (iter != words.begin() + i + 1) {
            results[tid].push_back(make_pair(iter - words.begin(), i));
            ++palin_cnt;
        }
    }

    for(int tid=0; tid < thread_cnt; ++tid) {
        for (int i = 0; i < results[tid].size(); ++i) {
            ofs << words[results[tid][i].first] << " " << words[results[tid][i].second] << endl;
        }
    }

    finish = omp_get_wtime();

    cout << "# of words (include duplication) : " << words.size() << endl;
    cout << "# of palindrome pairs : " << palin_cnt << endl;
    cout << "Elapsed Time : " << finish - start << "(s)." << endl;

    return 0;
}