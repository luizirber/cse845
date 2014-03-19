import scipy.stats as stats
import cPickle as pickle
import copy
import math
import numpy as np

def get_file_lines(file_str):
    """
    Input: string of file name
    Output: List of lines in file
    """
    
    file_name=str(file_str)
    try:
        f=open(file_name)
    except:
        raise StandardError(file_name+" does not exist")
    lines=f.read().splitlines()
    f.close()
    assert len(lines)!=0,"length of "+str(file_str)+"=0"
    return copy.deepcopy(lines)
"""
def get_ecotypes_for_update(file_loc,type_data):
    ecotypes_and_counts=[]
    ecotype_file_lines=get_file_lines(str(file_loc))
    for line in ecotype_file_lines:
        if len(line)!=0 and line[0]!="#":
            temp=str(line).split(" ")
            eco_str=""
            if type_data=="phenotype":
                num=int(temp[0])
                for i in range(6,15):
                    assert str(temp[i]) in ["0","1"], "Unknown entry in phenotype string: "+str(temp[i])
                    eco_str+=str(temp[i])   
            elif type_data=="genotype":
                num=int(temp[1])
                eco_str=str(temp[2])
            else:
                raise StandardError("Incorrect Type Data Entry: "+str(type_data))
            assert len(eco_str)==9,"Incorrect Ecotype String Length: "+str(eco_str)
            ecotypes_and_counts+=[(eco_str,num)] 
    return copy.deepcopy(ecotypes_and_counts)

def get_final_ecotype_num_data(fluc_levels,type_data,fluc_type):
    for fluc_level in fluc_levels:
        ecotype_mean=0
        ecotype_se=0
        ecotype_sd=0
        ecotype_counts=[]
        for replicate in range(1,31):
            if type_data=="phenotype":
                file_str="../data_"+str(fluc_type)+"_"+str(fluc_level)+"/phenotypes/phenotype_"+str(replicate)+".dat"
            elif type_data=="genotype":
                file_str="../data_"+str(fluc_type)+"_"+str(fluc_level)+"/genotypes_time/genotype_"+str(replicate)+".dat"
            else:
                raise StandardError("Incorrect Type Data Entry: "+str(type_data))
            ecos_for_update=copy.deepcopy(get_ecotypes_for_update(file_str,type_data))
            present_ecotypes=[]
            if type_data=="genotype":
                ecotype_counts+=[len(ecos_for_update)]
            elif type_data=="phenotype":
                for i in range(len(ecos_for_update)):
                    if str(ecos_for_update[i][0]) not in present_ecotypes:
                        present_ecotypes+=[str(ecos_for_update[i][0])]
                ecotype_counts+=[len(present_ecotypes)]
        ecotype_mean=stats.nanmean(ecotype_counts)
        ecotype_se=stats.sem(ecotype_counts)
        ecotype_sd=stats.nanstd(ecotype_counts)
        pickle.dump(ecotype_mean,open("../plot_data/"+str(type_data)+"_nums_mean_"+str(fluc_type)+"_"+str(fluc_level)+".data","wb"))
        pickle.dump(ecotype_se,open("../plot_data/"+str(type_data)+"_nums_se_"+str(fluc_type)+"_"+str(fluc_level)+".data","wb"))
        pickle.dump(ecotype_sd,open("../plot_data/"+str(type_data)+"_nums_sd_"+str(fluc_type)+"_"+str(fluc_level)+".data","wb"))
    return "success"

def get_task_num_ecotypes_for_update(file_loc):
    ecotypes=[]
    ecotype_file_lines=get_file_lines(str(file_loc))
    for line in ecotype_file_lines:
        if len(line)!=0 and line[0]!="#":
            temp=str(line).split(" ")
            eco_str=list(temp)
            if len(eco_str)==10 and eco_str[9]=="":
                eco_str=eco_str[:9]
            assert len(eco_str)==9,"Incorrect Ecotype String Length: "+str(eco_str)
            ecotypes+=[eco_str] 
    return copy.deepcopy(ecotypes)

def get_final_task_num_ecotypes(fluc_levels,fluc_type):
    for fluc_level in fluc_levels:
        ecotype_mean=0
        ecotype_se=0
        ecotype_sd=0
        ecotype_counts=[]
        for replicate in range(1,31):
            file_str="../data_"+str(fluc_type)+"_"+str(fluc_level)+"/task_nums/replicate_"+str(replicate)+".data"
            ecos_for_update=copy.deepcopy(get_task_num_ecotypes_for_update(file_str))
            present_ecotypes=[]
            for i in range(len(ecos_for_update)):
                if list(ecos_for_update[i]) not in present_ecotypes:
                    present_ecotypes+=[list(ecos_for_update[i])]
            ecotype_counts+=[len(present_ecotypes)]
        ecotype_mean=stats.nanmean(ecotype_counts)
        ecotype_se=stats.sem(ecotype_counts)
        ecotype_sd=stats.nanstd(ecotype_counts)
        pickle.dump(ecotype_mean,open("../plot_data/task_num_ecotype_nums_mean_"+str(fluc_type)+"_"+str(fluc_level)+".data","wb"))
        pickle.dump(ecotype_se,open("../plot_data/task_num_ecotype_nums_se_"+str(fluc_type)+"_"+str(fluc_level)+".data","wb"))
        pickle.dump(ecotype_sd,open("../plot_data/task_num_ecotype_nums_sd_"+str(fluc_type)+"_"+str(fluc_level)+".data","wb"))
    return "success"
"""
def get_resource_counts(fluc_levels,fluc_type):
    for fluc_level in fluc_levels:
        resource_counts_mean=[[] for i in range(9)]
        resource_counts_se=[[] for i in range(9)]
        resource_counts_sd=[[] for i in range(9)]
        resource_counts=[]
        for replicate in range(1,31):
            replicate_counts=[[] for i in range(9)]
            resources_for_replicate=get_file_lines("../data_"+str(fluc_type)+"_"+str(fluc_level)+"/replicate_"+str(replicate)+"/resource.dat")
            for i in range(len(resources_for_replicate)):
                if len(resources_for_replicate[i])!=0 and resources_for_replicate[i][0]!="#":
                    temp=str(resources_for_replicate[i]).split(" ")
                    for j in range(1,10):
                        replicate_counts[j-1]+=[temp[j]]
            assert len(replicate_counts[0])==500000/50+1,""+str(len(replicate_counts[0]))
            resource_counts+=[copy.deepcopy(replicate_counts)]
        assert len(resource_counts)==30,""+str(len(resource_counts))
        for resource in range(9):
            for update in range(0,500001,50):
                update_data=[float(resource_counts[i][resource][update/50]) for i in range(30)]
                resource_counts_mean[resource]+=[stats.nanmean(update_data)]
                resource_counts_se[resource]+=[stats.sem(update_data)]
                resource_counts_sd[resource]+=[stats.nanstd(update_data)]
        pickle.dump(resource_counts_mean,open("../plot_data/resource_counts_mean_"+str(fluc_type)+"_"+str(fluc_level)+".data","wb"))
        pickle.dump(resource_counts_se,open("../plot_data/resource_counts_se_"+str(fluc_type)+"_"+str(fluc_level)+".data","wb"))
        pickle.dump(resource_counts_sd,open("../plot_data/resource_counts_sd_"+str(fluc_type)+"_"+str(fluc_level)+".data","wb"))
    return "success"

