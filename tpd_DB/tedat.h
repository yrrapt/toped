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
//        Created: Tue Feb 25 2003
//     Originator: Svilen Krustev - skr@toped.org.uk
//    Description: Layout primitives
//---------------------------------------------------------------------------
//  Revision info
//---------------------------------------------------------------------------
//      $Revision$
//          $Date$
//        $Author$
//===========================================================================
#ifndef TEDAT_H
#define TEDAT_H

#include <string>
#include <map>
#include <vector>
#include "tedstd.h"
#include "tenderer.h"
#include "drawprop.h"

namespace laydata {
//==============================================================================
   /*! Abstract class - the base of all layout objects.\n To optimize the RAM
       usage having in mind the huge potential number of objects, we must have
       only the absolute minimum of data fields here or none at all. It was
       decided not to use the standard C++ containers for the layout objects.
       The main reason for this is the clipping algorithm and as a consequence
       the QuadTree class as a main data holder. In bref the TdtData object
       doesn't know neither about the layer it belongs to nor the QuadTree it
       is sorted in. */
   class TdtData  {
   public:
   //! The default constructor.
      TdtData(SH_STATUS sel = sh_active) : _status(sel)/*, _next(NULL) */{};
      //! Return the overlapping box of the object.
      virtual   DBbox      overlap()  const = 0;
      //! Return the overlapping box of the object.
      virtual   void       vlOverlap(const layprop::DrawProperties&, DBbox&) const {assert(false);}
   //! Move the object relatively using the input CTM
      virtual   Validator* move(const CTM&, SGBitSet& plst) = 0;
   //! Rotate or flip (transfer the object using input CTM
      virtual   void       transfer(const CTM&) = 0;
   //! Copy the object and move it using the input CTM
      virtual   TdtData*   copy(const CTM&) = 0;
   //! A preparation for drawing - calculating all drawing objects using translation matrix stack.
      virtual   void       openGlPrecalc(layprop::DrawProperties&, pointlist&) const = 0;
   //! Draw the outline of the objects
      virtual   void       openGlDrawLine(layprop::DrawProperties&, const pointlist&) const = 0;
   //! Draw the object texture
      virtual   void       openGlDrawFill(layprop::DrawProperties&, const pointlist&) const = 0;
   //! Draw the outlines of the selected objects
      virtual   void       openGlDrawSel(const pointlist&, const SGBitSet*) const = 0;
   //! Clean-up the calculated drawing objects
      virtual   void       openGlPostClean(layprop::DrawProperties&, pointlist& ptlist) const {ptlist.clear();}
      virtual   void       drawRequest(tenderer::TopRend&) const = 0;
   //! Draw the outlines of the selected objects
      virtual   void       drawSRequest(tenderer::TopRend&, const SGBitSet*) const = 0;
   //! Draw the objects in motion during copy/move and similar operations
      virtual   void       motionDraw(const layprop::DrawProperties&, ctmqueue&, SGBitSet*) const = 0;
   //! Print an object description on the toped console.
      virtual   void       info(std::ostringstream&, real) const = 0;
   //! Write the TdtData object in TDT file.
      virtual   void       write(TEDfile* const tedfile) const = 0;
   //! Write the TdtData object in GDS file.
      virtual   void       gdsWrite(DbExportFile&) const = 0;
   //! Write the TdtData object in CIF file.
      virtual   void       cifWrite(DbExportFile&) const = 0;
   //! Write the TdtData object in PS file.
      virtual   void       psWrite(PSFile&, const layprop::DrawProperties&) const = 0;
   //!
      virtual   bool       pointInside(const TP);
   //! shape cut with the input polygon
      virtual   void       polyCut(pointlist&, ShapeList**) = 0;
   //! shrink/stretch
      virtual   void       stretch(int bfactor, ShapeList**) = 0;
   //!
      virtual  pointlist   shape2poly() const = 0;
   //! Returns the next TdtData object ot NULL if it doesn't exists
//      TdtData*             next() const         {return _next;}
   //! Changes the pointer to the next tdtddata object
//      void                 nextIs(TdtData* nxt) {_next = nxt;};
   //! Set the _selected flag in case the object is entirely overlapped by select_in box
      void                 selectInBox(DBbox&, DataList*, bool);
      void                 selectThis(DataList*);
      bool                 unselect(DBbox&, SelectDataPair&, bool);
      void                 setStatus(SH_STATUS s) {_status = s;}
      SH_STATUS            status() const {return _status;}
      virtual word         numPoints() const = 0;
      virtual             ~TdtData(){};
      virtual word         lType() const = 0;
   protected:
      virtual void         selectPoints(DBbox&, SGBitSet&) = 0;
      virtual void         unselectPoints(DBbox&, SGBitSet&) = 0;
      SH_STATUS            _status;
   //! A pointer to the next TdtData object
//      TdtData*             _next;
   };

//==============================================================================
   class TdtBox : public TdtData   {
   public:
                           TdtBox(const TP& p1, const TP& p2);
                           TdtBox(TEDfile* const tedfile);
                          ~TdtBox();
      virtual DBbox        overlap() const;
      virtual Validator*   move(const CTM&, SGBitSet& plst);
      virtual void         transfer(const CTM&);
      virtual TdtData*     copy(const CTM&);

