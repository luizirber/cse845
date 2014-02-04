//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2002 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef HARDWARE_CPU_HH
#define HARDWARE_CPU_HH

#include "hardware_base.hh"

#include "../defs.hh"

#include "../tools/functions.hh"
#include "../tools/tBuffer.hh"

#include "cpu_memory.hh"
#include "cpu_stack.hh"
#include "cpu_defs.hh"
#include "label.hh"
#include "head.hh"

class cInstLib;
class cOrganism;

/**
 * This class is needed to run several threads on a single genome.
 *
 * @see cCPUStack, cCPUHead, cHardwareCPU
 **/

struct cHardwareCPU_Thread {
private:
  int id;
public:
  int reg[NUM_REGISTERS];
  cCPUHead heads[NUM_HEADS];
  cCPUStack stack;
  UCHAR cur_stack;              // 0 = local stack, 1 = global stack.
  UCHAR cur_head;

  UCHAR input_pointer;
  tBuffer<int> input_buf;
  tBuffer<int> output_buf;
  cCodeLabel read_label;
  cCodeLabel next_label;
public:
  cHardwareCPU_Thread(cHardwareBase * in_hardware=NULL, int _id=-1);
  cHardwareCPU_Thread(const cHardwareCPU_Thread & in_thread, int _id=-1);
  ~cHardwareCPU_Thread();

  void operator=(const cHardwareCPU_Thread & in_thread);

  void Reset(cHardwareBase * in_hardware, int _id);
  int GetID() { return id; }
  void SetID(int _id) { id = _id; }

  void SaveState(ostream & fp);
  void LoadState(istream & fp);
};

///////////////////////////////////////////////////////////////////
// The following is a speed up for single-threaded runs.
//
// The variable cur_thread need only be used in multi-threaded runs.  When
// we are only going to have a single threads, we can allow cur_thread to
// always be 0.
///////////////////////////////////////////////////////////////////

#ifdef THREADS   // Multiple Threads Allowed
# define CUR_THREAD cur_thread

# ifdef SINGLE_IO_BUFFER   // For Single IOBuffer vs IOBuffer for each Thread
#  define IO_THREAD 0
# else
#  define IO_THREAD cur_thread
# endif

#else  // Single Thread
# define CUR_THREAD 0
# define IO_THREAD 0
#endif


/**
 * Each organism may have a cHardwareCPU structure which keeps track of the
 * current status of all the components of the simulated hardware.
 *
 * @see cHardwareCPU_Thread, cCPUStack, cCPUMemory, cInstLib
 **/

class cHardwareCPU : public cHardwareBase {
private:
  cCPUMemory memory;          // Memory...
  cCPUStack global_stack;     // A stack that all threads share.
  int thread_time_used;

  cHardwareCPU_Thread * threads;
  int thread_id_chart;
  int num_threads;
  int cur_thread;

  // Flags...
  bool mal_active;         // Has an allocate occured since last divide?
  bool advance_ip;         // Should the IP advance after this instruction?

  // Instruction costs...
#ifdef INSTRUCTION_COSTS
  tArray<int> inst_cost;
  tArray<int> inst_ft_cost;
#endif

public:
  cHardwareCPU(cOrganism * in_organism, cInstLib * in_inst_lib);
  ~cHardwareCPU();
  void Recycle(cOrganism * new_organism, cInstLib * in_inst_lib);

  void Reset();
  void SingleProcess(ostream * trace_fp=NULL);
  void LoadGenome(const cGenome & new_genome);

  // --------  Helper methods  --------
  bool OK();
  void PrintStatus(ostream & fp);
  void SetupProcess();


  // --------  Flag Accessors --------
  bool GetMalActive() const   { return mal_active; }

  // --------  Stack Manipulation...  --------
  inline void StackPush(int value);
  inline int StackPop();
  inline void StackFlip();
  inline int GetStack(int depth=0, int stack_id=-1) const;
  inline void StackClear();
  inline void SwitchStack();
  int GetActiveStackID() const { return threads[CUR_THREAD].cur_stack; }

