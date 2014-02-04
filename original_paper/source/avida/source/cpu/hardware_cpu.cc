//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2002 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#include <iomanip.h>

#include "hardware_cpu.hh"

#include "../tools/string_util.hh"

#include "../main/config.hh"
#include "../main/inst_lib.hh"
#include "../main/genome_util.hh"
#include "../main/organism.hh"
#include "../main/phenotype.hh"

#include "test_cpu.hh"


/////////////////////
//  cHardwareCPU_Thread
/////////////////////

cHardwareCPU_Thread::cHardwareCPU_Thread(cHardwareBase * in_hardware, int _id)
  : input_buf(INPUT_BUF_SIZE),
    output_buf(OUTPUT_BUF_SIZE)
{
  Reset(in_hardware, _id);
}

cHardwareCPU_Thread::cHardwareCPU_Thread(const cHardwareCPU_Thread & in_thread, int _id)
  : input_buf(INPUT_BUF_SIZE),
    output_buf(OUTPUT_BUF_SIZE)
{
   id = _id;
   if (id == -1) id = in_thread.id;
   for (int i = 0; i < NUM_REGISTERS; i++) {
     reg[i] = in_thread.reg[i];
   }
   for (int i = 0; i < NUM_HEADS; i++) {
     heads[i] = in_thread.heads[i];
   }
   stack = in_thread.stack;
   input_pointer = in_thread.input_pointer;
}

cHardwareCPU_Thread::~cHardwareCPU_Thread() {}

void cHardwareCPU_Thread::operator=(const cHardwareCPU_Thread & in_thread)
{
  id = in_thread.id;
  for (int i = 0; i < NUM_REGISTERS; i++) {
    reg[i] = in_thread.reg[i];
  }
  for (int i = 0; i < NUM_HEADS; i++) {
    heads[i] = in_thread.heads[i];
  }
  stack = in_thread.stack;
  input_pointer = in_thread.input_pointer;
  input_buf = in_thread.input_buf;
  output_buf = in_thread.output_buf;
}

void cHardwareCPU_Thread::Reset(cHardwareBase * in_hardware, int _id)
{
  id = _id;

  for (int i = 0; i < NUM_REGISTERS; i++) reg[i] = 0;
  for (int i = 0; i < NUM_HEADS; i++) heads[i].Reset(in_hardware);

  stack.Clear();
  cur_stack = 0;
  cur_head = HEAD_IP;
  input_pointer = 0;
  input_buf.Clear();
  output_buf.Clear();
  read_label.Clear();
  next_label.Clear();
}




void cHardwareCPU_Thread::SaveState(ostream & fp){
  assert(fp.good());
  fp << "cHardwareCPU_Thread" << endl;

  // registers
  for( int i=0; i<NUM_REGISTERS; ++i ){
    fp<<reg[i]<<endl;
  }

  // heads (@TCC does not handle parasites!!!)
  for( int i=0; i<NUM_HEADS; ++i ){
    fp<<heads[i].GetPosition()<<endl;
  }

  stack.SaveState(fp);

  fp<<"|"; // marker
  fp<<cur_stack;
  fp<<cur_head;
  fp<<input_pointer;
  fp<<endl;

  // IO buffers
  input_buf.SaveState(fp);
  output_buf.SaveState(fp);

  // Code labels
  read_label.SaveState(fp);
  next_label.SaveState(fp);
}



void cHardwareCPU_Thread::LoadState(istream & fp){
  assert(fp.good());
  cString foo;
  fp >> foo;
  assert( foo == "cHardwareCPU_Thread");

  // registers
  for( int i=0; i<NUM_REGISTERS; ++i ){
    fp>>reg[i];
  }

  // heads (@TCC does not handle parasites!!!)
  for( int i=0; i<NUM_HEADS; ++i ){
    int pos;
    fp>>pos;
    heads[i].AbsSet(pos);
  }

  // stack
  stack.LoadState(fp);

  char marker; fp>>marker; assert( marker == '|' );
  fp.get(cur_stack);
  fp.get(cur_head);
  fp.get(input_pointer);

  // IO buffers
  input_buf.LoadState(fp);
  output_buf.LoadState(fp);

  // Code labels
  read_label.LoadState(fp);
  next_label.LoadState(fp);
}



///////////////
//  cHardwareCPU
///////////////

cHardwareCPU::cHardwareCPU(cOrganism * in_organism, cInstLib * in_inst_lib)
  : cHardwareBase(in_organism, in_inst_lib)
{
  threads = NULL;                     // Threads created in Reset()
  memory = in_organism->GetGenome();  // Initialize memory...
  Reset();                            // Setup the rest of the hardware...
}


cHardwareCPU::~cHardwareCPU()
{
  if (threads != NULL) {
    delete [] threads;
  }
}


void cHardwareCPU::Recycle(cOrganism * new_organism, cInstLib * in_inst_lib)
{
  cHardwareBase::Recycle(new_organism, in_inst_lib);
  memory = new_organism->GetGenome();
  Reset();
}


void cHardwareCPU::Reset()
{
  global_stack.Clear();
  thread_time_used = 0;

  // If more than one thread exists, delete them!
  if (threads != NULL && num_threads != 1) {
    delete [] threads;
    threads = NULL;
  }

  // If we do not have a thread, build a new one.
  if (threads == NULL) {
    threads = new cHardwareCPU_Thread[1];
    num_threads = 1;
  }

  // Reset the thread (we know we have exactly one now.)
  threads[0].Reset(this, 0);
  thread_id_chart = 1; // Mark first thread as taken...
  cur_thread = 0;

  mal_active = false;
  //  advance_ip = true;

#ifdef INSTRUCTION_COSTS
  // instruction cost arrays
  const int num_inst_cost = GetNumInst();
  inst_cost.Resize(num_inst_cost);
  inst_ft_cost.Resize(num_inst_cost);

  for (int i = 0; i < num_inst_cost; i++) {
    inst_cost[i] = GetInstLib().GetCost(cInstruction(i));
    inst_ft_cost[i] = GetInstLib().GetFTCost(cInstruction(i));
  }
#endif

}


// This function processes the very next command in the genome, and is made
// to be as optimized as possible.  This is the heart of avida.

void cHardwareCPU::SingleProcess(ostream * trace_fp)
{
  advance_ip = true;
  organism->GetPhenotype().IncTimeUsed();

#ifdef THREADS
  if (GetNumThreads() > 1) thread_time_used++;
  for (int i = 0; i < GetNumThreads(); i++)
#endif
    {
      SetupProcess();

#ifdef BREAKPOINTS
      if (IP().FlagBreakpoint() == true) {
	organism->DoBreakpoint();
      }
#endif

      // Print the status of this CPU at each step...
      if (trace_fp != NULL) {
  	organism->PrintStatus(*trace_fp);
  	(*trace_fp) << " Mem (" << GetMemory().GetSize() << "):"
		    << "  " << GetMemory().AsString()
		    << endl;
      }

      // Find the instruction to be executed
      const cInstruction & cur_inst = IP().GetInst();

#ifdef INSTRUCTION_COSTS
      // Asses Costs
      bool exec = true;  // Flag to say if we execute or not...
      assert(cur_inst.GetOp() < num_inst_cost);
      // If first time cost hasn't been paid off...
      if ( inst_ft_cost[cur_inst.GetOp()] > 0 ) {
	inst_ft_cost[cur_inst.GetOp()]--;       // dec cost
	exec = false;
      }

      // Per use cost
      if ( GetInstLib().GetCost(cur_inst) > 0 && exec == true){
	if ( inst_cost[cur_inst.GetOp()] > 1 ){ // if isn't paid off (>1)
	  inst_cost[cur_inst.GetOp()]--;        // dec cost
	  exec = false;
	} else {                                 // else, reset cost array
	  inst_cost[cur_inst.GetOp()] = GetInstLib().GetCost(cur_inst);
	}
      }

      // Prob of exec
      if ( GetInstLib().GetProbFail(cur_inst) > 0.0 && exec == true ){
	exec = ~( g_random.P(GetInstLib().GetProbFail(cur_inst)) );
      }

      // If we are to execute it...
      if (exec == true)
#endif
	{
	  // get a pointer to the corrisponding method...
	  tHardwareMethod inst_ptr = GetInstLib().GetFunction(cur_inst);

	  // Mark the instruction as executed
	  IP().FlagExecuted() = true;

#ifdef INSTRUCTION_COUNT
	  // instruction execution count
	  organism->CPUStats().inst_stats.IncCount(cur_inst.GetOp());
#endif
	
	  // And execute it.
	  // Execution Error?
#ifdef EXECUTION_ERRORS
	  if( organism->TestExeErr() ){
	    // Get random inst
	    inst_ptr = GetInstLib().GetRandFunction();
	  }
#endif
	  (this->*inst_ptr)();
	
	  if (advance_ip == true) IP().Advance();
	
	} // if exec (& #ifdef INSTRUCTION_COSTS)

    } // for each thread (& #ifdef THREADS)

  // Kill creatures who have reached their max num of instructions executed
  const int max_executed = organism->GetMaxExecuted();
  if ( max_executed > 0 &&
       organism->GetPhenotype().GetTimeUsed() >= max_executed ) {
    organism->Die();
  }
}


void cHardwareCPU::LoadGenome(const cGenome & new_genome)
{
  GetMemory() = new_genome;
}


bool cHardwareCPU::OK()
{
  bool result = true;

  if (!memory.OK()) result = false;

  for (int i = 0; i < num_threads; i++) {
    assert (threads[i].input_pointer < IO_SIZE);

    if (threads[i].stack.OK() == false) result = false;
    if (threads[i].next_label.OK() == false) result = false;
  }

  return result;
}

