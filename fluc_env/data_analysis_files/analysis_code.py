import scipy.stats as stats
import cPickle as pickle
import copy
import math

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

