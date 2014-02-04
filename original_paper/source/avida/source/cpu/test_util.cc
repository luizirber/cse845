//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#include "test_util.hh"

#include <fstream.h>
#include <iomanip.h>

#include "../main/genome.hh"
#include "../main/genotype.hh"
#include "../main/inst_util.hh"
#include "../main/organism.hh"
#include "../main/phenotype.hh"
#include "../main/stats.hh"

#include "hardware_base.hh"
#include "test_cpu.hh"

void cTestUtil::PrintGenome(const cGenome & genome, cString filename,
			    cGenotype * genotype, int update_out)
{
  if (filename == "") filename.Set("%03d-unnamed", genome.GetSize());

  // Build the test info for printing.
  cCPUTestInfo test_info;
  test_info.TestThreads();
  cTestCPU::TestGenome(test_info, genome);

  // Open the file...

  ofstream fp(filename());

  // @CAO Fix!!!!!!
  if( fp.good() == false ) {
    cerr << "Unable to open output file '" <<  filename() << "'" << endl;
    return;
  }

  // Print the useful info at the top...

  fp << "# Filename........: " << filename << endl;

  if (update_out >= 0) fp << "# Update Output...: " << update_out << endl;
  else fp << "# Update Output...: N/A" << endl;

  fp << "# Is Viable.......: " << test_info.IsViable()                 << endl
     << "# Repro Cycle Size: " << test_info.GetMaxCycle()              << endl
     << "# Depth to Viable.: " << test_info.GetDepthFound()            << endl;

  if (genotype != NULL) {
    fp << "# Update Created..: " << genotype->GetUpdateBorn()     << endl
       << "# Genotype ID.....: " << genotype->GetID()             << endl
       << "# Parent Gen ID...: " << genotype->GetParentID()       << endl
       << "# Tree Depth......: " << genotype->GetDepth()          << endl
       << "# Parent Distance.: " << genotype->GetParentDistance() << endl
      ;
  }
  fp << endl;

  const int num_levels = test_info.GetMaxDepth() + 1;
  for (int j = 0; j < num_levels; j++) {
    fp << "# Generation: " << j << endl;
    cOrganism * organism = test_info.GetTestOrganism(j);
    assert(organism != NULL);
    cPhenotype & phenotype = organism->GetPhenotype();

    fp << "# Merit...........: "
       << setw(12) << setfill(' ') << phenotype.GetMerit() << endl;
    fp << "# Gestation Time..: "
       << setw(12) << setfill(' ') << phenotype.GetGestationTime() << endl;
    fp << "# Fitness.........: "
       << setw(12) << setfill(' ') << phenotype.GetFitness() << endl;
    fp << "# Errors..........: "
       << setw(12) << setfill(' ') << phenotype.GetLastNumErrors() << endl;
    fp << "# Genome Size.....: "
       << setw(12) << setfill(' ') << organism->GetGenome().GetSize() << endl;
    fp << "# Copied Size.....: "
       << setw(12) << setfill(' ') << phenotype.GetCopiedSize() << endl;
    fp << "# Executed Size...: "
       << setw(12) << setfill(' ') << phenotype.GetExecutedSize() << endl;

    fp << "# Offspring.......: ";
    if (phenotype.GetNumDivides() == 0)
      fp << setw(12) << setfill(' ') << "NONE";
    else if (phenotype.CopyTrue() == true)
      fp << setw(12) << setfill(' ') << "SELF";
    else if (test_info.GetCycleTo() != -1)
      fp << setw(12) << setfill(' ') << test_info.GetCycleTo();
    else
      fp << setw(12) << setfill(' ') << (j+1);
    fp << endl;

    fp << endl;     // Skip line
  }
  
  // Display the tasks performed...
  cPhenotype & phenotype = test_info.GetTestOrganism()->GetPhenotype();
  for (int i = 0; i < phenotype.GetEnvironment().GetTaskLib().GetSize(); i++) {
    fp << "# "<< phenotype.GetEnvironment().GetTaskLib().GetTask(i).GetName()
       << "\t" << phenotype.GetLastTaskCount()[i]
       << endl;
  }
  fp << endl; // Skip line

  // Display the genome
  const cInstLib & inst_lib =
    test_info.GetTestOrganism()->GetHardware().GetInstLib();
  cInstUtil::SaveGenome(fp, inst_lib, genome);
}