void cHardwareCPU::PrintStatus(ostream & fp)
{
  fp << organism->GetPhenotype().GetTimeUsed() << " "
     << "IP:" << IP().GetPosition() << "    "

     << "AX:" << Register(REG_AX) << " "
     << setbase(16) << "[0x" << Register(REG_AX) << "]  " << setbase(10)

     << "BX:" << Register(REG_BX) << " "
     << setbase(16) << "[0x" << Register(REG_BX) << "]  " << setbase(10)

     << "CX:" << Register(REG_CX) << " "
     << setbase(16) << "[0x" << Register(REG_CX) << "]" << setbase(10)

     << endl;

  fp << "  R-Head:" << GetHead(HEAD_READ).GetPosition() << " "
     << "W-Head:" << GetHead(HEAD_WRITE).GetPosition()  << " "
     << "F-Head:" << GetHead(HEAD_FLOW).GetPosition()   << "  "
     << "RL:" << GetReadLabel().AsString() << "   "
     << "NEXT: >> " << inst_lib->GetName(IP().GetInst())() << " <<"
     << endl;
  fp.flush();
}


void cHardwareCPU::SetupProcess()
{
#ifdef THREADS
  NextThread();
#endif
  IP().Adjust();
}


void cHardwareCPU::DoInput(const int value)
{
  organism->DoInput(value, GetInputBuffer(), GetOutputBuffer());
}

void cHardwareCPU::DoOutput(const int value)
{
  organism->DoOutput(value, GetInputBuffer(), GetOutputBuffer());
}




/////////////////////////////////////////////////////////////////////////
// Method: cHardwareCPU::FindLabel(direction)
//
// Search in 'direction' (+ or - 1) from the instruction pointer for the
// compliment of the label in 'next_label' and return a pointer to the
// results.  If direction is 0, search from the beginning of the genome.
//
/////////////////////////////////////////////////////////////////////////

cCPUHead cHardwareCPU::FindLabel(int direction)
{
  cCPUHead & inst_ptr = IP();

  // Start up a search head at the position of the instruction pointer.
  cCPUHead search_head(inst_ptr);
  cCodeLabel & search_label = threads[CUR_THREAD].next_label;

  // Make sure the label is of size > 0.

  if (search_label.GetSize() == 0) {
    return inst_ptr;
  }

  // Call special functions depending on if jump is forwards or backwards.
  int found_pos = 0;
  if( direction < 0 ) {
    found_pos = FindLabel_Backward(search_label, inst_ptr.GetMemory(),
			   inst_ptr.GetPosition() - search_label.GetSize());
  }

  // Jump forward.
  else if (direction > 0) {
    found_pos = FindLabel_Forward(search_label, inst_ptr.GetMemory(),
			   inst_ptr.GetPosition());
  }

  // Jump forward from the very beginning.
  else {
    found_pos = FindLabel_Forward(search_label, inst_ptr.GetMemory(), 0);
  }
  
  // Return the last line of the found label, if it was found.
  if (found_pos >= 0) search_head.Set(found_pos - 1);

  // Return the found position (still at start point if not found).
  return search_head;
}


// Search forwards for search_label from _after_ position pos in the
// memory.  Return the first line _after_ the the found label.  It is okay
// to find search label's match inside another label.

int cHardwareCPU::FindLabel_Forward(const cCodeLabel & search_label,
				 const cGenome & search_genome, int pos)
{
  assert (pos < search_genome.GetSize() && pos >= 0);

  int search_start = pos;
  int label_size = search_label.GetSize();
  bool found_label = false;

  // Move off the template we are on.
  pos += label_size;

  // Search until we find the complement or exit the memory.
  while (pos < search_genome.GetSize()) {

    // If we are within a label, rewind to the beginning of it and see if
    // it has the proper sub-label that we're looking for.

    if (inst_lib->IsNop(search_genome[pos])) {
      // Find the start and end of the label we're in the middle of.

      int start_pos = pos;
      int end_pos = pos + 1;
      while (start_pos > search_start &&
	     inst_lib->IsNop( search_genome[start_pos - 1] )) {
	start_pos--;
      }
      while (end_pos < search_genome.GetSize() &&
	     inst_lib->IsNop( search_genome[end_pos] )) {
	end_pos++;
      }
      int test_size = end_pos - start_pos;

      // See if this label has the proper sub-label within it.
      int max_offset = test_size - label_size + 1;
      int offset = start_pos;
      for (offset = start_pos; offset < start_pos + max_offset; offset++) {

	// Test the number of matches for this offset.
	int matches;
	for (matches = 0; matches < label_size; matches++) {
	  if (search_label[matches] !=
	      inst_lib->GetNopMod( search_genome[offset + matches] )) {
	    break;
	  }
	}

	// If we have found it, break out of this loop!
	if (matches == label_size) {
	  found_label = true;
	  break;
	}
      }

      // If we've found the complement label, set the position to the end of
      // the label we found it in, and break out.

      if (found_label == true) {
	// pos = end_pos;
	pos = label_size + offset;
	break;
      }

      // We haven't found it; jump pos to just after the current label being
      // checked.
      pos = end_pos;
    }

    // Jump up a block to the next possible point to find a label,
    pos += label_size;
  }

  // If the label was not found return a -1.
  if (found_label == false) pos = -1;

  return pos;
}

// Search backwards for search_label from _before_ position pos in the
// memory.  Return the first line _after_ the the found label.  It is okay
// to find search label's match inside another label.

int cHardwareCPU::FindLabel_Backward(const cCodeLabel & search_label,
				  const cGenome & search_genome, int pos)
{
  assert (pos < search_genome.GetSize());

  int search_start = pos;
  int label_size = search_label.GetSize();
  bool found_label = false;

  // Move off the template we are on.
  pos -= label_size;

  // Search until we find the complement or exit the memory.
  while (pos >= 0) {
    // If we are within a label, rewind to the beginning of it and see if
    // it has the proper sub-label that we're looking for.

    if (inst_lib->IsNop( search_genome[pos] )) {
      // Find the start and end of the label we're in the middle of.

      int start_pos = pos;
      int end_pos = pos + 1;
      while (start_pos > 0 && inst_lib->IsNop(search_genome[start_pos - 1])) {
	start_pos--;
      }
      while (end_pos < search_start &&
	     inst_lib->IsNop(search_genome[end_pos])) {
	end_pos++;
      }
      int test_size = end_pos - start_pos;

      // See if this label has the proper sub-label within it.
      int max_offset = test_size - label_size + 1;
      for (int offset = start_pos; offset < start_pos + max_offset; offset++) {

	// Test the number of matches for this offset.
	int matches;
	for (matches = 0; matches < label_size; matches++) {
	  if (search_label[matches] !=
	      inst_lib->GetNopMod(search_genome[offset + matches])) {
	    break;
	  }
	}

	// If we have found it, break out of this loop!
	if (matches == label_size) {
	  found_label = true;
	  break;
	}
      }

      // If we've found the complement label, set the position to the end of
      // the label we found it in, and break out.

      if (found_label == true) {
	pos = end_pos;
	break;
      }

      // We haven't found it; jump pos to just before the current label
      // being checked.
      pos = start_pos - 1;
    }

    // Jump up a block to the next possible point to find a label,
    pos -= label_size;
  }

  // If the label was not found return a -1.
  if (found_label == false) pos = -1;

  return pos;
}

// Search for 'in_label' anywhere in the hardware.
cCPUHead cHardwareCPU::FindLabel(const cCodeLabel & in_label, int direction)
{
  assert (in_label.GetSize() > 0);

  // IDEALY:
  // Keep making jumps (in the proper direction) equal to the label
  // length.  If we are inside of a label, check its size, and see if
  // any of the sub-labels match properly.
  // FOR NOW:
  // Get something which works, no matter how inefficient!!!

  cCPUHead temp_head(this);

  while (temp_head.InMemory()) {
    // IDEALY: Analyze the label we are in; see if the one we are looking
    // for could be a sub-label of it.  Skip past it if not.

    int i;
    for (i = 0; i < in_label.GetSize(); i++) {
      if (!inst_lib->IsNop(temp_head.GetInst()) ||
	  in_label[i] != inst_lib->GetNopMod(temp_head.GetInst())) {
	break;
      }
    }
    if (i == threads[CUR_THREAD].next_label.GetSize()) {
      temp_head.AbsJump(i - 1);
      return temp_head;
    }

    temp_head.AbsJump(direction);     // IDEALY: MAKE LARGER JUMPS
  }

  temp_head.AbsSet(-1);
  return temp_head;
}

// @CAO: direction is not currently used; should be used to indicate the
// direction which the heads[HEAD_IP] should progress through a creature.
cCPUHead cHardwareCPU::FindFullLabel(const cCodeLabel & in_label)
{
  // cout << "Running FindFullLabel with " << in_label.AsString() << endl;

  assert(in_label.GetSize() > 0); // Trying to find label of 0 size!

  cCPUHead temp_head(this);

  while (temp_head.InMemory()) {
    // If we are not in a label, jump to the next checkpoint...
    if (inst_lib->IsNop(temp_head.GetInst())) {
      temp_head.AbsJump(in_label.GetSize());
      continue;
    }

    // Otherwise, rewind to the begining of this label...

    while (!(temp_head.AtFront()) && inst_lib->IsNop(temp_head.GetInst(-1)))
      temp_head.AbsJump(-1);

    // Calculate the size of the label being checked, and make sure they
    // are equal.

    int checked_size = 0;
    while (inst_lib->IsNop(temp_head.GetInst(checked_size))) {
      checked_size++;
    }
    if (checked_size != in_label.GetSize()) {
      temp_head.AbsJump(checked_size + 1);
      continue;
    }

    // cout << "Testing label at line " << temp_head.GetPosition() << endl;

    // ...and do the comparison...

    int j;
    bool label_match = true;
    for (j = 0; j < in_label.GetSize(); j++) {
      if (!inst_lib->IsNop(temp_head.GetInst(j)) ||
	  in_label[j] != inst_lib->GetNopMod(temp_head.GetInst(j))) {
	temp_head.AbsJump(in_label.GetSize() + 1);
	label_match = false;
	break;
      }
    }

    if (label_match) {
      // If we have found the label, return the position after it.
      temp_head.AbsJump(j - 1);
      return temp_head;
    }

    // We have not found the label... increment i.

    temp_head.AbsJump(in_label.GetSize() + 1);
  }

  // The label does not exist in this creature.

  temp_head.AbsSet(-1);
  return temp_head;
}


