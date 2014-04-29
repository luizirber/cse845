import numpy as np
import matplotlib.pyplot as plt
import cPickle as pickle
import math

def get_sync_env():
    fig,(s1)=plt.subplots(1,1,figsize=(9,6))
    for i in range(10):
        y_value=100
        if i%2==1:
            y_value=1
        s1.plot(range(i*100,i*100+101),[y_value for j in range(101)],"b-")
    s1.axis([0,1000,-50,200])
    s1.set_yticks([1,100])
    s1.set_ylabel("Resource Inflow")
    s1.set_xlabel("Updates")
    s1.set_title("Synchronous Environment: 100 update-long fluctuations")
    plt.savefig("paper_figs/sync_env.png")
    
def get_stag_env():
    fig,(s1)=plt.subplots(1,1,figsize=(9,6))
    for i in range(10):
        y_value_r=100
        y_value_k=2
        if i%2==1:
            y_value_r=1
            y_value_k=99
        s1.plot(range(i*100,i*100+101),[y_value_r for j in range(101)],"r-")
        s1.plot(range(i*100,i*100+101),[y_value_k for j in range(101)],"b-")
    s1.axis([0,1000,-50,200])
    s1.set_yticks([1,100])
    s1.set_ylabel("Resource Inflow")
    s1.set_xlabel("Updates")
    s1.set_title("Staggered Environment: 100 update-long fluctuations")
    plt.savefig("paper_figs/stag_env.png")
    
get_sync_env()
get_stag_env()