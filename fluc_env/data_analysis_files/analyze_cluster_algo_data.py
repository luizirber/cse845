import scipy.stats as stats
import cPickle as pickle
import copy

FILENAME_PATTERN = "../plot_data/cluster_species_%s_%s_%s_u%d.data"

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

def count_species(inflow_rates,fluc_type,updates=[500000]):
    for update in updates:
        for i in inflow_rates:
            rate_species_nums=[]
            for replicate in range(1,31):
                data=get_file_lines("../data_%s_%s/clustering_history/replicate_%d/%d.output" % (fluc_type, i, replicate, update))
                for line in data:
                    if "Number of Clusters" in line:
                        rate_species_nums+=[int(line[21])]
                        break
            rate_mean=stats.nanmean(rate_species_nums)
            rate_se=stats.sem(rate_species_nums)
            rate_sd=stats.nanstd(rate_species_nums)
            pickle.dump(rate_mean,open(FILENAME_PATTERN % ('mean', fluc_type, i, update), 'wb'))
            pickle.dump(rate_se,open(FILENAME_PATTERN % ('se', fluc_type, i, update), 'wb'))
            pickle.dump(rate_sd,open(FILENAME_PATTERN % ('sd', fluc_type, i, update), 'wb'))
    return "success"

def count_final_species(inflow_rates,fluc_type):
    for i in inflow_rates:
        rate_species_nums=[]
        for replicate in range(1,31):
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

updates = [100000, 200000, 300000, 400000, 500000]
#count_species(["1","10","100","1000","10000"],"sync", updates)
#count_species(["1","10","100","1000","10000"],"stag", updates)
#count_species(["1","10","100","1000","10000"],"lowhigh")
#count_final_species(["10"],"lowhigh")
count_final_species(["100000"],"sync")
count_final_species(["100000"],"stag")
count_final_species(["100000"],"lowhigh")

