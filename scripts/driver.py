import subprocess
import numpy as np
import matplotlib.pyplot as plt

runs = 50

def outlier_filter(datas, threshold = 2):
    datas = np.array(datas)
    z = np.abs((datas - datas.mean()) / datas.std())
    return datas[z < threshold]

def data_processing(data_set, n):
    catgories = data_set[0].shape[0] 
    samples = data_set[0].shape[1]
    final = np.zeros((catgories, samples))
    for c in range(catgories):        
        for s in range(samples):
            final[c][s] =                                                    \
                outlier_filter([data_set[i][c][s] for i in range(n)]).mean()
    return final


if __name__ == "__main__":
    Ys = []
    for i in range(runs):
        # comp_proc_user = subprocess.run('sudo ./client > /dev/null', shell = True)
        comp_proc_kernel = subprocess.run('sudo ./measure > /dev/null', shell = True)
        kernelOutput = np.loadtxt('TimeTaken.txt', dtype = 'float', delimiter = ':', usecols = 1).T
        userOutput = np.loadtxt('UserTimeTaken.txt', dtype = 'float', delimiter = ':', usecols = 1).T
        user2kernelOutput = userOutput-kernelOutput
        output = np.vstack((kernelOutput,userOutput,user2kernelOutput))
        Ys.append(np.delete(output, 0, axis=1))



    X = range(0,len(Ys[0][0]))
    Y = data_processing(Ys, runs)

    fig, ax = plt.subplots(1, 1, sharey = True)
    ax.set_title('perf', fontsize = 16)
    ax.set_xlabel(r'$n_{th}$ fibonacci', fontsize = 16)
    ax.set_ylabel('time (ns)', fontsize = 16)

    ax.plot(X, Y[0], marker = '+', markersize = 7, label = 'kernel')
    ax.plot(X, Y[1], marker = '*', markersize = 3, label = 'user')
    ax.plot(X, Y[2], marker = '^', markersize = 3, label = 'kernel to user')
    ax.legend(loc = 'upper left')

    plt.show()