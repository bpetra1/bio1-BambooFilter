import sys
import os
import random

if __name__ == "__main__":
    bases = ['A', 'C', 'G', 'T']
    filename = "data/" + sys.argv[1] + "/genome.txt"
    os.makedirs(os.path.dirname(filename), exist_ok=True)
    file = open(filename, "w")
    for i in range(int(sys.argv[1])):
        file.write(random.choice(bases))
    file.close()