int cHardwareCPU::Inject(const cCodeLabel & in_label, const cGenome & injection)
{
  // Make sure the genome will be below max size after injection.

  const int new_size = injection.GetSize() + GetMemory().GetSize();
  if (new_size > MAX_CREATURE_SIZE) return 1; // (inject fails)

  const int inject_line = FindFullLabel(in_label).GetPosition();

  // Abort if no compliment is found.
  if (inject_line == -1) return 2; // (inject fails)

  // Inject the code!
  InjectCode(injection, inject_line+1);

  return 0; // (inject succeeds!)
}


void cHardwareCPU::InjectCode(const cGenome & inject_code, const int line_num)
{
  assert(line_num >= 0);
  assert(line_num <= memory.GetSize());
  assert(memory.GetSize() + inject_code.GetSize() < MAX_CREATURE_SIZE);

  // Inject the new code.
  const int inject_size = inject_code.GetSize();
  memory.Insert(line_num, inject_code);
  
  // Set instruction flags on the injected code
  for (int i = line_num; i < line_num + inject_size; i++) {
    memory.FlagInjected(i) = true;
  }
  organism->GetPhenotype().IsModified() = true;

  // Adjust all of the heads to take into account the new mem size.

  for (int i=0; i < NUM_HEADS; i++) {
    if (!threads[CUR_THREAD].heads[i].TestParasite() &&
	threads[CUR_THREAD].heads[i].GetPosition() > line_num)
      threads[CUR_THREAD].heads[i].Jump(inject_size);
  }
}


void cHardwareCPU::Mutate(int mut_point)
{
  // Test if trying to mutate outside of genome...
  assert(mut_point >= 0 && mut_point < GetMemory().GetSize());

  GetMemory()[mut_point] = GetRandomInst();
  GetMemory().FlagMutated(mut_point) = true;
  GetMemory().FlagPointMut(mut_point) = true;
  //organism->GetPhenotype().IsMutated() = true;
  organism->CPUStats().mut_stats.point_mut_count++;
}

int cHardwareCPU::PointMutate(const double mut_rate)
{
  const int num_muts =
    g_random.GetRandBinomial(GetMemory().GetSize(), mut_rate);

  for (int i = 0; i < num_muts; i++) {
    const int pos = g_random.GetUInt(GetMemory().GetSize());
    Mutate(pos);
  }

  return num_muts;
}


void cHardwareCPU::ReadInst(const int in_inst) const
{
  if (inst_lib->IsNop( cInstruction(in_inst) )) {
    threads[CUR_THREAD].read_label.AddNop(in_inst);
  } else {
    threads[CUR_THREAD].read_label.Clear();
  }
}


void cHardwareCPU::AdjustHeads()
{
  for (int i = 0; i < num_threads; i++) {
    for (int j = 0; j < NUM_HEADS; j++) {
      threads[i].heads[j].Adjust();
    }
  }
}



// This function looks at the current position in the info of a creature,
// and sets the next_label to be the sequence of nops which follows.  The
// instruction pointer is left on the last line of the label found.

void cHardwareCPU::ReadLabel(int max_size)
{
  int count = 0;
  cCPUHead * inst_ptr = &( IP() );

  threads[CUR_THREAD].next_label.Clear();

  while (inst_lib->IsNop(inst_ptr->GetNextInst()) &&
	 (count < max_size)) {
    count++;
    inst_ptr->Advance();
    threads[CUR_THREAD].next_label.
      AddNop(inst_lib->GetNopMod(inst_ptr->GetInst()));

    // If this is the first line of the template, mark it executed.
    if (threads[CUR_THREAD].next_label.GetSize() <=
	cConfig::GetMaxLabelExeSize()) {
      inst_ptr->FlagExecuted() = true;
    }
  }
}


bool cHardwareCPU::ForkThread()
{
#ifdef THREADS
  if (num_threads == cConfig::GetMaxCPUThreads()) return false;

  cHardwareCPU_Thread * new_threads = new cHardwareCPU_Thread[num_threads + 1];
  for (int i = 0; i < num_threads; i++) {
    new_threads[i] = threads[i];
  }
  new_threads[num_threads] = threads[CUR_THREAD];

  // Find the first free bit in thread_id_chart to determine the new
  // thread id.
  int new_id = 0;
  while ( (thread_id_chart >> new_id) & 1 == 1) new_id++;
  new_threads[num_threads].SetID(new_id);
  thread_id_chart |= (1 << new_id);

  delete [] threads;
  threads = new_threads;

  num_threads++;
#endif

  return true;
}


int cHardwareCPU::TestParasite() const
{
  return IP().TestParasite();
}


bool cHardwareCPU::KillThread()
{
#ifdef THREADS
  // Make sure that there is always at least one thread...
  if (num_threads == 1) return false;

  // Note the current thread and set the current back one.
  int kill_thread = cur_thread;
  PrevThread();
  
  // Turn off this bit in the thread_id_chart...
  thread_id_chart ^= 1 << threads[kill_thread].GetID();

  // Kill the thread!
  cHardwareCPU_Thread * new_threads = new cHardwareCPU_Thread[num_threads - 1];

  int i;
  for (i = 0; i < kill_thread; i++) {
    new_threads[i] = threads[i];
  }
  for (i = kill_thread + 1; i < num_threads; i++) {
    new_threads[i - 1] = threads[i];
  }

  delete [] threads;
  threads = new_threads;

  if (cur_thread > kill_thread) cur_thread--;
  num_threads--;
#endif

  return true;
}


void cHardwareCPU::SaveState(ostream & fp)
{
  // note, memory & child_memory handled by cpu (@CAO Not any more!)
  assert(fp.good());

  fp<<"cHardwareCPU"<<endl;

  // global_stack (in inverse order so load can just push)
  global_stack.SaveState(fp);

  fp << thread_time_used<<endl;
  fp << num_threads<<endl;
  fp << cur_thread<<endl;

  // Threads
  for( int i=0; i<num_threads; ++i ){
    threads[i].SaveState(fp);
  }
}


void cHardwareCPU::LoadState(istream & fp)
{
  // note, memory & child_memory handled by cpu (@CAO Not any more!)
  assert(fp.good());

  cString foo;
  fp>>foo;
  assert( foo == "cHardwareCPU" );

  // global_stack
  global_stack.LoadState(fp);

  fp >> thread_time_used;
  fp >> num_threads;
  fp >> cur_thread;

  // Threads
  for( int i=0; i<num_threads; ++i ){
    threads[i].LoadState(fp);
  }
}


////////////////////////////
//  Instruction Helpers...
////////////////////////////

inline int cHardwareCPU::FindModifiedRegister(int default_register)
{
  assert(default_register < NUM_REGISTERS);  // Reg ID too high.

  if (GetInstLib().IsNop(IP().GetNextInst())) {
    IP().Advance();
    default_register = GetInstLib().GetNopMod(IP().GetInst());
    IP().FlagExecuted() = true;
  }
  return default_register;
}


inline int cHardwareCPU::FindModifiedHead(int default_head)
{
  assert(default_head < NUM_HEADS); // Head ID too high.

  if (GetInstLib().IsNop(IP().GetNextInst())) {
    IP().Advance();
    default_head = GetInstLib().GetNopMod(IP().GetInst());
    IP().FlagExecuted() = true;
  }
  return default_head;
}


inline int cHardwareCPU::FindComplementRegister(int base_reg)
{
  const int comp_reg = base_reg + 1;
  return (comp_reg  == NUM_REGISTERS) ? 0 : comp_reg;
}


inline void cHardwareCPU::Fault(int fault_loc, int fault_type, cString fault_desc)
{
  organism->Fault(fault_loc, fault_type, fault_desc);
}


void cHardwareCPU::DoCrossoverMutation(int start1, int end1, cCPUMemory & in_mem,
				    int start2, int end2)
{
  cCPUMemory & c1 = GetMemory();
  cCPUMemory & c2 = in_mem;
  assert( start1 >= 0  &&  start1 < c1.GetSize() );
  assert( end1   >= 0  &&  end1   < c1.GetSize() );
  assert( start2 >= 0  &&  start2 < c2.GetSize() );
  assert( end2   >= 0  &&  end2   < c2.GetSize() );

  // @CAO for the moment, force start to be less than end.
  if (start1 > end1) Swap(start1, end1);
  if (start2 > end2) Swap(start2, end2);

  // Calculate size of sections crossing over...
  int size1 = end1 - start1 + 1;
  int size2 = end2 - start2 + 1;

  int new_size1 = c1.GetSize() - size1 + size2;
  int new_size2 = c2.GetSize() - size2 + size1;

  // Don't Crossover if offspring will be illegal!!!
  if( new_size1 < MIN_CREATURE_SIZE || new_size1 > MAX_CREATURE_SIZE ||
      new_size2 < MIN_CREATURE_SIZE || new_size2 > MAX_CREATURE_SIZE ){
    return;
  }

  cGenome cross1 = cGenomeUtil::Crop(c1, start1, end1);
  cGenome cross2 = cGenomeUtil::Crop(c2, start2, end2);

  c1.Replace(start1, size1, cross2);
  c2.Replace(start2, size2, cross1);
}

bool cHardwareCPU::Allocate_Necro(const int old_size, const int new_size)
{
  GetMemory().ResizeOld(new_size);
  return true;
}

bool cHardwareCPU::Allocate_Random(const int old_size, const int new_size)
{
  GetMemory().Resize(new_size);

  for (int i = old_size; i < new_size; i++) {
    GetMemory()[i] = GetInstLib().GetRandomInst();
  }
  return true;
}

bool cHardwareCPU::Allocate_Default(const int old_size, const int new_size)
{
  GetMemory().Resize(new_size);

  // New space already defaults to default instruction...
//    for (int i = old_size; i < new_size; i++) {
//      GetMemory()[i] = cInstruction::GetInstDefault();
//    }
  return true;
}

