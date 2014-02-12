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

