import numpy as np
import matplotlib.pyplot as plt

def loadfile(filename):
    data = open(filename, 'r')
    time = []
    for i in data.read().splitlines():
        # print(float(i.split(':')[1]))
        time.append(float(i.split(':')[1]))
    
    data.close()
    return time

if __name__ == '__main__':
    time1 = loadfile('/home/bonian/LinuxKernelImplement/fibdrv/TimeTaken.txt')
    X = range(101)
    
    fig = plt.figure()
    plt.xlabel("nth fabonacci")
    plt.ylabel("time(ns)")
    plt.title("Time Measurement of Original Fabonacci Sequence")
    plt.plot(X, time1, marker = '+', markersize = 8)
    plt.legend()
    plt.grid()
    fig.savefig('data.png')