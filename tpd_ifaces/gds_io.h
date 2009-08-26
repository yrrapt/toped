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
//        Created: Jun 14 1998
//      Copyright: (C) 2001-2006 Svilen Krustev - skr@toped.org.uk
//    Description: GDSII parser
//---------------------------------------------------------------------------
//  Revision info
//---------------------------------------------------------------------------
//      $Revision$
//          $Date$
//        $Author$
//===========================================================================
#if !defined(GDSIO_H_INCLUDED)
#define GDSIO_H_INCLUDED

#include <stdio.h>
#include <vector>
#include <string>
#include <set>
#include <wx/ffile.h>
#include "../tpd_common/ttt.h"

//GDS data types
#define gdsDT_NODATA       0
#define gdsDT_BIT          1
#define gdsDT_INT2B        2
#define gdsDT_INT4B        3
#define gdsDT_REAL4B       4
#define gdsDT_REAL8B       5
#define gdsDT_ASCII        6
////////////////////////////////
#define GDS_MAX_LAYER      256
// GDS record types
// Described according to "Design Data Translators Reference Manual" - 
// CADance documentation, September 1994
#define gds_HEADER         0x00
#define gds_BGNLIB         0x01
#define gds_LIBNAME        0x02
#define gds_UNITS          0x03
#define gds_ENDLIB         0x04
#define gds_BGNSTR         0x05
#define gds_STRNAME        0x06
#define gds_ENDSTR         0x07
#define gds_BOUNDARY       0x08
#define gds_PATH           0x09
#define gds_SREF           0x0A
#define gds_AREF           0x0B
#define gds_TEXT           0x0C
#define gds_LAYER          0x0D
#define gds_DATATYPE       0x0E
#define gds_WIDTH          0x0F
#define gds_XY             0x10
#define gds_ENDEL          0x11
#define gds_SNAME          0x12
#define gds_COLROW         0x13
#define gds_TEXTNODE       0x14
#define gds_NODE           0x15
#define gds_TEXTTYPE       0x16
#define gds_PRESENTATION   0x17
#define gds_SPACING        0x18
#define gds_STRING         0x19
#define gds_STRANS         0x1A
#define gds_MAG            0x1B
#define gds_ANGLE          0x1C
#define gds_UINTEGER       0x1D
#define gds_USTRING        0x1E
#define gds_REFLIBS        0x1F
#define gds_FONTS          0x20
#define gds_PATHTYPE       0x21
#define gds_GENERATION     0x22
#define gds_ATTRTABLE      0x23
#define gds_STYPTABLE      0x24
#define gds_STRTYPE        0x25
#define gds_ELFLAGS        0x26
#define gds_ELKEY          0x27
#define gds_LINKTYPE       0x28
#define gds_LINKKEYS       0x29
#define gds_NODETYPE       0x2A
#define gds_PROPATTR       0x2B
#define gds_PROPVALUE      0x2C
#define gds_BOX            0x2D
#define gds_BOXTYPE        0x2E
#define gds_PLEX           0x2F
#define gds_BGNEXTN        0x30
#define gds_ENDEXTN        0x31
#define gds_TYPENUM        0x32
#define gds_TYPECODE       0x33
#define gds_STRCLASS       0x34
#define gds_RESERVED       0x35
#define gds_FORMAT         0x36
#define gds_MASK           0x37
#define gds_ENDMASKS       0x38
#define gds_LIBDIRSIZE     0x39
#define gds_SRFNAME        0x3A
#define gds_LIBSECUR       0x3B
#define gds_BORDER         0x3C
#define gds_SOFTFENCE      0x3D
#define gds_HARDFENCE      0x3E
#define gds_SOFTWIRE       0x3F
#define gds_HARDWIRE       0x40
#define gds_PATHPORT       0x41
#define gds_NODEPORT       0x42
#define gds_USERCONSTRAINT 0x43
#define gds_SPACER_ERROR   0x44
#define gds_CONTACT        0x45

class LayerMapGds;

namespace laydata {
   class tdtcell;
   class tdtlibdir;
}

namespace GDSin {
   class GdsFile;
   class GdsStructure;
   class GdsRecord;
   class GdsLibrary;

   typedef SGHierTree<GdsStructure>       GDSHierTree;

   typedef struct {word Year,Month,Day,Hour,Min,Sec;} GDStime;

