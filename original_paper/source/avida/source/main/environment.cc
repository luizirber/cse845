//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#include "environment.hh"

#include "../tools/string_util.hh"

cEnvironment::cEnvironment()
{
}

cEnvironment::cEnvironment(const cString & filename)
{
  if (Load(filename) == false) {
    cerr << "Unable to load environment... aborting!" << endl;
    abort();
  }
}

bool cEnvironment::ParseSetting(cString entry, cString & var_name,
				cString & var_value, const cString & var_type)
{
  // Make sure we have an actual entry to parse.
  if (entry.GetSize() == 0) {
    cerr << "Error: Empty setting to parse in " << var_type << endl;
    return false;
  }

  // Collect the values...
  var_name = entry.Pop('=');
  var_value = entry;

  // Make sure we have both a name and a value...
  if (var_name.GetSize() == 0) {
    cerr << "Error: No variable povided to set to '" << var_value
	 << "' in " << var_type << endl;
    return false;
  }

  if (var_value.GetSize() == 0) {
    cerr << "Error: No value given for '" << var_name
	 << "' in " << var_type << endl;
    return false;
  }

  // Make the names case insensitive.
  var_name.ToLower();

  return true;
}

bool cEnvironment::AssertInputInt(const cString & input,
				  const cString & name,
				  const cString & type)
{
  if (input.IsNumeric() == false) {
    cerr << "Error: In " << type << "," << name << " set to non-integer."
	 << endl;
    return false;
  }
  return true;
}

bool cEnvironment::AssertInputDouble(const cString & input,
				     const cString & name,
				     const cString & type)
{
  if (input.IsNumber() == false) {
    cerr << "Error: In " << type << "," << name << " set to non-number."
	 << endl;
    return false;
  }
  return true;
}

bool cEnvironment::AssertInputValid(void * input,
				    const cString & name,
				    const cString & type,
				    const cString & value)
{
  if (input == NULL) {
    cerr << "Error: In " << type << ", '" << name << "' setting of '"
	 << value << "' not found." << endl;
    return false;
  }
  return true;
}



bool cEnvironment::LoadReactionProcess(cReaction * reaction, cString desc)
{
  //  cerr << "ENV: Loading reaction process '" << desc << "'." << endl;

  cReactionProcess * new_process = reaction->AddProcess();

  // Loop through all entries in description.
  while (desc.GetSize() > 0) {
    // Entries are divided by colons.
    cString var_entry = desc.Pop(':');
    cString var_name;
    cString var_value;
    const cString var_type =
      cStringUtil::Stringf("reaction '%s' process", reaction->GetName()());

    // Parse this entry.
    if (!ParseSetting(var_entry, var_name, var_value, var_type)) return false;

    // Now that we know we have a variable name and its value, set it!
    if (var_name == "resource") {
      cResource * test_resource = resource_lib.GetResource(var_value);
      if (!AssertInputValid(test_resource, "resource", var_type, var_value)) {
	return false;
      }
      new_process->SetResource(test_resource);
    }
    else if (var_name == "value") {
      if (!AssertInputDouble(var_value, "value", var_type)) return false;
      new_process->SetValue(var_value.AsDouble());
    }
    else if (var_name == "type") {
      if (var_value=="add") new_process->SetType(REACTION_PROCTYPE_ADD);
      else if (var_value=="mult") new_process->SetType(REACTION_PROCTYPE_MULT);
      else if (var_value=="pow") new_process->SetType(REACTION_PROCTYPE_POW);
      else {
	cerr << "Unknown reaction process type '" << var_value
	     << "' found in '" << reaction->GetName() << "'." << endl;
	return false;
      }
    }
    else if (var_name == "max") {
      if (!AssertInputDouble(var_value, "max", var_type)) return false;
      new_process->SetMaxNumber(var_value.AsDouble());
    }
    else if (var_name == "min") {
      if (!AssertInputDouble(var_value, "min", var_type)) return false;
      new_process->SetMinNumber(var_value.AsDouble());
    }
    else if (var_name == "frac") {
      if (!AssertInputDouble(var_value, "frac", var_type)) return false;
      double in_frac = var_value.AsDouble();
      if (in_frac > 1.0) in_frac = 1.0;
      new_process->SetMaxFraction(in_frac);
    }
    else if (var_name == "product") {
      cResource * test_resource = resource_lib.GetResource(var_value);
      if (!AssertInputValid(test_resource, "product", var_type, var_value)) {
	return false;
      }
      new_process->SetProduct(test_resource);
    }
    else if (var_name == "conversion") {
      if (!AssertInputDouble(var_value, "conversion", var_type)) return false;
      new_process->SetConversion(var_value.AsDouble());
    }
    else {
      cerr << "Error: Unknown process variable '" << var_name
	   << "' in reaction '" << reaction->GetName() << "'" << endl;
      return false;
    }
  }

  return true;
}

