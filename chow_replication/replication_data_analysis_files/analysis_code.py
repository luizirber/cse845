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

