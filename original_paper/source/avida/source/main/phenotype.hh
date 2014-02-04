//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef PHENOTYPE_HH
#define PHENOTYPE_HH

#include "../tools/tArray.hh"
#include "../tools/merit.hh"

#include "environment.hh"

class sCreatureInfo;

/*************************************************************************
 *
 * The cPhenotype object contains a general description of all the
 * phenotypic characteristics an organism has displayed.  That is, it
 * monitors all of the organisms behaviors.
 *
 * After a phenotype is created in organism and organism within a population,
 * it must have either SetupOffspring() or SetupInject() run on it to prime
 * it for that population.  After that, it must have DivideReset() run on it
 * whenever it produces an offspring.
 *
 * If the phenotype is part of an organism in a test cpu, no initial priming
 * is required, and SetupTestDivide() needs to be run once it finally has
 * produced an offspring in order to properly lock in any final values.
 *
 * In addition to a reference to the relevent environment, the data
 * contained within this class comes in six flavors:
 *   1. Calculations made at the previous divide.
 *   2. Stats which are accumulated over each gestation cycle.
 *   3. The final result of accumulations over the previous gestation cycle.
 *   4. Accumulations over the entire life of the genome.
 *   5. A collection of flags to indicate the presence of characteristics.
 *   6. Information about the child being constructed.
 *
 *************************************************************************/

class cPhenotype {
private:
  const cEnvironment & environment;
  bool initialized;

  // 1. These are values calculated at the last divide (of self or offspring)
  cMerit merit;          // Relative speed of CPU
  int genome_length;     // Number of instructions in genome.
  int copied_size;       // Instructions copied into genome.
  int executed_size;     // Instructions executed from genome.
  int gestation_time;    // CPU cycles to produce offspring (or be produced)
  int gestation_start;   // Total instructions executed at last divide.
  double fitness;        // Relative efective replication rate...

  // 2. These are "in progress" variables, updated as the organism operates
  double cur_bonus;               // Current Bonus
  int cur_num_errors;             // Total instructions executed illeagally.
  tArray<int> cur_task_count;     // Total times each task was performed
  tArray<int> cur_reaction_count; // Total times each reaction was triggered.

  // 3. These mark the status of "in progess" variables at the last divide.
  double last_merit_base;         // Either constant or based on genome length.
  double last_bonus;
  int last_num_errors;
  tArray<int> last_task_count;
  tArray<int> last_reaction_count;

  // 4. Records from this organisms life...
  int num_divides;       // Total successful divides organism has produced.
  int generation;        // Number of birth events to original ancestor.
  int time_used;         // Total CPU cycles consumed.
  int age;               // Number of updates organism has survived for.
  cString fault_desc;    // A description of the most recent error.
  double neutral_metric; // Undergoes drift (gausian 0,1) per generation

  // 5. Status Flags...  (updated at each divide)
  bool is_injected;      // Was this organism injected into the population?
  bool is_parasite;      // Has this organism ever executed outside code?
  bool is_modifier;      // Has this organism modified another?
  bool is_modified;      // Has this organism been modified by another?
  bool is_fertile;       // Do we allow this organisms to produce offspring?
  bool is_mutated;       // Has this organism been subject to any mutations?
  bool parent_true;      // Is this genome an exact copy of its parent's?

  // 6. Child information...
  bool copy_true;        // Can this genome produce an exact copy of itself?
  bool child_fertile;    // Will this organism's next child be fertile?
  int child_copied_size; // Instruction copied into child.

private:
  static int CalcSizeMerit(int full_size, int copied_size, int exe_size);

public:
  cPhenotype(const cEnvironment & environment);
  ~cPhenotype();

  bool OK();

  // Run when being setup *as* and offspring.
  void SetupOffspring(const cPhenotype & parent_phenotype, int _length);

  // Run when being setup as an injected organism.
  void SetupInject(int _length);

  // Run when this organism successfully executes a divide.
  void DivideReset(int _length);
  
  // Same as DivideReset(), but only run in test CPUs.
  void TestDivideReset(int _length);
  
  // Input and Output Reaction Tests
  bool TestInput(tBuffer<int> & inputs, tBuffer<int> & outputs);
  bool TestOutput(tBuffer<int> & inputs, tBuffer<int> & outputs,
		  const tArray<double> & res_in, tArray<double> & res_change);