def get_generations_per_update(fluc_levels,fluc_type):
    for fluc_level in fluc_levels:
        gens_mean=[]
        gens_se=[]
        gens_sd=[]
        for replicate in range(1,31):
            gen_counts=[]
            gens_for_replicate=get_file_lines("../data_"+str(fluc_type)+"_"+str(fluc_level)+"/replicate_"+str(replicate)+"/time.dat")
            for i in range(len(gens_for_replicate)):
                if len(gens_for_replicate[i])!=0 and gens_for_replicate[i][0]!="#":
                    temp=str(gens_for_replicate[i]).split(" ")
                    if i!=0:
                        gen_counts+=[float(temp[2])/(50.0*i)]
            assert len(gen_counts)==500000/50+1,""+str(len(gen_counts))
            gens_mean+=[stats.nanmean(gen_counts)]
            gens_se+=[stats.sem(gen_counts)]
            gens_sd+=[stats.nanstd(gen_counts)]
        pickle.dump(gens_mean,open("../plot_data/gens_mean_"+str(fluc_type)+"_"+str(fluc_level)+".data","wb"))
        pickle.dump(gens_se,open("../plot_data/gens_se_"+str(fluc_type)+"_"+str(fluc_level)+".data","wb"))
        pickle.dump(gens_sd,open("../plot_data/gens_sd_"+str(fluc_type)+"_"+str(fluc_level)+".data","wb"))
    return "success"

