import random

with open('input.txt','w') as f:
    for i in range(4000):
        f.write(str(random.uniform(-1, 1)))
        f.write('\n')