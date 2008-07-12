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
//   This file is a part of Toped project (C) 2001-2008 Toped developers    =
// ------------------------------------------------------------------------ =
//           $URL$
//        Created: Sun May 04 2008
//     Originator: Svilen Krustev - skr@toped.org.uk
//    Description: CIF parser
//---------------------------------------------------------------------------
//  Revision info
//---------------------------------------------------------------------------
//      $Revision$
//          $Date$
//        $Author$
//===========================================================================
#if !defined(CIFIO_H_INCLUDED)
#define CIFIO_H_INCLUDED

#include "../tpd_common/ttt.h"
#include <string>

int ciflex(void);
int ciferror (char *s);

namespace CIFin {

/*
A formal CIF 2.0 definition in Wirth notation is given below. Taken from 
http://ai.eecs.umich.edu/people/conway/VLSI/ImplGuide/ImplGuide.pdf
"A guide to LSI Implementation", Second Edition, Robert W. Hon Carlo H. Sequin

cifFile              = {{blank} [command] semi} endCommand {blank}.

command              = primCommand | defDeleteCommand | 
                       defStartCommand semi {{blank}[primCommand]semi} defFinishCommand.

primCommand          = polygonCommand       | boxCommand    | roundFlashCommand |
                       wireCommand          | layerCommand  | callCommand       | 
                       userExtensionCommand | commentCommand

polygonCommand       = "P" <*{blank}*> path.
boxCommand           = "B" <*{blank}*> integer sep integer sep point [sep point]
roundFlashCommand    = "R" integer sep point.
wireCommand          = "W" <*{blank}*> integer sep path.
layerCommand         = "L" {blank} shortname.
defStartCommand      = "D" {blank} "S" integer [sep integer sep integer].
defFinishCommand     = "D" {blank} "F".
defDeleteCommand     = "D" {blank} "D" integer.
callCommand          = "C" <*{blank}*> integer transformation.
userExtensionCommand = digit userText
commentCommand       = "(" commentText ")".
endCommand           = "E".

transformation       = {{blank} ("T" <*{blank}*> point | "M" {blank} "X" | "M" {blank} "Y" | "R" <*{blank}*> point)}.
path                 = point {sep point}.
point                = sInteger sep sInteger.
sInteger             = {sep} ["-"]integerD.
integer              = {sep} integerD.
integerD             = digit{digit}.
shortname            = c[c][c][c]
c                    = digit | upperChar
userText             = {userChar}.
commentText          = {commentChar} | commentText "(" commentText ")" commentText.

semi                 = {blank};{blank}.
sep                  = upperChar | blank;
digit                = "0" | "1" | "2" | ... | "9"
upperChar            = "A" | "B" | "C" | ... | "Z"
blank                = any ASCII character except digit, upperChar, "-" , "(" , ")" , ";".
userChar             = any ASCII character except ";".
commentChar          = any ASCII character except "(" or ")".

<* ... *> -> included in the parser (or the scanner) because existing CIF files are using this syntax, but
             it breaks the formal syntax definition of CIF

===========================================================================================================
The user extensions below - as described in http://www.rulabinsky.com/cavd/text/chapb.html
(Steven M. Rubin Copyright © 1994)
===========================================================================================================

0 x y layer N name;           Set named node on specified layer and position
0V x1 y1 x2 y2 ... xn yn;     Draw vectors
2A "msg" T x y;               Place message above specified location
2B "msg" T x y;               Place message below specified location
2C "msg" T x y;               Place message centered at specified location
2L "msg" T x y;               Place message left of specified location
2R "msg" T x y;               Place message right of specified location
4A lowx lowy highx highy;     Declare cell boundary
4B instancename;              Attach instance name to cell
4N signalname x y;            Labels a signal at a location
9  cellname;                  Declare cell name
91 instancename;              Attach instance name to cell
94 label x y;                 Place label in specified location
95 label length width x y;    Place label in specified area

*/
   typedef enum {
      cif_BOX        ,
      cif_POLY       ,
      cif_WIRE       ,
      cif_REF        ,
      cif_LBL_LOC    ,
      cif_LBL_SIG
   } CifDataType;

   class CIFStructure;
   class CIFFile;

   typedef SGHierTree<CIFStructure>       CIFHierTree;
   typedef std::list<CIFStructure*>       CIFSList;


   class CIFData {
      public:
                              CIFData(CIFData* last) : _last(last) {};
                  CIFData*    last()         {return _last;}
         virtual CifDataType dataType() = 0;
         virtual            ~CIFData() {}
      protected:
         CIFData*    _last;
   };

   class CIFBox : public CIFData {
      public:
                     CIFBox(CIFData*, word, word, TP*, TP*);
         CifDataType dataType()     {return cif_BOX;}
      protected:
         word        _length;
         word        _width;
         TP*         _center;
         TP*         _direction;
   };

