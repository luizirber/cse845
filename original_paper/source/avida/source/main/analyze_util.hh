//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef ANALYZE_UTIL_HH
#define ANALYZE_UTIL_HH

class cGenome;
class cInstLib;
class cPopulation;
class ofstream;

// This is a static class used to do various forms of complex analysis
// on genomes.

class cAnalyzeUtil {
private:
public:
  // Generic test-CPU analysis
  static void TestGenome(const cGenome & genome, cInstLib & inst_lib,
			 ofstream & fp, int update);

  static void TestInsSizeChangeRobustness(ofstream & fp,
                 const cInstLib & inst_lib, const cGenome & in_genome,
                 int num_trials, int update);


  // Landscape-based analysis
  static cGenome CalcLandscape(int dist, const cGenome & genome,
			       cInstLib & inst_lib);
  static void AnalyzeLandscape(const cGenome & genome, cInstLib & inst_lib,
			       int sample_size=1000, int min_found=0,
			       int max_sample_size=0, int update=-1);
  static void PairTestLandscape(const cGenome & genome, cInstLib & inst_lib,
				int sample_size=0, int update=-1);


  // Population-wide analysis
  static void CalcConsensus(cPopulation * population, int lines_saved);

  static void AnalyzePopulation(cPopulation * pop, ofstream & fp,
				double sample_prob=1, bool landscape=false,
				bool save_genotype=false);

  static void PrintDetailedFitnessData(cPopulation * pop, ofstream & datafp,
    ofstream & histofp, ofstream & histo_testCPU_fp, bool save_max_f_genotype,
    bool print_fitness_histo, double hist_fmax, double hist_fstep);

  static void PrintGeneticDistanceData(cPopulation * pop, ofstream & fp,
				const char *creature_name );
  static void GeneticDistancePopDump(cPopulation * pop, ofstream & fp,
		    const char * creature_name, bool save_creatures=false);

  static void TaskSnapshot(cPopulation * pop, ofstream & fp);
  static void PrintViableTasksData(cPopulation *pop, ofstream &fp);
  static void PrintTreeDepths(cPopulation * pop, ofstream & fp);

  static void PrintDepthHistogram(ofstream &fp, cPopulation * pop);
  static void PrintGenotypeAbundanceHistogram(ofstream &fp, cPopulation * pop);
  static void PrintSpeciesAbundanceHistogram(ofstream &fp, cPopulation * pop);
};

#endif