   /*** GdsRecord ***************************************************************
   >>> Constructor --------------------------------------------------------------
   > reads 'rl' bytes from the input file 'Gf'. Initializes all data fields
   > including 'isvalid'. This constructor is called ONLY from 'getNextRecord'
   > method of GdsFile class
   >> input parameters ->   Gf   - file handler
   >                        rl   - record length
   >                        rt   - record type
   >                        dt   - data type
   >>> Data fields --------------------------------------------------------------
   > reclen      - length of current GDS record
   > rectype   - type of current GdsRecord
   > datatype   - type of data that this record contain
   > record      - the information record
   > numread   - number of really read bytes in this record
   > isvalid   - true if numread == reclen, otherwise - false
   >>> Methods ------------------------------------------------------------------
   > Get_rectype()   - inline function - see definition
   > Get_record()      - inline function - see definition
   > Get_reclen()      - inline function - see definition
   > Ret_data(...)   - Return data function. This function is responsible for
   >                   proper GDS data treatement. It converts 'raw' GDS data
   >                   that 'record' field contains to C format. Called by all
   >                   GDS classes taking part in GDS reading.
   ******************************************************************************/
   class   GdsRecord {
      public:
                           GdsRecord(wxFFile& Gf, word rl, byte rt, byte dt);
                           GdsRecord(byte rt, byte dt, word rl);
         bool              retData(void* var, word curnum = 0, byte len = 0);
         size_t            flush(wxFFile& Gf);
         void              add_int2b(const word);
         void              add_int4b(const int4b);
         void              add_real8b(const real);
         void              add_ascii(const char*);
         byte              recType() const                     { return _recType;}
         byte*             record() const                      { return _record;}
         word              recLen() const                      { return _recLen;}
         bool              valid() const                       { return _valid;}
                          ~GdsRecord();
      private:
         byte*             ieee2gds(double);
         double            gds2ieee(byte*);
         bool              _valid;
         word              _recLen;
         byte              _recType;
         byte              _dataType;
         byte*             _record;
         size_t            _numread;
         word              _index;
   };

   class   GdsNode/*:public GdsData*/ {
      public:
                              GdsNode(GdsFile*, int2b&, int2b&);
         byte                 gdsDataType()                     { return gds_NODE;}
         virtual             ~GdsNode()                         {                 }
      protected:
         pointlist            _plist;
   };

   /*** GdsStructure ************************************************************
   >>> Constructor --------------------------------------------------------------
   > Reads a GDSII structure
   >> input parameters ->   cf - pointer to the top GdsFile structure.
   >                      lst - pointer to last GdsStructure
   >>> Data fields --------------------------------------------------------------
   > HaveParent         - true if current structure has a parrent structure.
                          Used when hierarchy is created (GdsLibrary.HierOut)
   > children            - Array of pointers to all GdsStructures referenced
                          inside this structure. Used when hierarchy is created
   > tvstruct            - HTREEITEM structure used for hierarhy window;
   > Allay[...]         - An array with info whether a given layer is used in
                          GDS structure (and down in hierarhy) or not.
   > Compbylay[]        - An array of pointers with all GdsData grouped by GDS
                          layer. This structure doesn't contain data of type GdsRef
                          and GdsARef because they doesn't belong to any layer
   > UsedLayers         - list of pointers with all used GDS layers in this
                          structure and down in hierarchy
   > Fdata               - Pointer to first GdsData of this structure
   > last               - Pointer to last (next) GdsStructure
   > strname[33]         - GDSII name of the structure
   >>> Methods ------------------------------------------------------------------
   > RegisterStructure()- Creates a tree of referenced GdsStructure. Changes
                          children valiable
   > HierOut()            - Updates HierList window. Here Allay[] and UsedLayers
                          variables are changed
   > PPreview()         - Calculates the box overlapping GdsStructure
   > PSOut_v2()         - Post Script output
   > GetLast()            - ...
   > Get_StrName()      - ...
   > Get_Fdata()         - ...
   > Get_GDSlay()         - ...
   > Get_Allay(byte i)   - ...
   ******************************************************************************/

   class   GdsStructure {
      public:
         typedef std::list<GdsStructure*>  ChildStructure;