   class CIFPoly : public CIFData {
      public:
                     CIFPoly(CIFData* last, pointlist*);
         CifDataType dataType()     {return cif_POLY;}
      protected:
         pointlist*  _poly;
   };

   class CIFWire : public CIFData {
      public:
                     CIFWire(CIFData* last, pointlist*, word);
         CifDataType dataType()     {return cif_WIRE;}
      protected:
         pointlist*  _poly;
         word        _width;
   };

   class CIFRef : public CIFData {
      public:
                     CIFRef(CIFData* last, word, CTM*);
         CIFRef*     last()                           {return static_cast<CIFRef*>(CIFData::last());}
         word        cell()                           {return _cell;}
         CifDataType dataType()                       {return cif_REF;}
      protected:
         word        _cell;
         CTM*        _location;
   };

   class CIFLabelLoc : public CIFData {
      public:
                     CIFLabelLoc(CIFData*, std::string, TP*);
         CifDataType dataType()                       {return cif_LBL_LOC;}
      protected:
         std::string _label;
         TP*         _location;
   };

   class CIFLabelSig : public CIFLabelLoc {
      public:
                     CIFLabelSig(CIFData*, std::string, TP*);
         CifDataType dataType()                       {return cif_LBL_SIG;}
   };

   class CIFLayer {
      public:
                        CIFLayer(std::string name, CIFLayer* last);
         std::string    name()                        {return _name;}
         CIFLayer*      last()                        {return _last;}
         CIFData*       firstData()                   {return _first;}
         void           addBox(word, word, TP*, TP* direction = NULL);
         void           addPoly(pointlist* poly);
         void           addWire(pointlist* poly, word width);
         void           addLabelLoc(std::string, TP*);
         void           addLabelSig(std::string, TP*);
      private:
         std::string    _name;
         CIFLayer*      _last;
         CIFData*       _first;
   };
   typedef std::list<CIFLayer*>     CifLayerList;

   class CIFStructure  {
      public:
                        CIFStructure(word, CIFStructure*, word=1,word=1);
         void           cellNameIs(std::string cellName) {_cellName = cellName;}
         void           cellOverlapIs(TP* bl, TP* tr) {_overlap = DBbox(*bl, *tr);}
         CIFStructure*  last() const                  {return _last;}
         word           ID() const                    {return _ID;}
         std::string    cellName() const              {return _cellName;}
         void           parentFound()                 {_orphan = false;}
         bool           orphan()                      {return _orphan;}
         bool           traversed() const             {return _traversed;}
         void           set_traversed(bool trv)       { _traversed = trv;}
         CIFLayer*      firstLayer()                  {return _first;}
         CIFLayer*      secureLayer(std::string);
         void           addRef(word cell, CTM* location);
         void           collectLayers(CifLayerList&);
         void           hierPrep(CIFFile&);
         CIFHierTree*   hierOut(CIFHierTree*, CIFStructure*);
      // to cover the requirements of the hierarchy template
         int            libID() const                 {return TARGETDB_LIB;}
      private:
         word           _ID;
         CIFStructure*  _last;
         word           _a;
         word           _b;
         std::string    _cellName;
         CIFLayer*      _first;
         CIFRef*        _refirst;
         DBbox          _overlap;
         bool           _orphan;
         bool           _traversed;       //! For hierarchy traversing purposes
         CIFSList       _children;
   };
//   typedef std::list<CIFStructure*> CifCellList;

   class   CIFFile {
      public:
                        CIFFile(std::string);
                       ~CIFFile();
         bool           status() {return _status;}
         void           addStructure(word, word = 1, word = 1);
         void           doneStructure();
         void           addBox(word, word, TP*, TP* direction = NULL);
         void           addPoly(pointlist*);
         void           addWire(pointlist*, word);
         void           addRef(word, CTM*);
         void           addLabelLoc(char*, TP*, char* layname = NULL);
         void           addLabelSig(char*, TP*);
         void           secureLayer(char*);
         void           curCellName(char*);
         void           curCellOverlap(TP*, TP*);
         void           collectLayers(nameList&);
         CIFStructure*  getStructure(word);
         void           hierPrep();
         void           hierOut();
         std::string    Get_libname() const  {return _filename;}
         CIFHierTree*   hiertree()           {return _hiertree;}
      protected:
         bool           _status;          //!
         CIFStructure*  _first;           //! poiter to the first defined cell
         CIFStructure*  _current;         //! the working (current) cell
         CIFStructure*  _default;         //! pointer to the default cell - i.e. the scratch pad
         CIFLayer*      _curlay;          //!
         CIFHierTree*   _hiertree;        //! Tree of instance hierarchy
         std::string    _filename;
   };
}

#endif // !defined(CIFIO_H_INCLUDED)