  // --------  Tasks & IO  --------
  tBuffer<int> & GetInputBuffer() { return threads[CUR_THREAD].input_buf; }
  tBuffer<int> & GetOutputBuffer() { return threads[CUR_THREAD].output_buf; }
  void DoInput(const int value);
  void DoOutput(const int value);

  // --------  Head Manipulation (including IP)  --------
  inline void SetActiveHead(const int new_head)
  { threads[CUR_THREAD].cur_head = (UCHAR) new_head; }

  int GetCurHead() const { return threads[CUR_THREAD].cur_head; }
  const cCPUHead & GetHead(int head_id) const
    { return threads[CUR_THREAD].heads[head_id]; }
  cCPUHead & GetHead(int head_id) { return threads[CUR_THREAD].heads[head_id];}

  const cCPUHead & GetActiveHead() const { return GetHead(GetCurHead()); }
  cCPUHead & GetActiveHead() { return GetHead(GetCurHead()); }

  void AdjustHeads();

  inline const cCPUHead & IP() const
    { return threads[CUR_THREAD].heads[HEAD_IP]; }
  inline cCPUHead & IP() { return threads[CUR_THREAD].heads[HEAD_IP]; }


  // --------  Label Manipulation  -------
  void ReadLabel(int max_size=MAX_LABEL_SIZE);
  cCodeLabel & GetLabel() { return threads[CUR_THREAD].next_label; }
  cCodeLabel & GetReadLabel() { return threads[CUR_THREAD].read_label; }


  // --------  Register Manipulation  --------
  int Register(int reg_id) const { return threads[CUR_THREAD].reg[reg_id]; }
  int & Register(int reg_id) { return threads[CUR_THREAD].reg[reg_id]; }

  // --------  Memory Manipulation  --------
  inline const cCPUMemory & Memory() const { return memory; }
  inline cCPUMemory & Memory() { return memory; }

  // --------  Thread Manipulation  --------
  bool ForkThread(); // Adds a new thread based off of cur_thread.
  bool KillThread(); // Kill the current thread!
  inline void PrevThread(); // Shift the current thread in use.
  inline void NextThread();

  // --------  Tests  --------

  int TestParasite() const;

  // --------  Accessors  --------
  const cCPUMemory & GetMemory() const { return memory; }
  cCPUMemory & GetMemory() { return memory; }

  int GetThreadTimeUsed() const { return thread_time_used; }
  int GetNumThreads() const     { return num_threads; }
  int GetCurThread() const      { return CUR_THREAD; }
  int GetCurThreadID() const    { return threads[CUR_THREAD].GetID(); }

  int GetThreadDist() const {
    if (num_threads == 1) return 0;
    return threads[0].heads[HEAD_IP].GetPosition() -
      threads[1].heads[HEAD_IP].GetPosition();
  }

  // Complex label manipulation...
  cCPUHead FindLabel(int direction);
  int FindLabel_Forward(const cCodeLabel & search_label,
			  const cGenome & search_genome, int pos);
  int FindLabel_Backward(const cCodeLabel & search_label,
			  const cGenome & search_genome, int pos);
  cCPUHead FindLabel(const cCodeLabel & in_label, int direction);
  cCPUHead FindFullLabel(const cCodeLabel & in_label);

  int GetType() const { return HARDWARE_TYPE_VIRTUAL_CPU; }
  int Inject(const cCodeLabel & in_label, const cGenome & injection);
  void InjectCode(const cGenome & injection, const int line_num);

  void Mutate(const int mut_point);
  int PointMutate(const double mut_rate);

  void ReadInst(const int in_inst) const;

  void SaveState(ostream & fp);
  void LoadState(istream & fp);

  void InitInstLib(const cString & filename, cInstLib & lib);


private:
 
 /////////---------- Instruction Helpers ------------//////////

  int FindModifiedRegister(int default_register);
  int FindModifiedHead(int default_head);
  int FindComplementRegister(int base_reg);

