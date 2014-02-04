//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef LINEAGE_HH
#define LINEAGE_HH

#include <assert.h>
#include "../defs.hh"

#include <map>

class cGenotype;

// struct to compare to genotypes
struct gt_gentype
{
  bool operator()(const cGenotype * g1, const cGenotype * g2) const;
};

class cLineage {
private:
  static int m_max_id;
  int m_id;
  int m_parent_id;
  int m_update_born;
  int m_num_CPUs;
  int m_total_CPUs;
  int m_total_genotypes;
  double m_start_fitness;
  mutable double m_max_fitness;
  mutable double m_ave_fitness;
  double m_generation_born;

  map<const cGenotype *, int, gt_gentype> m_genotype_map;
  bool m_threshold;
  mutable bool m_ave_fitness_changed;
  mutable double m_max_fitness_ever;
  double m_lineage_stat1;
  double m_lineage_stat2;

  void CalcCurrentFitness() const;
public:
  /**
   * Creates a new lineage with a given start fitness and parent id.
   * The first genotype has to be added with AddGenotype later.
   *
   * @param start_fitness The initial fitness of the lineage.
   * @param parent_id The id of the parent's lineage.
   * @param id The requested id for this lineage. If negative, a generated
   * value is used. Attention: if you request lineage id's, it is your
   * responsibility not to request two identical ones for different lineages
   * (the class @ref cLineageControl does that correctly).
   **/
  cLineage(double start_fitness, int parent_id, int id,
	   int update, double generation,
	   double lineage_stat1 = 0.0, double lineage_stat2 = 0.0);
  ~cLineage();

  /**
   * Adds one instance of the given genotype to the lineage.
   **/
  void AddCreature( cGenotype * genotype );

  /**
   * Removes on instance of the given genotype from the lineage.
   *
   * @return True if the removal of this creature potentially creates a new
   * best lineage, otherwise false.
   **/
  bool RemoveCreature( cGenotype * genotype );
  void SetThreshold() { m_threshold = true; }

  /**
   * @return The id of the lineage.
   **/
  int GetID() const { return m_id; }

  /**
   * @return The id of the parent lineage.
   **/
  int GetParentID() const { return m_parent_id; }

  /**
   * @return An integer that is one larger than the maximum id used so far.
   **/
  int GetMaxID() const { return m_max_id; }

  /**
   * @return The update at which the lineage was created.
   **/
  int GetUpdateBorn() const { return m_update_born; }

  /**
   * @return The generation at which the lineage was created.
   **/
  double GetGenerationBorn() const { return m_generation_born; }

  /**
   * @return The initial fitness of the lineage.
   **/
  double GetStartFitness() const { return m_start_fitness; }

  /**
   * @return The current maximum fitness of the lineage.
   **/
  double GetMaxFitness() const { return m_max_fitness; }
  
  /**
   * @return The average fitness of the lineage.
   **/
  double GetAveFitness() const {
    if ( m_ave_fitness_changed )
      CalcCurrentFitness();
    return m_ave_fitness; }

  /**
   * @return The current number of genotypes in the lineage.
   **/
  int GetNumGenotypes() const { return m_genotype_map.size(); }

  /**
   * @return The current number of creatures in the lineage.
   **/
  int GetNumCreatures() const { return m_num_CPUs; }

  /**
   * Counts the number of creatures in the lineage.
   *
   * Should always equal GetNumCreatures(), otherwise something
   * is wrong.
   **/
  int CountNumCreatures() const;

  /**
   * @return The total number of creatures that have ever existed
   * in this lineage.
   **/
  int GetTotalCreatures() const { return m_total_CPUs; }

  /**
   * @return The total number of genotypes that have ever been part
   * of this lineage.
   **/
  int GetTotalGenotypes() const { return m_total_genotypes; }

  bool GetThreshold() const { return m_threshold; }
    
  /**
   * @return The maximum fitness ever attained by this lineage
   **/
  double GetMaxFitnessEver() const { return m_max_fitness_ever; }

  /**
   * @return The value of the first fitness criteria for
   * the creation of this lineage
   **/
  double GetLineageStat1 () const { return m_lineage_stat1; }

  /**
   * @return The value of the second fitness criteria for
   * the creation of this lineage
   **/
  double GetLineageStat2 () const { return m_lineage_stat2; }

};

#endif