def get_total_resource_counts(fluc_levels,fluc_type):
    for fluc_level in fluc_levels:
        resource_counts_mean=[]
        resource_counts_se=[]
        resource_counts_sd=[]
        resource_counts=[]
        for replicate in range(1,31):
            replicate_counts=[]
            resources_for_replicate=get_file_lines("../data_"+str(fluc_type)+"_"+str(fluc_level)+"/replicate_"+str(replicate)+"/resource.dat")
            for i in range(len(resources_for_replicate)):
                if len(resources_for_replicate[i])!=0 and resources_for_replicate[i][0]!="#":
                    temp=str(resources_for_replicate[i]).split(" ")
                    update_resource_count=0
                    for j in range(1,10):
                        update_resource_count+=float(temp[j])
                    replicate_counts+=[update_resource_count]
            assert len(replicate_counts)==500000/50+1,""+str(len(replicate_counts))
            resource_counts+=[copy.deepcopy(replicate_counts)]
        assert len(resource_counts)==30,""+str(len(resource_counts))
        for update in range(0,400001,50):
            update_data=[float(resource_counts[i][update/50]) for i in range(30)]
            resource_counts_mean+=[stats.nanmean(update_data)]
            resource_counts_se+=[stats.sem(update_data)]
            resource_counts_sd+=[stats.nanstd(update_data)]
        pickle.dump(resource_counts_mean,open("../plot_data/total_resource_counts_mean_"+str(fluc_type)+"_"+str(fluc_level)+".data","wb"))
        pickle.dump(resource_counts_se,open("../plot_data/total_resource_counts_se_"+str(fluc_type)+"_"+str(fluc_level)+".data","wb"))
        pickle.dump(resource_counts_sd,open("../plot_data/total_resource_counts_sd_"+str(fluc_type)+"_"+str(fluc_level)+".data","wb"))
    return "success"

def get_total_task_counts(fluc_levels,fluc_type):
    for fluc_level in fluc_levels:
        task_counts_mean=[]
        task_counts_se=[]
        task_counts_sd=[]
        task_counts=[]
        for replicate in range(1,31):
            replicate_counts=[]
            tasks_for_replicate=get_file_lines("../data_"+str(fluc_type)+"_"+str(fluc_level)+"/replicate_"+str(replicate)+"/tasks.dat")
            for i in range(len(tasks_for_replicate)):
                if len(tasks_for_replicate[i])!=0 and tasks_for_replicate[i][0]!="#":
                    temp=str(tasks_for_replicate[i]).split(" ")
                    update_task_count=0
                    for j in range(1,10):
                        update_task_count+=float(temp[j])
                    replicate_counts+=[update_task_count]
            assert len(replicate_counts)==500000/50+1,""+str(len(replicate_counts))
            task_counts+=[copy.deepcopy(replicate_counts)]
        assert len(task_counts)==30,""+str(len(task_counts))
        for update in range(0,400001,50):
            update_data=[float(task_counts[i][update/50]) for i in range(30)]
            task_counts_mean+=[stats.nanmean(update_data)]
            task_counts_se+=[stats.sem(update_data)]
            task_counts_sd+=[stats.nanstd(update_data)]
        pickle.dump(task_counts_mean,open("../plot_data/total_task_counts_mean_"+str(fluc_type)+"_"+str(fluc_level)+".data","wb"))
        pickle.dump(task_counts_se,open("../plot_data/total_task_counts_se_"+str(fluc_type)+"_"+str(fluc_level)+".data","wb"))
        pickle.dump(task_counts_sd,open("../plot_data/total_task_counts_sd_"+str(fluc_type)+"_"+str(fluc_level)+".data","wb"))
    return "success"