  void Fault(int fault_loc, int fault_type, cString fault_desc="");

  void DoCrossoverMutation(int start1, int end1,
			   cCPUMemory & in, int start2, int end2);

  bool Allocate_Necro(const int old_size, const int new_size);
  bool Allocate_Random(const int old_size, const int new_size);
  bool Allocate_Default(const int old_size, const int new_size);
  bool Allocate_Main(const int allocated_size);

  bool Divide_Main(const int divide_point, const int extra_lines=0);

  bool Divide_CheckViable(const int child_size, const int parent_size,
			  const int extra_lines);
  void Divide_DoMutations();
  void Divide_TestFitnessMeasures();

  void HeadCopy_ErrorCorrect(double reduction);

public:
  /////////---------- Instruction Library ------------//////////

  // Flow Control
  void Inst_If0();
  void Inst_IfEqu();
  void Inst_IfNot0();
  void Inst_IfNEqu();
  void Inst_IfGr0();
  void Inst_IfGr();
  void Inst_IfGrEqu0();
  void Inst_IfGrEqu();
  void Inst_IfLess0();
  void Inst_IfLess();
  void Inst_IfLsEqu0();
  void Inst_IfLsEqu();
  void Inst_IfBit1();
  void Inst_IfANotEqB();
  void Inst_IfBNotEqC();
  void Inst_IfANotEqC();

  void Inst_JumpF();
  void Inst_JumpB();
  void Inst_JumpP();
  void Inst_JumpSelf();
  void Inst_Call();
  void Inst_Return();

  // Stack and Register Operations
  void Inst_Pop();
  void Inst_Push();
  void Inst_HeadPop();
  void Inst_HeadPush();

  void Inst_PopA();
  void Inst_PopB();
  void Inst_PopC();
  void Inst_PushA();
  void Inst_PushB();
  void Inst_PushC();

  void Inst_SwitchStack();
  void Inst_FlipStack();
  void Inst_Swap();
  void Inst_SwapAB();
  void Inst_SwapBC();
  void Inst_SwapAC();
  void Inst_CopyReg();
  void Inst_CopyRegAB();
  void Inst_CopyRegAC();
  void Inst_CopyRegBA();
  void Inst_CopyRegBC();
  void Inst_CopyRegCA();
  void Inst_CopyRegCB();
  void Inst_Reset();

  // Single-Argument Math
  void Inst_ShiftR();
  void Inst_ShiftL();
  void Inst_Bit1();
  void Inst_SetNum();
  void Inst_Inc();
  void Inst_Dec();
  void Inst_Zero();
  void Inst_Not();
  void Inst_Neg();
  void Inst_Square();
  void Inst_Sqrt();
  void Inst_Minus17();

  // Double Argument Math
  void Inst_Add();
  void Inst_Sub();
  void Inst_Mult();
  void Inst_Div();
  void Inst_Mod();
  void Inst_Nand();
  void Inst_Nor();
  void Inst_And();
  void Inst_Order();
  void Inst_Xor();

  // Biological
  void Inst_Copy();
  void Inst_ReadInst();
  void Inst_WriteInst();
  void Inst_StackReadInst();
  void Inst_StackWriteInst();
  void Inst_Compare();
  void Inst_IfNCpy();
  void Inst_Allocate();
  void Inst_Divide();
  void Inst_CAlloc();
  void Inst_CDivide();
  void Inst_MaxAlloc();
  void Inst_Inject();
  void Inst_InjectRand();
  void Inst_Repro();

  // I/O and Sensory
  void Inst_TaskGet();
  void Inst_TaskStackGet();
  void Inst_TaskStackLoad();
  void Inst_TaskPut();
  void Inst_TaskIO();
  void Inst_SearchF();
  void Inst_SearchB();
  void Inst_MemSize();

  // Environment

  void Inst_RotateL();
  void Inst_RotateR();
  void Inst_SetCopyMut();
  void Inst_ModCopyMut();

  // Multi-threading...