      virtual void         openGlPrecalc(layprop::DrawProperties&, pointlist&) const;
      virtual void         openGlDrawLine(layprop::DrawProperties&, const pointlist&) const;
      virtual void         openGlDrawFill(layprop::DrawProperties&, const pointlist&) const;
      virtual void         openGlDrawSel(const pointlist&, const SGBitSet*) const;
      virtual void         drawRequest(tenderer::TopRend&) const;
      virtual void         drawSRequest(tenderer::TopRend&, const SGBitSet*) const;
      virtual void         motionDraw(const layprop::DrawProperties&, ctmqueue&, SGBitSet*) const;

      virtual void         info(std::ostringstream&, real) const;
      virtual void         write(TEDfile* const tedfile) const;
      virtual void         gdsWrite(DbExportFile&) const;
      virtual void         cifWrite(DbExportFile&) const;
      virtual void         psWrite(PSFile&, const layprop::DrawProperties&) const;
      virtual word         numPoints() const {return 4;};
      virtual void         polyCut(pointlist&, ShapeList**);
      virtual void         stretch(int bfactor, ShapeList**);
      virtual pointlist    shape2poly() const;
      virtual word         lType() const {return _lmbox;}
   protected:
      void                 selectPoints(DBbox&, SGBitSet&);
      void                 unselectPoints(DBbox&, SGBitSet&);
   private:
      enum {
            p1x  = 0,
            p1y  = 1,
            p2x  = 2,
            p2y  = 3
      };
      void              normalize(SGBitSet& psel);
      pointlist*        movePointsSelected(const SGBitSet&, const CTM&, const CTM& = CTM()) const;
      int4b             _pdata[4];
   };

//==============================================================================
   class TdtPoly : public TdtData   {
      public:
                           TdtPoly(const pointlist& plist);
                           TdtPoly(int4b* plist, unsigned psize);
                           TdtPoly(TEDfile* const tedfile);
                          ~TdtPoly();
         virtual DBbox     overlap() const;
         virtual Validator* move(const CTM&, SGBitSet& plst);
         virtual void      transfer(const CTM&);
         virtual TdtData*  copy(const CTM&);

         virtual void      openGlPrecalc(layprop::DrawProperties&, pointlist&) const;
         virtual void      openGlDrawLine(layprop::DrawProperties&, const pointlist&) const;
         virtual void      openGlDrawFill(layprop::DrawProperties&, const pointlist&) const;
         virtual void      openGlDrawSel(const pointlist&, const SGBitSet*) const;
         virtual void      drawRequest(tenderer::TopRend&) const;
         virtual void      drawSRequest(tenderer::TopRend&, const SGBitSet*) const;
         virtual void      motionDraw(const layprop::DrawProperties&, ctmqueue&, SGBitSet*) const;

         virtual void      info(std::ostringstream&, real) const;
         virtual void      write(TEDfile* const tedfile) const;
         virtual void      gdsWrite(DbExportFile&) const;
         virtual void      cifWrite(DbExportFile&) const;
         virtual void      psWrite(PSFile&, const layprop::DrawProperties&) const;
         virtual word      numPoints() const {return _psize;}
         virtual bool      point_inside(const TP);
         virtual void      polyCut(pointlist&, ShapeList**);
         virtual void      stretch(int bfactor, ShapeList**);
         virtual pointlist shape2poly() const;
         virtual word      lType() const {return _lmpoly;}
      private:
         void              selectPoints(DBbox&, SGBitSet&);
         void              unselectPoints(DBbox&, SGBitSet&);
         pointlist*        movePointsSelected(const SGBitSet&, const CTM&, const CTM& = CTM()) const;
         int4b*            _pdata;
         unsigned          _psize;
         TessellPoly       _teseldata;
   };

//==============================================================================
   class TdtWire : public TdtData   {
      public:
                           TdtWire(const pointlist&, WireWidth);
                           TdtWire(int4b*, unsigned, WireWidth);
                           TdtWire(TEDfile* const tedfile);
                          ~TdtWire();
         virtual DBbox     overlap() const;
         virtual Validator* move(const CTM&, SGBitSet& plst);
         virtual void      transfer(const CTM&);
         virtual TdtData*  copy(const CTM&);