bool cHardwareCPU::Allocate_Main(const int allocated_size)
{
  // must do divide before second allocate & must allocate positive amount...
  if (cConfig::GetRequireAllocate() && mal_active == true) {
    Fault(FAULT_LOC_ALLOC, FAULT_TYPE_ERROR, "Allocate already active");
    return false;
  }
  if (allocated_size < 1) {
    Fault(FAULT_LOC_ALLOC, FAULT_TYPE_ERROR,
	  cStringUtil::Stringf("Allocate of %d too small", allocated_size));
    return false;
  }

  const int old_size = GetMemory().GetSize();
  const int new_size = old_size + allocated_size;

  // Make sure that the new size is in range.
  if (new_size > MAX_CREATURE_SIZE  ||  new_size < MIN_CREATURE_SIZE) {
    Fault(FAULT_LOC_ALLOC, FAULT_TYPE_ERROR,
	  cStringUtil::Stringf("Invalid post-allocate size (%d)",
			       new_size));
    return false;
  }

  const int max_alloc_size = (int) (old_size * cConfig::GetChildSizeRange());
  if (allocated_size > max_alloc_size) {
    Fault(FAULT_LOC_ALLOC, FAULT_TYPE_ERROR,
	  cStringUtil::Stringf("Allocate too large (%d > %d)",
			       allocated_size, max_alloc_size));
    return false;
  }

  const int max_old_size =
    (int) (allocated_size * cConfig::GetChildSizeRange());
  if (old_size > max_old_size) {
    Fault(FAULT_LOC_ALLOC, FAULT_TYPE_ERROR,
	  cStringUtil::Stringf("Allocate too small (%d > %d)",
			       old_size, max_old_size));
    return false;
  }

  switch (cConfig::GetAllocMethod()) {
  case ALLOC_METHOD_NECRO:
    // Only break if this succeeds -- otherwise just do random.
    if (Allocate_Necro(old_size, new_size) == true) break;
  case ALLOC_METHOD_RANDOM:
    Allocate_Random(old_size, new_size);
    break;
  case ALLOC_METHOD_DEFAULT:
    Allocate_Default(old_size, new_size);
    break;
  }

  mal_active = true;

  return true;
}


bool cHardwareCPU::Divide_CheckViable(const int child_size, const int parent_size,
				   const int extra_lines)
{
  // Make sure the organism is okay with dividing now...
  if (organism->Divide_CheckViable() == false) return false; // (divide fails)

  // If required, make sure an allocate has occured.
  if (cConfig::GetRequireAllocate() && mal_active == false) {
    Fault(FAULT_LOC_DIVIDE, FAULT_TYPE_ERROR, "Must allocate before divide");
    return false; //  (divide fails)
  }

  // Make sure that neither parent nor child will be below the minimum size.

  const int genome_size = organism->GetGenome().GetSize();
  const double size_range = cConfig::GetChildSizeRange();
  const int min_size = Max(MIN_CREATURE_SIZE, (int) (genome_size/size_range));
  const int max_size = Min(MAX_CREATURE_SIZE, (int) (genome_size*size_range));
  
  if (child_size < min_size || child_size > max_size) {
    Fault(FAULT_LOC_DIVIDE, FAULT_TYPE_ERROR,
	  cStringUtil::Stringf("Invalid offspring length (%d)", child_size));
    return false; // (divide fails)
  }
  if (parent_size < min_size || parent_size > max_size) {
    Fault(FAULT_LOC_DIVIDE, FAULT_TYPE_ERROR,
	  cStringUtil::Stringf("Invalid post-divide length (%d)",parent_size));
    return false; // (divide fails)
  }

  // Count the number of lines executed in the parent, and make sure the
  // specified fraction has been reached.

  int executed_size = 0;
  for (int i = 0; i < parent_size; i++) {
    if (GetMemory().FlagExecuted(i)) executed_size++;
  }

  const int min_exe_lines = (int) (parent_size * cConfig::GetMinExeLines());
  if (executed_size < min_exe_lines) {
    Fault(FAULT_LOC_DIVIDE, FAULT_TYPE_ERROR,
	  cStringUtil::Stringf("Too few executed lines (%d < %d)",
			       executed_size, min_exe_lines));
    return false; // (divide fails)
  }
	
  // Count the number of lines which were copied into the child, and make
  // sure the specified fraction has been reached.

  int copied_size = 0;
  for (int i = parent_size; i < parent_size + child_size; i++) {
    if (GetMemory().FlagCopied(i)) copied_size++;
  }

  const int min_copied =  (int) (child_size * cConfig::GetMinCopiedLines());
  if (copied_size < min_copied) {
    Fault(FAULT_LOC_DIVIDE, FAULT_TYPE_ERROR,
	  cStringUtil::Stringf("Too few copied commands (%d < %d)",
			       copied_size, min_copied));
    return false; // (divide fails)
  }

  // Save the information we collected here...
  organism->GetPhenotype().SetLinesExecuted(executed_size);
  organism->GetPhenotype().SetLinesCopied(copied_size);

  return true; // (divide succeeds!)
}

// For QSORT in Divide_DoMutations ... MOVE THIS --@TCC
int IntCompareFunction(const void * a, const void * b){
  if( *((int*)a) > *((int*)b) ) return 1;
  if( *((int*)a) < *((int*)b) ) return -1;
  return 0;
}


void cHardwareCPU::Divide_DoMutations()
{
  sCPUStats & cpu_stats = organism->CPUStats();
  cCPUMemory & child_genome = organism->ChildGenome();

  // Divide Mutations
  if (organism->TestDivideMut()) {
    const UINT mut_line = g_random.GetUInt(child_genome.GetSize());
    child_genome[mut_line] = GetRandomInst();
    cpu_stats.mut_stats.divide_mut_count++;
  }

  // Divide Insertions
  if (organism->TestDivideIns() && child_genome.GetSize() < MAX_CREATURE_SIZE){
    const UINT mut_line = g_random.GetUInt(child_genome.GetSize() + 1);
    child_genome.Insert(mut_line, GetRandomInst());
    cpu_stats.mut_stats.divide_insert_mut_count++;
  }

  // Divide Deletions
  if (organism->TestDivideDel() && child_genome.GetSize() > MIN_CREATURE_SIZE){
    const UINT mut_line = g_random.GetUInt(child_genome.GetSize());
    // if( child_genome.FlagCopied(mut_line) == true) copied_size_change--;
    child_genome.Remove(mut_line);
    cpu_stats.mut_stats.divide_delete_mut_count++;
  }

  // Insert Mutations (per site)
  if(organism->GetInsMutProb() > 0){
    int num_mut = g_random.GetRandBinomial(child_genome.GetSize(),
					   organism->GetInsMutProb());
    // If would make creature to big, insert up to MAX_CREATURE_SIZE
    if( num_mut + child_genome.GetSize() > MAX_CREATURE_SIZE ){
      num_mut = MAX_CREATURE_SIZE - child_genome.GetSize();
    }
    // If we have lines to insert...
    if( num_mut > 0 ){
      // Build a list of the sites where mutations occured
      static int mut_sites[MAX_CREATURE_SIZE];
      for (int i = 0; i < num_mut; i++) {
	mut_sites[i] = g_random.GetUInt(child_genome.GetSize() + 1);
      }
      // Sort the list
      qsort( (void*)mut_sites, num_mut, sizeof(int), &IntCompareFunction );
      // Actually do the mutations (in reverse sort order)
      for(int i = num_mut-1; i >= 0; i--) {
	child_genome.Insert(mut_sites[i], GetRandomInst());
	cpu_stats.mut_stats.insert_mut_count++;
      }
    }
  }

  // Delete Mutations (per site)
  if( organism->GetDelMutProb() > 0 ){
    int num_mut = g_random.GetRandBinomial(child_genome.GetSize(),
					   organism->GetDelMutProb());
    // If would make creature too small, delete down to MIN_CREATURE_SIZE
    if (child_genome.GetSize() - num_mut < MIN_CREATURE_SIZE) {
      num_mut = child_genome.GetSize() - MIN_CREATURE_SIZE;
    }

    // If we have lines to delete...
    for (int i = 0; i < num_mut; i++) {
      int site = g_random.GetUInt(child_genome.GetSize());
      // if (child_genome.FlagCopied(site) == true) copied_size_change--;
      child_genome.Remove(site);
      cpu_stats.mut_stats.delete_mut_count++;
    }
  }

  // Mutations in the parent's genome
  if (organism->GetParentMutProb() > 0) {
    for (int i = 0; i < GetMemory().GetSize(); i++) {
      if (organism->TestParentMut()) {
	GetMemory()[i] = GetRandomInst();
	cpu_stats.mut_stats.parent_mut_line_count++;
      }
    }
  }

  //// Crossover!! ////
  if( organism->TestCrossover() ){
    DoCrossoverMutation( g_random.GetUInt(GetMemory().GetSize()),
			 g_random.GetUInt(GetMemory().GetSize()),
			 child_genome,
			 g_random.GetUInt(child_genome.GetSize()),
			 g_random.GetUInt(child_genome.GetSize()) );
  }
  if (organism->TestAlignedCrossover()) {
    int startpt =
      g_random.GetUInt(Min(GetMemory().GetSize(), child_genome.GetSize()));
    DoCrossoverMutation( startpt, g_random.GetUInt(GetMemory().GetSize()),
			 child_genome, startpt,
			 g_random.GetUInt(child_genome.GetSize()) );
  }

  // Count up mutated lines
  for(int i = 0; i < GetMemory().GetSize(); i++){
    if (GetMemory().FlagPointMut(i) == true) {
      cpu_stats.mut_stats.point_mut_line_count++;
    }
  }
  for(int i = 0; i < child_genome.GetSize(); i++){
    if( child_genome.FlagCopyMut(i) == true) {
      cpu_stats.mut_stats.copy_mut_line_count++;
    }
  }
}


