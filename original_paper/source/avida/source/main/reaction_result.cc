//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#include "reaction_result.hh"

cReactionResult::cReactionResult(const int num_resources,
				 const int num_tasks,
				 const int num_reactions)
  : resources_consumed(num_resources)
  , resources_produced(num_resources)
  , tasks_done(num_tasks)
  , reactions_triggered(num_reactions)
  , bonus_add(0.0)
  , bonus_mult(1.0)
  , active_reaction(false)
{
}

//  cReactionResult::cReactionResult(const cReactionResult & _in)
//    : resources_consumed(_in.resources_consumed)
//    , resources_produced(_in.resources_produced)
//    , tasks_done(_in.tasks_done)
//    , reactions_triggered(_in.reactions_triggered)
//    , bonus_add(_in.bonus_add)
//    , bonus_mult(_in.bonus_mult)
//    , active_reaction(_in.active_reaction)
//  {
//  }

cReactionResult::~cReactionResult()
{
}


void cReactionResult::ActivateReaction()
{
  // If this reaction is already active, don't worry about it.
  if (active_reaction == true) return;

  // To activate the reaction, we must initialize all counter settings.
  resources_consumed.SetAll(0);
  resources_produced.SetAll(0);
  tasks_done.SetAll(false);
  reactions_triggered.SetAll(false);

  // And finally note that this is indeed already active.
  active_reaction = true;
}


void cReactionResult::Consume(int id, double num)
{
  ActivateReaction();
  resources_consumed[id] += num;
}


void cReactionResult::Produce(int id, double num)
{
  ActivateReaction();
  resources_produced[id] += num;
}


void cReactionResult::MarkTask(int id)
{
  ActivateReaction();
  tasks_done[id] = true;
}


void cReactionResult::MarkReaction(int id)
{
  ActivateReaction();
  reactions_triggered[id] = true;
}


void cReactionResult::AddBonus(double value)
{
  ActivateReaction();
  bonus_add += value;
}


void cReactionResult::MultBonus(double value)
{
  ActivateReaction();
  bonus_mult *= value;
}


double cReactionResult::GetConsumed(int id)
{
  if (GetActive() == false) return 0.0;
  return resources_consumed[id];
}


double cReactionResult::GetProduced(int id)
{
  if (GetActive() == false) return 0.0;
  return resources_produced[id];
}

bool cReactionResult::ReactionTriggered(int id)
{
  if (GetActive() == false) return false;
  return reactions_triggered[id];
}

bool cReactionResult::TaskDone(int id)
{
  if (GetActive() == false) return false;
  return tasks_done[id];
}

double cReactionResult::GetAddBonus()
{
  if (GetActive() == false) return 0.0;
  return bonus_add;
}

double cReactionResult::GetMultBonus()
{
  if (GetActive() == false) return 1.0;
  return bonus_mult;
}

