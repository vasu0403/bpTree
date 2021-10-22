import sys
import numpy as np
import matplotlib.pyplot as plt

plt.style.use('seaborn-deep')

filename = 'range_output3'

btree = {}
withoutBtree = {}
with open(filename) as file:
    for line in file:
        words = line.split()
        if(len(words) != 2):
            continue
        if words[0] not in btree.keys():
          btree[words[0]] = 0
        if words[1] not in withoutBtree.keys():
          withoutBtree[words[1]] = 0
        btree[words[0]] += 1
        withoutBtree[words[1]] += 1
btree = dict(sorted(btree.items()))
withoutBtree = dict(sorted(withoutBtree.items()))
plt.bar(list(btree.keys()), btree.values(), color='g', label = 'B+ tree')
plt.bar(list(withoutBtree.keys()), withoutBtree.values(), color = 'r', label = 'Without B+ tree')
plt.legend(loc='upper left')
plt.xlabel("Number of block accesses")
plt.ylabel("Frequency")
plt.show()