bool cEnvironment::LoadReactionRequisite(cReaction * reaction, cString desc)
{
  // cerr << "ENV: Loading reaction requisite '" << desc << "'." << endl;

  cReactionRequisite * new_requisite = reaction->AddRequisite();

  // Loop through all entries in description.
  while (desc.GetSize() > 0) {
    // Entries are divided by colons.
    cString var_entry = desc.Pop(':');
    cString var_name;
    cString var_value;
    const cString var_type =
      cStringUtil::Stringf("reaction '%s' requisite", reaction->GetName()());

    // Parse this entry.
    if (!ParseSetting(var_entry, var_name, var_value, var_type)) return false;

    // Now that we know we have a variable name and its value, set it!
    if (var_name == "reaction") {
      cReaction * test_reaction = reaction_lib.GetReaction(var_value);
      if (!AssertInputValid(test_reaction, "reaction", var_type, var_value)) {
	return false;
      }
      new_requisite->AddReaction(test_reaction);
    }
    else if (var_name == "noreaction") {
      cReaction * test_reaction = reaction_lib.GetReaction(var_value);
      if (!AssertInputValid(test_reaction,"noreaction",var_type, var_value)) {
	return false;
      }
      new_requisite->AddNoReaction(test_reaction);
    }
    else if (var_name == "min_count") {
      if (!AssertInputInt(var_value, "min_count", var_type)) return false;
      new_requisite->SetMinTaskCount(var_value.AsInt());
    }
    else if (var_name == "max_count") {
      if (!AssertInputInt(var_value, "max_count", var_type)) return false;
      new_requisite->SetMaxTaskCount(var_value.AsInt());
    }
    else {
      cerr << "Error: Unknown requisite variable '" << var_name
	   << "' in reaction '" << reaction->GetName() << "'" << endl;
      return false;
    }
  }

  return true;
}


bool cEnvironment::LoadResource(cString desc)
{
  cerr << "ENV: Loading resource '" << desc << "'." << endl;

  if (desc.GetSize() == 0) {
    cerr << "Warning: Resource line with no resources listed." << endl;
    return false;
  }

  while (desc.GetSize() > 0) {
    cString cur_resource = desc.PopWord();
    const cString name = cur_resource.Pop(':');
    cResource * new_resource = resource_lib.AddResource(name);

    while (cur_resource.GetSize() != 0) {
      cString var_entry = cur_resource.Pop(':');
      cString var_name;
      cString var_value;
      const cString var_type = cStringUtil::Stringf("resource '%s'", name());

      // Parse this entry.
      if (!ParseSetting(var_entry, var_name, var_value, var_type)) {
	return false;
      }

      if (var_name == "inflow") {
	if (!AssertInputDouble(var_value, "inflow", var_type)) return false;
	new_resource->SetInflow( var_value.AsDouble() );
      }
      else if (var_name == "outflow") {
	if (!AssertInputDouble(var_value, "outflow", var_type)) return false;
	new_resource->SetOutflow( var_value.AsDouble() );
      }
      else if (var_name == "initial") {
	if (!AssertInputDouble(var_value, "initial", var_type)) return false;
	new_resource->SetInitial( var_value.AsDouble() );
      }
      else {
	cerr << "Error: Unknown variable '" << var_name
	     << "' in resource '" << name << "'" << endl;
	return false;
      }
    }
  }

  return true;
}