         virtual void      openGlPrecalc(layprop::DrawProperties&, pointlist&) const;
         virtual void      openGlDrawLine(layprop::DrawProperties&, const pointlist&) const;
         virtual void      openGlDrawFill(layprop::DrawProperties&, const pointlist&) const;
         virtual void      openGlDrawSel(const pointlist&, const SGBitSet*) const;
         virtual void      drawRequest(tenderer::TopRend&) const;
         virtual void      drawSRequest(tenderer::TopRend&, const SGBitSet*) const;
         virtual void      motionDraw(const layprop::DrawProperties&, ctmqueue&, SGBitSet*) const;

         virtual void      info(std::ostringstream&, real) const;
         virtual void      write(TEDfile* const tedfile) const;
         virtual void      gdsWrite(DbExportFile&) const;
         virtual void      cifWrite(DbExportFile&) const;
         virtual void      psWrite(PSFile&, const layprop::DrawProperties&) const;
         virtual word      numPoints() const {return _psize;}
         virtual bool      point_inside(const TP);
         virtual void      polyCut(pointlist&, ShapeList**){};
         virtual void      stretch(int bfactor, ShapeList**);
         virtual pointlist shape2poly() const;
         virtual word      lType() const {return _lmwire;}
      private:
         void              selectPoints(DBbox&, SGBitSet&);
         void              unselectPoints(DBbox&, SGBitSet&);
         pointlist*        movePointsSelected(const SGBitSet&, const CTM&, const CTM& = CTM()) const;
         float             get_distance(TP p1, TP p2, TP p0);
         WireWidth         _width;
         int4b*            _pdata;
         unsigned          _psize;
   };

//==============================================================================
   class TdtCellRef : public TdtData  {
   public:
                           TdtCellRef(CellDefin str, CTM trans) : TdtData(),
                                          _structure(str), _translation(trans) {};
                           TdtCellRef(TEDfile* const tedfile);
//                          ~TdtCellRef() {};
      virtual DBbox        overlap() const;
      virtual   void       vlOverlap(const layprop::DrawProperties&, DBbox&) const;
      virtual Validator*   move(const CTM& trans, SGBitSet&) {
                                            _translation *= trans; return NULL;};
      virtual void         transfer(const CTM& trans) {_translation *= trans;};
      virtual TdtData*     copy(const CTM& trans) {return DEBUG_NEW TdtCellRef(
                                               _structure,_translation*trans);};
//       TdtCellRef*          getShapeOver(TP);
      virtual void         openGlPrecalc(layprop::DrawProperties&, pointlist&) const;
      virtual void         openGlDrawLine(layprop::DrawProperties&, const pointlist&) const;
      virtual void         openGlDrawFill(layprop::DrawProperties&, const pointlist&) const;
      virtual void         openGlDrawSel(const pointlist&, const SGBitSet*) const;
      virtual void         openGlPostClean(layprop::DrawProperties&, pointlist& ptlist) const;
      virtual void         drawRequest(tenderer::TopRend&) const;
      virtual void         drawSRequest(tenderer::TopRend&, const SGBitSet*) const;
      virtual void         motionDraw(const layprop::DrawProperties&, ctmqueue&, SGBitSet*) const;

      virtual void         info(std::ostringstream&, real) const;
      virtual void         write(TEDfile* const tedfile) const;
      virtual void         gdsWrite(DbExportFile&) const;
      virtual void         cifWrite(DbExportFile&) const;
      virtual void         psWrite(PSFile&, const layprop::DrawProperties&) const;
      virtual void         ungroup(TdtDesign*, TdtCell*, AtticList*);
      virtual word         numPoints() const {return 1;};
      virtual void         polyCut(pointlist&, ShapeList**) {};
      virtual void         stretch(int bfactor, ShapeList**) {};
      virtual pointlist    shape2poly() const {return pointlist();/*return empty list*/}
      virtual ArrayProperties arrayProps() const {return ArrayProperties();}
      virtual word         lType() const {return _lmref;}
      std::string          cellname() const;
      TdtCell*             cStructure() const;
      TdtDefaultCell*      structure() const {return _structure;}
      CTM                  translation() const {return _translation;};
   protected:
      void                 selectPoints(DBbox&, SGBitSet&) {return;}
      void                 unselectPoints(DBbox&, SGBitSet&) {return;}
      CellDefin            _structure; // pointer to the cell definition
      CTM                  _translation;
//   private:
//      bool                 ref_visible(ctmstack&, const layprop::DrawProperties&) const;
   };

//==============================================================================
   class TdtCellAref : public TdtCellRef  {
   public:
                           TdtCellAref(CellDefin str, CTM trans, ArrayProperties& arrprops) :
                              TdtCellRef(str, trans), _arrprops(arrprops) {};
                           TdtCellAref(TEDfile* const tedfile);
//                          ~TdtCellAref() {};
      virtual DBbox        overlap() const;
      virtual TdtData*     copy(const CTM& trans) {return DEBUG_NEW TdtCellAref(
                              _structure,_translation * trans, _arrprops);};

