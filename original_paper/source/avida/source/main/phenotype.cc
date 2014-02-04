//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#include "phenotype.hh"

#include "../tools/tools.hh"

#include "config.hh"

cPhenotype::cPhenotype(const cEnvironment & _environment)
  : environment(_environment)
  , initialized(false)
  , cur_task_count(environment.GetTaskLib().GetSize())
  , cur_reaction_count(environment.GetReactionLib().GetSize())
  , last_task_count(environment.GetTaskLib().GetSize())
  , last_reaction_count(environment.GetReactionLib().GetSize())
{
}

cPhenotype::~cPhenotype()
{
}

bool cPhenotype::OK()
{
  assert(genome_length >= 0);
  assert(copied_size >= 0);
  assert(executed_size >= 0);
  assert(gestation_time >= 0);
  assert(gestation_start >= 0);
  assert(fitness >= 0.0);
  assert(cur_bonus >= 0.0);
  assert(cur_num_errors >= 0);
  assert(last_merit_base >= 0.0);
  assert(last_bonus >= 0.0);
  assert(last_num_errors >= 0);
  assert(num_divides >= 0);
  assert(generation >= 0);
  assert(time_used >= 0);
  assert(age >= 0);
  assert(child_copied_size >= 0);

  return true;
}


/**
 * This function is run whenever a new organism is being constructed inside
 * of its parent.
 *
 * Assumptions:
 *     - parent_phenotype has had DivideReset run on it already!
 *     - this is the first method run on an otherwise freshly built phenotype.
 **/

void cPhenotype::SetupOffspring(const cPhenotype & parent_phenotype,
				int _length)
{
  // Copy divide values from parent, which should already be setup.
  merit           = parent_phenotype.merit;
  genome_length   = _length;
  copied_size     = parent_phenotype.child_copied_size;
  executed_size   = parent_phenotype.executed_size;
  gestation_time  = parent_phenotype.gestation_time;
  gestation_start = 0;
  fitness         = parent_phenotype.fitness;

  assert(genome_length > 0);
  assert(copied_size > 0);
  assert(executed_size > 0);
  assert(gestation_time > 0);

  // Initialize current values, as neeeded.
  cur_bonus       = 1;
  cur_num_errors  = 0;
  cur_task_count.SetAll(0);
  cur_reaction_count.SetAll(0);

  // Copy last values from parent
  last_merit_base     = parent_phenotype.last_merit_base;
  last_bonus          = parent_phenotype.last_bonus;
  last_num_errors     = parent_phenotype.last_num_errors;
  last_task_count     = parent_phenotype.last_task_count;
  last_reaction_count = parent_phenotype.last_reaction_count;
    
  // Setup other miscellaneous values...
  num_divides     = 0;
  generation      = parent_phenotype.generation;
  if (cConfig::GetGenerationIncMethod() != GENERATION_INC_BOTH) generation++;
  time_used       = 0;
  age             = 0;
  fault_desc      = "";
  neutral_metric  = parent_phenotype.neutral_metric + g_random.GetRandNormal();

  // Setup flags...
  is_injected   = false;
  is_parasite   = false;
  is_modifier   = false;
  is_modified   = false;
  is_fertile    = parent_phenotype.child_fertile;
  is_mutated    = false;
  parent_true   = parent_phenotype.copy_true;

  // Setup child info...
  copy_true         = false;
  child_fertile     = true;
  child_copied_size = 0;

  initialized = true;
}


/**
 * This function is run whenever a new organism is being constructed via
 * some form of injection into the population, or in a test environment.
 *
 * Assumptions:
 *     - Updates to these values (i.e. resetting of merit) will occur afterward
 *     - This is the first method run on an otherwise freshly built phenotype.
 **/

void cPhenotype::SetupInject(int _length)
{
  // Setup reasonable initial values injected organism...
  merit           = _length;
  genome_length   = _length;
  copied_size     = _length;
  executed_size   = _length;
  gestation_time  = 0;
  gestation_start = 0;
  fitness         = 0;

  // Initialize current values, as neeeded.
  cur_bonus       = 1;
  cur_num_errors  = 0;
  cur_task_count.SetAll(0);
  cur_reaction_count.SetAll(0);

  // Copy last values from parent
  last_merit_base = _length;
  last_bonus      = 1;
  last_num_errors = 0;
  last_task_count.SetAll(0);
  last_reaction_count.SetAll(0);
    
  // Setup other miscellaneous values...
  num_divides     = 0;
  generation      = 0;
  time_used       = 0;
  age             = 0;
  fault_desc      = "";
  neutral_metric  = 0;

  // Setup flags...
  is_injected   = true;
  is_parasite   = false;
  is_modifier   = false;
  is_modified   = false;
  is_fertile    = true;
  is_mutated    = false;
  parent_true   = true;

  // Setup child info...
  copy_true         = false;
  child_fertile     = true;
  child_copied_size = 0;

  initialized = true;
}


