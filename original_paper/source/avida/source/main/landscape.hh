//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2000 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef LANDSCAPE_HH
#define LANDSCAPE_HH

#include "../tools/tArray.hh"
#include "../tools/tMatrix.hh"

#include "../main/genome.hh"

#include "../cpu/test_cpu.hh"

struct cLandscape {
private:
  const cInstLib & inst_lib;
  cGenome base_genome;
  cGenome peak_genome;
  double base_fitness;
  double base_merit;
  double base_gestation;
  double peak_fitness;
  double total_fitness;
  double total_sqr_fitness;

  int distance;
  int del_distance;
  int ins_distance;

  int trials;

  int total_count;

  int dead_count;
  int neg_count;
  int neut_count;
  int pos_count;

  double test_fitness;

  int * site_count;

  double total_entropy;
  double complexity;

  cCPUTestInfo test_info;  // Info used for all cpu calculations.
  double neut_min;         // These two variables are a range around the base
  double neut_max;         //   fitness to be counted as neutral mutations.
  tMatrix<double> fitness_chart; // Chart of all one-step mutations.

private:
  void BuildFitnessChart();
  void ProcessGenome(cGenome & in_genome);
  void ProcessBase();
  void Process_Body(cGenome & cur_genome, int cur_distance, int start_line);

  void HillClimb_Body(ofstream & fp, cGenome & cur_genome, int & gen);
  void HillClimb_Print(ofstream & fp, const cGenome & _genome,
		       const int gen) const;

  double TestMutPair(cGenome & mod_genome, int line1, int line2,
    const cInstruction & mut1, const cInstruction & mut2, ostream & fp);
public:
  cLandscape(const cGenome & in_genome, const cInstLib & in_inst_lib);
  ~cLandscape();

  void Reset(const cGenome & in_genome);

  void Process(int in_distance=1);
  void ProcessDelete();
  void ProcessInsert();
  void PredictWProcess(ostream & fp, int update=-1);
  void PredictNuProcess(ostream & fp, int update=-1);

  void SampleProcess(int in_trials);
  int RandomProcess(int in_trials, int in_distance=1, int min_found=0,
		     int max_trials=0, bool print_if_found=false);

  void TestPairs(int in_trials, ostream & fp);
  void TestAllPairs(ostream & fp);

  void HillClimb(ofstream & fp);
  void HillClimb_Neut(ofstream & fp);
  void HillClimb_Rand(ofstream & fp);

  void PrintStats(ofstream & fp, int update=-1);
  void PrintEntropy(ofstream & fp);
  void PrintSiteCount(ofstream & fp);
  void PrintBase(cString filename);
  void PrintPeak(cString filename);

  inline const cGenome & GetPeakGenome() { return peak_genome; }
  inline double GetAveFitness() { return total_fitness / total_count; }
  inline double GetAveSqrFitness() { return total_sqr_fitness / total_count; }
  inline double GetPeakFitness() { return peak_fitness; }

  inline double GetProbDead() const { return ((double)dead_count)/total_count;}
  inline double GetProbNeg()  const { return ((double)neg_count) /total_count;}
  inline double GetProbNeut() const { return ((double)neut_count)/total_count;}
  inline double GetProbPos()  const { return ((double)pos_count) /total_count;}

  inline int GetNumTrials() const { return trials; }
};

#endif