// test whether the offspring creature contains an advantageous mutation.
void cHardwareCPU::Divide_TestFitnessMeasures()
{
  // If we didn't have a genotype change, we can stop right here.
  cPhenotype & phenotype = organism->GetPhenotype();
  phenotype.CopyTrue() = ( organism->ChildGenome() == organism->GetGenome() );
  phenotype.ChildFertile() = true;

  if (phenotype.CopyTrue() == true) return;

  // Only continue if we're supposed to do a fitness test on divide...
  if (organism->GetTestOnDivide() == false) return;

  const double parent_fitness = organism->GetTestFitness();
  const double neut_min = parent_fitness * FITNESS_NEUTRAL_MIN;
  const double neut_max = parent_fitness * FITNESS_NEUTRAL_MAX;
  
  cCPUTestInfo test_info;
  cTestCPU::TestGenome(test_info, organism->ChildGenome());
  const double child_fitness = test_info.GetGenotypeFitness();
  
  bool revert = false;
  bool sterilize = false;
  
  // If implicit mutations are turned off, make sure this won't spawn one.
  if (organism->GetFailImplicit() == true) {
    if (test_info.GetMaxDepth() > 0) sterilize = true;
  }
  
  if (child_fitness == 0.0) {
    // Fatal mutation... test for reversion.
    if (g_random.P(organism->GetRevertFatal())) revert = true;
    if (g_random.P(organism->GetSterilizeFatal())) sterilize = true;
  } else if (child_fitness < neut_min) {
    if (g_random.P(organism->GetRevertNeg())) revert = true;
    if (g_random.P(organism->GetSterilizeNeg())) sterilize = true;
  } else if (child_fitness <= neut_max) {
    if (g_random.P(organism->GetRevertNeut())) revert = true;
    if (g_random.P(organism->GetSterilizeNeut())) sterilize = true;
  } else {
    if (g_random.P(organism->GetRevertPos())) revert = true;
    if (g_random.P(organism->GetSterilizePos())) sterilize = true;
  }
  
  // Ideally, we won't have reversions and sterilizes turned on at the same
  // time, but if we do, give sterilizes the priority.
  if (sterilize == true) {
    organism->GetPhenotype().ChildFertile() = false;
    return; // Child is now infertile.
  } else if (revert == true) {
    organism->ChildGenome() = organism->GetGenome();
    return; // Parent and child are now identical; skip remaining tests.
  }

}


bool cHardwareCPU::Divide_Main(const int div_point, const int extra_lines)
{
  const int child_size = GetMemory().GetSize() - div_point - extra_lines;

  // Make sure this divide will produce a viable offspring.
  const bool viable = Divide_CheckViable(child_size, div_point, extra_lines);
  if (viable == false) return false;

  // Since the divide will now succeed, set up the information to be sent
  // to the new organism
  cGenome & child_genome = organism->ChildGenome();
  child_genome = cGenomeUtil::Crop(memory, div_point, div_point+child_size);

  // Cut off everything in this memory past the divide point.
  GetMemory().Resize(div_point);

  // Handle Divide Mutations...
  Divide_DoMutations();

  // Many tests will require us to run the offspring through a test CPU;
  // this is, for example, to see if mutations need to be reverted or if
  // lineages need to be updated.
  Divide_TestFitnessMeasures();

  // Build the new organism...
  cOrganism * child = organism->Divide_SetupChild();

#ifdef INSTRUCTION_COSTS
  // reset first time instruction costs
  for (int i = 0; i < inst_ft_cost.GetSize(); i++) {
    inst_ft_cost[i] = GetInstLib().GetFTCost(cInstruction(i));
  }
#endif

  mal_active = false;
  if (cConfig::GetDivideMethod() == DIVIDE_METHOD_SPLIT) {
    advance_ip = false;
  }

  organism->ActivateDivide(child);

  return true;
}


//////////////////////////
// And the instructions...
//////////////////////////

void cHardwareCPU::Inst_If0()          // Execute next if ?bx? ==0.
{
  const int reg_used = FindModifiedRegister(REG_BX);
  if (Register(reg_used) != 0)  IP().Advance();
}

void cHardwareCPU::Inst_IfNot0()       // Execute next if ?bx? != 0.
{
  const int reg_used = FindModifiedRegister(REG_BX);
  if (Register(reg_used) == 0)  IP().Advance();
}

void cHardwareCPU::Inst_IfEqu()      // Execute next if bx == ?cx?
{
  const int reg_used = FindModifiedRegister(REG_BX);
  const int reg_used2 = FindComplementRegister(reg_used);
  if (Register(reg_used) != Register(reg_used2))  IP().Advance();
}

void cHardwareCPU::Inst_IfNEqu()     // Execute next if bx != ?cx?
{
  const int reg_used = FindModifiedRegister(REG_BX);
  const int reg_used2 = FindComplementRegister(reg_used);
  if (Register(reg_used) == Register(reg_used2))  IP().Advance();
}

void cHardwareCPU::Inst_IfGr0()       // Execute next if ?bx? ! < 0.
{
  const int reg_used = FindModifiedRegister(REG_BX);
  if (Register(reg_used) <= 0)  IP().Advance();
}

void cHardwareCPU::Inst_IfGr()       // Execute next if bx > ?cx?
{
  const int reg_used = FindModifiedRegister(REG_BX);
  const int reg_used2 = FindComplementRegister(reg_used);
  if (Register(reg_used) <= Register(reg_used2))  IP().Advance();
}

void cHardwareCPU::Inst_IfGrEqu0()       // Execute next if ?bx? != 0.
{
  const int reg_used = FindModifiedRegister(REG_BX);
  if (Register(reg_used) < 0)  IP().Advance();
}

void cHardwareCPU::Inst_IfGrEqu()       // Execute next if bx > ?cx?
{
  const int reg_used = FindModifiedRegister(REG_BX);
  const int reg_used2 = FindComplementRegister(reg_used);
  if (Register(reg_used) < Register(reg_used2)) IP().Advance();
}

void cHardwareCPU::Inst_IfLess0()       // Execute next if ?bx? != 0.
{
  const int reg_used = FindModifiedRegister(REG_BX);
  if (Register(reg_used) >= 0)  IP().Advance();
}

void cHardwareCPU::Inst_IfLess()       // Execute next if ?bx? < ?cx?
{
  const int reg_used = FindModifiedRegister(REG_BX);
  const int reg_used2 = FindComplementRegister(reg_used);
  if (Register(reg_used) >=  Register(reg_used2))  IP().Advance();
}

void cHardwareCPU::Inst_IfLsEqu0()       // Execute next if ?bx? != 0.
{
  const int reg_used = FindModifiedRegister(REG_BX);
  if (Register(reg_used) > 0) IP().Advance();
}

void cHardwareCPU::Inst_IfLsEqu()       // Execute next if bx > ?cx?
{
  const int reg_used = FindModifiedRegister(REG_BX);
  const int reg_used2 = FindComplementRegister(reg_used);
  if (Register(reg_used) >  Register(reg_used2))  IP().Advance();
}

void cHardwareCPU::Inst_IfBit1()
{
  const int reg_used = FindModifiedRegister(REG_BX);
  if ((Register(reg_used) & 1) == 0)  IP().Advance();
}

void cHardwareCPU::Inst_IfANotEqB()     // Execute next if AX != BX
{
  if (Register(REG_AX) == Register(REG_BX) )  IP().Advance();
}

void cHardwareCPU::Inst_IfBNotEqC()     // Execute next if BX != CX
{
  if (Register(REG_BX) == Register(REG_CX) )  IP().Advance();
}

void cHardwareCPU::Inst_IfANotEqC()     // Execute next if AX != BX
{
  if (Register(REG_AX) == Register(REG_CX) )  IP().Advance();
}

void cHardwareCPU::Inst_JumpF()
{
  ReadLabel();
  GetLabel().Rotate(1);

  // If there is no label, jump BX steps.
  if (GetLabel().GetSize() == 0) {
    GetActiveHead().Jump(Register(REG_BX));
    return;
  }

  // Otherwise, try to jump to the complement label.
  const cCPUHead jump_location(FindLabel(1));
  if ( jump_location.GetPosition() != -1 ) {
    GetActiveHead().Set(jump_location);
    return;
  }

  // If complement label was not found; record an error.
  organism->Fault(FAULT_LOC_JUMP, FAULT_TYPE_ERROR,
		  "jump-f: No complement label");
}


void cHardwareCPU::Inst_JumpB()
{
  ReadLabel();
  GetLabel().Rotate(1);

  // If there is no label, jump BX steps.
  if (GetLabel().GetSize() == 0) {
    GetActiveHead().Jump(-Register(REG_BX));
    return;
  }

  // otherwise jump to the complement label.
  const cCPUHead jump_location(FindLabel(-1));
  if ( jump_location.GetPosition() != -1 ) {
    GetActiveHead().Set(jump_location);
    return;
  }

  // If complement label was not found; record an error.
  organism->Fault(FAULT_LOC_JUMP, FAULT_TYPE_ERROR,
		  "jump-b: No complement label");
}

void cHardwareCPU::Inst_JumpP()
{
  cOrganism * other_organism = organism->GetNeighbor();

  // Make sure the other organism was found and that its hardware is of the
  // same type, or else we won't be able to be parasitic on it.
  if (other_organism == NULL ||
      other_organism->GetHardware().GetType() != GetType()) {
    // Without another organism, its hard to determine if we're dealing
    // with a parasite.  For the moment, we'll assume it is and move on.
    // @CAO Do better!
    organism->GetPhenotype().IsParasite() = true;
    return;
  }

  // Otherwise, grab the hardware from the neighbor, and use it!
  cHardwareCPU & other_hardware = (cHardwareCPU &) other_organism->GetHardware();

  ReadLabel();
  GetLabel().Rotate(1);

  // If there is no label, jump to line BX in creature.
  if (GetLabel().GetSize() == 0) {
    const int new_pos = Register(REG_BX);
    IP().Set(new_pos, &other_hardware);
    organism->GetPhenotype().IsParasite() = true;
    return;
  }

  // otherwise jump to the complement label.
  const cCPUHead jump_location(other_hardware.FindFullLabel(GetLabel()));
  if (jump_location.GetPosition() != -1) {
    IP().Set(jump_location);
    organism->GetPhenotype().IsParasite() = true;
    return;
  }

  // If complement label was not found; record a warning (since the
  // actual neighbors are not under the organisms control, this is not
  // a full-scale error).
  organism->Fault(FAULT_LOC_JUMP, FAULT_TYPE_WARNING,
		  "jump-p: No complement label");
}

void cHardwareCPU::Inst_JumpSelf()
{
  ReadLabel();
  GetLabel().Rotate(1);

  // If there is no label, jump to line BX in creature.
  if (GetLabel().GetSize() == 0) {
    IP().Set(Register(REG_BX), this);
    return;
  }

  // otherwise jump to the complement label.
  const cCPUHead jump_location( FindFullLabel(GetLabel()) );
  if ( jump_location.GetPosition() != -1 ) {
    IP().Set(jump_location);
    return;
  }

  // If complement label was not found; record an error.
  organism->Fault(FAULT_LOC_JUMP, FAULT_TYPE_ERROR,
		  "jump-slf: no complement label");
}

