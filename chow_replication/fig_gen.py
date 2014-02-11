import numpy as np
import matplotlib.pyplot as plt
import cPickle as pickle
import math

inflow_key=["01","1","10","100"]
x_values=[-1,0,1,2]

data_holder_mean=[0 for i in xrange(len(inflow_key))]
data_holder_se=[0 for i in xrange(len(inflow_key))]

index=0
for i in inflow_key:
    for k in ["mean","se"]:
        f=open("/users/thomas/documents/research/chow_replication/replication_plot_data/phenotype_nums_"+str(k)+"_"+str(i)+".data","rb")

        if k=="mean":
            data_holder_mean[index]=pickle.load(f)
        elif k=="se":
            data_holder_se[index]=pickle.load(f)
        f.close()
    index+=1
            
fig1,(s1)=plt.subplots(1,1,figsize=(6,4))
s1.errorbar(x_values,data_holder_mean,yerr=data_holder_se,fmt="ko")
s1.plot(x_values,data_holder_mean,"k-")
s1.axis([-2,6,0,10])
s1.set_title("Final Phenotype Richness")
s1.set_ylabel("Number of Phenotypes")
s1.set_xlabel("Log10 Inflow Rate")

plt.show()