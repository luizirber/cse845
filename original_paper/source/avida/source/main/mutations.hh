//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef MUTATIONS_HH
#define MUTATIONS_HH

class cMutationRates {
private:
  double point_mut_prob;
  double copy_mut_prob;
  double ins_mut_prob;
  double del_mut_prob;
  double divide_mut_prob;
  double divide_ins_prob;
  double divide_del_prob;
  double parent_mut_prob;
  double crossover_prob;
  double aligned_cross_prob;
  double exe_err_prob;

public:
  cMutationRates();
  ~cMutationRates();

  void Clear();

  bool TestCopyMut() const;
  bool TestPointMut() const;
  bool TestDivideMut() const;
  bool TestDivideIns() const;
  bool TestDivideDel() const;
  bool TestParentMut() const;
  bool TestCrossover() const;
  bool TestAlignedCrossover() const;
  bool TestExeErr() const;

  double GetCopyMutProb() const      { return copy_mut_prob; }
  double GetInsMutProb() const       { return ins_mut_prob; }
  double GetDelMutProb() const       { return del_mut_prob; }
  double GetPointMutProb() const     { return point_mut_prob; }
  double GetDivideMutProb() const    { return divide_mut_prob; }
  double GetDivideInsProb() const    { return divide_ins_prob; }
  double GetDivideDelProb() const    { return divide_del_prob; }
  double GetParentMutProb() const    { return parent_mut_prob; }
  double GetCrossoverProb() const    { return crossover_prob; }
  double GetAlignedCrossProb() const { return aligned_cross_prob; }
  double GetExeErrProb() const       { return exe_err_prob; }

  void SetCopyMutProb(double in_prob)   { copy_mut_prob   = in_prob; }
  void SetInsMutProb(double in_prob)    { ins_mut_prob    = in_prob; }
  void SetDelMutProb(double in_prob)    { del_mut_prob    = in_prob; }
  void SetPointMutProb(double in_prob)  { point_mut_prob  = in_prob; }
  void SetDivideMutProb(double in_prob) { divide_mut_prob = in_prob; }
  void SetDivideInsProb(double in_prob) { divide_ins_prob = in_prob; }
  void SetDivideDelProb(double in_prob) { divide_del_prob = in_prob; }
  void SetParentMutProb(double in_prob) { parent_mut_prob = in_prob; }
  void SetCrossoverProb(double in_prob) { crossover_prob  = in_prob; }
  void SetAlignedCrossProb(double in)   { aligned_cross_prob = in; }
  void SetExeErrProb(double in_prob)    { exe_err_prob    = in_prob; }
};

#endif
