import sys
import random

genome = ""
file_positive = None
file_negative = None
negatives_found = 0

def find_negatives(s, k):
    global negatives_found
    if negatives_found >= 100:
        return
    if k == 0:
        if s not in genome:
            file_negative.write(s + '\n')
            negatives_found += 1
    else:
        bases = ['A', 'C', 'G', 'T']
        random.shuffle(bases)
        for b in bases:
            find_negatives(s + b, k - 1)

def find_positives(k):
    for i in random.sample(range(0, len(genome) - k + 1), 100):
        file_positive.write(genome[i:(i+k)] + '\n')
    
# k je velicina k-mera, predaje se kao argument skripte
def main(k):
    global genome, file_positive, file_negative
    f = open("data/5000000/genome.txt", "r")
    genome = ''.join(f.readlines()).replace('\n', '')
    f.close()
    file_positive = open("data/5000000/positives_" + k + ".txt", "w")
    file_negative= open("data/5000000/negatives_" + k + ".txt", "w")
    find_negatives("", int(k))
    find_positives(int(k))
    file_positive.close()
    file_negative.close()

if __name__ == "__main__":
    main(sys.argv[1])
    