def get_phylo_depth_changes(fluc_levels,fluc_type,data_type):
    
    assert type(fluc_levels)==list
    assert type(fluc_type)==str
    assert fluc_type in ["sync","stag","lowhigh"]
    assert type(data_type)==str
    assert data_type in ["raw","avg"]
    
    for fluc_level in fluc_levels:
        
        fluc_length=int(fluc_level)
        
        if data_type=="avg":
            start_slope_means=[]
            start_slope_se=[]
            end_slope_means=[]
            end_slope_se=[]
        else:
            start_slopes=[[] for i in range(30)]
            end_slopes=[[] for i in range(30)]
            
        for replicate in range(1,31):
            avg_depth_for_updates=[]
            start_inflow_slopes=[]
            end_inflow_slopes=[]
            averages_for_replicate=get_file_lines("../data_"+str(fluc_type)+"_"+str(fluc_level)+"/replicate_"+str(replicate)+"/average.dat")
            for line in averages_for_replicate:
                if len(line)!=0 and line[0]!="#":
                    temp=line.split(" ")
                    update=int(temp[0])
                    if update%fluc_length==0:
                        depth=float(temp[11])
                        avg_depth_for_updates+=[float(depth)]
                        
            for i in range(len(avg_depth_for_updates)-1):
                if i%2==0:
                    start_inflow_slopes+=[math.fabs(avg_depth_for_updates[i]-avg_depth_for_updates[i+1])]
                else:
                    end_inflow_slopes+=[math.fabs(avg_depth_for_updates[i]-avg_depth_for_updates[i+1])]
                    
            if data_type=="avg":
                start_slope_means+=[stats.nanmean(start_inflow_slopes)]
                start_slope_se+=[stats.sem(start_inflow_slopes)]
                end_slope_means+=[stats.nanmean(end_inflow_slopes)]
                end_slope_se+=[stats.sem(end_inflow_slopes)]
            else:
                start_slopes[replicate-1]=list(start_inflow_slopes)
                end_slopes[replicate-1]=list(end_inflow_slopes)
                
        if data_type=="avg":
            pickle.dump(start_slope_means,open("../plot_data/start_slope_mean_"+str(fluc_type)+"_"+str(fluc_level)+".data","wb"))
            pickle.dump(end_slope_means,open("../plot_data/end_slope_mean_"+str(fluc_type)+"_"+str(fluc_level)+".data","wb"))
            pickle.dump(start_slope_se,open("../plot_data/start_slope_se_"+str(fluc_type)+"_"+str(fluc_level)+".data","wb"))
            pickle.dump(end_slope_se,open("../plot_data/end_slope_se_"+str(fluc_type)+"_"+str(fluc_level)+".data","wb"))
        else:
            pickle.dump(start_slopes,open("../plot_data/start_slope_raw_"+str(fluc_type)+"_"+str(fluc_level)+".data","wb"))
            pickle.dump(end_slopes,open("../plot_data/end_slope_raw_"+str(fluc_type)+"_"+str(fluc_level)+".data","wb"))
            
    return "success"

