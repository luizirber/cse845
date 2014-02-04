//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#include "data_manager.hh"

#include "datafile.hh"

cDataManager_Base::~cDataManager_Base()
{
}


bool cDataManager_Base::PrintRow(cDataFile & data_file,
				 cString row_entries, char sep)
{
  bool result = true;

  row_entries.CompressWhitespace();

  // If we haven't output the header for this file yet, do so.
  if ( data_file.HeaderDone() == false ) {
    // Setup the format string for the top...
    cString format(row_entries);
    cString out_filetype(filetype);

    // Make sure its space seperated...
    if (sep != ' ') {
      int pos = -1;
      while ( (pos = format.Find(sep)) != -1 ) format[pos] = ' ';
    }

    // Write out the filetype and format strip
    out_filetype.Insert("#filetype ");
    format.Insert("#format ");
    data_file.WriteRawComment(out_filetype);
    data_file.WriteRawComment(format);

    // Setup the human-readable description...
    cString header_entries(row_entries);
    cString cur_desc;
    data_file.WriteComment(" ");
    data_file.WriteComment("Legend:");
    while (header_entries.GetSize() > 0) {
      cString cur_entry( header_entries.Pop(sep) );
      if ( GetDesc(cur_entry, cur_desc) == false ) {
	result = false;
	continue;
      }
      data_file.WriteColumnDesc(cur_desc);
    }
    data_file.FlushComments();
    data_file.Endl();
  }


  ofstream & fp = data_file.GetOFStream();
  while (row_entries.GetSize() > 0) {
    cString cur_entry( row_entries.Pop(sep) );
    if ( Print(cur_entry, fp) == false ) {
      cerr << "Data manager unable to find entry '"
	   << cur_entry << "'" << endl;
      result = false;
      continue;
    }
    fp << " ";
  }
  fp << endl;

  return result;
}