void cHardwareCPU::Inst_Call()
{
  // Put the starting location onto the stack
  const int location = IP().GetPosition();
  StackPush(location);

  // Jump to the compliment label (or by the ammount in the bx register)
  ReadLabel();
  GetLabel().Rotate(1);

  if (GetLabel().GetSize() == 0) {
    IP().Jump(Register(REG_BX));
    return;
  }

  const cCPUHead jump_location(FindLabel(1));
  if (jump_location.GetPosition() != -1) {
    IP().Set(jump_location);
    return;
  }

  // If complement label was not found; record an error.
  organism->Fault(FAULT_LOC_JUMP, FAULT_TYPE_ERROR,
		  "call: no complement label");
}

void cHardwareCPU::Inst_Return()
{
  IP().Set(StackPop());
}

void cHardwareCPU::Inst_Pop()
{
  const int reg_used = FindModifiedRegister(REG_BX);
  Register(reg_used) = StackPop();
}

void cHardwareCPU::Inst_Push()
{
  const int reg_used = FindModifiedRegister(REG_BX);
  StackPush(Register(reg_used));
}

void cHardwareCPU::Inst_HeadPop()
{
  const int head_used = FindModifiedHead(HEAD_IP);
  GetHead(head_used).Set(StackPop(), this);
}

void cHardwareCPU::Inst_HeadPush()
{
  const int head_used = FindModifiedHead(HEAD_IP);
  StackPush(GetHead(head_used).GetPosition());
  if (head_used == HEAD_IP) {
    GetHead(head_used).Set(GetHead(HEAD_FLOW));
    advance_ip = false;
  }
}

void cHardwareCPU::Inst_PopA() { Register(REG_AX) = StackPop(); }
void cHardwareCPU::Inst_PopB() { Register(REG_BX) = StackPop(); }
void cHardwareCPU::Inst_PopC() { Register(REG_CX) = StackPop(); }

void cHardwareCPU::Inst_PushA() { StackPush(Register(REG_AX)); }
void cHardwareCPU::Inst_PushB() { StackPush(Register(REG_AX)); }
void cHardwareCPU::Inst_PushC() { StackPush(Register(REG_AX)); }

void cHardwareCPU::Inst_SwitchStack() { SwitchStack(); }
void cHardwareCPU::Inst_FlipStack()   { StackFlip(); }

void cHardwareCPU::Inst_Swap()
{
  const int reg_used = FindModifiedRegister(REG_BX);
  const int other_reg = FindComplementRegister(reg_used);
  Swap(Register(reg_used), Register(other_reg));
}

void cHardwareCPU::Inst_SwapAB() { Swap(Register(REG_AX), Register(REG_BX)); }
void cHardwareCPU::Inst_SwapBC() { Swap(Register(REG_BX), Register(REG_CX)); }
void cHardwareCPU::Inst_SwapAC() { Swap(Register(REG_AX), Register(REG_CX)); }

void cHardwareCPU::Inst_CopyReg()
{
  const int reg_used = FindModifiedRegister(REG_BX);
  const int other_reg = FindComplementRegister(reg_used);
  Register(other_reg) = Register(reg_used);
}

void cHardwareCPU::Inst_CopyRegAB() { Register(REG_AX) = Register(REG_BX); }
void cHardwareCPU::Inst_CopyRegAC() { Register(REG_AX) = Register(REG_CX); }
void cHardwareCPU::Inst_CopyRegBA() { Register(REG_BX) = Register(REG_AX); }
void cHardwareCPU::Inst_CopyRegBC() { Register(REG_BX) = Register(REG_CX); }
void cHardwareCPU::Inst_CopyRegCA() { Register(REG_CX) = Register(REG_AX); }
void cHardwareCPU::Inst_CopyRegCB() { Register(REG_CX) = Register(REG_BX); }

void cHardwareCPU::Inst_Reset()
{
  Register(REG_AX) = 0;
  Register(REG_BX) = 0;
  Register(REG_CX) = 0;
  StackClear();
}

void cHardwareCPU::Inst_ShiftR()
{
  const int reg_used = FindModifiedRegister(REG_BX);
  Register(reg_used) >>= 1;
}

void cHardwareCPU::Inst_ShiftL()
{
  const int reg_used = FindModifiedRegister(REG_BX);
  Register(reg_used) <<= 1;
}

void cHardwareCPU::Inst_Bit1()
{
  const int reg_used = FindModifiedRegister(REG_BX);
  Register(reg_used) |=  1;
}

void cHardwareCPU::Inst_SetNum()
{
  ReadLabel();
  Register(REG_BX) = GetLabel().AsInt();
}

void cHardwareCPU::Inst_Inc()
{
  const int reg_used = FindModifiedRegister(REG_BX);
  Register(reg_used) += 1;
}

void cHardwareCPU::Inst_Dec()
{
  const int reg_used = FindModifiedRegister(REG_BX);
  Register(reg_used) -= 1;
}

void cHardwareCPU::Inst_Zero()
{
  const int reg_used = FindModifiedRegister(REG_BX);
  Register(reg_used) = 0;
}

void cHardwareCPU::Inst_Neg()
{
  const int reg_used = FindModifiedRegister(REG_BX);
  Register(reg_used) = 0-Register(reg_used);
}

void cHardwareCPU::Inst_Square()
{
  const int reg_used = FindModifiedRegister(REG_BX);
  Register(reg_used) = Register(reg_used) * Register(reg_used);
}

void cHardwareCPU::Inst_Sqrt()
{
  const int reg_used = FindModifiedRegister(REG_BX);
  const int value = Register(reg_used);
  if (value > 1) Register(reg_used) = (int) sqrt((double) value);
  else if (value < 0) {
    Fault(FAULT_LOC_MATH, FAULT_TYPE_ERROR, "sqrt: value is negative");
  }
}

void cHardwareCPU::Inst_Minus17()
{
  const int reg_used = FindModifiedRegister(REG_BX);
  Register(reg_used) -= 17;
}

void cHardwareCPU::Inst_Add()
{
  const int reg_used = FindModifiedRegister(REG_BX);
  Register(reg_used) = Register(REG_BX) + Register(REG_CX);
}

void cHardwareCPU::Inst_Sub()
{
  const int reg_used = FindModifiedRegister(REG_BX);
  Register(reg_used) = Register(REG_BX) - Register(REG_CX);
}

void cHardwareCPU::Inst_Mult()
{
  const int reg_used = FindModifiedRegister(REG_BX);
  Register(reg_used) = Register(REG_BX) * Register(REG_CX);
}

void cHardwareCPU::Inst_Div()
{
  const int reg_used = FindModifiedRegister(REG_BX);
  if (Register(REG_CX) != 0) {
    Register(reg_used) = Register(REG_BX) / Register(REG_CX);
  } else {
    Fault(FAULT_LOC_MATH, FAULT_TYPE_ERROR, "div: dividing by 0");
  }
}

void cHardwareCPU::Inst_Mod()
{
  const int reg_used = FindModifiedRegister(REG_BX);
  if (Register(REG_CX) != 0) {
    Register(reg_used) = Register(REG_BX) % Register(REG_CX);
  } else {
    Fault(FAULT_LOC_MATH, FAULT_TYPE_ERROR, "mod: modding by 0");
  }
}

void cHardwareCPU::Inst_Nand()
{
  const int reg_used = FindModifiedRegister(REG_BX);
  Register(reg_used) = ~(Register(REG_BX) & Register(REG_CX));
}

void cHardwareCPU::Inst_Nor()
{
  const int reg_used = FindModifiedRegister(REG_BX);
  Register(reg_used) = ~(Register(REG_BX) | Register(REG_CX));
}

void cHardwareCPU::Inst_And()
{
  const int reg_used = FindModifiedRegister(REG_BX);
  Register(reg_used) = (Register(REG_BX) & Register(REG_CX));
}

void cHardwareCPU::Inst_Not()
{
  const int reg_used = FindModifiedRegister(REG_BX);
  Register(reg_used) = ~(Register(reg_used));
}

void cHardwareCPU::Inst_Order()
{
  if (Register(REG_BX) > Register(REG_CX)) {
    Swap(Register(REG_BX), Register(REG_CX));
  }
}

void cHardwareCPU::Inst_Xor()
{
  const int reg_used = FindModifiedRegister(REG_BX);
  Register(reg_used) = Register(REG_BX) ^ Register(REG_CX);
}

void cHardwareCPU::Inst_Copy()
{
  const cCPUHead from(this, Register(REG_BX));
  cCPUHead to(this, Register(REG_AX) + Register(REG_BX));
  sCPUStats & cpu_stats = organism->CPUStats();

  if (organism->TestCopyMut()) {
    to.SetInst(GetRandomInst());
    to.FlagMutated() = true;  // Mark this instruction as mutated...
    to.FlagCopyMut() = true;  // Mark this instruction as copy mut...
    //organism->GetPhenotype().IsMutated() = true;
    cpu_stats.mut_stats.copy_mut_count++;
  } else {
    to.SetInst(from.GetInst());
    to.FlagMutated() = false;  // UnMark
    to.FlagCopyMut() = false;  // UnMark
  }

  to.FlagCopied() = true;  // Set the copied flag.
  cpu_stats.mut_stats.copies_exec++;
}

void cHardwareCPU::Inst_ReadInst()
{
  const int reg_used = FindModifiedRegister(REG_CX);
  const cCPUHead from(this, Register(REG_BX));

  // Dis-allowing mutations on read, for the moment (write only...)
  // @CAO This allows perfect error-correction...
  Register(reg_used) = from.GetInst().GetOp();
}

