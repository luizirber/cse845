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

def get_phenotypes_for_update(file_loc):
    """
    Gets the phenotypes and their abundance for one update of an avida run
    Input: File name of phenotype file from analysis mode
    Output: List of tuples of the form (phenotype string, number of phenotypes)
    """
    phenotypes_and_counts=[]
    phenotype_file_lines=get_file_lines(str(file_loc))
    for line in phenotype_file_lines:
        if len(line)!=0 and line[0]!="#":
            temp=str(line).split(" ")
            pheno_str=""
            num=int(temp[0])
            for i in range(6,15):
                assert str(temp[i]) in ["0","1"], "Unknown entry in phenotype string: "+str(temp[i])
                pheno_str+=str(temp[i])
            assert len(pheno_str)==9,"Incorrect Phenotype String Length: "+str(pheno_str)
            phenotypes_and_counts+=[(pheno_str,num)] 
    return copy.deepcopy(phenotypes_and_counts)

def get_phenotype_nums_for_replicate(file_loc,replicate_num):
    """
    Gets the number of unique phenotypes for a replicate at the end of a run
    Input: File location that ends with: .../replicate_num
           The replicate number (ranges from 1 to 30)
    output: Number of phenotypes present at the end of run
    """
    phenotype_counts=0
    file_str=str(file_loc)+"/phenotypes/phenotype_"+str(replicate_num)+".dat"
    phenos_for_update=copy.deepcopy(get_phenotypes_for_update(file_str))
    phenos=[]
    for i in range(len(phenos_for_update)):
        if str(phenos_for_update[i][0]) not in phenos:
            phenos+=[str(phenos_for_update[i][0])]
    phenotype_counts+=len(phenos)
    return copy.deepcopy(phenotype_counts)

def get_phenotype_nums_for_treatment(resource_inflow):
    """
    Gets the number of unique phenotypes for every replicate for a specific treatment
    Input: The treatment (i.e. number of resources inflowing per update)
    Ouput: String of 30 phenotype counts (1 per replicate)
    """
    phenotype_counts=[]
    file_loc="../data_"+str(resource_inflow)
    for replicate in range(1,31):
        p_replicate_counts=copy.deepcopy(get_phenotype_nums_for_replicate(str(file_loc),str(replicate)))
        phenotype_counts+=[copy.deepcopy(p_replicate_counts)]
    assert len(phenotype_counts)==30, "Don't have 30 phenotype counts; inflow="+str(resource_inflow)
    return copy.deepcopy(phenotype_counts)

def get_phenotype_nums_stat_for_treatment(resource_inflow,stat):
    """
    Calculates either mean or standard error of number of phenotypes for a treatment
    Input: The treatment (i.e. number of resources inflowing per update)
           The statistics to be calculated (either mean or se)
    Output: The statistics calculated
    """
    phenotype_stats=0
    phenotype_counts=copy.deepcopy(get_phenotype_nums_for_treatment(str(resource_inflow)))
    if stat=="mean":
        phenotype_stats=stats.nanmean(phenotype_counts)
    elif stat=="se":
        phenotype_stats=stats.sem(phenotype_counts)
    return copy.deepcopy(phenotype_stats)

def get_phenotype_nums(resource_inflows):
    """
    Gets the mean and se for various treatments (i.e. resource inflows) and saves the data to be plotted later using pickle
    Input: list of treatments
    Output: Nothing; saves data to files
    """
    for j in resource_inflows:
        plot_data_mean=get_phenotype_nums_stat_for_treatment(str(j),stat="mean")
        plot_data_se=get_phenotype_nums_stat_for_treatment(str(j),stat="se")
        pickle.dump(plot_data_mean,open("../replication_plot_data/phenotype_nums_mean_"+str(j)+".data","wb"))
        pickle.dump(plot_data_se,open("../replication_plot_data/phenotype_nums_se_"+str(j)+".data","wb"))
    return "success"

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

def get_final_ecotype_num_data(resource_levels,type_data):
    for resource_level in resource_levels:
        ecotype_mean=0
        ecotype_se=0
        ecotype_sd=0
        ecotype_counts=[]
        for replicate in range(1,31):
            if type_data=="phenotype":
                file_str="../data_"+str(resource_level)+"/phenotypes/phenotype_"+str(replicate)+".dat"
            elif type_data=="genotype":
                file_str="../data_"+str(resource_level)+"/genotypes_time/genotype_"+str(replicate)+".dat"
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
        pickle.dump(ecotype_mean,open("../replication_plot_data/"+str(type_data)+"_nums_mean_"+str(resource_level)+".data","wb"))
        pickle.dump(ecotype_se,open("../replication_plot_data/"+str(type_data)+"_nums_se_"+str(resource_level)+".data","wb"))
        pickle.dump(ecotype_sd,open("../replication_plot_data/"+str(type_data)+"_nums_sd_"+str(resource_level)+".data","wb"))
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

