import numpy as np
import matplotlib.pyplot as plt
import cPickle as pickle
import math    
    
def gen_fig_1():
    
    length_key=["1","10","100","1000","10000","100000"]
    x_values=[0,1,2,3,4,5]
    
    data_holder_mean=[0 for i in xrange(len(length_key)*3)]
    data_holder_se=[0 for i in xrange(len(length_key)*3)]
    
    a=open("/users/thomas/documents/research/cse845f/chow_replication/replication_plot_data/cluster_species_mean_100.data","rb")
    high_inflow_mean=pickle.load(a)
    a.close()
    b=open("/users/thomas/documents/research/cse845f/chow_replication/replication_plot_data/cluster_species_mean_1.data","rb")
    low_inflow_mean=pickle.load(b)
    b.close()
    c=open("/users/thomas/documents/research/cse845f/chow_replication/replication_plot_data/cluster_species_mean_10.data","rb")
    med_inflow_mean=pickle.load(c)
    c.close()
    
    index=0
    for h in ["sync","lowhigh","stag"]:
        for i in length_key:
            for k in ["mean","se"]:
                f=open("/users/thomas/documents/research/cse845f/fluc_env/plot_data/cluster_species_"+str(k)+"_"+str(h)+"_"+str(i)+".data","rb")
                if k=="mean":
                    data_holder_mean[index]=pickle.load(f)
                elif k=="se":
                    data_holder_se[index]=pickle.load(f)
                f.close()
            index+=1
                
    fig1,(s1)=plt.subplots(1,1,figsize=(9,6))
    s1.errorbar(x_values,data_holder_mean[0:6],yerr=[i*1.96 for i in data_holder_se[0:6]],fmt="ko")
    s1.errorbar(x_values,data_holder_mean[6:12],yerr=[i*1.96 for i in data_holder_se[6:12]],fmt="ko",fillstyle="none")
    s1.errorbar(x_values,data_holder_mean[12:18],yerr=[i*1.96 for i in data_holder_se[12:18]],fmt="ks")
    s1.plot([i for i in range(-1,7)],[high_inflow_mean for i in range(-1,7)],"r-")
    s1.plot([i for i in range(-1,7)],[med_inflow_mean for i in range(-1,7)],"b-")
    s1.plot([i for i in range(-1,7)],[low_inflow_mean for i in range(-1,7)],"g-")
    s1.plot(x_values,data_holder_mean[0:6],"k-")
    s1.plot(x_values,data_holder_mean[6:12],"k--")
    s1.plot(x_values,data_holder_mean[12:18],"k:")
    s1.axis([-0.5,5.5,0,5])
    s1.set_title("Final Species Richness (n=30; 95% C.I.)")
    s1.set_ylabel("Number of Species")
    s1.set_xlabel("Log10 Fluctuation Length")
    s1.legend(("Synchronous: start high","Synchronous: start low","Staggered","100 Inflow","10 Inflow","1 Inflow"),loc=1)
    plt.savefig("paper_figs/fig_1_species_richness.png")
    return 0

def gen_fig_2():

    length_key=["1","10","100","1000","10000"]
    x_values=[0,1,2,3,4]
    
    data_holder_mean=[[] for i in xrange(len(length_key)*2)]
    data_holder_se=[[] for i in xrange(len(length_key)*2)]
    
    index=0
    for fluc_type in ["sync","stag"]:
        for i in length_key:
            for k in ["mean","se"]:
                f=open("/users/thomas/documents/research/cse845f/fluc_env/plot_data/total_task_counts_"+str(k)+"_"+str(fluc_type)+"_"+str(i)+".data","rb")
                if k=="mean":
                    data_holder_mean[index]=pickle.load(f)
                elif k=="se":
                    data_holder_se[index]=pickle.load(f)
                f.close()
            index+=1
                
    fig1,(s1,s2)=plt.subplots(1,2,figsize=(18,6))
    s1.plot([j for j in range(1001)],data_holder_mean[0][7000:],"y-")
    s1.plot([j for j in range(1001)],data_holder_mean[1][7000:],"b-")
    s1.plot([j for j in range(1001)],data_holder_mean[2][7000:],"g-")
    s1.plot([j for j in range(1001)],data_holder_mean[3][7000:],"r-") 
    s1.plot([j for j in range(1001)],data_holder_mean[4][7000:],"k-") 
    s1.set_title("Synchronous Fluctuations; Last 50k updates")
    s1.set_ylabel("Total Tasks Performed")
    s1.set_xlabel("'Time'")
    s1.legend(("1","10","100","1000","10000"))
    s1.axis([0,1000,0,20000])
    s2.plot([j for j in range(1001)],data_holder_mean[5][7000:],"y-")
    s2.plot([j for j in range(1001)],data_holder_mean[6][7000:],"b-")
    s2.plot([j for j in range(1001)],data_holder_mean[7][7000:],"g-")
    s2.plot([j for j in range(1001)],data_holder_mean[8][7000:],"r-") 
    s2.plot([j for j in range(1001)],data_holder_mean[9][7000:],"k-") 
    s2.set_title("Staggered Fluctuations; Last 50k updates")
    s2.set_ylabel("Total Tasks Performed")
    s2.set_xlabel("'Time'")
    s2.legend(("1","10","100","1000","10000"))
    s2.axis([0,1000,0,20000])
    plt.savefig("paper_figs/fig_2_total_task_performance.png")

    return 0

