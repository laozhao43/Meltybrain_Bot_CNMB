import numpy as np
import pandas as pd
from scipy.optimize import least_squares

import matplotlib.pyplot as plt
import os

print (os.getcwd())
data=[[],[]]
with open('5173.csv','r') as f:
    for line in f:
        vals=line.split(',')
        accel=int(abs(float(vals[0])*100))
        beacon=float(vals[1])*1000/360

        data[0].append(accel)
        data[1].append(beacon)
    f.close()
def fun(x,t,y):
    return x[0]/(np.sqrt((t+x[2])/x[1]))-y
x0=np.ones(3)
res_robust=least_squares(fun,x0,loss='linear',f_scale=0.1,args=(data[0],data[1]))
print(res_robust.x)
t_test=np.linspace(0,3000,500)
plt.plot(data[0],data[1],'.b',label='data')
plt.plot(t_test,fun(res_robust.x,t_test,0),'-r',label='best fit')
print(data[1])
plt.legend()
plt.show()