bool cEnvironment::LoadReaction(cString desc)
{
  // Make sure this reaction has a description...
  if (desc.GetSize() == 0) {
    cerr << "Error: Each reaction must include a name and trigger." << endl;
    return false;
  }

  // Load in the reaction name
  const cString name = desc.PopWord();
  cReaction * new_reaction = reaction_lib.AddReaction(name);

  // If only a name was present, assume this reaction is a pre-declaration.
  if (desc.GetSize() == 0) {
    // cerr << "ENV: Pre-declaring reaction '" << name << "'." << endl;
    return true;
  }

  // Make sure this reaction hasn't already been loaded with a different
  // definition.
  if (new_reaction->GetTask() != NULL) {
    cerr << "Error: Re-defining reaction '" << name << "'." << endl;
    return false;
  }

  // Finish loading in this reaction.
  const cString trigger = desc.PopWord();
  cerr << "ENV: Loading reaction '" << name << "' with trigger '"
       << trigger << "'." << endl;

  // Load the task trigger
  cTaskEntry * cur_task = task_lib.AddTask(trigger);
  if (cur_task == NULL) {
    cerr << "...failed to find task in cTaskLib..." << endl;
    return false;
  }
  new_reaction->SetTask(cur_task);      // Attack task to reaction.

  while (desc.GetSize()) {
    cString desc_entry = desc.PopWord();      // Get the next argument
    cString entry_type = desc_entry.Pop(':'); // Determine argument type
    entry_type.ToLower();                     // Make case insensitive.

    // Determine the type of each argument and process it.
    if (entry_type == "process") {
      if (LoadReactionProcess(new_reaction, desc_entry) == false) {
	cerr << "...failed in loading reaction-process..." << endl;
	return false;
      }
    }
    else if (entry_type == "requisite") {
      if (LoadReactionRequisite(new_reaction, desc_entry) == false) {
	cerr << "...failed in loading reaction-requisite..." << endl;
	return false;
      }
    }
    else {
      cerr << "Unknown entry type '" << entry_type
  	   << "' in reaction '" << name << "'"
  	   << endl;
      return false;
    }
  }

  return true;
}

bool cEnvironment::Load(const cString & filename)
{
  cerr << "ENV: Loading file '" << filename << "'." << endl;

  cInitFile infile(filename);
  if (infile.Good() == false) {
    cerr << "Error: Failed to load environment '" << filename << "'." << endl;
    return false;
  }

  infile.Load();
  infile.Close();
  infile.Compress();

  // cerr << "ENV: found " << infile.GetNumLines() << " commands." << endl;

  while( infile.GetNumLines() ){
    cString line = infile.RemoveLine(); // Load next line from file.
    cString type = line.PopWord();      // Determine type of this entry.
    type.ToLower();                     // Make type case insensitive.
    if (type == "resource") {
      if (LoadResource(line) == false) {
	cerr << "...failed in loading resource..." << endl;
	return false;
      }
    }
    else if (type == "reaction") {
      if (LoadReaction(line) == false) {
	cerr << "...failed in loading reaction..." << endl;
	return false;
      }
    }
    else {
      cerr << "Error: Unknown environment keyword '" << type << "." << endl;
      return false;
    }
  }

  // Make sure that all pre-declared reactions have been loaded correctly.
  for (int i = 0; i < reaction_lib.GetSize(); i++) {
    if (reaction_lib.GetReaction(i)->GetTask() == NULL) {
      cerr << "Error: Pre-declared reaction '"
	   << reaction_lib.GetReaction(i)->GetName()
	   << "' never defined." << endl;
      return false;
    }
  }

  return true;
}



bool cEnvironment::TestInput( cReactionResult & result,
			      const tBuffer<int> & inputs,
			      const tBuffer<int> & outputs,
			      const tArray<double> & resource_count ) const
{
  // @CAO nothing for the moment...
  return false;
}


bool cEnvironment::TestOutput( cReactionResult & result,
			       const tBuffer<int> & inputs,
			       const tBuffer<int> & outputs,
			       const tArray<int> & task_count,
			       const tArray<int> & reaction_count,
			       const tArray<double> & resource_count ) const
{
  // Do setup for reaction tests...
  task_lib.SetupTests(inputs, outputs);

  // Loop through all reactions to see if any have been triggered...
  const int num_reactions = reaction_lib.GetSize();
  for (int i = 0; i < num_reactions; i++) {
    cReaction * cur_reaction = reaction_lib.GetReaction(i);
    assert(cur_reaction != NULL);

    // Examine the task trigger associated with this reaction
    cTaskEntry * cur_task = cur_reaction->GetTask();
    assert(cur_task != NULL);
    const double task_quality = task_lib.TestOutput(*cur_task);
    const int task_id = cur_task->GetID();

    // If this task wasn't performed, move on to the next one.
    if (task_quality == 0.0) continue;

    // Mark this task as performed...
    result.MarkTask(task_id);

    // Examine requisites on this reaction
    if (TestRequisites(cur_reaction->GetRequisites(), task_count[task_id],
		       reaction_count) == false) {
      continue;
    }

    // And lets process it!
    DoProcesses(cur_reaction->GetProcesses(), resource_count,
		task_quality, result);

    // Mark this reaction as occuring...
    result.MarkReaction(cur_reaction->GetID());
  }

  return result.GetActive();
}


