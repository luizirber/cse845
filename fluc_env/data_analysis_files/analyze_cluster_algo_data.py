import scipy.stats as stats
import cPickle as pickle
import copy

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

def count_species(inflow_rates,fluc_type):
    for i in inflow_rates:
        rate_species_nums=[]
        for replicate in range(1,31):
            if i=="10" and replicate==15:
                continue
            data=get_file_lines("../clustering_files/cluster_"+str(fluc_type)+"_"+str(i)+"/output_files/output_"+str(replicate)+".dat")
            for line in data:
                if "Number of Clusters" in line:
                    rate_species_nums+=[int(line[21])]
                    break
        rate_mean=stats.nanmean(rate_species_nums)
        rate_se=stats.sem(rate_species_nums)
        rate_sd=stats.nanstd(rate_species_nums)
        pickle.dump(rate_mean,open("../plot_data/cluster_species_mean_"+str(fluc_type)+"_"+str(i)+".data","wb"))
        pickle.dump(rate_se,open("../plot_data/cluster_species_se_"+str(fluc_type)+"_"+str(i)+".data","wb"))
        pickle.dump(rate_sd,open("../plot_data/cluster_species_sd_"+str(fluc_type)+"_"+str(i)+".data","wb"))
    return "success"

#count_species(["1","10","100","1000","10000"],"sync")
#count_species(["1","10","100","1000","10000"],"stag")
#count_species(["1","10","100","1000","10000"],"lowhigh")
count_species(["10"],"lowhigh")