def gen_fig_3():

    length_key=["1","10","100","1000","10000"]
    x_values=[0,1,2,3,4]
    
    data_holder_mean=[[] for i in xrange(len(length_key)*2)]
    data_holder_se=[[] for i in xrange(len(length_key)*2)]
    
    index=0
    for fluc_type in ["sync","stag"]:
        for i in length_key:
            for k in ["mean","se"]:
                f=open("/users/thomas/documents/research/cse845f/fluc_env/plot_data/total_resource_counts_"+str(k)+"_"+str(fluc_type)+"_"+str(i)+".data","rb")
                if k=="mean":
                    data_holder_mean[index]=pickle.load(f)
                elif k=="se":
                    data_holder_se[index]=pickle.load(f)
                f.close()
            index+=1
                
    fig1,(s1,s2)=plt.subplots(1,2,figsize=(18,6))
    s1.plot([j for j in range(1001)],data_holder_mean[0][7000:],"y-")
    s1.plot([j for j in range(1001)],data_holder_mean[1][7000:],"b-")
    s1.plot([j for j in range(1001)],data_holder_mean[2][7000:],"g-")
    s1.plot([j for j in range(1001)],data_holder_mean[3][7000:],"r-") 
    s1.plot([j for j in range(1001)],data_holder_mean[4][7000:],"k-") 
    s1.set_title("Synchronous Fluctuations; Last 50k updates")
    s1.set_ylabel("Total Resource Levels")
    s1.set_xlabel("'Time'")
    s1.legend(("1","10","100","1000","10000"))
    s1.axis([0,1000,0,30000])
    s2.plot([j for j in range(1001)],data_holder_mean[5][7000:],"y-")
    s2.plot([j for j in range(1001)],data_holder_mean[6][7000:],"b-")
    s2.plot([j for j in range(1001)],data_holder_mean[7][7000:],"g-")
    s2.plot([j for j in range(1001)],data_holder_mean[8][7000:],"r-") 
    s2.plot([j for j in range(1001)],data_holder_mean[9][7000:],"k-") 
    s2.set_title("Staggered Fluctuations; Last 50k updates")
    s2.set_ylabel("Total Resource Levels")
    s2.set_xlabel("'Time'")
    s2.legend(("1","10","100","1000","10000"))
    s2.axis([0,1000,0,30000])
    plt.savefig("paper_figs/fig_3_total_resource_levels.png")

    return 0