bool cEnvironment::TestRequisites(const tList<cReactionRequisite> & req_list,
		  int task_count, const tArray<int> & reaction_count) const
{
  const int num_reqs = req_list.GetSize();

  // If there are no requisites, there is nothing to meet!
  if (num_reqs == 0) return true;

  tConstListIterator<cReactionRequisite> req_it(req_list);
  for (int i = 0; i < num_reqs; i++) {
    // See if this requisite batch can be satisfied.
    const cReactionRequisite * cur_req = req_it.Next();
    bool satisfied = true;

    // Have all reactions been met?
    tConstListIterator<cReaction> reaction_it(cur_req->GetReactions());
    while (reaction_it.Next() != NULL) {
      int react_id = reaction_it.Get()->GetID();
      if (reaction_count[react_id] == 0) {
	satisfied = false;
	break;
      }
    }
    if (satisfied == false) continue;

    // Have all no-reactions been met?
    tConstListIterator<cReaction> noreaction_it(cur_req->GetNoReactions());
    while (noreaction_it.Next() != NULL) {
      int react_id = noreaction_it.Get()->GetID();
      if (reaction_count[react_id] != 0) {
	satisfied = false;
	break;
      }
    }
    if (satisfied == false) continue;

    // Have all task counts been met?
    if (task_count < cur_req->GetMinTaskCount()) continue;

    // Have all reactions been met?
    if (task_count >= cur_req->GetMaxTaskCount()) continue;

    return true;
  }

  return false;
}


void cEnvironment::DoProcesses(const tList<cReactionProcess> & process_list,
			       const tArray<double> & resource_count,
			       const double task_quality,
			       cReactionResult & result) const
{
  const int num_process = process_list.GetSize();

  tConstListIterator<cReactionProcess> process_it(process_list);
  for (int i = 0; i < num_process; i++) {
    // See if this requisite batch can be satisfied.
    const cReactionProcess * cur_process = process_it.Next();
    const double max_consumed = cur_process->GetMaxNumber();
    const double min_consumed = cur_process->GetMinNumber();

    // Determine resource consumption
    double consumed = 0.0;
    cResource * in_resource = cur_process->GetResource();

    // Test if infinite resource
    if (in_resource == NULL) {
      consumed = max_consumed;
    }

    // Otherwise we're using a finite resource
    else {
      const int res_id = in_resource->GetID();

      consumed = resource_count[res_id] - result.GetConsumed(res_id);
      consumed *= cur_process->GetMaxFraction();

      // Make sure we're not above the maximum consumption.
      if (consumed > max_consumed) consumed = max_consumed;

      // Test if we are below the minimum consumption.
      if (consumed < min_consumed) consumed = 0.0;

      // If we don't actually have any resource to consume stop here.
      if (consumed == 0.0) continue;

      // Mark in the results the resource consumed.
      result.Consume(res_id, consumed);
    }

    // Calculate the bonus
    double bonus = consumed * cur_process->GetValue() * task_quality;

    switch (cur_process->GetType()) {
    case REACTION_PROCTYPE_ADD:
      result.AddBonus(bonus);
      break;
    case REACTION_PROCTYPE_MULT:
      result.MultBonus(bonus);
      break;
    case REACTION_PROCTYPE_POW:
      result.MultBonus( pow(2.0, bonus) );
      break;
    default:
      assert(false);  // Should not get here!
      break;
    };

    // Determine byproducts
    cResource * product = cur_process->GetProduct();
    if (product == NULL) continue;

    int product_id = product->GetID();
    double product_size = consumed * cur_process->GetConversion();
    result.Produce(product_id, product_size);
  }
}