/**
 * This function is run whenever an organism executes a successful divide.
 **/

void cPhenotype::DivideReset(int _length)
{
  assert(time_used > 0);
  assert(initialized == true);

  // Update these values as needed...
  int cur_merit_base = CalcSizeMerit(genome_length,copied_size,executed_size);
  merit = cur_merit_base * cur_bonus;
  assert(merit.GetDouble() != 0);

  genome_length   = _length;
  (void) copied_size;          // Unchanged
  (void) executed_size;        // Unchanged
  gestation_time  = time_used - gestation_start;
  gestation_start = time_used;
  fitness         = merit.GetDouble() / gestation_time;

  // Lock in cur values as last values.
  last_merit_base     = cur_merit_base;
  last_bonus          = cur_bonus;
  last_num_errors     = cur_num_errors;
  last_task_count     = cur_task_count;
  last_reaction_count = cur_reaction_count;
    
  // Reset cur values.
  cur_bonus       = 1;
  cur_num_errors  = 0;
  cur_task_count.SetAll(0);
  cur_reaction_count.SetAll(0);

  // Setup other miscellaneous values...
  num_divides++;
  (void) generation;
  (void) time_used;
  age             = 0;
  fault_desc      = "";
  (void) neutral_metric;

  // Leave flags alone...
  (void) is_injected;
  (void) is_parasite;
  (void) is_modifier;
  (void) is_modified;
  (void) is_fertile;
  (void) is_mutated;
  (void) parent_true;

  // Reset child info...
  (void) copy_true;
  child_fertile     = true;
  (void) child_copied_size;;

  // A few final changes if the parent was supposed to be be considered
  // a second child on the divide.
  if (cConfig::GetDivideMethod() == DIVIDE_METHOD_SPLIT) {
    gestation_start = 0;
    time_used = 0;
    neutral_metric += g_random.GetRandNormal();
  }

  if (cConfig::GetGenerationIncMethod() == GENERATION_INC_BOTH) generation++;
}


/**
 * This function runs whenever a *test* CPU divides. It processes much of
 * the information for that CPU in order to actively reflect its executed
 * and copied size in its merit.
 **/

void cPhenotype::TestDivideReset(int _length)
{
  assert(time_used > 0);
  assert(initialized == true);

  // Update these values as needed...
  int cur_merit_base = CalcSizeMerit(genome_length,copied_size,executed_size);
  merit           = cur_merit_base * cur_bonus;
  genome_length   = _length;
  (void) copied_size;                            // Unchanged
  (void) executed_size;                          // Unchanged
  gestation_time  = time_used - gestation_start;
  gestation_start = time_used;
  fitness         = merit.GetDouble() / gestation_time;

  // Lock in cur values as last values.
  last_merit_base     = cur_merit_base;
  last_bonus          = cur_bonus;
  last_num_errors     = cur_num_errors;
  last_task_count     = cur_task_count;
  last_reaction_count = cur_reaction_count;
    
  // Reset cur values.
  cur_bonus       = 1;
  cur_num_errors  = 0;
  cur_task_count.SetAll(0);
  cur_reaction_count.SetAll(0);

  // Setup other miscellaneous values...
  num_divides++;
  generation++;
  (void) time_used;
  (void) age;
  (void) fault_desc;
  (void) neutral_metric;

  // Leave flags alone...
  (void) is_injected;
  (void) is_parasite;
  (void) is_modifier;
  (void) is_modified;
  (void) is_fertile;
  (void) is_mutated;
  (void) parent_true;

  // Reset child info...
  (void) copy_true;
  (void) child_fertile;
  (void) child_copied_size;
}



bool cPhenotype::TestInput(tBuffer<int> & inputs, tBuffer<int> & outputs)
{
  assert(initialized == true);
  // For the moment, lets not worry about inputs...
  return false; // Nothing happened...
}

bool cPhenotype::TestOutput(tBuffer<int> & inputs, tBuffer<int> & outputs,
		    const tArray<double> & res_in, tArray<double> & res_change)
{
  assert(initialized == true);

  const int num_resources = environment.GetResourceLib().GetSize();
  const int num_tasks = environment.GetTaskLib().GetSize();
  const int num_reactions = environment.GetReactionLib().GetSize();

  cReactionResult result(num_resources, num_tasks, num_reactions);
			 
  // Run everything through the environment.
  bool found = environment.TestOutput(result, inputs, outputs, cur_task_count,
				      cur_reaction_count, res_in);

  // If nothing was found, stop here.
  if (found == false) {
    res_change.SetAll(0.0);
    return false;  // Nothing happened.
  }

  // Update the phenotype with the results...
  // Start with updating task and reaction counters
  for (int i = 0; i < num_tasks; i++) {
    if (result.TaskDone(i) == true) cur_task_count[i]++;
  }
  for (int i = 0; i < num_reactions; i++) {
    if (result.ReactionTriggered(i) == true) cur_reaction_count[i]++;
  }

  // Update the merit bonus
  cur_bonus *= result.GetMultBonus();
  cur_bonus += result.GetAddBonus();

  // Denote consumed resources...
  for (int i = 0; i < res_in.GetSize(); i++) {
    res_change[i] = result.GetProduced(i) - result.GetConsumed(i);
  }

  return true;
}


