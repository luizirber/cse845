//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef POPULATION_HH
#define POPULATION_HH

#include "../tools/int.hh"
#include "../tools/file.hh"
#include "../tools/merit.hh"
#include "../tools/tList.hh"
#include "../tools/tArray.hh"

#include "../defs.hh"

#include "birth_chamber.hh"
#include "inst_lib.hh"
#include "pop_interface.hh"
#include "resource_count.hh"
#include "stats.hh"

class cEnvironment;
class cGenebank;
class cGenome;
class cGenotype;
class cLineage;
class cLineageControl;
class cOrganism;
class cPopulationCell;
class sReproData;
class cSchedule;


class cPopulation {
private:
  // Components...
  cSchedule * schedule;                // Handles allocation of CPU cycles
  tArray<cPopulationCell> cell_array;  // Local cells composing the population
  cResourceCount resource_count;       // Global resources available
  cBirthChamber birth_chamber;         // Global birth chamber.

  // Data Tracking...
  cStats stats;                      // Main statistics object...
  cGenebank * genebank;                // Tracks genotypes
  cLineageControl * lineage_control;   // Tracks Linages
  tList<cPopulationCell> reaper_queue; // Death order in some mass-action runs

  // Default organism setups...
  cEnvironment & environment;          // Physics & Chemestry description
  cPopulationInterface default_interface;  // Organism interface to population

  // Other data...
  int world_x;                         // Structured population width.
  int world_y;                         // Structured population
  int num_organisms;                   // Cell count with living organisms

  // Outside interactions...
  bool sync_events;   // Do we need to sync up the event list with population?

  ///////////////// Private Methods ////////////////////
  void BuildTimeSlicer(); // Build the schedule object

  // Methods to place offspring in the population.
  cPopulationCell & PositionChild(cPopulationCell & parent_cell);
  void PositionAge(cPopulationCell & parent_cell,
		   tList<cPopulationCell> & found_list);
  void PositionMerit(cPopulationCell & parent_cell,
		     tList<cPopulationCell> & found_list);
  void FindEmptyCell(tList<cPopulationCell> & cell_list,
		     tList<cPopulationCell> & found_list);

  // Update statistics collecting...
  void UpdateOrganismStats();
  void UpdateGenotypeStats();
  void UpdateSpeciesStats();
  void UpdateDominantStats();

  /**
   * Attention: InjectGenotype does *not* add the genotype to the genebank.
   * You have to do that yourself!
   **/ 
  void InjectGenotype(int pos, cGenotype *genotype);
  void InjectGenome(int pos, const cGenome & genome);

  void LineageSetupOrganism(cOrganism * organism, cLineage * lineage,
			    int lin_label, cGenotype * parent_genotype=NULL);

  // Must be called to activate *any* organism in the population.
  void ActivateOrganism(cOrganism * in_organism, cPopulationCell &target_cell);

public:
  cPopulation(const cPopulationInterface & in_interface,
	      cEnvironment & in_environment);
  ~cPopulation();

  // Activate the offspring of an organism in the population.
  void ActivateOffspring(cOrganism * in_organism, cOrganism & parent_organism);

  // Inject and organism from the outside world.
  void Inject(const cGenome & genome, int cell_id=-1, double merit=-1,
	      int lineage_label=0, double neutral_metric=0 );

  // Deactivate an organism in the population (required for deactivations)
  void KillOrganism(cPopulationCell & in_cell);

  // Process a single organism one instruction...
  int ScheduleOrganism();          // Determine next organism to be processed.
  void ProcessStep(double step_size);
  void ProcessStep(double step_size, int cell_id);

  // Calculate the statistics from the most recent update.
  void CalcUpdateStats();

  // Clear all but a subset of cells...
  void SerialTransfer( int transfer_size, bool ignore_deads );

  // Saving and loading...
  bool SaveClone(ofstream & fp);
  bool LoadClone(ifstream & fp);
  bool LoadDumpFile(cString filename, int update);
  bool SavePopulation(ofstream & fp);
  bool LoadPopulation(ifstream & fp);
  bool DumpMemorySummary(ofstream & fp);

  bool OK();

  int GetSize() { return cell_array.GetSize(); }
  int GetWorldX() { return world_x; }
  int GetWorldY() { return world_y; }
  int GetUpdate() { return stats.GetUpdate(); }
  double GetGeneration() { return stats.SumGeneration().Average(); }

  cPopulationCell & GetCell(int in_num);
  const tArray<double> & GetResources() const
    { return resource_count.GetResources(); }
  cBirthChamber & GetBirthChamber(int id) { return birth_chamber; }

  void UpdateResources(const tArray<double> & res_change);
  void UpdateResource(int id, double change);

  cStats & GetStats() { return stats; }
  cGenebank & GetGenebank() { return *genebank; }
  cLineageControl * GetLineageControl() { return lineage_control; }
  cEnvironment & GetEnvironment() { return environment; }
  int GetNumOrganisms() { return num_organisms; }

  bool GetSyncEvents() { return sync_events; }
  void SetSyncEvents(bool _in) { sync_events = _in; }
};

#endif
