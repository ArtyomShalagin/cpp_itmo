#include <iostream>
#include <map>
#include <vector>
#include <utility>
#include "bimap.h"

using namespace std;

char randchar() {
    const char charset[] =
            "0123456789"
                    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                    "abcdefghijklmnopqrstuvwxyz";
    const size_t max_index = (sizeof(charset) - 1);
    return charset[rand() % max_index];
}

string random_string(size_t length) {
    std::string str(length, 0);
    std::generate_n(str.begin(), length, randchar);
    return str;
}

bool test(size_t n = 10000, size_t len = 100) {
//bool test(size_t n = 10, size_t len = 4) {
    if (n % 2 != 0) n++;
    vector<string>* a = new vector<string>();
    for (int i = 0; i < n; i++) {
        a->push_back(random_string(len));
    }
    vector<string> args = *a;

    bimap* test_bimap = new bimap();
    map<string, string> *left_map = new map<string, string>(), *right_map = new map<string, string>();

    for (int i = 0; i < n / 2; i++) {
        left_map->insert(pair<string, string>(args[i], args[i + n / 2]));
        right_map->insert(pair<string, string>(args[i + n / 2], args[i]));
    }
    for (int i = 0; i < n / 2; i++) {
        test_bimap->insert(args[i], args[i + n / 2]);
    }

    vector<string> *my_res_left = new vector<string>(), *corr_res_left = new vector<string>();
    for (auto i = left_map->begin(); i != left_map->end(); i++) {
        corr_res_left->push_back((*i).first);
    }
    for (auto i = test_bimap->begin_left(); i != test_bimap->end_left(); i++) {
        my_res_left->push_back(*i);
    }

    int corr_left = 0, mist_left = 0, corr_right = 0, mist_right = 0;
    for (int i = 0; i < n / 2; i++) {
        if ((*my_res_left)[i] == (*corr_res_left)[i])
            corr_left++;
        else
            mist_left++;
    }

    vector<string> *my_res_right = new vector<string>(), *corr_res_right = new vector<string>();
    for (auto i = right_map->begin(); i != right_map->end(); i++) {
        corr_res_right->push_back((*i).first);
    }
    for (auto i = test_bimap->begin_right(); i != test_bimap->end_right(); i++) {
        my_res_right->push_back(*i);
    }

    for (int i = 0; i < n / 2; i++) {
        if ((*my_res_right)[i] == (*corr_res_right)[i])
            corr_right++;
        else
            mist_right++;
    }

    int corr_flip = 0, mist_flip = 0;
    for (auto i = test_bimap->begin_left(); i != test_bimap->end_left(); i++) {
        bool ok = i == i.flip().flip();
        if (ok)
            corr_flip++;
        else
            mist_flip++;
    }
    bool override_corr = true;
    int corr_found = 0, mist_found = 0;
    for (int i = 0; i < n / 2; i++) {
        bimap::left_iterator iter_left = test_bimap->find_left(args[i]);
        bimap::right_iterator iter_right = test_bimap->find_right(args[i + n / 2]);
        if (iter_left == test_bimap->end_left() || iter_right == test_bimap->end_right()) {
            mist_found++;
            continue;
        } else
            corr_found++;
        string truth = "mne grustno, ya ustal :(";
        test_bimap->insert(args[i], truth);
        override_corr &= (*(test_bimap->find_left(args[i]).flip()) == *iter_right);
        if (i % 2 == 0)
            test_bimap->erase(iter_left);
        else
            test_bimap->erase(iter_right);
    }
    bool empty = test_bimap->begin_left() == test_bimap->end_left() &&
                 test_bimap->begin_right() == test_bimap->end_right();

    delete a;
    delete my_res_left;
    delete my_res_right;
    delete corr_res_left;
    delete corr_res_right;
    delete left_map;
    delete right_map;
    delete test_bimap;

    cout << "test passed, n = " << n << "; len = " << len << endl;
    cout << "\tleft iteration test: correct = " << corr_left << "; wrong = " << mist_left << endl;
    cout << "\tright iteration test: correct = " << corr_right << "; wrong = " << mist_right << endl;
    cout << "\tflip test: correct = " << corr_flip << "; wrong = " << mist_flip << endl;
    cout << "\tfind test: correct = " << corr_found << "; wrong = " << mist_found << endl;
    cout << "\terase test: " << (empty ? "passed" : "failed") << endl;
    cout << "\toverride test: " << (override_corr ? "passed" : "failed") << endl << endl;

    return corr_left == n / 2 && corr_right == n / 2 && corr_flip == n / 2 &&
           corr_found == n / 2 && empty && override_corr;
}

void run_tests() {
    srand(3788);
    int tests = 10;
    for (int i = 0; i < tests; i++) {
        bool res = test();
        if (!res) {
            break;
        }
    }
}

int main() {
    run_tests();
//    bimap map;
//    map.insert("a", "b");
//    bimap::left_iterator i = map.find_left("a");
//    map.erase(i);
//    cout << (*i) << endl;
//    bimap::left_iterator iter = map.end_left();
//    iter++;

    return 0;
}