///// For Loading and Saving State: /////


bool cPhenotype::SaveState(ofstream & fp)
{
  assert(fp.good());
  fp << "cPhenotype" << endl;

  fp << merit.GetDouble()   << " ";
  fp << genome_length       << " ";
  fp << copied_size         << " ";
  fp << executed_size       << " ";
  fp << gestation_time      << " ";
  fp << gestation_start     << " ";
  fp << fitness             << " ";

  fp << cur_bonus           << " ";
  fp << cur_num_errors      << " ";
  for (int i = 0; i < cur_task_count.GetSize(); i++) {
    fp << cur_task_count[i] << " ";
  }
  for (int i = 0; i < cur_reaction_count.GetSize(); i++) {
    fp << cur_reaction_count[i] << " ";
  }

  fp << last_merit_base     << " ";
  fp << last_bonus          << " ";
  fp << last_num_errors     << " ";
  for (int i = 0; i < last_task_count.GetSize(); i++) {
    fp << last_task_count[i] << " ";
  }
  for (int i = 0; i < last_reaction_count.GetSize(); i++) {
    fp << last_reaction_count[i] << " ";
  }

  fp << num_divides         << " ";
  fp << generation          << " ";
  fp << time_used           << " ";
  fp << age                 << " ";
  fp << neutral_metric      << " ";

  fp << is_injected         << " ";
  fp << is_parasite         << " ";
  fp << is_modifier         << " ";
  fp << is_modified         << " ";
  fp << is_fertile          << " ";
  fp << is_mutated          << " ";
  fp << parent_true         << " ";

  fp << copy_true           << " ";
  fp << child_fertile       << " ";

  fp << endl;
  return true;
}


bool cPhenotype::LoadState(ifstream & fp)
{
  double tmp_merit;
  assert(fp.good());
  if( !fp.good() ) return false;

  fp >> tmp_merit;  merit = tmp_merit;
  fp >> genome_length;
  fp >> copied_size;
  fp >> executed_size;
  fp >> gestation_time;
  fp >> gestation_start;
  fp >> fitness;

  fp >> cur_bonus;
  fp >> cur_num_errors;
  for (int i = 0; i < cur_task_count.GetSize(); i++) {
    fp >> cur_task_count[i];
  }
  for (int i = 0; i < cur_reaction_count.GetSize(); i++) {
    fp >> cur_reaction_count[i];
  }

  fp >> last_merit_base;
  fp >> last_bonus;
  fp >> last_num_errors;
  for (int i = 0; i < last_task_count.GetSize(); i++) {
    fp >> last_task_count[i];
  }
  for (int i = 0; i < last_reaction_count.GetSize(); i++) {
    fp >> last_reaction_count[i];
  }

  fp >> num_divides;
  fp >> generation;
  fp >> time_used;
  fp >> age;
  fp >> neutral_metric;

  fp >> is_injected;
  fp >> is_parasite;
  fp >> is_modifier;
  fp >> is_modified;
  fp >> is_fertile;
  fp >> is_mutated;
  fp >> parent_true;

  fp >> copy_true;
  fp >> child_fertile;

  return true;
}

void cPhenotype::PrintStatus(ostream & fp)
{
  fp << "  MeritBase:"
     << CalcSizeMerit(genome_length,copied_size,executed_size)
     << " Bonus: " << cur_bonus
     << " Errors:" << cur_num_errors
     << " Tasks:";

  for (int i = 0; i < cur_task_count.GetSize(); i++) {
    fp << " " << cur_task_count[i];
  }

  fp << endl;
}

int cPhenotype::CalcSizeMerit(int full_size, int copied_size, int exe_size)
{
  assert(full_size > 0);
  assert(exe_size > 0);
  assert(copied_size > 0);

  int out_size = cConfig::GetBaseSizeMerit();

  switch (cConfig::GetSizeMeritMethod()) {
  case SIZE_MERIT_COPIED:
    out_size = copied_size;
    break;
  case SIZE_MERIT_EXECUTED:
    out_size = exe_size;
    break;
  case SIZE_MERIT_FULL:
    out_size = full_size;
    break;
  case SIZE_MERIT_LEAST:
    out_size = full_size;
    if (out_size > copied_size) out_size = copied_size;
    if (out_size > exe_size)    out_size = exe_size;
    break;
  case SIZE_MERIT_SQRT_LEAST:
    out_size = full_size;
    if (out_size > copied_size) out_size = copied_size;
    if (out_size > exe_size)    out_size = exe_size;
    out_size = (int) sqrt((double) out_size);
    break;
  case SIZE_MERIT_OFF:
  default:
    out_size = 100;
    break;
  }

  return out_size;
}