  // State saving and loading, and printing...
  bool SaveState(ofstream & fp);
  bool LoadState(ifstream & fp);
  void PrintStatus(ostream & fp);


  /////////////////////  Accessors -- Retrieving  ////////////////////
  const cEnvironment & GetEnvironment() const { return environment; };

  const cMerit & GetMerit() const
    { assert(initialized == true); return merit; }
  int GetGenomeLength() const
    { assert(initialized == true); return genome_length; }
  int GetCopiedSize() const
    { assert(initialized == true); return copied_size; }
  int GetExecutedSize() const
    { assert(initialized == true); return executed_size; }
  int GetGestationTime() const
    { assert(initialized == true); return gestation_time; }
  int GetGestationStart() const
    { assert(initialized == true); return gestation_start; }
  double GetFitness() const
    { assert(initialized == true); return fitness; }

  double GetCurBonus() const
    { assert(initialized == true); return cur_bonus; }
  int GetCurNumErrors()const
    { assert(initialized == true); return cur_num_errors; }
  const tArray<int> & GetCurTaskCount() const
    { assert(initialized == true); return cur_task_count; }
  const tArray<int> & GetCurReactionCount() const
    { assert(initialized == true); return cur_reaction_count;}

  double GetLastMeritBase() const
    { assert(initialized == true); return last_merit_base; }
  double GetLastBonus() const
    { assert(initialized == true); return last_bonus; }
  int GetLastNumErrors() const
    { assert(initialized == true); return last_num_errors; }
  const tArray<int> & GetLastTaskCount() const
    { assert(initialized == true); return last_task_count; }
  const tArray<int> & GetLastReactionCount() const
    { assert(initialized == true); return last_reaction_count; }

  int GetNumDivides() const { assert(initialized == true); return num_divides;}
  int GetGeneration() const { assert(initialized == true); return generation; }
  int GetTimeUsed()   const { assert(initialized == true); return time_used; }
  int GetAge()        const { assert(initialized == true); return age; }
  const cString & GetFault() const
    { assert(initialized == true); return fault_desc; }
  double GetNeutralMetric() const
    { assert(initialized == true); return neutral_metric; }

  bool IsInjected() const { assert(initialized == true); return is_injected; }
  bool IsParasite() const { assert(initialized == true); return is_parasite; }
  bool IsModifier() const { assert(initialized == true); return is_modifier; }
  bool IsModified() const { assert(initialized == true); return is_modified; }
  bool IsFertile() const  { assert(initialized == true); return is_fertile; }
  bool IsMutated() const  { assert(initialized == true); return is_mutated; }
  bool ParentTrue() const { assert(initialized == true); return parent_true; }

  bool CopyTrue() const   { assert(initialized == true); return copy_true; }
  bool ChildFertile() const
    { assert(initialized == true); return child_fertile;}
  int GetChildCopiedSize() const
    { assert(initialized == true); return child_copied_size; }


  ////////////////////  Accessors -- Modifying  ///////////////////
  void SetMerit(const cMerit & in_merit) { merit = in_merit; }
  void SetGestationTime(int in_time) { gestation_time = in_time; }
  void SetFault(const cString & in_fault) { fault_desc = in_fault; }
  void SetNeutralMetric(double _in){ neutral_metric = _in; }
  void SetLinesExecuted(int _exe_size) { executed_size = _exe_size; }
  void SetLinesCopied(int _copied_size) { child_copied_size = _copied_size; }
  
  void IncAge()      { assert(initialized == true); age++; }
  void IncTimeUsed() { assert(initialized == true); time_used++; }
  void IncErrors()   { assert(initialized == true); cur_num_errors++; }

  bool & IsInjected() { assert(initialized == true); return is_injected; }
  bool & IsParasite() { assert(initialized == true); return is_parasite; }
  bool & IsModifier() { assert(initialized == true); return is_modifier; }
  bool & IsModified() { assert(initialized == true); return is_modified; }
  bool & IsFertile()  { assert(initialized == true); return is_fertile; }
  bool & IsMutated()  { assert(initialized == true); return is_mutated; }
  bool & ParentTrue() { assert(initialized == true); return parent_true; }
  bool & CopyTrue()   { assert(initialized == true); return copy_true; }
  bool & ChildFertile() { assert(initialized == true); return child_fertile; }
};

#endif