  void Inst_ForkThread();
  void Inst_KillThread();
  void Inst_ThreadID();

  // Head-based instructions...

  void Inst_SetHead();
  void Inst_AdvanceHead();
  void Inst_MoveHead();
  void Inst_JumpHead();
  void Inst_GetHead();
  void Inst_IfLabel();
  void Inst_HeadDivide();
  void Inst_HeadRead();
  void Inst_HeadWrite();
  void Inst_HeadCopy();
  void Inst_HeadSearch();
  void Inst_SetFlow();

  void Inst_HeadCopy2();
  void Inst_HeadCopy3();
  void Inst_HeadCopy4();
  void Inst_HeadCopy5();
  void Inst_HeadCopy6();
  void Inst_HeadCopy7();
  void Inst_HeadCopy8();
  void Inst_HeadCopy9();
  void Inst_HeadCopy10();

  // Direct Matching Templates

  void Inst_DMJumpF();
  void Inst_DMJumpB();
  void Inst_DMCall();
  void Inst_DMSearchF();
  void Inst_DMSearchB();

  // Relative Addressed Jumps

  void Inst_REJumpF();
  void Inst_REJumpB();

  // Absoulte Addressed Jumps

  void Inst_ABSJump();


  // Biologically inspired reproduction
  void Inst_BCAlloc();
  void Inst_BCopy();
  void Inst_BDivide();
private:
  void Inst_BCopy_Main(double mut_prob); // Internal called by all BCopy's
public:
  // Bio Error Correction
  void Inst_BCopyDiv2();
  void Inst_BCopyDiv3();
  void Inst_BCopyDiv4();
  void Inst_BCopyDiv5();
  void Inst_BCopyDiv6();
  void Inst_BCopyDiv7();
  void Inst_BCopyDiv8();
  void Inst_BCopyDiv9();
  void Inst_BCopyDiv10();
  void Inst_BCopyPow2();
  void Inst_BIfNotCopy();
  void Inst_BIfCopy();


  //// Placebo ////
  void Inst_Skip();
};


//////////////////
//  cHardwareCPU
//////////////////

inline void cHardwareCPU::NextThread()
{
#ifdef THREADS
  cur_thread++;
  if (cur_thread >= num_threads) cur_thread = 0;
#endif
}

inline void cHardwareCPU::PrevThread()
{
#ifdef THREADS
  if (cur_thread == 0) cur_thread = num_threads - 1;
  else cur_thread--;
#endif
}

inline void cHardwareCPU::StackPush(int value)
{
  if (threads[CUR_THREAD].cur_stack == 0) {
    threads[CUR_THREAD].stack.Push(value);
  } else {
    global_stack.Push(value);
  }
}

inline int cHardwareCPU::StackPop()
{
  int pop_value;

  if (threads[CUR_THREAD].cur_stack == 0) {
    pop_value = threads[CUR_THREAD].stack.Pop();
  } else {
    pop_value = global_stack.Pop();
  }

  return pop_value;
}

inline void cHardwareCPU::StackFlip()
{
  if (threads[CUR_THREAD].cur_stack == 0) {
    threads[CUR_THREAD].stack.Flip();
  } else {
    global_stack.Flip();
  }
}

inline int cHardwareCPU::GetStack(int depth, int stack_id) const
{
  int value = 0;

  if (stack_id == -1) stack_id = threads[CUR_THREAD].cur_stack;

  if (stack_id == 0) value = threads[CUR_THREAD].stack.Get(depth);
  else if (stack_id == 1) value = global_stack.Get(depth);

  return value;
}

inline void cHardwareCPU::StackClear()
{
  if (threads[CUR_THREAD].cur_stack == 0) {
    threads[CUR_THREAD].stack.Clear();
  } else {
    global_stack.Clear();
  }
}

inline void cHardwareCPU::SwitchStack()
{
  threads[CUR_THREAD].cur_stack++;
  if (threads[CUR_THREAD].cur_stack > 1) threads[CUR_THREAD].cur_stack = 0;
}

#endif
