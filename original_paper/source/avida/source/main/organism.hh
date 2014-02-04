//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef ORGANISM_HH
#define ORGANISM_HH

#include <fstream.h>

#include "../tools/merit.hh"
#include "../tools/tBuffer.hh"

#include "../defs.hh"

#include "genome.hh"
#include "mutations.hh"
#include "pop_interface.hh"
#include "phenotype.hh"

#include "../cpu/cpu_memory.hh"
#include "../cpu/cpu_stats.hh"

class cEnvironment;
class cGenotype;
class cHardwareBase;
class cLineage;
class cPopulation;
class cBirthChamber;

/**
 * The cOrganism class controls the running and manages all the statistics
 * about a creature.  Effectively the chemistry acting on the genome.
 **/

class cOrganism {
protected:
  cHardwareBase * hardware;  // The actual machinary running this organism.
  cGenotype * genotype;      // Information about organisms with this genome.
  cPhenotype phenotype;      // Descriptive attributes of organism.
  const cGenome initial_genome;        // Initial genome; can never be changed!

  cMutationRates mut_rates;            // Rate of all possible mutations.
  cPopulationInterface pop_interface;  // Interface back to the population.

  cCPUMemory child_genome; // Child genome, while under construction.
  sCPUStats cpu_stats;     // Info for statistics

  int max_executed;      // Max number of instruction executed before death.

  int lineage_label;     // a lineages tag; inherited unchanged in offspring
  cLineage * lineage;    // A lineage descriptor... (different from label)

#ifdef DEBUG
  bool initialized;      // Has this CPU been initialized yet, w/hardware.
#endif
  static int instance_count;

public:
  void PrintStatus(ostream & fp);

  // Divide functions
  bool Divide_CheckViable();
  cOrganism * Divide_SetupChild();
  void ActivateDivide(cOrganism * child);

  // Other Special Functions
  void Fault(int fault_loc, int fault_type, cString fault_desc="");

public:
  cOrganism(const cGenome & in_genome,
	    const cPopulationInterface & in_interface,
	    const cEnvironment & in_environment);
  ~cOrganism();

  cHardwareBase & GetHardware() { return *hardware; }
  cOrganism * GetNeighbor() { return pop_interface.GetNeighbor(); }
  cBirthChamber & GetBirthChamber() { return pop_interface.GetBirthChamber(); }
  int GetNeighborhoodSize() { return pop_interface.GetNumNeighbors(); }
  void Rotate(int direction) { pop_interface.Rotate(direction); }
  void DoBreakpoint() { pop_interface.Breakpoint(); }
  int GetInput() { return pop_interface.GetInput(); }
  void Die() { pop_interface.Die(); }
  int GetCellID() { return pop_interface.GetCellID(); }

  // Input & Output Testing
  void DoInput(const int value, tBuffer<int> & in_buf, tBuffer<int> & out_buf);
  void DoOutput(const int value, tBuffer<int> & in_buf, tBuffer<int> &out_buf);

  int OK();

  double GetTestFitness();
  double CalcMeritRatio();

  cCPUMemory & ChildGenome() { return child_genome; }
  sCPUStats & CPUStats() { return cpu_stats; }


  bool TestCopyMut() const { return MutationRates().TestCopyMut(); }
  bool TestDivideMut() const { return MutationRates().TestDivideMut(); }
  bool TestDivideIns() const { return MutationRates().TestDivideIns(); }
  bool TestDivideDel() const { return MutationRates().TestDivideDel(); }
  bool TestParentMut() const { return MutationRates().TestParentMut(); }
  bool TestCrossover() const { return MutationRates().TestCrossover(); }
  bool TestAlignedCrossover() const
    { return MutationRates().TestAlignedCrossover(); }
  
  double GetCopyMutProb() const { return MutationRates().GetCopyMutProb(); }
  void SetCopyMutProb(double _p) { return MutationRates().SetCopyMutProb(_p); }

  double GetInsMutProb() const { return MutationRates().GetInsMutProb(); }
  double GetDelMutProb() const { return MutationRates().GetDelMutProb(); }
  double GetParentMutProb() const { return MutationRates().GetParentMutProb();}


  bool GetTestOnDivide() const;
  bool GetFailImplicit() const;

  bool GetRevertFatal() const;
  bool GetRevertNeg() const;
  bool GetRevertNeut() const;
  bool GetRevertPos() const;

  bool GetSterilizeFatal() const;
  bool GetSterilizeNeg() const;
  bool GetSterilizeNeut() const;
  bool GetSterilizePos() const;


  // Access to private variables
  int GetMaxExecuted() const { return max_executed; }

  void SetLineageLabel( int in_label ) { lineage_label = in_label; }
  int GetLineageLabel() const { return lineage_label; }
  
  void SetLineage( cLineage * in_lineage ) { lineage = in_lineage; }
  cLineage * GetLineage() const { return lineage; }

  void SetGenotype(cGenotype * in_genotype) { genotype = in_genotype; }
  cGenotype * GetGenotype() const { return genotype; }

  const cMutationRates & MutationRates() const { return mut_rates; }
  cMutationRates & MutationRates() { return mut_rates; }
  const cPopulationInterface & PopInterface() const { return pop_interface; }
  cPopulationInterface & PopInterface() { return pop_interface; }
  const cGenome & GetGenome() const { return initial_genome; }

  int GetCurGestation() const;
  const cPhenotype & GetPhenotype() const { return phenotype; }
  cPhenotype & GetPhenotype() { return phenotype; }

  void SaveState(ofstream & fp);
  void LoadState(ifstream & fp);

  // --------  DEBUG ---------
  static int GetInstanceCount() { return instance_count; }
};

#endif
