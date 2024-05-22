#include <iostream>
#include <fstream>
#include <string>
#include "bamboo_filter.hpp"
using namespace std;

int main() {
    ifstream file("data/genome.txt");
    string genome;
    while (file) { 
        string line;
        file >> line; 
        genome += line;
    }

    int k;
    cout << "Unesi k (10, 20, 50, 100 ili 200): ";
    cin >> k;

    BambooFilter b(16, 4);
    for (int i = 0; i < genome.size() - k + 1; i++) {
        b.insert(genome.substr(i, k));
    }

    int true_positives = 0;
    int true_negatives = 0;
    int false_positives = 0;
    int false_negatives = 0;

    file.open("data/negatives_" + to_string(k) + ".txt");
    while (file) {
        string k_mer;
        file >> k_mer; 
        if (b.lookup(k_mer)) {
            false_negatives++;
        } else {
            true_negatives++;
        }
    }

    file.open("data/positives_" + to_string(k) + ".txt");
    while (file) {
        string k_mer;
        file >> k_mer; 
        if (b.lookup(k_mer)) {
            true_positives++;
        } else {
            false_positives++;
        }
    }

    int total = true_positives + true_negatives + false_positives + false_negatives;
    cout << "Accuracy: " << true_positives + true_negatives << " / " << total << '\n';
    cout << "False positives: " << false_positives << " / " << true_positives + false_positives << '\n'; 
    cout << "False negatives: " << false_negatives << " / " << true_negatives + false_negatives << '\n'; 
}