void cHardwareCPU::Inst_WriteInst()
{
  cCPUHead to(this, Register(REG_AX) + Register(REG_BX));
  const int reg_used = FindModifiedRegister(REG_CX);
  const int value = Mod(Register(reg_used), GetNumInst());
  sCPUStats & cpu_stats = organism->CPUStats();

  // Change value on a mutation...
  if (organism->TestCopyMut()) {
    to.SetInst(GetRandomInst());
    to.FlagMutated() = true;      // Mark this instruction as mutated...
    to.FlagCopyMut() = true;      // Mark this instruction as copy mut...
    //organism->GetPhenotype().IsMutated() = true;
    cpu_stats.mut_stats.copy_mut_count++;
  } else {
    to.SetInst(cInstruction(value));
    to.FlagMutated() = false;     // UnMark
    to.FlagCopyMut() = false;     // UnMark
  }

  to.FlagCopied() = true;  // Set the copied flag.
  cpu_stats.mut_stats.copies_exec++;
}

void cHardwareCPU::Inst_StackReadInst()
{
  const int reg_used = FindModifiedRegister(REG_CX);
  cCPUHead from(this, Register(reg_used));
  StackPush(from.GetInst().GetOp());
}

void cHardwareCPU::Inst_StackWriteInst()
{
  const int reg_used = FindModifiedRegister(REG_BX);
  cCPUHead to(this, Register(REG_AX) + Register(reg_used));
  const int value = Mod(StackPop(), GetNumInst());
  sCPUStats & cpu_stats = organism->CPUStats();

  // Change value on a mutation...
  if (organism->TestCopyMut()) {
    to.SetInst(GetRandomInst());
    to.FlagMutated() = true;      // Mark this instruction as mutated...
    to.FlagCopyMut() = true;      // Mark this instruction as copy mut...
    //organism->GetPhenotype().IsMutated() = true;
    cpu_stats.mut_stats.copy_mut_count++;
  } else {
    to.SetInst(cInstruction(value));
    to.FlagMutated() = false;     // UnMark
    to.FlagCopyMut() = false;     // UnMark
  }

  to.FlagCopied() = true;  // Set the copied flag.
  cpu_stats.mut_stats.copies_exec++;
}

void cHardwareCPU::Inst_Compare()
{
  const int reg_used = FindModifiedRegister(REG_CX);
  cCPUHead from(this, Register(REG_BX));
  cCPUHead to(this, Register(REG_AX) + Register(REG_BX));

  // Compare is dangerous -- it can cause mutations!
  if (organism->TestCopyMut()) {
    to.SetInst(GetRandomInst());
    to.FlagMutated() = true;      // Mark this instruction as mutated...
    to.FlagCopyMut() = true;      // Mark this instruction as copy mut...
    //organism->GetPhenotype().IsMutated() = true;
  }

  Register(reg_used) = from.GetInst().GetOp() - to.GetInst().GetOp();
}

void cHardwareCPU::Inst_IfNCpy()
{
  const cCPUHead from(this, Register(REG_BX));
  const cCPUHead to(this, Register(REG_AX) + Register(REG_BX));

  // Allow for errors in this test...
  if (organism->TestCopyMut()) {
    if (from.GetInst() != to.GetInst()) IP().Advance();
  } else {
    if (from.GetInst() == to.GetInst()) IP().Advance();
  }
}

void cHardwareCPU::Inst_Allocate()   // Allocate bx more space...
{
  const int size = GetMemory().GetSize();
  if( Allocate_Main(Register(REG_BX)) )  Register(REG_AX) = size;
}

void cHardwareCPU::Inst_Divide()  { Divide_Main(Register(REG_AX));  }
void cHardwareCPU::Inst_CDivide() { Divide_Main(GetMemory().GetSize() / 2); }
void cHardwareCPU::Inst_CAlloc()  { Allocate_Main(GetMemory().GetSize()); }

void cHardwareCPU::Inst_MaxAlloc()   // Allocate maximal more
{
  const int cur_size = GetMemory().GetSize();
  const int alloc_size = Min((int) (cConfig::GetChildSizeRange() * cur_size),
			     MAX_CREATURE_SIZE - cur_size);
  if( Allocate_Main(alloc_size) )  Register(REG_AX) = cur_size;
}


void cHardwareCPU::Inst_Repro()
{
  // Setup child
  cCPUMemory & child_genome = organism->ChildGenome();
  child_genome = GetMemory();
  organism->GetPhenotype().SetLinesCopied(GetMemory().GetSize());

  int lines_executed = 0;
  for ( int i = 0; i < GetMemory().GetSize(); i++ ) {
    if ( GetMemory().FlagExecuted(i) == true ) lines_executed++;
  }
  organism->GetPhenotype().SetLinesExecuted(lines_executed);

  // Perform Copy Mutations...
  if (organism->GetCopyMutProb() > 0) { // Skip this if no mutations....
    for (int i = 0; i < GetMemory().GetSize(); i++) {
      if (organism->TestCopyMut()) {
	child_genome[i]=GetRandomInst();
	//organism->GetPhenotype().IsMutated() = true;
      }
    }
  }
  Divide_DoMutations();

  // Many tests will require us to run the offspring through a test CPU;
  // this is, for example, to see if mutations need to be reverted or if
  // lineages need to be updated.
  Divide_TestFitnessMeasures();

  // Build the new organism...
  cOrganism * child = organism->Divide_SetupChild();

#ifdef INSTRUCTION_COSTS
  // reset first time instruction costs
  for (int i = 0; i < inst_ft_cost.GetSize(); i++) {
    inst_ft_cost[i] = GetInstLib().GetFTCost(cInstruction(i));
  }
#endif

  if (cConfig::GetDivideMethod() == DIVIDE_METHOD_SPLIT) advance_ip = false;

  organism->ActivateDivide(child);
}

// The inject instruction can be used instead of a divide command, paired
// with an allocate.  Note that for an inject to work, one needs to have a
// broad range for sizes allowed to be allocated.
//
// This command will cut out from read-head to write-head.
// It will then look at the template that follows the command and inject it
// into the complement template found in a neighboring organism.

void cHardwareCPU::Inst_Inject()
{
  AdjustHeads();
  const int start_pos = GetHead(HEAD_READ).GetPosition();
  const int end_pos = GetHead(HEAD_WRITE).GetPosition();
  const int inject_size = end_pos - start_pos;

  // Make sure the creature will still be above the minimum size,
  if (inject_size <= 0) {
    Fault(FAULT_LOC_INJECT, FAULT_TYPE_ERROR, "inject: no code to inject");
    return; // (inject fails)
  }
  if (start_pos < MIN_CREATURE_SIZE) {
    Fault(FAULT_LOC_INJECT, FAULT_TYPE_ERROR, "inject: new size too small");
    return; // (inject fails)
  }

  // Since its legal to cut out the injected piece, do so.
  cGenome inject_code( cGenomeUtil::Crop(GetMemory(), start_pos, end_pos) );
  GetMemory().Remove(start_pos, inject_size);

  // If we don't have a host, stop here.
  cOrganism * host_organism = organism->GetNeighbor();
  if (host_organism == NULL) return;

  // Scan for the label to match...
  ReadLabel();

  // If there is no label, abort.
  if (GetLabel().GetSize() == 0) {
    Fault(FAULT_LOC_INJECT, FAULT_TYPE_ERROR, "inject: label required");
    return; // (inject fails)
  }

  // Search for the label in the host...
  GetLabel().Rotate(1);

  const int inject_signal =
    host_organism->GetHardware().Inject(GetLabel(), inject_code);
  if (inject_signal == 1) {
    Fault(FAULT_LOC_INJECT, FAULT_TYPE_WARNING, "inject: host too large.");
    return; // Inject failed.
  }
  if (inject_signal == 2) {
    Fault(FAULT_LOC_INJECT, FAULT_TYPE_WARNING, "inject: target not in host.");
    return; // Inject failed.
  }

  // Set the relevent flags.
  organism->GetPhenotype().IsModifier() = true;
}


void cHardwareCPU::Inst_InjectRand()
{
  // Rotate to a random facing and then run the normal inject instruction
  const int num_neighbors = organism->GetNeighborhoodSize();
  organism->Rotate(g_random.GetUInt(num_neighbors));
  Inst_Inject();
}

void cHardwareCPU::Inst_TaskGet()
{
  const int reg_used = FindModifiedRegister(REG_CX);
  const int value = organism->GetInput();
  Register(reg_used) = value;
  DoInput(value);
}

void cHardwareCPU::Inst_TaskStackGet()
{
  const int value = organism->GetInput();
  StackPush(value);
  DoInput(value);
}

void cHardwareCPU::Inst_TaskStackLoad()
{
  for (int i = 0; i < IO_SIZE; i++) StackPush( organism->GetInput() );
}

void cHardwareCPU::Inst_TaskPut()
{
  const int reg_used = FindModifiedRegister(REG_BX);
  const int value = Register(reg_used);
  Register(reg_used) = 0;
  DoOutput(value);
}

void cHardwareCPU::Inst_TaskIO()
{
  const int reg_used = FindModifiedRegister(REG_BX);

  // Do the "put" component
  const int value_out = Register(reg_used);
  DoOutput(value_out);  // Check for tasks compleated.

  // Do the "get" component
  const int value_in = organism->GetInput();
  Register(reg_used) = value_in;
  DoInput(value_in);
}

void cHardwareCPU::Inst_SearchF()
{
  ReadLabel();
  GetLabel().Rotate(1);
  const int search_size = FindLabel(1).GetPosition() - IP().GetPosition();
  Register(REG_BX) = search_size;
  Register(REG_CX) = GetLabel().GetSize();
}

void cHardwareCPU::Inst_SearchB()
{
  ReadLabel();
  GetLabel().Rotate(1);
  const int search_size = IP().GetPosition() - FindLabel(-1).GetPosition();
  Register(REG_BX) = search_size;
  Register(REG_CX) = GetLabel().GetSize();
}

void cHardwareCPU::Inst_MemSize()
{
  Register(FindModifiedRegister(REG_BX)) = GetMemory().GetSize();
}