def gen_fig_7():
    length_key=["1","10","100","1000","10000","100000"]
    x_values=[0,1,2,3,4,5]
    
    generation_mean=[[] for i in xrange(3*len(length_key))]
    generation_se=[[] for i in xrange(3*len(length_key))]
    
    chow_gen_mean=[[] for i in range(7)]
    chow_gen_se=[[] for i in range(7)]
    
    index=0
    
    for j in ["sync","lowhigh","stag"]:
        for i in length_key:
            for k in ["means","ses"]:
                f=open("/users/thomas/documents/research/cse845f/fluc_env/plot_data/generation_"+str(k)+"_"+str(j)+"_"+str(i)+".data","rb")
                if k=="means":
                    generation_mean[index]=pickle.load(f)
                elif k=="ses":
                    generation_se[index]=pickle.load(f)
                f.close()
            index+=1
            
    index=0
    for j in ["01","1","10","100","1000","10000","100000"]:
        for k in ["means","ses"]:
            f=open("/users/thomas/documents/research/cse845f/chow_replication/replication_plot_data/generation_"+str(k)+"_"+str(j)+".data","rb")
            if k=="means":
                chow_gen_mean[index]=pickle.load(f)
            elif k=="ses":
                chow_gen_se[index]=pickle.load(f)
            f.close()
        index+=1
        
    fig1,(s1)=plt.subplots(1,1,figsize=(9,6))
    s1.errorbar([0,1,2,3,4,5],[generation_mean[i][8000] for i in range(6)],[1.96*generation_se[i][8000] for i in range(6)],fmt="ko")
    s1.errorbar([0,1,2,3,4,5],[generation_mean[i][8000] for i in range(6,12)],[1.96*generation_se[i][8000] for i in range(6,12)],fmt="ko",fillstyle="none")
    s1.errorbar([0,1,2,3,4,5],[generation_mean[i][8000] for i in range(12,18)],[1.96*generation_se[i][8000] for i in range(12,18)],fmt="ks")
    s1.axis([-1,6,0,70000])
    s1.plot([-1,0,1,2,3,4,5,6,],[chow_gen_mean[3][8000] for i in range(8)],"r-")
    s1.plot([-1,0,1,2,3,4,5,6,],[chow_gen_mean[1][8000] for i in range(8)],"g-")
    s1.legend(("Sync: start high","Sync: start low","Staggered","Inflow=100","Inflow=1"),loc=2)
    s1.set_ylabel("Avg. Generation")
    s1.set_xlabel("Log10 Fluctuation Length")
    s1.set_title("Final Avg. Generation")
    plt.savefig("paper_figs/fig_7_total_generations.png")

def gen_fig_8():
    length_key=["1","10","100","1000","10000","100000"]
    x_values=[0,1,2,3,4,5]
    
    generation_mean=[[] for i in xrange(3*len(length_key))]
    generation_se=[[] for i in xrange(3*len(length_key))]
    
    chow_gen_mean=[[] for i in range(7)]
    chow_gen_se=[[] for i in range(7)]
    
    index=0
    
    for j in ["sync","lowhigh","stag"]:
        for i in length_key:
            for k in ["means","ses"]:
                f=open("/users/thomas/documents/research/cse845f/fluc_env/plot_data/generation_"+str(k)+"_"+str(j)+"_"+str(i)+".data","rb")
                if k=="means":
                    generation_mean[index]=pickle.load(f)
                elif k=="ses":
                    generation_se[index]=pickle.load(f)
                f.close()
            index+=1
            
    index=0
    for j in ["01","1","10","100","1000","10000","100000"]:
        for k in ["means","ses"]:
            f=open("/users/thomas/documents/research/cse845f/chow_replication/replication_plot_data/generation_"+str(k)+"_"+str(j)+".data","rb")
            if k=="means":
                chow_gen_mean[index]=pickle.load(f)
            elif k=="ses":
                chow_gen_se[index]=pickle.load(f)
            f.close()
        index+=1
        
    fig1,(s1)=plt.subplots(1,1,figsize=(9,6))
    s1.plot([0,1,2,3,4,5],[math.log10(generation_mean[i][8000]/(400000.0/10**i)) for i in range(6)],"ko")
    s1.plot([0,1,2,3,4,5],[math.log10(generation_mean[i][8000]/(400000.0/10**(i%6))) for i in range(6,12)],"ko",fillstyle="none")
    s1.plot([0,1,2,3,4,5],[math.log10(generation_mean[i][8000]/(400000.0/10**(i%12))) for i in range(12,18)],"ks")
    s1.axis([-1,6,-2,4.5])
    s1.legend(("Sync: start high","Sync: start low","Staggered","Inflow=100","Inflow=1"),loc=2)
    s1.set_ylabel("Log10 Generations per fluctuation")
    s1.set_xlabel("Log10 Fluctuation Length")
    s1.set_title("Generations per fluctuation")
    plt.savefig("paper_figs/fig_8_generations_per_fluc.png")

gen_fig_1()
gen_fig_2()
gen_fig_3()
gen_fig_7()
gen_fig_8()