def get_count(data_category,specific_data,fluc_levels,fluc_type):
    assert data_category in ["resource","tasks"]
    assert type(fluc_levels)==list
    assert type(fluc_type)==str
    assert fluc_type in ["sync","stag","lowhigh"]
    assert specific_data>=0
    assert specific_data<=8
    
    for fluc_level in fluc_levels:
        treatment_counts_mean=[]
        treatment_counts_se=[]
        treatment_counts_sd=[]
        treatment_counts=[]
        for replicate in range(1,31):
            replicate_counts=[]
            data_for_replicate=get_file_lines("../data_"+str(fluc_type)+"_"+str(fluc_level)+"/replicate_"+str(replicate)+"/"+str(data_category)+".dat")
            for i in range(len(data_for_replicate)):
                if len(data_for_replicate[i])!=0 and data_for_replicate[i][0]!="#":
                    temp=str(data_for_replicate[i]).split(" ")
                    update_count=float(temp[specific_data+1])
                    replicate_counts+=[update_count]
            assert len(replicate_counts)==500000/50+1,""+str(len(replicate_counts))
            treatment_counts+=[copy.deepcopy(replicate_counts)]
        assert len(treatment_counts)==30,""+str(len(treatment_counts))
        for update in range(0,400001,50):
            update_data=[float(treatment_counts[i][update/50]) for i in range(30)]
            treatment_counts_mean+=[stats.nanmean(update_data)]
            treatment_counts_se+=[stats.sem(update_data)]
            treatment_counts_sd+=[stats.nanstd(update_data)]
        pickle.dump(treatment_counts_mean,open("../plot_data/"+str(data_category)+"_"+str(specific_data)+"_counts_mean_"+str(fluc_type)+"_"+str(fluc_level)+".data","wb"))
        pickle.dump(treatment_counts_se,open("../plot_data/"+str(data_category)+"_"+str(specific_data)+"_counts_se_"+str(fluc_type)+"_"+str(fluc_level)+".data","wb"))
        pickle.dump(treatment_counts_sd,open("../plot_data/"+str(data_category)+"_"+str(specific_data)+"_counts_sd_"+str(fluc_type)+"_"+str(fluc_level)+".data","wb"))
    return "success"

def get_equ_counts(fluc_levels,fluc_type,update):
    assert type(fluc_levels)==list
    assert type(fluc_type)==str
    assert fluc_type in ["sync","stag","lowhigh"]
    assert type(update)==int
    
    for fluc_level in fluc_levels:
        treatment_counts_1=0
        treatment_counts=[]
        for replicate in range(1,31):
            data_for_replicate=get_file_lines("../data_"+str(fluc_type)+"_"+str(fluc_level)+"/replicate_"+str(replicate)+"/tasks.dat")
            for i in range(len(data_for_replicate)):
                if len(data_for_replicate[i])!=0 and data_for_replicate[i][0]!="#":
                    temp=str(data_for_replicate[i]).split(" ")
                    if temp[0]==str(update):
                        if int(temp[9])==0:
                            treatment_counts+=[0]
                        else:
                            treatment_counts+=[1]
                        break
        assert len(treatment_counts)==30,""+str(len(treatment_counts))
        treatment_counts_1=treatment_counts.count(1)
        pickle.dump(treatment_counts_1,open("../plot_data/equ_counts_1_"+str(fluc_type)+"_"+str(fluc_level)+"_"+str(update)+".data","wb"))
    return "success"

def get_site_proportion_data(fluc_levels,fluc_type):
    """
    Returns the mean and std. error of the proportion of sites used in tasks (generated by modularity analysis)
    """
    assert type(fluc_levels)==list
    assert type(fluc_type)==str
    assert fluc_type in ["sync","stag","lowhigh"]
    
    for fluc_level in fluc_levels:
        treatment_means=[]
        treatment_ses=[]
        for update in range(0,400001,20000):
            update_data=[]
            for replicate in range(1,31):
                data_for_replicate=get_file_lines("../data_"+str(fluc_type)+"_"+str(fluc_level)+"/modularity/replicate_"+str(replicate)+"_"+str(update)+".dat")
                assert len(data_for_replicate)==1, ""+str(len(data_for_replicate))
                temp=str(data_for_replicate[0]).split(" ")
                update_data+=[float(temp[3])]
            assert len(update_data)==30,""+str(len(update_data))
            treatment_means+=[stats.nanmean(update_data)]
            treatment_ses+=[stats.sem(update_data)]
        pickle.dump(treatment_means,open("../plot_data/site_proportion_means_"+str(fluc_type)+"_"+str(fluc_level)+".data","wb"))
        pickle.dump(treatment_ses,open("../plot_data/site_proportion_ses_"+str(fluc_type)+"_"+str(fluc_level)+".data","wb"))
    return "success"
