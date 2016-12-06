#include <set>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <string>
#include <vector>
#include <cstdlib>
#include "timer.h"
#include <omp.h>

using namespace std;

const string WORD_FILE_NAME = "words.txt";
const string RESULT_FILE_NAME = "result_bst_parallel.txt";

size_t get_cache_line_size() {
    FILE * p = 0;
    p = fopen("/sys/devices/system/cpu/cpu0/cache/index0/coherency_line_size", "r");
    unsigned int i = 0;
    if (p) {
        fscanf(p, "%d", &i);
        fclose(p);
    }
    return i;
}

int main(int argc, char **argv)
{
    if (argc != 2) {
        cerr << "[Usage] " << argv[0] << " <the number of threads>" << endl;
        return 1;
    }

    int cache_line_size = get_cache_line_size();
    int palin_cnt_jump = cache_line_size / sizeof(int);

    cout << "Cache Line Size : " << cache_line_size << endl;

    int thread_cnt = atoi(argv[1]);
    cout << "The number of threads : " << thread_cnt << endl;

    ifstream ifs;
    ifs.open(WORD_FILE_NAME.c_str(), ios::in);

    ofstream ofs;
    ofs.open(RESULT_FILE_NAME.c_str(), ios::trunc);

    set<string> word_set;
    vector<string> words;

    int palin_cnt = 0;

    double start, finish;

    start = omp_get_wtime();

    string word;
    ifs >> word;
    if (!ifs.eof()) {
        do {
            transform(word.begin(), word.end(), word.begin(), ::tolower);
            word_set.insert(word);
            words.push_back(word);
            ifs >> word;
        } while (!ifs.eof());
    }

    // for avoiding false sharing.
    int *local_palin_cnt_arr = new int[thread_cnt * palin_cnt_jump];

    for (int i=0; i<thread_cnt; ++i)
        local_palin_cnt_arr[i * palin_cnt_jump] = 0;

    #pragma omp parallel num_threads(thread_cnt)
    {
        int tid = omp_get_thread_num();
        int tnum = omp_get_num_threads();
        int N = word_set.size();

        int& local_palin_cnt = local_palin_cnt_arr[tid * palin_cnt_jump];

        int start = tid * N / tnum;
        int end = (tid+1) * N / tnum;

        //cout << "# of works : " << end-start << endl;

        for(int i = start; i < end; ++i) {
            string reverse_word(words[i]);
            reverse(reverse_word.begin(), reverse_word.end());

            set<string>::iterator target = word_set.find(reverse_word);
            if (target != word_set.end() && *target >= words[i]) {
                #pragma omp critical(output)
                ofs << words[i] << " " << reverse_word << endl;
                ++local_palin_cnt;
            }
        }
    }

    // reduction
    for (int i=0; i<thread_cnt; ++i)
        palin_cnt += local_palin_cnt_arr[i * palin_cnt_jump];

    finish = omp_get_wtime();

    cout << "# of words (except duplication) : " << word_set.size() << endl;
    cout << "# of palindrome pairs : " << palin_cnt << endl;
    cout << "Elapsed Time : " << finish - start << "(s)." << endl;

    return 0;
}