void cHardwareCPU::Inst_RotateL()
{
  const int num_neighbors = organism->GetNeighborhoodSize();

   // If this organism has no neighbors, ignore rotate.
  if (num_neighbors == 0) return;

  ReadLabel();

  // Always rotate at least once.
  organism->Rotate(-1);

  // If there is no label, then the one rotation was all we want.
  if (!GetLabel().GetSize()) return;

  // Rotate until a complement label is found (or all have been checked).
  GetLabel().Rotate(1);
  for (int i = 1; i < num_neighbors; i++) {
    cOrganism * neighbor = organism->GetNeighbor();

    // Assuming we have a neighbor and it is of the same hardware type,
    // search for the label in it.
    if (neighbor != NULL &&
	neighbor->GetHardware().GetType() == GetType()) {

      // If this facing has the full label, stop here.
      cHardwareCPU & cur_hardware = (cHardwareCPU &) neighbor->GetHardware();
      if (cur_hardware.FindFullLabel( GetLabel() ).InMemory()) return;
    }

    // Otherwise keep rotating...
    organism->Rotate(-1);
  }
}

void cHardwareCPU::Inst_RotateR()
{
  const int num_neighbors = organism->GetNeighborhoodSize();

   // If this organism has no neighbors, ignore rotate.
  if (num_neighbors == 0) return;

  ReadLabel();

  // Always rotate at least once.
  organism->Rotate(-1);

  // If there is no label, then the one rotation was all we want.
  if (!GetLabel().GetSize()) return;

  // Rotate until a complement label is found (or all have been checked).
  GetLabel().Rotate(1);
  for (int i = 1; i < num_neighbors; i++) {
    cOrganism * neighbor = organism->GetNeighbor();

    // Assuming we have a neighbor and it is of the same hardware type,
    // search for the label in it.
    if (neighbor != NULL &&
	neighbor->GetHardware().GetType() == GetType()) {

      // If this facing has the full label, stop here.
      cHardwareCPU & cur_hardware = (cHardwareCPU &) neighbor->GetHardware();
      if (cur_hardware.FindFullLabel( GetLabel() ).InMemory()) return;
    }

    // Otherwise keep rotating...
    organism->Rotate(1);
  }
}

void cHardwareCPU::Inst_SetCopyMut()
{
  const int reg_used = FindModifiedRegister(REG_BX);
  const int new_mut_rate = Max( Register(reg_used), 1 );
  organism->SetCopyMutProb(((double) new_mut_rate) / 10000.0);
}

void cHardwareCPU::Inst_ModCopyMut()
{
  const int reg_used = FindModifiedRegister(REG_BX);
  const double new_mut_rate = organism->GetCopyMutProb() +
    ((double) Register(reg_used)) / 10000.0;
  if (new_mut_rate > 0.0) organism->SetCopyMutProb(new_mut_rate);
}


// Multi-threading.

void cHardwareCPU::Inst_ForkThread()
{
  IP().Advance();
  if (!ForkThread()) Fault(FAULT_LOC_THREAD_FORK, FAULT_TYPE_FORK_TH);
}

void cHardwareCPU::Inst_KillThread()
{
  if (!KillThread()) Fault(FAULT_LOC_THREAD_KILL, FAULT_TYPE_KILL_TH);
  else advance_ip = false;
}

void cHardwareCPU::Inst_ThreadID()
{
  const int reg_used = FindModifiedRegister(REG_BX);
  Register(reg_used) = GetCurThreadID();
}


// Head-based instructions

void cHardwareCPU::Inst_SetHead()
{
  const int head_used = FindModifiedHead(HEAD_IP);
  SetActiveHead(head_used);
}

void cHardwareCPU::Inst_AdvanceHead()
{
  const int head_used = FindModifiedHead(HEAD_WRITE);
  GetHead(head_used).Advance();
}
 
void cHardwareCPU::Inst_MoveHead()
{
  const int head_used = FindModifiedHead(HEAD_IP);
  GetHead(head_used).Set(GetHead(HEAD_FLOW));
  if (head_used == HEAD_IP) advance_ip = false;
}

void cHardwareCPU::Inst_JumpHead()
{
  const int head_used = FindModifiedHead(HEAD_IP);
  GetHead(head_used).Jump( Register(REG_CX) );
}

void cHardwareCPU::Inst_GetHead()
{
  const int head_used = FindModifiedHead(HEAD_IP);
  Register(REG_CX) = GetHead(head_used).GetPosition();
}

void cHardwareCPU::Inst_IfLabel()
{
  ReadLabel();
  GetLabel().Rotate(1);
  if (GetLabel() != GetReadLabel())  IP().Advance();
}

void cHardwareCPU::Inst_HeadDivide()
{
  AdjustHeads();
  const int divide_pos = GetHead(HEAD_READ).GetPosition();
  int child_end =  GetHead(HEAD_WRITE).GetPosition();
  if (child_end == 0) child_end = GetMemory().GetSize();
  const int extra_lines = GetMemory().GetSize() - child_end;
  Divide_Main(divide_pos, extra_lines);

  // Re-adjust heads.
  AdjustHeads();
}

void cHardwareCPU::Inst_HeadRead()
{
  const int head_id = FindModifiedHead(HEAD_READ);
  GetHead(head_id).Adjust();
  sCPUStats & cpu_stats = organism->CPUStats();

  // Mutations only occur on the read, for the moment.
  int read_inst = 0;
  if (organism->TestCopyMut()) {
    read_inst = GetRandomInst().GetOp();
    cpu_stats.mut_stats.copy_mut_count++;  // @CAO, hope this is good!
  } else {
    read_inst = GetHead(head_id).GetInst().GetOp();
  }
  Register(REG_BX) = read_inst;
  ReadInst(read_inst);

  cpu_stats.mut_stats.copies_exec++;  // @CAO, this too..
  GetHead(head_id).Advance();
}

void cHardwareCPU::Inst_HeadWrite()
{
  const int head_id = FindModifiedHead(HEAD_WRITE);
  cCPUHead & active_head = GetHead(head_id);

  active_head.Adjust();

  int value = Register(REG_BX);
  if (value < 0 || value >= GetNumInst()) value = 0;

  active_head.SetInst(cInstruction(value));
  active_head.FlagCopied() = true;

  // Advance the head after write...
  active_head++;
}

void cHardwareCPU::Inst_HeadCopy()
{
  // For the moment, this cannot be nop-modified.
  cCPUHead & read_head = GetHead(HEAD_READ);
  cCPUHead & write_head = GetHead(HEAD_WRITE);
  sCPUStats & cpu_stats = organism->CPUStats();

  read_head.Adjust();
  write_head.Adjust();

  // Do mutations.
  cInstruction read_inst = read_head.GetInst();
  if (organism->TestCopyMut()) {
    read_inst = GetRandomInst();
    cpu_stats.mut_stats.copy_mut_count++; 
    write_head.FlagMutated() = true;
    write_head.FlagCopyMut() = true;
    //organism->GetPhenotype().IsMutated() = true;
  }
  ReadInst(read_inst.GetOp());

  cpu_stats.mut_stats.copies_exec++;

  write_head.SetInst(read_inst);
  write_head.FlagCopied() = true;  // Set the copied flag...

  read_head.Advance();
  write_head.Advance();
}

void cHardwareCPU::HeadCopy_ErrorCorrect(double reduction)
{
  // For the moment, this cannot be nop-modified.
  cCPUHead & read_head = GetHead(HEAD_READ);
  cCPUHead & write_head = GetHead(HEAD_WRITE);
  sCPUStats & cpu_stats = organism->CPUStats();

  read_head.Adjust();
  write_head.Adjust();

  // Do mutations.
  cInstruction read_inst = read_head.GetInst();
  if ( g_random.P(organism->GetCopyMutProb() / reduction) ) {
    read_inst = GetRandomInst();
    cpu_stats.mut_stats.copy_mut_count++; 
    write_head.FlagMutated() = true;
    write_head.FlagCopyMut() = true;
    //organism->GetPhenotype().IsMutated() = true;
  }
  ReadInst(read_inst.GetOp());

  cpu_stats.mut_stats.copies_exec++;

  write_head.SetInst(read_inst);
  write_head.FlagCopied() = true;  // Set the copied flag...

  read_head.Advance();
  write_head.Advance();
}

void cHardwareCPU::Inst_HeadCopy2()  { HeadCopy_ErrorCorrect(2); }
void cHardwareCPU::Inst_HeadCopy3()  { HeadCopy_ErrorCorrect(3); }
void cHardwareCPU::Inst_HeadCopy4()  { HeadCopy_ErrorCorrect(4); }
void cHardwareCPU::Inst_HeadCopy5()  { HeadCopy_ErrorCorrect(5); }
void cHardwareCPU::Inst_HeadCopy6()  { HeadCopy_ErrorCorrect(6); }
void cHardwareCPU::Inst_HeadCopy7()  { HeadCopy_ErrorCorrect(7); }
void cHardwareCPU::Inst_HeadCopy8()  { HeadCopy_ErrorCorrect(8); }
void cHardwareCPU::Inst_HeadCopy9()  { HeadCopy_ErrorCorrect(9); }
void cHardwareCPU::Inst_HeadCopy10() { HeadCopy_ErrorCorrect(10); }

void cHardwareCPU::Inst_HeadSearch()
{
  ReadLabel();
  GetLabel().Rotate(1);
  cCPUHead found_pos = FindLabel(0);
  const int search_size = found_pos.GetPosition() - IP().GetPosition();
  Register(REG_BX) = search_size;
  Register(REG_CX) = GetLabel().GetSize();
  GetHead(HEAD_FLOW).Set(found_pos);
  GetHead(HEAD_FLOW).Advance();
}

void cHardwareCPU::Inst_SetFlow()
{
  const int reg_used = FindModifiedRegister(REG_CX);
  GetHead(HEAD_FLOW).Set(Register(reg_used), this);
}

// Direct Matching Templates

void cHardwareCPU::Inst_DMJumpF()
{
  ReadLabel();

  // If there is no label, jump BX steps.
  if (GetLabel().GetSize() == 0) {
    IP().Jump(Register(REG_BX));
    return;
  }

  // Otherwise, jump to the label.
  cCPUHead jump_location(FindLabel(1));
  if (jump_location.GetPosition() != -1) {
    IP().Set(jump_location);
    return;
  }

  // If complement label was not found; record an error.
  organism->Fault(FAULT_LOC_JUMP, FAULT_TYPE_ERROR,
		  "dm-jump-f: no complement label");
}


//// Placebo insts ////
void cHardwareCPU::Inst_Skip()
{
  IP().Advance();
}