         typedef std::set<std::string> NameSet;
                              GdsStructure(GdsFile*);
         void                 import(GdsFile*, laydata::tdtcell*, laydata::tdtlibdir*, const LayerMapGds&);
         GDSHierTree*         hierOut(GDSHierTree* Htree, GdsStructure* parent);
         void                 collectLayers(GdsLayers&, bool);
         void                 linkReferences(GdsLibrary* const);
         std::string          strctName() const                { return _strctName;    }
         bool                 traversed() const                { return _traversed;    }
         void                 set_traversed(bool tf)           { _traversed = tf;      }
         int                  libID() const                    { return TARGETDB_LIB;  } // to cover the requirements of the hierarchy template
         bool                 haveParent() const               { return _haveParent;   }
         wxFileOffset         strSize() const                  { return _strSize;      }
                             ~GdsStructure();
      protected:
         void                 importBox(GdsFile*, laydata::tdtcell*, const LayerMapGds&);
         void                 importPoly(GdsFile*, laydata::tdtcell*, const LayerMapGds&);
         void                 importPath(GdsFile*, laydata::tdtcell*, const LayerMapGds&);
         void                 importText(GdsFile*, laydata::tdtcell*, real, const LayerMapGds&);
         void                 importSref(GdsFile*, laydata::tdtcell*, laydata::tdtlibdir*, const LayerMapGds&);
         void                 importAref(GdsFile*, laydata::tdtcell*, laydata::tdtlibdir*, const LayerMapGds&);
         void                 skimBox(GdsFile*);
         void                 skimBoundary(GdsFile*);
         void                 skimPath(GdsFile*);
         void                 skimText(GdsFile*);
         void                 skimSRef(GdsFile*);
         void                 skimARef(GdsFile*);
         void                 updateContents(int2b, int2b);
         void                 pathConvert(pointlist&, word, int4b, int4b );
         int                  arrGetStep(TP&, TP&, int2b);
         GdsLayers            _contSummary; // contents summary
         bool                 _haveParent;
         std::string          _strctName;
         bool                 _traversed;       //! For hierarchy traversing purposes
         NameSet              _referenceNames;
         ChildStructure       _children;
         wxFileOffset         _filePos;
         wxFileOffset         _strSize;
   };

   /*** GDSLibrary **************************************************************
   >>> Constructor --------------------------------------------------------------
   > Reads a GDSII library
   >> input parameters ->   cf - pointer to the top GdsFile structure.
   >                       cr - pointer to last GdsRecord structure
   >>> Data fields --------------------------------------------------------------
   > libname            - library name (string)
   > fonts[4]            - GDSII font names used in the library;
   > DBU                  - Data Base Units in meters
   > UU                  - DBU's in one User Unit
   > maxver               - (generations) max versions
   > Fstruct            - pointer to first structure in the GDSII library
   >>> Methods ------------------------------------------------------------------
   > SetHierarchy()      - This method is called to organize hierarhy of the struc-
                          tures. Each GdsRef or GdsARef receives a pointer to its
                          corresponding GdsStructure.
   > HierOut            - Called by corresponding method in GdsFile. Calls HierOut
                          method of GdsStructure. The goal of all that calls is to
                          update hierarchy window
   > Get_Fstruct()      - ...
   > Get_DBU()            - ...
   > Get_UU()            - ...
   ******************************************************************************/
   class   GdsLibrary
   {
   public:
      typedef std::map<std::string, GdsStructure*> StructureMap;
                              GdsLibrary(GdsFile* , GdsRecord* );
      void                    linkReferences();
      GDSHierTree*            hierOut();
      GdsStructure*           getStructure(const std::string);
      void                    collectLayers(GdsLayers&);
      double                  dbu()                            { return _dbu;        }
      double                  uu()                             { return _uu;         }
      std::string             libName() const                  { return _libName;    }
      const StructureMap&     structures() const               { return _structures; }
                             ~GdsLibrary();
   protected:
      std::string             _libName;
      std::string             _allFonts[4];
      double                  _dbu;
      double                  _uu;
      int2b                   _maxver;
      StructureMap            _structures;
   };

