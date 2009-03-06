//===========================================================================
//                                                                          =
//   This program is free software; you can redistribute it and/or modify   =
//   it under the terms of the GNU General Public License as published by   =
//   the Free Software Foundation; either version 2 of the License, or      =
//   (at your option) any later version.                                    =
// ------------------------------------------------------------------------ =
//                  TTTTT    OOO    PPPP    EEEE    DDDD                    =
//                  T T T   O   O   P   P   E       D   D                   =
//                    T    O     O  PPPP    EEE     D    D                  =
//                    T     O   O   P       E       D   D                   =
//                    T      OOO    P       EEEEE   DDDD                    =
//                                                                          =
//   This file is a part of Toped project (C) 2001-2007 Toped developers    =
// ------------------------------------------------------------------------ =
//           $URL$
//        Created: Mon Aug 11 2003
//        $Author$
//    Description: Reader of Mentor Graphics Calibre drc errors files
//---------------------------------------------------------------------------
//  Revision info
//---------------------------------------------------------------------------
//      $Revision$
//          $Date$
//        $Author$
//===========================================================================
//      Comments :
//===========================================================================

#if !defined(CALBR_READER_H_INCLUDED)
#define CALBR_READER_H_INCLUDED

#include <fstream>
#include <string>

namespace Calbr
{


class drcRuleCheck
{
private:
	long			_curResCount; //current result count
	long			_origResCount;//original result count
	std::string _ruleCheckName;
	std::string _timeStamp;
	std::string _header;

};

class CalbrFile
{
public:
	CalbrFile(const std::string &fileName);
	~CalbrFile();
private:
	FILE*          _calbrFile;
	std::string    _fileName;
	std::string    _cellName;
	long				_precision;

	std::ifstream	_inFile;
	bool				parse();

};
}
#endif //CALBR_READER_H_INCLUDED