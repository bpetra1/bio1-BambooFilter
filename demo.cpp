#include <iostream>
#include <fstream>
#include <string>
#include "bamboo_filter.hpp"
using namespace std;

int main() {
    ifstream file("data/genome/genome.txt");
    string genome;
    while (file) { 
        string line;
        file >> line; 
        genome += line;
    }

    int k;
    cout << "Unesi k (10, 20, 50, 100 ili 200): ";
    cin >> k;

    BambooFilter b;
    for (int i = 0; i < genome.size() - k + 1; i++) {
        b.insert(genome.substr(i, k));
    }

    int true_positives = 0;
    int true_negatives = 0;
    int false_positives = 0;
    int false_negatives = 0;

    ifstream neg_file("data/genome/negatives_" + to_string(k) + ".txt");
    while (neg_file) {
        string k_mer;
        neg_file >> k_mer; 
        if (k_mer.empty()) {
            continue;
        }
        if (b.lookup(k_mer)) {
            false_positives++;
        } else {
            true_negatives++;
        }
    }

    ifstream pos_file("data/genome/positives_" + to_string(k) + ".txt");
    while (pos_file) {
        string k_mer;
        pos_file >> k_mer; 
        if (k_mer.empty()) {
            continue;
        }
        if (b.lookup(k_mer)) {
            true_positives++;
        } else {
            false_negatives++;
        }
    }

    int total = true_positives + true_negatives + false_positives + false_negatives;
    cout << "Accuracy: " << true_positives + true_negatives << " / " << total << '\n';
    cout << "False positives: " << false_positives << " / " << true_positives + false_positives << '\n'; 
    cout << "False negatives: " << false_negatives << " / " << true_negatives + false_negatives << '\n'; 
}