   /*** GdsFile ***************************************************************
   >>> Constructor --------------------------------------------------------------
   > Opens the input GDS file and start reading it. Initializes all data fields
   >> input parameters ->   fn - GDSII file name for reading
   >                  progind - pointer to progress indicator control
   >                       lw - pointer to Log window control
   >>> Data fields --------------------------------------------------------------
   > GDSfh         - GDS input file handler
   > filename      - GDS input file name
   > file_length  - length of the input GDS file
   > file_pos      - current position of the input GDS file during read in
   > library      - pointer to GdsLibrary structure
   > prgrs_pos      - current position of the progress indicator control
   > logwin         - pointer to the error log edit control
   > StreamVerion   - GDS specific data - see gds_HEADER
   > libdirsize   - GDS specific data - see gds_LIBDIRSIZE
   > srfname      - GDS specific data - see gds_SRFNAME
   > t_access      - Date&time of last access to the GDSII file
   > t_modif      - Date&time of last modification of the GDSII file
                    Last two parameters not used anywhere
   >>> Methods ------------------------------------------------------------------
   > getNextRecord()      - Reads next record from the input stream. Retuns NULL if
   >                       error ocures during read in. This is the only function
   >                      used for reading of the input GDS file. Calls
   >                      'GdsRecord' constructor
   > libUnits()      - |Return library units
   > userUnits()    - |Return user units
                          |->Both methods call corresponding methods in GdsLibrary
   > GetStructure()      - Returns the pointer to GDSII structure with a given name
   > HierOut()            - Call corresponding method in GdsLibrary
   > GetReadErrors()      - Returns the number of errors during GDSII file reading
   > GetTimes()         - Reads values of t_access and t_modiff (see above)
   ******************************************************************************/
   class   GdsFile   {
      public:
                              GdsFile(std::string);
                              GdsFile(std::string, const LayerMapGds*, time_t);
         bool                 reopenFile();
         GdsRecord*           getNextRecord();
         GdsRecord*           setNextRecord(byte, word reclen = 0);
         double               libUnits();
         double               userUnits();
         void                 setTimes(GdsRecord*);
         bool                 checkCellWritten(std::string);
         void                 registerCellWritten(std::string);
         void                 flush(GdsRecord*);
         void                 updateLastRecord();
         bool                 getMappedLayType(word& gdslay, word& gdstype, word tdtlay);
         void                 setPosition(wxFileOffset);
         void                 closeFile();
         GdsStructure*        getStructure(const std::string nm){ return _library->getStructure(nm);  }
         void                 collectLayers(GdsLayers& lays)   { _library->collectLayers(lays);       }
         std::string          libname() const                  { return _library->libName();          }
         void                 hierOut()                        { _hierTree = _library->hierOut();     }
         GDSHierTree*         hierTree()                       { return _hierTree;                    }
         int                  gdsiiWarnings()                  { return _gdsiiWarnings;               }
         int                  incGdsiiWarnings()               { return ++_gdsiiWarnings;             }
         const GdsLibrary*    library() const                  { return _library;                     }
         wxFileOffset         filePos() const                  { return _filePos;                     }
                              ~GdsFile();
      protected:
         void                 getTimes(GdsRecord* wr);
         wxFFile              _gdsFh;
         std::string          _fileName;
         int2b                _streamVersion;
         int2b                _libDirSize;
         std::string          _srfName;
         GdsLibrary*          _library;
         wxFileOffset         _filePos;
         GDSHierTree*         _hierTree; // Tree of instance hierarchy
         nameList             _childnames;
         int                  _gdsiiWarnings;
         GDStime              _tModif;
         GDStime              _tAccess;
         const LayerMapGds*   _laymap;
         wxFileOffset         _prgrs_pos;
   };

   class Gds2Ted {
   public:
      typedef std::list<GDSin::GdsStructure*> StructureList;
                           Gds2Ted(GDSin::GdsFile*, laydata::tdtlibdir*, const LayerMapGds&);
      void                 run(const nameList&, bool, bool);
   protected:
      void                 preTraverseChildren(const GDSin::GDSHierTree*, bool);
      void                 convert(GDSin::GdsStructure*, bool);
      GDSin::GdsFile*      _src_lib;
      laydata::tdtlibdir*  _tdt_db;
      const LayerMapGds&   _theLayMap;
      real                 _coeff; // DBU difference
      StructureList        _convertList;
      wxFileOffset         _conversionLength;
   };


   // Function definition
     TP   get_TP(GdsRecord* cr, word curnum = 0, byte len=4);
     int4b* getCoordinateArray(GdsRecord* cr, word len);
//     void PrintChildren(GDSin::GDSHierTree*, std::string*);
}   
#endif // !defined(GDSIO_H_INCLUDED)