      virtual void         openGlPrecalc(layprop::DrawProperties&, pointlist&) const;
      virtual void         openGlDrawLine(layprop::DrawProperties&, const pointlist&) const;
      virtual void         openGlDrawFill(layprop::DrawProperties&, const pointlist&) const;
      virtual void         openGlDrawSel(const pointlist&, const SGBitSet*) const;
      virtual void         drawRequest(tenderer::TopRend&) const;
      virtual void         drawSRequest(tenderer::TopRend&, const SGBitSet*) const;
      virtual void         motionDraw(const layprop::DrawProperties&, ctmqueue&, SGBitSet*) const;

      virtual void         info(std::ostringstream&, real) const;
      virtual void         write(TEDfile* const tedfile) const;
      virtual void         gdsWrite(DbExportFile&) const;
      virtual void         cifWrite(DbExportFile&) const;
      virtual void         psWrite(PSFile&, const layprop::DrawProperties&) const;
      virtual word         lType() const {return _lmaref;}
      void                 ungroup(TdtDesign*, TdtCell*, AtticList*);
      ArrayProperties      arrayProps() const {return _arrprops;}
   private:
      DBbox                clearOverlap() const;
      ArrayProperties      _arrprops;
   };

//==============================================================================
   class TdtText : public TdtData  {
   public:
                           TdtText(std::string text, CTM trans);
                           TdtText(TEDfile* const tedfile);
//                          ~TdtText() {};
      virtual DBbox        overlap() const;
      virtual Validator*   move(const CTM& trans, SGBitSet&) {
                                            _translation *= trans; return NULL;};
      virtual void         transfer(const CTM& trans)  {_translation *= trans;};
      virtual TdtData*     copy(const CTM& trans) {return DEBUG_NEW TdtText(
                                                  _text,_translation * trans);};
      virtual void         openGlPrecalc(layprop::DrawProperties&, pointlist&) const;
      virtual void         openGlDrawLine(layprop::DrawProperties&, const pointlist&) const;
      virtual void         openGlDrawFill(layprop::DrawProperties&, const pointlist&) const;
      virtual void         openGlDrawSel(const pointlist&, const SGBitSet*) const;
      virtual void         openGlPostClean(layprop::DrawProperties&, pointlist& ptlist) const;
      virtual void         drawRequest(tenderer::TopRend&) const;
      virtual void         drawSRequest(tenderer::TopRend&, const SGBitSet*) const;
      virtual void         motionDraw(const layprop::DrawProperties&, ctmqueue&, SGBitSet*) const;

      virtual void         info(std::ostringstream&, real) const;
      virtual void         write(TEDfile* const tedfile) const;
      virtual void         gdsWrite(DbExportFile&) const;
      virtual void         cifWrite(DbExportFile&) const;
      virtual void         psWrite(PSFile&, const layprop::DrawProperties&) const;
      virtual word         numPoints() const {return 1;};
      virtual void         polyCut(pointlist&, ShapeList**) {};
      virtual void         stretch(int bfactor, ShapeList**) {};
      virtual pointlist    shape2poly() const {return pointlist();/*return empty list*/}
      virtual word         lType() const {return _lmtext;}
      const std::string    text() const {return _text;}
      void                 replaceStr(std::string newstr);
   protected:
      void                 selectPoints(DBbox&, SGBitSet&) {return;};
      void                 unselectPoints(DBbox&, SGBitSet&) {return;};
      CTM                  renderingAdjustment(const CTM&) const;
   private:
      std::string         _text;
      CTM                 _translation;
      DBbox               _overlap;
      TP                  _correction;
   };

//==============================================================================

   class ValidBox  : public Validator {
   public:
                                ValidBox(pointlist&);
      virtual laydata::TdtData* replacement();
      virtual std::string       failType();
      real                      area() {return _area;}
   private:
      real              _area;
   };