def get_final_task_num_ecotypes(resource_levels):
    for resource_level in resource_levels:
        ecotype_mean=0
        ecotype_se=0
        ecotype_sd=0
        ecotype_counts=[]
        for replicate in range(1,31):
            file_str="../data_"+str(resource_level)+"/task_nums/replicate_"+str(replicate)+".data"
            ecos_for_update=copy.deepcopy(get_task_num_ecotypes_for_update(file_str))
            present_ecotypes=[]
            for i in range(len(ecos_for_update)):
                if list(ecos_for_update[i]) not in present_ecotypes:
                    present_ecotypes+=[list(ecos_for_update[i])]
            ecotype_counts+=[len(present_ecotypes)]
        ecotype_mean=stats.nanmean(ecotype_counts)
        ecotype_se=stats.sem(ecotype_counts)
        ecotype_sd=stats.nanstd(ecotype_counts)
        pickle.dump(ecotype_mean,open("../replication_plot_data/task_num_ecotype_nums_mean_"+str(resource_level)+".data","wb"))
        pickle.dump(ecotype_se,open("../replication_plot_data/task_num_ecotype_nums_se_"+str(resource_level)+".data","wb"))
        pickle.dump(ecotype_sd,open("../replication_plot_data/task_num_ecotype_nums_sd_"+str(resource_level)+".data","wb"))
    return "success"

def get_site_proportion_data(inflow_rates):
    """
    Returns the mean and std. error of the proportion of sites used in tasks (generated by modularity analysis)
    """
    assert type(inflow_rates)==list
    
    for inflow in inflow_rates:
        treatment_means=[]
        treatment_ses=[]
        for update in range(0,400001,20000):
            update_data=[]
            for replicate in range(1,31):
                data_for_replicate=get_file_lines("../data_"+str(inflow)+"/modularity/replicate_"+str(replicate)+"_"+str(update)+".dat")
                assert len(data_for_replicate)==1, ""+str(len(data_for_replicate))
                temp=str(data_for_replicate[0]).split(" ")
                update_data+=[float(temp[3])]
            assert len(update_data)==30,""+str(len(update_data))
            treatment_means+=[stats.nanmean(update_data)]
            treatment_ses+=[stats.sem(update_data)]
        pickle.dump(treatment_means,open("../replication_plot_data/site_proportion_means_"+str(inflow)+".data","wb"))
        pickle.dump(treatment_ses,open("../replication_plot_data/site_proportion_ses_"+str(inflow)+".data","wb"))
    return "success"

def get_equ_counts(inflow_levels,update):
    assert type(inflow_levels)==list
    assert type(update)==int
    
    for inflow in inflow_levels:
        treatment_counts_1=0
        treatment_counts=[]
        for replicate in range(1,31):
            data_for_replicate=get_file_lines("../data_"+str(inflow)+"/replicate_"+str(replicate)+"/tasks.dat")
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
        pickle.dump(treatment_counts_1,open("../replication_plot_data/equ_counts_1_"+"_"+str(inflow)+"_"+str(update)+".data","wb"))
    return "success"

def get_generation_data(inflow_levels):
    assert type(inflow_levels)==list
    
    for inflow in inflow_levels:
        treatment_means=[]
        treatment_ses=[]
        treatment_data=[[] for i in range((400000/50)+1)]
        for replicate in range(1,31):
            data_for_replicate=get_file_lines("../data_"+str(inflow)+"/replicate_"+str(replicate)+"/average.dat")
            for line in data_for_replicate:
                if len(line)>0 and not line.startswith("#"):
                    temp=str(line).split(" ")
                    if int(temp[0])<=400000:
                        treatment_data[int(temp[0])/50]+=[float(temp[12])]
        for update in range(len(treatment_data)):
            treatment_means+=[stats.nanmean(treatment_data[update])]
            treatment_ses+=[stats.sem(treatment_data[update])]
        pickle.dump(treatment_means,open("../replication_plot_data/generation_means_"+str(inflow)+".data","wb"))
        pickle.dump(treatment_ses,open("../replication_plot_data/generation_ses_"+str(inflow)+".data","wb"))
    return "success"

def get_genotype_count_data(inflow_levels):
    assert type(inflow_levels)==list
    
    for inflow in inflow_levels:
        treatment_means=[]
        treatment_ses=[]
        treatment_data=[[] for i in range((400000/50)+1)]
        for replicate in range(1,31):
            data_for_replicate=get_file_lines("../data_"+str(inflow)+"/replicate_"+str(replicate)+"/count.dat")
            for line in data_for_replicate:
                if len(line)>0 and not line.startswith("#"):
                    temp=str(line).split(" ")
                    if int(temp[0])<=400000:
                        treatment_data[int(temp[0])/50]+=[float(temp[3])]
        for update in range(len(treatment_data)):
            treatment_means+=[stats.nanmean(treatment_data[update])]
            treatment_ses+=[stats.sem(treatment_data[update])]
        pickle.dump(treatment_means,open("../replication_plot_data/genotype_count_means_"+str(inflow)+".data","wb"))
        pickle.dump(treatment_ses,open("../replication_plot_data/genotype_count_ses_"+str(inflow)+".data","wb"))
    return "success"