   //===========================================================================
   class ValidPoly : public Validator {
   public:
                                ValidPoly(pointlist&);
      virtual laydata::TdtData* replacement();
      virtual std::string       failType();
   private:
      void                      angles();
      void                      normalize();
      void                      selfcrossing();
   };

   //===========================================================================
   class ValidWire : public Validator {
   public:
                                ValidWire(pointlist&, WireWidth);
      virtual laydata::TdtData* replacement();
      virtual std::string       failType();
   private:
      void                      angles();
      void                      endSegments();
      void                      selfcrossing();
      WireWidth                 _width;
   };
   //===========================================================================
   int            xangle(const TP&, const TP&);

//   void draw_select_marks(const DBbox&, const CTM&);
//   void draw_select_mark(const TP&);
//   void draw_overlapping_box(const DBbox&, const CTM&, const GLushort);
   TdtData* polymerge(const pointlist&, const pointlist&);
   TdtData* createValidShape(pointlist*);



//==============================================================================
   class TdtTmpData {
      public:
         virtual void      draw(const layprop::DrawProperties&, ctmqueue&) const = 0;
         //! Add a point to the TdtData object. Used to handle the objects under construction on the screen.
         virtual void      addpoint(TP){assert(false);}
         //! Removes a point from the TdtData object. Used to handle the objects under construction on the screen.
         virtual void      rmpoint(TP&){assert(false);}
         //! Flips the object. Used to handle the objects under construction on the screen.
         virtual void      objFlip()   {assert(false);}
         //! Rotates the object. Used to handle the objects under construction on the screen.
         virtual void      objRotate() {assert(false);}
         virtual          ~TdtTmpData(){};
   };

//==============================================================================
   class TdtTmpBox : public TdtTmpData {
      public:
                           TdtTmpBox() : _p1(NULL), _p2(NULL) {};
                          ~TdtTmpBox();
         virtual void      draw(const layprop::DrawProperties&, ctmqueue& ) const;
         virtual void      addpoint(TP);
         virtual void      rmpoint(TP&);
      private:
         TP*               _p1;
         TP*               _p2;
   };

//==============================================================================
   class TdtTmpPoly : public TdtTmpData {
      public:
                           TdtTmpPoly() {};
                          ~TdtTmpPoly() {};
         virtual void      draw(const layprop::DrawProperties&, ctmqueue& ) const;
         virtual void      addpoint(TP p)  {_plist.push_back(p);}
         virtual void      rmpoint(TP&);
      private:
         pointlist         _plist;
   };

//==============================================================================
   class TdtTmpWire : public TdtTmpData {
      public:
                           TdtTmpWire(WireWidth width) : _width(width)  {};
                          ~TdtTmpWire(){};
         virtual void      draw(const layprop::DrawProperties&, ctmqueue& ) const;
         virtual void      addpoint(TP p) {_plist.push_back(p);}
         virtual void      rmpoint(TP&);
      private:
         typedef std::list<TP>     TmpPlist;
         void              drawline(const pointlist&, const pointlist&) const;
         pointlist         _plist;
         WireWidth         _width;
   };

//==============================================================================
   class TdtTmpCellRef : public TdtTmpData {
      public:
                           TdtTmpCellRef(CellDefin str, CTM trans) :
                                       _structure(str), _translation(trans) {};
                          ~TdtTmpCellRef(){};
         virtual void      draw(const layprop::DrawProperties&, ctmqueue&) const;
         void              objFlip()   {_translation.FlipY(0.0)   ;}
         void              objRotate() {_translation.Rotate( 90.0);}
      protected:
         CellDefin         _structure; // pair (name - cell) pointer
         CTM               _translation;
   };

//==============================================================================
   class TdtTmpCellAref : public TdtTmpCellRef {
      public:
                           TdtTmpCellAref(CellDefin str, CTM trans, ArrayProperties& arrprops) :
                              TdtTmpCellRef(str, trans), _arrprops(arrprops) {};
                          ~TdtTmpCellAref(){};
         virtual void      draw(const layprop::DrawProperties&, ctmqueue&) const;
      private:
         ArrayProperties   _arrprops;
   };

//==============================================================================
   class TdtTmpText : public TdtTmpData {
      public:
                           TdtTmpText(std::string text, CTM trans);
                          ~TdtTmpText(){};
         virtual void      draw(const layprop::DrawProperties&, ctmqueue&) const;
         void              objFlip()   {_translation.FlipY(0.0)   ;}
         void              objRotate() {_translation.Rotate( 90.0);}
      protected:
         std::string       _text;
         CTM               _translation;
         DBbox             _overlap;
   };

}
#endif
