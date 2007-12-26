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
//        Created: Thu May  6 22:04:50 BST 2004
//     Originator: Svilen Krustev - skr@toped.org.uk
//    Description: Main Toped framework
//---------------------------------------------------------------------------
//  Revision info
//---------------------------------------------------------------------------
//      $Revision$
//          $Date$
//        $Author$
//===========================================================================

#include "tpdph.h"
#include <wx/sizer.h>
#include <wx/filename.h>
#include <wx/image.h>
#include <wx/aboutdlg.h>
#include <wx/tooltip.h>
#include <math.h>
#include "toped.h"
#include "../tpd_DB/datacenter.h"
#include "../tpd_DB/viewprop.h"
#include "tui.h"
#include "../ui/red_lamp.xpm"
#include "../ui/green_lamp.xpm"
#include "../ui/blue_lamp.xpm"
#include "../ui/toped32x32.xpm"
#ifndef WIN32
   #include "../ui/toped16x16.xpm"
#endif
#if wxCHECK_VERSION(2, 8, 0)
#define tpdfOPEN wxFD_OPEN
#define tpdfSAVE wxFD_SAVE
#else
#define tpdfOPEN wxOPEN
#define tpdfSAVE wxSAVE
#endif

extern const wxEventType         wxEVT_CANVAS_STATUS;
extern const wxEventType         wxEVT_CANVAS_ZOOM;
extern const wxEventType         wxEVT_SETINGSMENU;
extern const wxEventType         wxEVT_MOUSE_ACCEL;
extern const wxEventType         wxEVT_TPDSTATUS;
extern const wxEventType         wxEVT_CURRENT_LAYER;

extern DataCenter*               DATC;
extern console::ted_cmd*         Console;

tui::CanvasStatus::CanvasStatus(wxWindow* parent,
										  wxWindowID id , const wxPoint& pos ,
										  const wxSize& size , long style)
											: wxPanel( parent, -1, pos, size, style) 
{
   wxFont fontX = GetFont();
   fontX.SetWeight(wxBOLD);
   SetFont(fontX);
   _abort = DEBUG_NEW wxButton(this, TBSTAT_ABORT, wxT("Abort"));
   _abort->Disable();
   SetBackgroundColour(wxColour(wxT("BLACK")));
   SetForegroundColour(wxColour(wxT("CYAN")));
   X_pos = DEBUG_NEW wxStaticText(this, -1, wxT("0.00"), wxDefaultPosition, wxSize(80,20),
                                                   wxST_NO_AUTORESIZE /*| wxALIGN_RIGHT*/);
   Y_pos = DEBUG_NEW wxStaticText(this, -1, wxT("0.00"), wxDefaultPosition, wxSize(80,20),
                                                   wxST_NO_AUTORESIZE /*| wxALIGN_RIGHT*/);
   _dX = DEBUG_NEW wxStaticText(this, -1, wxT(""), wxDefaultPosition, wxSize(80,20),
                                                   wxST_NO_AUTORESIZE /*| wxALIGN_RIGHT*/);
   _dY = DEBUG_NEW wxStaticText(this, -1, wxT(""), wxDefaultPosition, wxSize(100,20),
                                                   wxST_NO_AUTORESIZE/* | wxALIGN_RIGHT*/);
   
   _selected = DEBUG_NEW wxStaticText(this, -1, wxT("0"), wxDefaultPosition, wxSize(40,20),
                                                   wxST_NO_AUTORESIZE | wxALIGN_LEFT);
   wxBoxSizer *thesizer = DEBUG_NEW wxBoxSizer( wxHORIZONTAL );
   thesizer->Add(_abort, 0, wxALL | wxALIGN_CENTER, 3);
   thesizer->Add(10,0,1);
   thesizer->Add(DEBUG_NEW wxStaticText(this, -1, wxT("Selected: ")), 0, wxALIGN_CENTER, 3 );
   thesizer->Add(_selected, 0, wxALIGN_CENTER, 3 );
//   thesizer->Add(10,0,1);  
   thesizer->Add(DEBUG_NEW wxStaticText(this, -1, wxT("dX: ")), 0, wxLEFT | wxRIGHT | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, 3 );
   thesizer->Add(_dX, 0, wxALIGN_CENTER, 3 );
   thesizer->Add(DEBUG_NEW wxStaticText(this, -1, wxT("dY: ")), 0, wxLEFT | wxRIGHT | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, 3 );
   thesizer->Add(_dY, 0, wxALIGN_CENTER, 3 );
//   thesizer->Add(20,0,1);  
   thesizer->Add(DEBUG_NEW wxStaticText(this, -1, wxT("X: ")), 0, wxLEFT | wxRIGHT | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, 3 );
   thesizer->Add(X_pos, 0, wxALIGN_CENTER, 3 );
   thesizer->Add(DEBUG_NEW wxStaticText(this, -1, wxT("Y: ")), 0, wxLEFT | wxRIGHT | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, 3 );
   thesizer->Add(Y_pos, 0, wxALIGN_CENTER, 3 );
   SetSizerAndFit(thesizer);
}

tui::CanvasStatus::~CanvasStatus() {
   delete X_pos;
   delete Y_pos;
   delete _dX;
   delete _dY;
   delete _selected;
   delete _abort;
}

void tui::CanvasStatus::setXpos(wxString coordX){
   X_pos->SetLabel(coordX);
   X_pos->Refresh();
}

void tui::CanvasStatus::setYpos(wxString coordY){
   Y_pos->SetLabel(coordY);
   Y_pos->Refresh();
}

void tui::CanvasStatus::setdXpos(wxString coordX){
   _dX->SetLabel(coordX);
   _dX->Refresh();
}

void tui::CanvasStatus::setdYpos(wxString coordY){
   _dY->SetLabel(coordY);
   _dY->Refresh();
}
   
void tui::CanvasStatus::setSelected(wxString numsel) {
   _selected->SetLabel(numsel);
}
//-----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(tui::TopedStatus, wxStatusBar)
    EVT_SIZE(tui::TopedStatus::OnSize)
END_EVENT_TABLE()

tui::TopedStatus::TopedStatus(wxWindow* parent) : wxStatusBar(parent, wxID_ANY)
{
   const unsigned Field_Max = 3;
   static const int widths[Field_Max] = { -1, -1, 32 };

    SetFieldsCount(Field_Max);
    SetStatusWidths(Field_Max, widths);
    _lamp = DEBUG_NEW wxStaticBitmap(this, wxID_ANY, wxIcon(green_lamp));
}

void tui::TopedStatus::OnThreadON(wxString cmd)
{
   SetStatusText(cmd,1);
   _lamp->SetBitmap(wxIcon(red_lamp));
}

void tui::TopedStatus::OnThreadWait()
{
//   SetStatusText(wxT("Ready..."),1);
   _lamp->SetBitmap(wxIcon(blue_lamp));
}

void tui::TopedStatus::OnThreadOFF()
{
   SetStatusText(wxT("Ready..."),1);
   _lamp->SetBitmap(wxIcon(green_lamp));
}

void tui::TopedStatus::OnSize(wxSizeEvent& event)
{
    wxRect rect;
    GetFieldRect(2, rect);
    wxSize size = _lamp->GetSize();

    _lamp->Move(rect.x + (rect.width - size.x) / 2,
                rect.y + (rect.height - size.y) / 2);

    event.Skip();
}

//-----------------------------------------------------------------------------
// The TopedFrame event table (TOPED main event table)
BEGIN_EVENT_TABLE( tui::TopedFrame, wxFrame )
   EVT_MENU( TMFILE_NEW          , tui::TopedFrame::OnNewDesign   )
   EVT_MENU( TMFILE_OPEN         , tui::TopedFrame::OnTDTRead     )
   EVT_MENU( TMFILE_INCLUDE      , tui::TopedFrame::OnTELLRead    )
   EVT_MENU( TMGDS_OPEN          , tui::TopedFrame::OnGDSRead     )
   EVT_MENU( TMGDS_IMPORT        , tui::TopedFrame::OnGDSimport   )
   
   EVT_MENU( TMGDS_TRANSLATE     , tui::TopedFrame::OnGDStranslate)
   EVT_MENU( TMGDS_EXPORTL       , tui::TopedFrame::OnGDSexportLIB)
   EVT_MENU( TMGDS_EXPORTC       , tui::TopedFrame::OnGDSexportCELL)
   
   EVT_MENU( TMGDS_CLOSE         , tui::TopedFrame::OnGDSclose    )
   EVT_MENU( TMFILE_SAVE         , tui::TopedFrame::OnTDTSave     )
   EVT_MENU( TMFILE_SAVEAS       , tui::TopedFrame::OnTDTSaveAs   )
   EVT_MENU( TMPROP_SAVE         , tui::TopedFrame::OnPropSave    )
   EVT_MENU( TMFILE_EXIT         , tui::TopedFrame::OnQuit        )
   
   EVT_MENU( TMEDIT_UNDO         , tui::TopedFrame::OnUndo        )
   EVT_MENU( TMEDIT_COPY         , tui::TopedFrame::OnCopy        )
   EVT_MENU( TMEDIT_MOVE         , tui::TopedFrame::OnMove        )
   EVT_MENU( TMEDIT_DELETE       , tui::TopedFrame::OnDelete      )
   EVT_MENU( TMEDIT_ROTATE90     , tui::TopedFrame::OnRotate      )
   EVT_MENU( TMEDIT_FLIPX        , tui::TopedFrame::OnFlipX       )
   EVT_MENU( TMEDIT_FLIPY        , tui::TopedFrame::OnFlipY       )
   EVT_MENU( TMEDIT_POLYCUT      , tui::TopedFrame::OnPolyCut     )
   EVT_MENU( TMEDIT_MERGE        , tui::TopedFrame::OnMerge       )
   
   EVT_MENU( TMVIEW_ZOOMIN       , tui::TopedFrame::OnzoomIn      )
   EVT_MENU( TMVIEW_ZOOMOUT      , tui::TopedFrame::OnzoomOut     )
   EVT_MENU( TMVIEW_PANLEFT      , tui::TopedFrame::OnpanLeft     )
   EVT_MENU( TMVIEW_PANRIGHT     , tui::TopedFrame::OnpanRight    )
   EVT_MENU( TMVIEW_PANUP        , tui::TopedFrame::OnpanUp       )
   EVT_MENU( TMVIEW_PANDOWN      , tui::TopedFrame::OnpanDown     )
   EVT_MENU( TMVIEW_ZOOMALL      , tui::TopedFrame::OnZoomAll     )
   
   EVT_MENU( TMCELL_NEW          , tui::TopedFrame::OnCellNew     )
   EVT_MENU( TMCELL_OPEN         , tui::TopedFrame::OnCellOpen    )
   EVT_MENU( TMCELL_REMOVE       , tui::TopedFrame::OnCellRemove  )
   EVT_MENU( TMCELL_PUSH         , tui::TopedFrame::OnCellPush    )
   EVT_MENU( TMCELL_PREV         , tui::TopedFrame::OnCellPrev    )
   EVT_MENU( TMCELL_POP          , tui::TopedFrame::OnCellPop     )
   EVT_MENU( TMCELL_TOP          , tui::TopedFrame::OnCellTop     )
   EVT_MENU( TMCELL_REF_B        , tui::TopedFrame::OnCellRef_B   )
   EVT_MENU( TMCELL_REF_M        , tui::TopedFrame::OnCellRef_M   )
   EVT_MENU( TMCELL_AREF_B       , tui::TopedFrame::OnCellARef_B  )
   EVT_MENU( TMCELL_AREF_M       , tui::TopedFrame::OnCellARef_M  )
   EVT_MENU( TMCELL_GROUP        , tui::TopedFrame::OnCellGroup   )
   EVT_MENU( TMCELL_UNGROUP      , tui::TopedFrame::OnCellUngroup )
   
   EVT_MENU( TMDRAW_BOX          , tui::TopedFrame::OnDrawBox     )
   EVT_MENU( TMDRAW_POLY         , tui::TopedFrame::OnDrawPoly    )
   EVT_MENU( TMDRAW_WIRE         , tui::TopedFrame::OnDrawWire    )
   EVT_MENU( TMDRAW_TEXT         , tui::TopedFrame::OnDrawText    )
   
   EVT_MENU( TMSEL_SELECT_IN     , tui::TopedFrame::OnSelectIn    )
   EVT_MENU( TMSEL_PSELECT_IN    , tui::TopedFrame::OnPselectIn   )
   EVT_MENU( TMSEL_SELECT_ALL    , tui::TopedFrame::OnSelectAll   )
   EVT_MENU( TMSEL_UNSELECT_IN   , tui::TopedFrame::OnUnselectIn  )
   EVT_MENU( TMSEL_PUNSELECT_IN  , tui::TopedFrame::OnPunselectIn )
   EVT_MENU( TMSEL_UNSELECT_ALL  , tui::TopedFrame::OnUnselectAll )
   

   EVT_MENU( TMSET_STEP          , tui::TopedFrame::OnStep        )
   EVT_MENU( TMSET_AUTOPAN       , tui::TopedFrame::OnAutopan     )
   EVT_MENU( TMSET_GRIDDEF       , tui::TopedFrame::OnGridDefine  )
   EVT_MENU( TMSET_GRID0         , tui::TopedFrame::OnGrid0       )
   EVT_MENU( TMSET_GRID1         , tui::TopedFrame::OnGrid1       )
   EVT_MENU( TMSET_GRID2         , tui::TopedFrame::OnGrid2       )
   EVT_MENU( TMSET_CELLMARK      , tui::TopedFrame::OnCellMark    )
   EVT_MENU( TMSET_TEXTMARK      , tui::TopedFrame::OnTextMark    )
   EVT_MENU( TMSET_CELLBOX       , tui::TopedFrame::OnCellBox     )
   EVT_MENU( TMSET_TEXTBOX       , tui::TopedFrame::OnTextBox     )
   
   EVT_MENU( TMSET_MARKER0       , tui::TopedFrame::OnMarker0     )
   EVT_MENU( TMSET_MARKER45      , tui::TopedFrame::OnMarker45    )
   EVT_MENU( TMSET_MARKER90      , tui::TopedFrame::OnMarker90    )
   EVT_MENU( TMSET_CURLONG       , tui::TopedFrame::OnLongCursor  )
   EVT_MENU( TMSET_DEFLAY        , tui::TopedFrame::OnDefineLayer )
   EVT_MENU( TMSET_DEFCOLOR      , tui::TopedFrame::OnDefineColor )
   EVT_MENU( TMSET_DEFFILL       , tui::TopedFrame::OnDefineFill  )
         
   EVT_MENU( TMADD_RULER         , tui::TopedFrame::OnAddRuler    )
   EVT_MENU( TMCLEAR_RULERS      , tui::TopedFrame::OnClearRulers )
  
      // EVT_MENU( TMHELP_ABOUTAPP     , tui::TopedFrame::OnAbout       )
   EVT_MENU_RANGE(TMDUMMY, TMDUMMY+2000 , tui::TopedFrame::OnMenu  )
   EVT_BUTTON(TBSTAT_ABORT       , tui::TopedFrame::OnAbort       )
   EVT_CLOSE(tui::TopedFrame::OnClose)
   EVT_SIZE( TopedFrame::OnSize )
   //EVT_SASH_DRAGGED_RANGE(ID_WIN_BROWSERS, ID_WIN_CANVAS, tui::TopedFrame::OnSashDrag)
   EVT_TECUSTOM_COMMAND(wxEVT_TPDSTATUS  , wxID_ANY, tui::TopedFrame::OnTopedStatus)
   EVT_TECUSTOM_COMMAND(wxEVT_CANVAS_STATUS, wxID_ANY, tui::TopedFrame::OnCanvasStatus)
   EVT_TECUSTOM_COMMAND(wxEVT_SETINGSMENU, wxID_ANY, tui::TopedFrame::OnUpdateSettingsMenu)
   EVT_TECUSTOM_COMMAND(wxEVT_MOUSE_ACCEL, wxID_ANY, tui::TopedFrame::OnMouseAccel)
   EVT_TECUSTOM_COMMAND(wxEVT_CURRENT_LAYER, wxID_ANY, tui::TopedFrame::OnCurrentLayer)
   EVT_COMMAND(wxID_ANY, wxEVT_COMMAND_ENTER, tui::TopedFrame::OnUncapturedMouseClick)
END_EVENT_TABLE()

// See the FIXME note in the bootom of browsers.cpp
//   EVT_COMMAND(wxID_ANY, wxEVT_INIT_DIALOG , tui::TopedFrame::OnDefineLayer )
   
   

tui::TopedFrame::TopedFrame(const wxString& title, const wxPoint& pos, 
                            const wxSize& size ) : wxFrame((wxFrame *)NULL, ID_WIN_TOPED, title, pos, size)
{
   SetIcon(wxICON(toped16x16));
   initView();
   wxCommandEvent dummy;
   OnzoomEmpty(dummy);
  
//   CreateStatusBar();
   _toped_status = DEBUG_NEW TopedStatus(this);
   SetStatusBar(_toped_status);
   _resourceCenter = DEBUG_NEW ResourceCenter;
   SetStatusText( wxT( "Toped loaded..." ) );
	//Put initMenuBar() at the end because in Windows it crashes
   initMenuBar();
	wxToolTip::Enable(true);
	wxToolTip::SetDelay(3000);
}

void tui::TopedFrame::OnClose(wxCloseEvent& event)
{
   if (event.CanVeto())
   {
      if (DATC->modified()) {
         wxMessageDialog dlg1(this,
                              wxT("Save the current design before closing?\n(Cancel to continue the session)"),
                              wxT("Current design contains unsaved data"),
                              wxYES_NO | wxCANCEL | wxICON_QUESTION);
         switch (dlg1.ShowModal()) {
            case wxID_YES:{
               wxCommandEvent event(wxEVT_COMMAND_MENU_SELECTED, m_windowId);
               event.SetEventObject(this);
               //GetEventHandler()->ProcessEvent(event);
               OnTDTSave(event);
            }
            case wxID_NO: break;
            case wxID_CANCEL: {
               event.Veto(TRUE);
               return;
            }
         }
      }
      delete this;
   }
   else delete this;
}

tui::TopedFrame::~TopedFrame() {
//   delete _laycanvas;
//   delete _cmdline;
//   delete _GLstatus;
//   delete _browsers;
	_winManager.UnInit();
   //delete mS_browsers;
   //delete mS_GLstatus;
   //delete mS_command;
   //delete mS_log;
   //delete mS_canvas;
   delete _resourceCenter;
//   delete _toped_status;
}

void tui::TopedFrame::initMenuBar() {
   //---------------------------------------------------------------------------
   // menuBar entry fileMenu
   /*gdsMenu=DEBUG_NEW wxMenu();
   gdsMenu->Append(TMGDS_OPEN   , wxT("parse")  , wxT("Parse GDS file"));
   gdsMenu->Append(TMGDS_TRANSLATE , wxT("translate to library") , wxT("Import GDS structure"));
   gdsMenu->Append(TMGDS_EXPORTC, wxT("export cell") , wxT("Export cell to GDS"));
   gdsMenu->Append(TMGDS_CLOSE  , wxT("close")  , wxT("Clear the parsed GDS file from memory"));
   */
   

   /*fileMenu=DEBUG_NEW wxMenu();
   fileMenu->Append(TMFILE_NEW   , wxT("New ...\tCTRL-N")    , wxT("Create DEBUG_NEW design"));
   fileMenu->Append(TMFILE_OPEN  , wxT("Open ...\tCTRL-O")   , wxT("Open a TDT file"));
   fileMenu->Append(TMFILE_INCLUDE, wxT("Include ...")       , wxT("Include a TELL file"));
   fileMenu->AppendSeparator();
   fileMenu->Append(TMGDS_EXPORTL, wxT("Export library to GDS") , wxT("Export library to GDS"));
   fileMenu->Append(TMGDS_IMPORT , wxT("Import GDS to library") , wxT("Import GDS structure"));
   //fileMenu->Append(TMGDS_MENU   , wxT("Advanced GDS operations") , gdsMenu , wxT("More granulated GDS related functions"));
   fileMenu->AppendSeparator();
   fileMenu->Append(TMFILE_SAVE  , wxT("Save\tCTRL-S")       , wxT("Save the database"));
   fileMenu->Append(TMFILE_SAVEAS, wxT("Save as ..."), wxT("Save the database under a DEBUG_NEW name"));
   fileMenu->AppendSeparator();
   fileMenu->Append(TMFILE_EXIT  , wxT("Exit")       , wxT("Exit Toped"));*/

   menuBar = DEBUG_NEW wxMenuBar();
   SetMenuBar( menuBar );

   _resourceCenter->appendMenu("&File/New ...",    "CTRL-N", &tui::TopedFrame::OnNewDesign,  "Create new design");
   _resourceCenter->appendMenu("&File/Open ...",   "CTRL-O", &tui::TopedFrame::OnTDTRead, "Open a TDT file" );
   _resourceCenter->appendMenu("&File/Include ...","",      &tui::TopedFrame::OnTELLRead, "Include a TELL file" );
   _resourceCenter->appendMenuSeparator("&File");
   _resourceCenter->appendMenu("&File/Export library to GDS","",  &tui::TopedFrame::OnGDSexportLIB, "Export library to GDS");
   _resourceCenter->appendMenu("&File/Import GDS to library","",  &tui::TopedFrame::OnGDSimport, "Import GDS structure" );
   
   _resourceCenter->appendMenu("&File/Advanced GDS operations/parse",   
            "", &tui::TopedFrame::OnGDSRead, "Parse GDS file" );
   _resourceCenter->appendMenu("&File/Advanced GDS operations/translate to library",   
            "", &tui::TopedFrame::OnGDStranslate, "Import GDS structure" );
   _resourceCenter->appendMenu("&File/Advanced GDS operations/export cell",   
            "", &tui::TopedFrame::OnGDSexportCELL, "Export cell to GDS" );
   _resourceCenter->appendMenu("&File/Advanced GDS operations/close",   
            "", &tui::TopedFrame::OnGDSclose, "Clear the parsed GDS file from memory" );
   _resourceCenter->appendMenuSeparator("&File");
      
   _resourceCenter->appendMenu("&File/Save",       "CTRL-S",  &tui::TopedFrame::OnTDTSave,  "Save the database");
   _resourceCenter->appendMenu("&File/Save as ...","",  &tui::TopedFrame::OnTDTSaveAs, "Save the database under a new name" );
   _resourceCenter->appendMenu("&File/Save properties...","",  &tui::TopedFrame::OnPropSave, "Save the layout properties" );
   _resourceCenter->appendMenuSeparator("&File");
  // _resourceCenter->appendMenu("&File/Snapshot ...","",  &tui::TopedFrame::OnTDTSnapshot, "Export screen to picture" );
  // _resourceCenter->appendMenuSeparator("&File");
   _resourceCenter->appendMenu("&File/Exit",        "",  &tui::TopedFrame::OnQuit, "Exit Toped" );

   

   //---------------------------------------------------------------------------
   // menuBar entry editMenu
   /*editMenu=DEBUG_NEW wxMenu();
   editMenu->Append(TMEDIT_UNDO  , wxT("Undo\tCTRL-Z")  , wxT("Undo last operation"));
   editMenu->AppendSeparator();
   editMenu->Append(TMEDIT_COPY  , wxT("Copy\tCTRL-C")  , wxT("Copy selected shapes"));
   editMenu->Append(TMEDIT_MOVE  , wxT("Move\tCTRL-M")  , wxT("Move selected shapes"));
   editMenu->Append(TMEDIT_DELETE, wxT("Delete\tCTRL-D"), wxT("Delete selected shapes"));
   editMenu->AppendSeparator();
   editMenu->Append(TMEDIT_ROTATE90, wxT("Rotate 90\tCTRL-9"), wxT("Rotate selected shapes on 90 deg. counter clockwise "));
   editMenu->Append(TMEDIT_FLIPX, wxT("Flip X\tCTRL-X"), wxT("Flip selected shapes towards X axis "));
   editMenu->Append(TMEDIT_FLIPY, wxT("Flip Y\tCTRL-Y"), wxT("Flip selected shapes towards Y axis "));
   editMenu->Append(TMEDIT_POLYCUT, wxT("Cut with poly\tCTRL-U"), wxT("Cut selected shapes with a polygon "));
   editMenu->Append(TMEDIT_MERGE, wxT("Merge\tCTRL-G"), wxT("Merge selected shpes"));
   */
   _resourceCenter->appendMenu("&Edit/Undo",       "CTRL-Z",  &tui::TopedFrame::OnUndo, "Undo last operation" );
   _resourceCenter->appendMenuSeparator("Edit");
   _resourceCenter->appendMenu("&Edit/Copy",       "CTRL-C",  &tui::TopedFrame::OnCopy, "Copy selected shapes" );
   _resourceCenter->appendMenu("&Edit/Move",       "CTRL-M",  &tui::TopedFrame::OnMove, "Move selected shapes" );
   _resourceCenter->appendMenu("&Edit/Delete",     "CTRL-D",  &tui::TopedFrame::OnDelete, "Delete selected shapes" );
   _resourceCenter->appendMenuSeparator("Edit");
   _resourceCenter->appendMenu("&Edit/Rotate 90",  "CTRL-9",  &tui::TopedFrame::OnRotate, "Rotate selected shapes on 90 deg. counter clockwise ");
   _resourceCenter->appendMenu("&Edit/Flip X",     "CTRL-X",  &tui::TopedFrame::OnFlipX, "Flip selected shapes towards X axis " );
   _resourceCenter->appendMenu("&Edit/Flip Y",     "CTRL-Y",  &tui::TopedFrame::OnFlipY, "Flip selected shapes towards Y axis " );
   _resourceCenter->appendMenu("&Edit/Cut with poly","CTRL-U",  &tui::TopedFrame::OnPolyCut, "Cut selected shapes with a polygon " );
   _resourceCenter->appendMenu("&Edit/Cut with box","CTRL-ALT-U", &tui::TopedFrame::OnBoxCut, "Cut selected shapes with a box " );
   _resourceCenter->appendMenu("&Edit/Merge",      "CTRL-G",  &tui::TopedFrame::OnMerge, "Merge selected shpes" );
   _resourceCenter->appendMenuSeparator("Edit");
   _resourceCenter->appendMenu("&Edit/Change Layer","",&tui::TopedFrame::OnChangeLayer, "Translate the objects to another layer" );
   _resourceCenter->appendMenu("&Edit/Change Text","",&tui::TopedFrame::OnChangeText, "Replace a text contents" );

   //---------------------------------------------------------------------------
   // menuBar entry viewMenu
   /*viewMenu=DEBUG_NEW wxMenu();
   viewMenu->AppendCheckItem(TMVIEW_VIEWTOOLBAR  , wxT("Toolbar")  , wxT("Show/Hide the tool bar"));
   viewMenu->AppendCheckItem(TMVIEW_VIEWSTATUSBAR, wxT("Statusbar"), wxT("Show/Hide the status bar"));
   viewMenu->AppendSeparator();
   viewMenu->Append(TMVIEW_ZOOMIN  , wxT("Zoom in\tF2")  , wxT("Zoom in current window"));
   viewMenu->Append(TMVIEW_ZOOMOUT , wxT("Zoom out\tF3") , wxT("Zoom out current window"));
   viewMenu->Append(TMVIEW_ZOOMALL , wxT("Zoom all\tF4") , wxT("Zoom the current cell"));
   viewMenu->AppendSeparator();
   viewMenu->Append(TMVIEW_PANLEFT , wxT("Pan left\tSHIFT-LEFT") , wxT("Move the view window left"));
   viewMenu->Append(TMVIEW_PANRIGHT, wxT("Pan right\tSHIFT-RIGHT"), wxT("Move the view window right"));
   viewMenu->Append(TMVIEW_PANUP   , wxT("Pan up\tSHIFT-UP")   , wxT("Move the view window up"));
   viewMenu->Append(TMVIEW_PANDOWN , wxT("Pan down\tSHIFT-DOWN") , wxT("Move the view window down"));
   viewMenu->AppendSeparator();
   viewMenu->Check(TMVIEW_VIEWTOOLBAR, true);
   viewMenu->Check(TMVIEW_VIEWSTATUSBAR, true);*/

    //???Add Toolbar & StatusBar check Item

   _resourceCenter->appendMenu("&View/Zoom in", "F2",  &tui::TopedFrame::OnzoomIn, "Zoom in current window" );
   _resourceCenter->appendMenu("&View/Zoom out","F3",  &tui::TopedFrame::OnzoomOut, "Zoom out current window" );
   _resourceCenter->appendMenu("&View/Zoom all","F4",  &tui::TopedFrame::OnZoomAll, "Zoom the current cell" );
   _resourceCenter->appendMenuSeparator("View");
   _resourceCenter->appendMenu("&View/Pan left",   "SHIFT-LEFT",  &tui::TopedFrame::OnpanLeft, "Move the view window left" );
   _resourceCenter->appendMenu("&View/Pan right",  "SHIFT-RIGHT", &tui::TopedFrame::OnpanRight, "Move the view window right" );
   _resourceCenter->appendMenu("&View/Pan up",     "SHIFT-UP",    &tui::TopedFrame::OnpanUp, "Move the view window up" );
   _resourceCenter->appendMenu("&View/Pan down",   "SHIFT-DOWN",  &tui::TopedFrame::OnpanDown, "Move the view window down" );
   _resourceCenter->appendMenuSeparator("View");

   //---------------------------------------------------------------------------
   // menuBar entry Cell
   /*cellMenu=DEBUG_NEW wxMenu();
   cellMenu->Append(TMCELL_NEW      , wxT("New Cell") , wxT("Create a new cell"));
   cellMenu->Append(TMCELL_OPEN     , wxT("Open Cell") , wxT("Open existing cell for editing"));
   cellMenu->AppendSeparator();
   cellMenu->Append(TMCELL_PUSH     , wxT("Edit Push\tF9") , wxT("Edit in place"));
   cellMenu->Append(TMCELL_PREV     , wxT("Edit Previous\tCtrl-F9") , wxT("Edit in place"));
   cellMenu->Append(TMCELL_POP      , wxT("Edit Pop\tF10") , wxT("Edit in place"));
   cellMenu->Append(TMCELL_TOP      , wxT("Edit Top\tCtrl-F10") , wxT("Edit in place"));
   cellMenu->AppendSeparator();
   cellMenu->Append(TMCELL_REF_M    , wxT("Cell Reference\tCtrl-R") , wxT("Cell reference"));
   cellMenu->Append(TMCELL_AREF_M   , wxT("Array of References\tAlt-R") , wxT("Array of cell references"));
   cellMenu->AppendSeparator();
   cellMenu->Append(TMCELL_GROUP    , wxT("Group Cell") , wxT("Group selected shapes in a cell"));
   cellMenu->Append(TMCELL_UNGROUP  , wxT("Unroup Cell") , wxT("Ungroup selected cell references"));
   */
   _resourceCenter->appendMenu("&Cell/New Cell",      "",   &tui::TopedFrame::OnCellNew, "Create a DEBUG_NEW cell" );
   _resourceCenter->appendMenu("&Cell/Open Cell",     "",   &tui::TopedFrame::OnCellOpen,    "Open existing cell for editing" );
   _resourceCenter->appendMenu("&Cell/Remove Cell",   "",   &tui::TopedFrame::OnCellRemove,  "Remove existing cell" );
   _resourceCenter->appendMenuSeparator("Cell");
   _resourceCenter->appendMenu("&Cell/Edit Push",     "F9", &tui::TopedFrame::OnCellPush, "Edit in place" );
   _resourceCenter->appendMenu("&Cell/Edit Previous", "Ctrl-F9",  &tui::TopedFrame::OnCellPrev, "Edit in place" );
   _resourceCenter->appendMenu("&Cell/Edit Pop",      "F10",&tui::TopedFrame::OnCellPop, "Edit in place" );
   _resourceCenter->appendMenu("&Cell/Edit Top","Ctrl-F10", &tui::TopedFrame::OnCellTop, "Edit in place" );
   _resourceCenter->appendMenuSeparator("Cell");
   _resourceCenter->appendMenu("&Cell/Cell Reference",      "Ctrl-R",   &tui::TopedFrame::OnCellRef_M, "Cell reference" );
   _resourceCenter->appendMenu("&Cell/Array of References", "Alt-R",    &tui::TopedFrame::OnCellARef_M,  "Array of cell references");
   _resourceCenter->appendMenuSeparator("Cell");
   _resourceCenter->appendMenu("&Cell/Group Cell",    "",  &tui::TopedFrame::OnCellGroup, "Group selected shapes in a cell" );
   _resourceCenter->appendMenu("&Cell/Unroup Cell",   "",  &tui::TopedFrame::OnCellUngroup, "Ungroup selected cell references" );
   _resourceCenter->appendMenuSeparator("Cell");
   _resourceCenter->appendMenu("&Cell/Change Reference","",&tui::TopedFrame::OnChangeRef, "Replace a cell reference" );

   //---------------------------------------------------------------------------
   // menuBar entry Draw
   /*drawMenu=DEBUG_NEW wxMenu();
   drawMenu->Append(TMDRAW_BOX , wxT("Box\tCTRL-B")     , wxT("Create new box on the current layer"));
   drawMenu->Append(TMDRAW_POLY, wxT("Polygon\tCTRL-L") , wxT("Create new polygon on the current layer"));
   drawMenu->Append(TMDRAW_WIRE, wxT("Wire ...\tCTRL-W"), wxT("Create new wire on the current layer"));
   drawMenu->Append(TMDRAW_TEXT, wxT("Text ...\tCTRL-T"), wxT("Add text on the current layer"));
   */
   _resourceCenter->appendMenu("&Draw/Box",      "CTRL-B", &tui::TopedFrame::OnDrawBox, "Create new box on the current layer" );
   _resourceCenter->appendMenu("&Draw/Polygon",  "CTRL-L", &tui::TopedFrame::OnDrawPoly, "Create new polygon on the current layer" );
   _resourceCenter->appendMenu("&Draw/Wire ...", "CTRL-W", &tui::TopedFrame::OnDrawWire, "Create new wire on the current layer" );
   _resourceCenter->appendMenu("&Draw/Text ...", "CTRL-T", &tui::TopedFrame::OnDrawText, "Add text on the current layer" );
   

   //---------------------------------------------------------------------------
   // menuBar entry Modify
   /*selectMenu=DEBUG_NEW wxMenu();
   selectMenu->Append(TMSEL_SELECT_IN   , wxT("Select\tCTRL-I")        , wxT("Select objects"));
   selectMenu->Append(TMSEL_PSELECT_IN  , wxT("Part select\tCTRL-P")  , wxT("Select object edges"));
   selectMenu->Append(TMSEL_SELECT_ALL  , wxT("Select all\tCTRL-A")    , wxT("Select all objects in the current cell"));
   selectMenu->AppendSeparator();
   selectMenu->Append(TMSEL_UNSELECT_IN , wxT("Unselect\tALT-I")      , wxT("Unselect objects"));
   selectMenu->Append(TMSEL_PUNSELECT_IN, wxT("Part unselect\tALT-P"), wxT("Unselect object edges"));
   selectMenu->Append(TMSEL_UNSELECT_ALL, wxT("Unselect all\tALT-A")  , wxT("Unselect all"));
   */

   _resourceCenter->appendMenu("&Select/Select",      "CTRL-I", &tui::TopedFrame::OnSelectIn, "Select objects"  );
   _resourceCenter->appendMenu("&Select/Part select", "CTRL-P", &tui::TopedFrame::OnPselectIn, "Select object edges" );
   _resourceCenter->appendMenu("&Select/Select all",  "CTRL-A", &tui::TopedFrame::OnSelectAll, "Select all objects in the current cell" );
   _resourceCenter->appendMenuSeparator("Select");
   _resourceCenter->appendMenu("&Select/Unselect",    "ALT-I", &tui::TopedFrame::OnUnselectIn, "Unselect objects" );
   _resourceCenter->appendMenu("&Select/Part unselect","ALT-P", &tui::TopedFrame::OnPunselectIn, "Unselect object edges" );
   _resourceCenter->appendMenu("&Select/Unselect all", "ALT-A", &tui::TopedFrame::OnUnselectAll, "Unselect all" );
   

   //---------------------------------------------------------------------------
   // menuBar entry Settings
   // first the sub menu
   markerMenu=DEBUG_NEW wxMenu();
   markerMenu->AppendRadioItem(TMSET_MARKER0    , wxT("Free")      , wxT("Marker is not restricted"));
   markerMenu->AppendRadioItem(TMSET_MARKER45   , wxT("45 degrees"), wxT("Restrict shape angles to 45 deg"));
   markerMenu->AppendRadioItem(TMSET_MARKER90   , wxT("Orthogonal"), wxT("Restrict shape angles to 90 deg"));
   // now the setting menu itself
   settingsMenu=DEBUG_NEW wxMenu();
   settingsMenu->Append         (TMSET_STEP     , wxT("Step")      , wxT("Select objects"));
   settingsMenu->AppendCheckItem(TMSET_AUTOPAN  , wxT("Auto Pan")  , wxT("Automatic window move"));
   settingsMenu->AppendSeparator();
   settingsMenu->Append         (TMSET_GRIDDEF  , wxT("Define grid"), wxT("Define/change the grid step"));
   settingsMenu->AppendCheckItem(TMSET_GRID0    , wxT("Grid 0")    , wxT("Draw/Hide Grid 0"));
   settingsMenu->AppendCheckItem(TMSET_GRID1    , wxT("Grid 1")    , wxT("Draw/Hide Grid 1"));
   settingsMenu->AppendCheckItem(TMSET_GRID2    , wxT("Grid 2")    , wxT("Draw/Hide Grid 2"));
   settingsMenu->AppendSeparator();
   settingsMenu->AppendCheckItem(TMSET_CELLMARK , wxT("Cell marks"), wxT("Draw/Hide Cell marks"));
   settingsMenu->AppendCheckItem(TMSET_TEXTMARK , wxT("Text marks"), wxT("Draw/Hide Text marks"));
   settingsMenu->AppendCheckItem(TMSET_CELLBOX  , wxT("Cell box")  , wxT("Draw/Hide Cell overlapping box"));
   settingsMenu->AppendCheckItem(TMSET_TEXTBOX  , wxT("Text box")  , wxT("Draw/Hide Text overlapping box"));
   settingsMenu->AppendSeparator();
   settingsMenu->Append         (TMSET_MARKER   , wxT("Marker") , markerMenu , wxT("Define marker movement"));
   settingsMenu->AppendCheckItem(TMSET_CURLONG  , wxT("Long cursor")  , wxT("Stretch the cursor cross"));
   settingsMenu->AppendSeparator();
   settingsMenu->Append         (TMSET_DEFLAY   , wxT("Define Layer") , wxT("Define a layer"));
   settingsMenu->Append         (TMSET_DEFCOLOR , wxT("Define Color") , wxT("Define a drawing color"));
   settingsMenu->Append         (TMSET_DEFFILL  , wxT("Define Fill")  , wxT("Define a drawing pattern"));
   //---------------------------------------------------------------------------
   // menuBar entry helpMenu
   /*helpMenu=DEBUG_NEW wxMenu();
   helpMenu->Append(TMHELP_ABOUTAPP       , wxT("About")          , wxT("About TOPED"));
   */
   _resourceCenter->appendMenu("&Other/Add Ruler"   , "", &tui::TopedFrame::OnAddRuler, "Add new ruler" );
   _resourceCenter->appendMenu("&Other/Clear Rulers", "", &tui::TopedFrame::OnClearRulers, "Clear all rulers" );
//   _resourceCenter->appendMenuSeparator("Other");
   
   _resourceCenter->appendMenu("&Help/About", "", &tui::TopedFrame::OnAbout, "About TOPED" );
   
   //---------------------------------------------------------------------------
   // MENUBAR CONFIGURATION
   //menuBar = DEBUG_NEW wxMenuBar();
   //menuBar->Append(fileMenu      , wxT("&File"  ));
   //menuBar->Append(editMenu      , wxT("&Edit"  ));
   //menuBar->Append(viewMenu      , wxT("&View"  ));
   //menuBar->Append(cellMenu      , wxT("&Cell"  ));
   //menuBar->Append(drawMenu      , wxT("&Draw"  ));
   //menuBar->Append(selectMenu    , wxT("&Select"));
    //menuBar->Append(helpMenu      , wxT("&Help"  ));
   //SetMenuBar( menuBar );
   // default menu values
   //settingsMenu->Check(TMSET_CELLMARK,true);
   //settingsMenu->Check(TMSET_TEXTMARK,true);
   _resourceCenter->buildMenu(menuBar);
  
   menuBar->Insert(menuBar->GetMenuCount()-1,settingsMenu  , wxT("Se&ttings"));
  
}

void tui::TopedFrame::initToolBars() 
{
	_resourceCenter->appendTool("main", "new.xpm", "ui/new.xpm","", &tui::TopedFrame::OnCellNew);
/*   wxToolBar* positionBar = CreateToolBar(wxTB_DOCKABLE |  wxTB_HORIZONTAL | wxNO_BORDER);
   X_pos = DEBUG_NEW wxStaticText(positionBar, -1, "", wxDefaultPosition, 
                              wxSize(100,32), wxST_NO_AUTORESIZE);
   Y_pos = DEBUG_NEW wxStaticText(positionBar, -1, "", wxDefaultPosition, 
                              wxSize(100,32), wxST_NO_AUTORESIZE);
//wxSIMPLE_BORDER | wxALIGN_RIGHT  |
   wxFont fontX = X_pos->GetFont();
   fontX.SetPointSize(fontX.GetPointSize()*3/2);
   X_pos->SetFont(fontX);
   Y_pos->SetFont(fontX);

   X_pos->SetBackgroundColour(wxColour("WHITE"));
   Y_pos->SetBackgroundColour(wxColour("WHITE"));
   X_pos->SetLabel("0.00");
   Y_pos->SetLabel("0.00");
   X_pos->Refresh();
   Y_pos->Refresh();
   positionBar->AddControl(X_pos);
   positionBar->AddSeparator();
   positionBar->AddControl(Y_pos);
   positionBar->AddSeparator();
   positionBar->Realize();*/
}

void tui::TopedFrame::initView() {
	_winManager.SetManagedWindow(this);
	
	/*mS_browsers = DEBUG_NEW wxControl(this, ID_WIN_BROWSERS,
                                        wxDefaultPosition, wxDefaultSize,
                               wxSW_3D | wxCLIP_CHILDREN);*/
   //mS_browsers->SetSize(wxSize(180, 1000));
   //mS_browsers->SetOrientation(wxLAYOUT_VERTICAL);
   //mS_browsers->SetAlignment(wxLAYOUT_LEFT);
   //mS_browsers->SetSashVisible(wxSASH_RIGHT, TRUE);
   _browsers = DEBUG_NEW browsers::browserTAB(this,ID_WIN_BROWSERS,
                                        wxDefaultPosition, wxDefaultSize,
                               wxCLIP_CHILDREN);
	_browsers->SetSize(wxSize(180, 1000));
	_browsers->SetArtProvider(new wxAuiSimpleTabArt);
   //---------------------------------------------------------------------------- 
   // The Layoutcanvas toolbar window
   //---------------------------------------------------------------------------- 
   /*mS_GLstatus = DEBUG_NEW wxWindow(this, ID_WIN_GLSTATUS	,
                                        wxDefaultPosition, wxDefaultSize,
                             wxNO_BORDER | wxSW_3D | wxCLIP_CHILDREN);*/
   //mS_GLstatus->SetSize(wxSize(1000, 30));
   /*mS_GLstatus->SetOrientation(wxLAYOUT_HORIZONTAL);
   mS_GLstatus->SetAlignment(wxLAYOUT_TOP);
   mS_GLstatus->SetBackgroundColour(wxColour(255, 0, 0));*/
   _GLstatus = DEBUG_NEW CanvasStatus(this, ID_WIN_GLSTATUS	,
                                        wxDefaultPosition, wxDefaultSize,
                             wxNO_BORDER | wxSW_3D | wxCLIP_CHILDREN);
	_GLstatus->SetSize(wxSize(1000, 30));

   //----------------------------------------------------------------------------
   // The command window
   //----------------------------------------------------------------------------
   //mS_command = DEBUG_NEW wxWindow(this, ID_WIN_COMMAND,
   //                            wxDefaultPosition, wxDefaultSize,
   //                            wxSW_3D | wxCLIP_CHILDREN);
   /*mS_command->SetDefaultSize(wxSize(1000, 30));
   mS_command->SetOrientation(wxLAYOUT_HORIZONTAL);
   mS_command->SetAlignment(wxLAYOUT_BOTTOM);
   mS_command->SetSashVisible(wxSASH_TOP, TRUE);*/
   // postponed initialization until the canvas window is initialized
//   _cmdline = DEBUG_NEW console::ted_cmd(mS_command);

   //----------------------------------------------------------------------------
   //the log window
   //----------------------------------------------------------------------------
   /*mS_log = DEBUG_NEW wxWindow(this, ID_WIN_LOG,
                               wxDefaultPosition, wxDefaultSize,
                               wxSW_3D | wxCLIP_CHILDREN);*/
   /*mS_log->SetDefaultSize(wxSize(1000, 150));
   mS_log->SetOrientation(wxLAYOUT_HORIZONTAL);
   mS_log->SetAlignment(wxLAYOUT_BOTTOM);
   mS_log->SetSashVisible(wxSASH_TOP, TRUE);*/
   //
   wxAuiNotebook* logpane = DEBUG_NEW wxAuiNotebook(this, ID_WIN_LOG, wxDefaultPosition, wxDefaultSize, //wxAUI_NB_DEFAULT_STYLE |wxNB_RIGHT| wxAUI_NB_TAB_EXTERNAL_MOVE | wxNO_BORDER);
															wxNB_RIGHT|wxNO_BORDER);
	logpane->SetSize(wxSize(1000, 150));
	wxAuiSimpleTabArt *docArt = new wxAuiSimpleTabArt;
	logpane->SetArtProvider(docArt);

   _cmdlog = DEBUG_NEW console::ted_log(logpane);
   logpane->AddPage(_cmdlog, wxT("Log"));
   _cmdbrowser = DEBUG_NEW console::TELLFuncList(logpane);
   logpane->AddPage(_cmdbrowser, wxT("Lib"));

   //----------------------------------------------------------------------------
   // the openGL window
   //---------------------------------------------------------------------------- 
   mS_canvas = DEBUG_NEW WinCanvas(this, ID_WIN_CANVAS,
                               wxDefaultPosition, wxSize(1000,1000),
										 wxAUI_NB_DEFAULT_STYLE|wxDOUBLE_BORDER|wxSW_3D | wxCLIP_CHILDREN);



	_cmdline = DEBUG_NEW console::ted_cmd(this, mS_canvas->canvas());
	_cmdline->SetSize(wxSize(wxSize(1000, 30)));
	_cmdline->SetWindowStyleFlag(wxSW_3D | wxCLIP_CHILDREN);

   _browsers->set_tellParser( _cmdline ) ;

	_winManager.AddPane(_browsers, wxAuiPaneInfo().Left().BestSize(wxSize(180,1000)).Layer(1).
												Floatable(false).CloseButton(false).CaptionVisible(false));
	_winManager.AddPane(mS_canvas, wxAuiPaneInfo().CentrePane().MaximizeButton(false).
												Floatable(true).CloseButton(false));
	_winManager.AddPane(_GLstatus, wxAuiPaneInfo().Top().Floatable(false).//Fixed().
												CloseButton(false).CaptionVisible(false).BestSize(wxSize(1000,30)));
	_winManager.AddPane(logpane, wxAuiPaneInfo().Bottom().Row(1).
												Floatable(false).BestSize(wxSize(1000, 150)).
												CloseButton(false).CaptionVisible(false));
	_winManager.AddPane(_cmdline, wxAuiPaneInfo().Bottom().Row(0).BestSize(wxSize(1000,30)).
												Floatable(false).CloseButton(false).CaptionVisible(false));

	_winManager.Update();
}


void tui::TopedFrame::OnQuit( wxCommandEvent& WXUNUSED( event ) ) {
   Close(FALSE);
}

void tui::TopedFrame::OnAbout( wxCommandEvent& WXUNUSED( event ) ) {
    wxAboutDialogInfo info;
    info.SetName(wxT("Toped"));
    info.SetVersion(wxT("0.8.x"));
    info.SetIcon(wxIcon( toped32x32_xpm ));
    info.SetWebSite(wxT("www.toped.org.uk"));
    info.SetDescription(wxT("Open source IC layout editor"));
    info.SetCopyright(wxT("(C) 2001-2007 Toped developers"));

    wxAboutBox(info);
}


/*void tui::TopedFrame::OnSashDrag(wxSashEvent& event) {
   if (event.GetDragStatus() == wxSASH_STATUS_OUT_OF_RANGE)
      return;
   switch (event.GetId()) {
//      case ID_WIN_GLSTATUS:
//         mS_GLstatus->SetDefaultSize(wxSize(1000, event.GetDragRect().height));
//         break;
      case ID_WIN_BROWSERS:
         mS_browsers->SetDefaultSize(wxSize(event.GetDragRect().width, 1000));
         break;
      case ID_WIN_LOG:
         mS_log->SetDefaultSize(wxSize(1000, event.GetDragRect().height));
         break;
   }
   wxLayoutAlgorithm layout;
   layout.LayoutFrame(this, mS_canvas);
}*/

void tui::TopedFrame::OnSize(wxSizeEvent& event)
{
	event.Skip();
   //wxLayoutAlgorithm layout;
   //layout.LayoutFrame(this, mS_canvas);
}

void tui::TopedFrame::OnCanvasStatus(wxCommandEvent& evt)
{
   switch (evt.GetInt()) {
      case CNVS_POS_X       : _GLstatus->setXpos(evt.GetString()); break;
      case CNVS_POS_Y       : _GLstatus->setYpos(evt.GetString()); break;
      case CNVS_DEL_X       : _GLstatus->setdXpos(evt.GetString()); break;
      case CNVS_DEL_Y       : _GLstatus->setdYpos(evt.GetString()); break;
      case CNVS_SELECTED    : _GLstatus->setSelected(evt.GetString());break;
      case CNVS_ABORTENABLE : _GLstatus->btn_abort_enable(true);break;
      case CNVS_ABORTDISABLE: _GLstatus->btn_abort_enable(false);break;
      default: assert(false);
   }   
}

void tui::TopedFrame::OnTopedStatus(wxCommandEvent& evt)
{
   switch (evt.GetInt()) {
      case console::TSTS_THREADON    : _toped_status->OnThreadON(evt.GetString()); break;
      case console::TSTS_THREADWAIT  : _toped_status->OnThreadWait(); break;
      case console::TSTS_THREADOFF   : _toped_status->OnThreadOFF(); break;
      default: assert(false);
   }
}

void tui::TopedFrame::OnNewDesign(wxCommandEvent& evt) {
   if (DATC->modified()) {
      wxMessageDialog dlg1(this,
         wxT("Current design contains unsaved data"),
         wxT("Save the current design before creating a new one?"),
         wxYES_NO | wxCANCEL | wxICON_QUESTION);
      switch (dlg1.ShowModal()) {
         case wxID_YES:OnTDTSave(evt);
         case wxID_NO: break;
         case wxID_CANCEL: return;
      }
   }
   wxTextEntryDialog dlg2(this,
      wxT("Name the new design"),
      wxT("Design name:"));
   wxString dname, ost;
   if ((wxID_OK == dlg2.ShowModal()) && ((dname = dlg2.GetValue()) != wxT(""))) {
      SetStatusText(wxT("Creating new file..."));
      ost << wxT("newdesign(\"") << dname << wxT("\");");
      _cmdline->parseCommand(ost);
      SetTitle(dname);
   }
   else SetStatusText(wxT("New file not created"));
}

void tui::TopedFrame::OnTDTRead(wxCommandEvent& evt) {
   if (DATC->modified()) {
      wxMessageDialog dlg1(this,
         wxT("Current design contains unsaved data"),
         wxT("Save the current design before creating a new one?"),
         wxYES_NO | wxCANCEL | wxICON_QUESTION);
      switch (dlg1.ShowModal()) {
         case wxID_YES:OnTDTSave(evt);
         case wxID_NO: break;
         case wxID_CANCEL: return;
      }
   }
   SetStatusText(wxT("Opening file..."));
   wxFileDialog dlg2(this, wxT("Select a file"), wxT(""), wxT(""),
      wxT("Toped files (*.tdt)|*.tdt|All files(*.*)|*.*"),
      tpdfOPEN);
   if (wxID_OK == dlg2.ShowModal()) {
      wxString filename = dlg2.GetFilename();
      wxString ost;
      ost << wxT("tdtread(\"") << dlg2.GetDirectory() << wxT("/") << dlg2.GetFilename() << wxT("\");");
      _cmdline->parseCommand(ost);
      wxString ost1;
//      ost1 << wxT("File ") << dlg2.GetFilename() << wxT(" loaded");
//      SetStatusText(ost1);
      SetTitle(dlg2.GetFilename());
   }
   else SetStatusText(wxT("Opening aborted"));
}

void tui::TopedFrame::OnTELLRead(wxCommandEvent& evt) {
   SetStatusText(wxT("Including command file..."));
   wxFileDialog dlg2(this, wxT("Select a file"), wxT(""), wxT(""),
      wxT("Tell files(*.tll)|*.tll|All files(*.*)|*.*"),
      tpdfOPEN);
   if (wxID_OK == dlg2.ShowModal()) {
      wxString filename = dlg2.GetFilename();
      wxString ost;
      ost << wxT("#include \"") << dlg2.GetDirectory() << wxT("/") << dlg2.GetFilename() << wxT("\";");
      _cmdline->parseCommand(ost);
//      SetStatusText(dlg2.GetFilename() + wxT(" parsed"));
   }
   else SetStatusText(wxT("include aborted"));
}

void tui::TopedFrame::OnGDSRead(wxCommandEvent& WXUNUSED(event)) {
   wxFileDialog dlg2(this, wxT("Select a file"), wxT(""), wxT(""),
                     wxT("Stream files(*.sf;*.gds)|*.sf;*.gds;*.SF;*.GDS|All files(*.*)|*.*"),
      tpdfOPEN);
   if (wxID_OK == dlg2.ShowModal()) {
      SetStatusText(wxT("Parsing GDS file..."));
      wxString filename = dlg2.GetFilename();
      wxString ost;
      ost << wxT("gdsread(\"") << dlg2.GetDirectory() << wxT("/") <<dlg2.GetFilename() << wxT("\");");
      _cmdline->parseCommand(ost);
//      wxString ost1;
//      ost1 << wxT("Stream ") << dlg2.GetFilename() << wxT(" loaded");
//      SetStatusText(ost1);
   }
   else SetStatusText(wxT("Parsing aborted"));
}

void tui::TopedFrame::OnTDTSave(wxCommandEvent& WXUNUSED(event)) {
   wxString ost; 
   ost << wxT("tdtsave();");
   SetStatusText(wxT("Saving file..."));
   wxString wxfilename(DATC->tedfilename().c_str(), wxConvUTF8);
   wxFileName datafile( wxfilename );
   assert(datafile.IsOk());
   if (datafile.FileExists() && DATC->neversaved()) {
      wxMessageDialog dlg1(this,
         wxT("File ") + wxfilename + wxT(" already exists. Overwrite ?"),
         wxT("Toped"),
         wxYES_NO | wxICON_QUESTION);
      switch (dlg1.ShowModal()) {
         case wxID_YES:_cmdline->parseCommand(ost); //Overwrite;
         case wxID_NO: return;
      }
   }
   else _cmdline->parseCommand(ost);;
//   SetStatusText(wxT("Design saved"));
}

void tui::TopedFrame::OnTDTSaveAs(wxCommandEvent& WXUNUSED(event)) {
   SetStatusText(wxT("Saving database under new filename..."));
   wxFileDialog dlg2(this, wxT("Save a design in a file"), wxT(""), wxT(""),
      wxT("Toped files |*.tdt"),
      tpdfSAVE);
   if (wxID_OK == dlg2.ShowModal()) {
      wxString filename = dlg2.GetPath();
      if(!checkFileOverwriting(filename))
      {
         SetStatusText(wxT("Saving aborted"));
         return;
      }

      wxString ost;
      ost << wxT("tdtsaveas(\"") << dlg2.GetDirectory() << wxT("/") <<dlg2.GetFilename() << wxT("\");");
      _cmdline->parseCommand(ost);
//      SetStatusText(wxT("Design saved in file: ")+dlg2.GetFilename());
   }   
   else SetStatusText(wxT("Saving aborted"));
}

void tui::TopedFrame::OnPropSave(wxCommandEvent& WXUNUSED(event)) 
{
   SetStatusText(wxT("Saving layout properties ..."));
   wxFileDialog dlg2(this, wxT("Save properties"), wxT(""), wxT(""),
      wxT("TELL files |*.tll"),
      tpdfSAVE);
   if (wxID_OK == dlg2.ShowModal()) {
      wxString filename = dlg2.GetPath();
      if(!checkFileOverwriting(filename))
      {
         SetStatusText(wxT("Saving aborted"));
         return;
      }

      wxString ost;
      ost << wxT("propsave(\"") << dlg2.GetDirectory() << wxT("/") <<dlg2.GetFilename() << wxT("\");");
      _cmdline->parseCommand(ost);
//      SetStatusText(wxT("Design saved in file: ")+dlg2.GetFilename());
   }
   else SetStatusText(wxT("Saving aborted"));
}

void tui::TopedFrame::OnTDTSnapshot(wxCommandEvent&)
{
	wxImage image = mS_canvas->canvas()->snapshot();

}

void tui::TopedFrame::OnCellNew(wxCommandEvent& WXUNUSED(event)) {
   wxTextEntryDialog dlg2(this,
      wxT("Cell name:"),
      wxT("Create new cell"));
   wxString cname, ost;
   if ((wxID_OK == dlg2.ShowModal()) && ((cname = dlg2.GetValue()) != wxT(""))) {
      SetStatusText(wxT("Creating new cell..."));
      ost << wxT("newcell(\"") << cname << wxT("\");");
      _cmdline->parseCommand(ost);
   }
   else SetStatusText(wxT("New cell not created"));
}

void tui::TopedFrame::OnCellOpen(wxCommandEvent& WXUNUSED(event)) {
   wxRect wnd = GetRect();
   wxPoint pos(wnd.x+wnd.width/2-100,wnd.y+wnd.height/2-50);
   tui::getCellOpen* dlg = NULL;
   try {
      dlg = DEBUG_NEW tui::getCellOpen(this, -1, wxT("Cell Open"), pos, wxT(""));
   }
   catch (EXPTN) {delete dlg;return;}
   if ( dlg->ShowModal() == wxID_OK ) {
      wxString ost;
      ost << wxT("opencell(\"") << dlg->get_selectedcell() << wxT("\");");
      _cmdline->parseCommand(ost);
   }
   delete dlg;
}

void tui::TopedFrame::OnCellRemove(wxCommandEvent&)
{
   wxRect wnd = GetRect();
   wxPoint pos(wnd.x+wnd.width/2-100,wnd.y+wnd.height/2-50);
   tui::getCellOpen* dlg = NULL;
   try {
      dlg = DEBUG_NEW tui::getCellOpen(this, -1, wxT("Cell Remove"), pos, wxT(""));
   }
   catch (EXPTN) {delete dlg;return;}
   if ( dlg->ShowModal() == wxID_OK ) {
      wxString ost;
      ost << wxT("removecell(\"") << dlg->get_selectedcell() << wxT("\");");
      _cmdline->parseCommand(ost);
   }
   delete dlg;
}

void tui::TopedFrame::OnGDStranslate(wxCommandEvent& WXUNUSED(event)) {
   wxRect wnd = GetRect();
   wxPoint pos(wnd.x+wnd.width/2-100,wnd.y+wnd.height/2-50);
   tui::getGDSimport* dlg = NULL;
   try {
      dlg = DEBUG_NEW tui::getGDSimport(this, -1, wxT("Import GDS structure"), pos,
                                          _browsers->TDTSelectedGDSName());
   }
   catch (EXPTN) {delete dlg;return;}
   if ( dlg->ShowModal() == wxID_OK ) {
      wxString ost;
      ost << wxT("gdsimport(\"") << dlg->get_selectedcell() << wxT("\" , ")
          << (dlg->get_recursive() ? wxT("true") : wxT("false"))   << wxT(" , ")
          << (dlg->get_overwrite() ? wxT("true") : wxT("false"))   <<wxT(");");
      _cmdline->parseCommand(ost);
   }
   delete dlg;
}

void tui::TopedFrame::OnGDSimport(wxCommandEvent& WXUNUSED(event) evt)
{
   // Here - try a hollow lock/unlock the database just to check that it exists
   try {DATC->lockDB(false);}
   catch (EXPTN) {return;}
   DATC->unlockDB();
   wxFileDialog dlg2(this, wxT("Select a file"), wxT(""), wxT(""),
                     wxT("Stream files |*.sf;*.gds"),
                     tpdfOPEN);
   if (wxID_OK != dlg2.ShowModal()) 
   {
      SetStatusText(wxT("Parsing aborted")); return;
   }
   SetStatusText(wxT("Importing GDS file..."));
   wxString filename = dlg2.GetFilename();
   wxString ost_int;
   ost_int << wxT("gdsread(\"") << dlg2.GetDirectory() << wxT("/") <<dlg2.GetFilename() << wxT("\")");
   wxString ost;
   ost << wxT("gdsimport(") << ost_int << wxT(", true, false );gdsclose();");
   _cmdline->parseCommand(ost);
//   SetStatusText(wxT("Stream ")+dlg2.GetFilename()+wxT(" imported"));
}

void tui::TopedFrame::OnGDSexportLIB(wxCommandEvent& WXUNUSED(event)) {
   SetStatusText(wxT("Exporting database to GDS file..."));
   wxFileDialog dlg2(this, wxT("Export design to GDS file"), wxT(""), wxT(""),
      wxT("GDS files |*.sf;*.gds"),
      tpdfSAVE);
   if (wxID_OK == dlg2.ShowModal()) {
      wxString filename = dlg2.GetPath();
      if(!checkFileOverwriting(filename))
      {
         SetStatusText(wxT("GDS export aborted"));
         return;
      }
      wxString ost;
      ost << wxT("gdsexport(\"") << dlg2.GetDirectory() << wxT("/") <<dlg2.GetFilename() << wxT("\");");
      _cmdline->parseCommand(ost);
//      SetStatusText(wxT("Design exported to: ")+dlg2.GetFilename());
   }
   else SetStatusText(wxT("GDS export aborted"));
}

void tui::TopedFrame::OnGDSexportCELL(wxCommandEvent& WXUNUSED(event)) {
   SetStatusText(wxT("Exporting a cell to GDS file..."));
   wxRect wnd = GetRect();
   wxPoint pos(wnd.x+wnd.width/2-100,wnd.y+wnd.height/2-50);
   tui::getGDSexport* dlg = NULL;
   try {
      dlg = DEBUG_NEW tui::getGDSexport(this, -1, wxT("GDS export cell"), pos, _browsers->TDTSelectedCellName());
   }
   catch (EXPTN) {delete dlg;return;}
   wxString cellname;
   bool recur;
   if ( dlg->ShowModal() == wxID_OK ) {
      cellname = dlg->get_selectedcell();
      recur = dlg->get_recursive();
      delete dlg;
   }
   else {delete dlg;return;}
   wxString oststr;
   oststr <<wxT("Exporting ") << cellname << wxT(" to GDS file");
   wxString fullCellName;
   fullCellName << cellname << wxT(".gds");
   wxFileDialog dlg2(this, oststr , wxT(""), fullCellName,
      wxT("GDS files |*.sf;*.gds"),
      tpdfSAVE);
   if (wxID_OK == dlg2.ShowModal()) {
      wxString filename = dlg2.GetPath();
      if(!checkFileOverwriting(filename))
      {
         SetStatusText(wxT("GDS export aborted"));
         return;
      }
      wxString ost;
      ost << wxT("gdsexport(\"") << cellname.c_str() << wxT("\" , ") <<
                        (recur ? wxT("true") : wxT("false")) << wxT(",\"") <<
                        (dlg2.GetDirectory()).c_str() << wxT("/") <<(dlg2.GetFilename()).c_str() << wxT("\");");
      _cmdline->parseCommand(ost);
//      SetStatusText(wxT("Design exported to: ")+dlg2.GetFilename());
   }
   else SetStatusText(wxT("GDS export aborted"));
}

void tui::TopedFrame::OnCellRef_B(wxCommandEvent& WXUNUSED(event)) {
   CellRef(_browsers->TDTSelectedCellName());
}

void tui::TopedFrame::OnCellRef_M(wxCommandEvent& WXUNUSED(event)) {
   CellRef(wxT(""));
}

void tui::TopedFrame::OnCellARef_B(wxCommandEvent& WXUNUSED(event)) {
   CellARef(_browsers->TDTSelectedCellName());
}

void tui::TopedFrame::OnCellARef_M(wxCommandEvent& WXUNUSED(event)) {
   CellARef(wxT(""));
}

void tui::TopedFrame::CellRef(wxString clname) {
   wxRect wnd = GetRect();
   wxPoint pos(wnd.x+wnd.width/2-100,wnd.y+wnd.height/2-50);
   tui::getCellRef* dlg = NULL;
   try {
      dlg = DEBUG_NEW tui::getCellRef(this, -1, wxT("Cell Reference"), pos, clname);
   }
   catch (EXPTN) {delete dlg;return;}   
   if ( dlg->ShowModal() == wxID_OK ) {
      wxString ost;
/*      ost << wxT("cellref(\"") << dlg->get_selectedcell() <<wxT("\",getpoint(),") 
          <<                     dlg->get_angle() << wxT(",") 
          << (dlg->get_flip() ? wxT("true") : wxT("false")) << wxT(",")
          <<                                wxT("1")  << wxT(");");*/
      ost << wxT("cellref(\"") << dlg->get_selectedcell() << wxT("\");");
      _cmdline->parseCommand(ost);
   }
   delete dlg;
}

void tui::TopedFrame::CellARef(wxString clname) {
   wxRect wnd = GetRect();
   wxPoint pos(wnd.x+wnd.width/2-100,wnd.y+wnd.height/2-50);
   tui::getCellARef* dlg = NULL;
   try {
      dlg = DEBUG_NEW tui::getCellARef(this, -1, wxT("Array of References"), pos, clname);
   }
   catch (EXPTN) {delete dlg; return;}   
   if ( dlg->ShowModal() == wxID_OK ) {
      wxString ost;
/*      ost << wxT("cellaref(\"") << dlg->get_selectedcell() <<wxT("\",getpoint(),") 
          <<                     dlg->get_angle() << wxT(",") 
          << (dlg->get_flip() ? wxT("true") : wxT("false")) << wxT(",")
          <<                                 wxT("1")  << wxT(",")
          <<                       dlg->get_col() << wxT(",") 
          <<                       dlg->get_row() << wxT(",") 
          <<                     dlg->get_stepX() << wxT(",") 
          <<                     dlg->get_stepY() << wxT(");");*/
      ost << wxT("cellaref(\"") << dlg->get_selectedcell() << wxT("\",")
          <<                       dlg->get_col() << wxT(",")
          <<                       dlg->get_row() << wxT(",") 
          <<                     dlg->get_stepX() << wxT(",") 
          <<                     dlg->get_stepY() << wxT(");");
      _cmdline->parseCommand(ost);
   }
   delete dlg;
}

void tui::TopedFrame::OnCellGroup(wxCommandEvent& WXUNUSED(event)) {
   // Here - try a hollow lock/unlock the database just to check that it exists
   try {DATC->lockDB();}
   catch (EXPTN) {return;}   
   DATC->unlockDB();
   //
   wxTextEntryDialog dlg2(this,
      wxT("Cell name:"),
      wxT("Create new cell from selected components"));
   wxString cname, ost;
   if ((wxID_OK == dlg2.ShowModal()) && ((cname = dlg2.GetValue()) != wxT(""))) {
      SetStatusText(wxT("Grouping in a new cell..."));
      ost << wxT("group(\"") << cname << wxT("\");");
      _cmdline->parseCommand(ost);
   }
   else SetStatusText(wxT("Groupping canceled"));
}

void tui::TopedFrame::OnDrawWire(wxCommandEvent& WXUNUSED(event)) {
   wxRect wnd = GetRect();
   wxPoint pos(wnd.x+wnd.width/2-100,wnd.y+wnd.height/2-50);
   tui::getSize* dlg = NULL;
   try {
      dlg = DEBUG_NEW tui::getSize(this, -1, wxT("Wire width"), pos, DATC->step() ,3);
   }
   catch (EXPTN) {delete dlg;return;}
   if ( dlg->ShowModal() == wxID_OK ) {
      wxString ost; ost << wxT("addwire(")<<dlg->value()<<wxT(");");
      _cmdline->parseCommand(ost);
   }
   delete dlg;
}

void tui::TopedFrame::OnDrawText(wxCommandEvent& WXUNUSED(event)) {
   wxRect wnd = GetRect();
   wxPoint pos(wnd.x+wnd.width/2-100,wnd.y+wnd.height/2-50);   
   tui::getTextdlg* dlg = NULL;
   try {
      dlg = DEBUG_NEW tui::getTextdlg(this, -1, wxT("Add text"), pos);
   }
   catch (EXPTN) {delete dlg;return;}
   if ( dlg->ShowModal() == wxID_OK ) {
/*      wxString ost; ost << wxT("addtext(\"")
                        << dlg->get_text()                      << wxT("\",")
                        << DATC->curlay()                       << wxT(",")
                        << wxT("getpoint(),")
                        << dlg->get_angle()                     << wxT(",")
                        << (dlg->get_flip() ? wxT("true") : wxT("false")) << wxT(",")
                        << dlg->get_size()                      << wxT(");");*/
      wxString ost; ost << wxT("addtext(\"")
                        << dlg->get_text()                      << wxT("\",")
                        << dlg->get_size()                      << wxT(");");
      _cmdline->parseCommand(ost);
   }
   delete dlg;
}

void tui::TopedFrame::OnStep(wxCommandEvent& WXUNUSED(event)) {
   wxRect wnd = GetRect();
   wxPoint pos(wnd.x+wnd.width/2-100,wnd.y+wnd.height/2-50);
   tui::getStep dlg(this, -1, wxT("Step size"), pos, DATC->step());
   if ( dlg.ShowModal() == wxID_OK ) {
      wxString ost; ost << wxT("step(")<<dlg.value()<<wxT(");");
      _cmdline->parseCommand(ost);
   }
}

void tui::TopedFrame::OnGridDefine(wxCommandEvent& WXUNUSED(event)) {
   real grid[3];
   const layprop::LayoutGrid *gr  = NULL;
   for (byte i = 0; i < 3; i++)
      if (NULL != (gr = DATC->grid(i))) grid[i] = gr->step();
      else                                    grid[i] = 0.0;
   wxRect wnd = GetRect();
   wxPoint pos(wnd.x+wnd.width/2-100,wnd.y+wnd.height/2-50);
   tui::getGrid dlg(this, -1, wxT("Grid size"), pos, grid[0], grid[1], grid[2]);
   if ( dlg.ShowModal() == wxID_OK ) {
      wxString ost; 
      ost << wxT("definegrid(0,")<<dlg.grid0()<<wxT(",\"white\");");
      ost << wxT("definegrid(1,")<<dlg.grid1()<<wxT(",\")white\");");
      ost << wxT("definegrid(2,")<<dlg.grid2()<<wxT(",\")white\");");
      _cmdline->parseCommand(ost);
   }
}

void tui::TopedFrame::OnGrid0(wxCommandEvent& WXUNUSED(event)) {
   wxString ost;
   ost << wxT("grid(0,") << (settingsMenu->IsChecked(TMSET_GRID0) ? wxT("true") : wxT("false")) << wxT(");");
   _cmdline->parseCommand(ost);   
}

void tui::TopedFrame::OnGrid1(wxCommandEvent& WXUNUSED(event)){
   wxString ost;
   ost << wxT("grid(1,") << (settingsMenu->IsChecked(TMSET_GRID1) ? wxT("true") : wxT("false")) << wxT(");");
   _cmdline->parseCommand(ost);   
}

void tui::TopedFrame::OnGrid2(wxCommandEvent& WXUNUSED(event)){
   wxString ost;
   ost << wxT("grid(2,") << (settingsMenu->IsChecked(TMSET_GRID2) ? wxT("true") : wxT("false")) << wxT(");");
   _cmdline->parseCommand(ost);
}

void tui::TopedFrame::OnCellMark(wxCommandEvent& WXUNUSED(event)){
  wxString ost;
  ost << wxT("hidecellmarks(") << (settingsMenu->IsChecked(TMSET_CELLMARK) ? wxT("false") : wxT("true")) <<
  wxT(");");
  _cmdline->parseCommand(ost);
}

void tui::TopedFrame::OnLongCursor(wxCommandEvent& WXUNUSED(event)){
  wxString ost;
  ost << wxT("longcursor(") << (settingsMenu->IsChecked(TMSET_CURLONG) ? wxT("true") : wxT("false")) <<
  wxT(");");
  _cmdline->parseCommand(ost);
}

void tui::TopedFrame::OnCellBox(wxCommandEvent& WXUNUSED(event)){
  wxString ost;
  ost << wxT("hidecellbox(") << (settingsMenu->IsChecked(TMSET_CELLBOX) ? wxT("false") : wxT("true")) <<
  wxT(");");
  _cmdline->parseCommand(ost);
}

void tui::TopedFrame::OnTextMark(wxCommandEvent& WXUNUSED(event)){
  wxString ost;
  ost << wxT("hidetextmarks(") << (settingsMenu->IsChecked(TMSET_TEXTMARK) ? wxT("false") : wxT("true")) <<
  wxT(");");
  _cmdline->parseCommand(ost);
}

void tui::TopedFrame::OnTextBox(wxCommandEvent& WXUNUSED(event)){
  wxString ost;
  ost << wxT("hidetextbox(") << (settingsMenu->IsChecked(TMSET_TEXTBOX) ? wxT("false") : wxT("true")) <<
  wxT(");");
  _cmdline->parseCommand(ost);
}

void tui::TopedFrame::OnAutopan(wxCommandEvent& WXUNUSED(event)){
   wxString ost;
   ost << wxT("autopan(")<< (settingsMenu->IsChecked(TMSET_AUTOPAN) ? wxT("true") : wxT("false")) << wxT(");");
   _cmdline->parseCommand(ost);   
}

void tui::TopedFrame::OnMarker0(wxCommandEvent& WXUNUSED(event)) {
   wxString ost;
   ost << wxT("shapeangle(0);");
   _cmdline->parseCommand(ost);   
}

void tui::TopedFrame::OnMarker45(wxCommandEvent& WXUNUSED(event)) {
   wxString ost;
   ost << wxT("shapeangle(45);");
   _cmdline->parseCommand(ost);   
}

void tui::TopedFrame::OnMarker90(wxCommandEvent& WXUNUSED(event)) {
   wxString ost;
   ost << wxT("shapeangle(90);");
   _cmdline->parseCommand(ost);   
}

void tui::TopedFrame::OnDefineLayer(wxCommandEvent& event)
{
   //word layno = _browsers->TDTSelectedLayNo();
	word layno = DATC->curlay();
   wxRect wnd = GetRect();
   wxPoint pos(wnd.x+wnd.width/2-100,wnd.y+wnd.height/2-50);
   tui::defineLayer dlg(this, -1, wxT("Define Layer"), pos, layno);
   if ( dlg.ShowModal() == wxID_OK )
   {
      wxString ost;
      ost      << wxT("layprop(\"") << dlg.layname()
               << wxT("\" , ")      << dlg.layno()
               << wxT(" , \"")      << dlg.color()
               << wxT("\" , \"")    << dlg.fill()
               << wxT("\" , \"")    << dlg.line()
               << wxT("\");");
      _cmdline->parseCommand(ost);
   }
}

void tui::TopedFrame::OnDefineColor(wxCommandEvent& WXUNUSED(event))
{
   wxRect wnd = GetRect();
   wxPoint pos(wnd.x+wnd.width/2-100,wnd.y+wnd.height/2-50);
   tui::defineColor dlg(this, -1, wxT("Color Definitions"), pos);
   if ( dlg.ShowModal() == wxID_OK )
   {
      const layprop::colorMAP colors = dlg.allColors();
      for(layprop::colorMAP::const_iterator CC = colors.begin() ; CC != colors.end(); CC++)
      {
         wxString ost;
         layprop::tellRGB* coldef = CC->second;
         ost   << wxT("definecolor(\"") << wxString(CC->first.c_str(), wxConvUTF8)
               << wxT("\" , ")      << coldef->red()
               << wxT(" , ")        << coldef->green()
               << wxT(" , ")        << coldef->blue()
               << wxT(" , ")        << coldef->alpha()
               << wxT(");");
         _cmdline->parseCommand(ost);
      }
   }
}

void tui::TopedFrame::OnDefineFill(wxCommandEvent& WXUNUSED(event))
{
   wxRect wnd = GetRect();
   wxPoint pos(wnd.x+wnd.width/2-100,wnd.y+wnd.height/2-50);
   tui::defineFill dlg(this, -1, wxT("Fill Definition"), pos);
   if ( dlg.ShowModal() == wxID_OK )
   {
      layprop::fillMAP patterns = dlg.allPatterns();
      for(layprop::fillMAP::const_iterator CC = patterns.begin() ; CC != patterns.end(); CC++)
      {
         wxString ost;
         byte* patdef = CC->second;
         ost   << wxT("definefill(\"") << wxString(CC->first.c_str(), wxConvUTF8)
               << wxT("\" , {");
         ost << patdef[0];
         for (byte i = 1; i < 128; i++)
            ost  << wxT(",") << patdef[i];
         ost   << wxT("});");
         _cmdline->parseCommand(ost);
      }
   }
}

void tui::TopedFrame::OnChangeRef( wxCommandEvent& WXUNUSED( event ))
{
   wxRect wnd = GetRect();
   wxPoint pos(wnd.x+wnd.width/2-100,wnd.y+wnd.height/2-50);
   tui::getCellRef* dlg = NULL;
   try {
      dlg = DEBUG_NEW tui::getCellRef(this, -1, wxT("Change Cell Reference"), pos, wxT(""));
   }
   catch (EXPTN) {delete dlg;return;}   
   if ( dlg->ShowModal() == wxID_OK ) 
   {
      wxString ost;
      ost << wxT("changeref(\"") << dlg->get_selectedcell() << wxT("\");");
      _cmdline->parseCommand(ost);
   }
   delete dlg;
}

void tui::TopedFrame::OnChangeText( wxCommandEvent& WXUNUSED( event ))
{
   wxTextEntryDialog dlg(this, wxT("New string:"), wxT("Change text string"));
   wxString cname, ost;
   if ((wxID_OK == dlg.ShowModal()) && ((cname = dlg.GetValue()) != wxT(""))) 
   {
      SetStatusText(wxT("Change text string ..."));
      wxString ost;
      ost << wxT("changestr(\"") << dlg.GetValue() << wxT("\");");
      _cmdline->parseCommand(ost);
   }
}

void tui::TopedFrame::OnChangeLayer( wxCommandEvent& WXUNUSED( event ))
{
   wxRect wnd = GetRect();
   wxPoint pos(wnd.x+wnd.width/2-100,wnd.y+wnd.height/2-50);
   tui::getSize* dlg = NULL;
   try {
      dlg = DEBUG_NEW tui::getSize(this, -1, wxT("Transfer to layer"), pos, 1, 0);
   }
   catch (EXPTN) {delete dlg;return;}
   if ( dlg->ShowModal() == wxID_OK )
   {
      wxString ost; ost << wxT("changelayer(")<<dlg->value()<<wxT(");");
      _cmdline->parseCommand(ost);
   }
   delete dlg;
}

void tui::TopedFrame::OnCurrentLayer( wxCommandEvent& WXUNUSED( event ))
{
   wxRect wnd = GetRect();
   wxPoint pos(wnd.x+wnd.width/2-100,wnd.y+wnd.height/2-50);
   tui::getSize* dlg = NULL;
   try {
      dlg = DEBUG_NEW tui::getSize(this, -1, wxT("Change current layer"), pos, 1, 0);
   }
   catch (EXPTN) {delete dlg;return;}
   if ( dlg->ShowModal() == wxID_OK )
   {
      unsigned long vlu;
      dlg->value().ToULong(&vlu);
      DATC->setcmdlayer((word)vlu);
   }
   delete dlg;
}

void tui::TopedFrame::OnMenu(wxCommandEvent& event)
{
   _resourceCenter->executeMenu(event.GetId());
}

void tui::TopedFrame::OnAbort(wxCommandEvent& WXUNUSED(event)) {
   wxCommandEvent eventButtonUP(wxEVT_COMMAND_ENTER);
   eventButtonUP.SetClientData((void*)NULL);
   eventButtonUP.SetInt(-1);
   wxPostEvent(Console, eventButtonUP);
}

void tui::TopedFrame::OnUpdateSettingsMenu(wxCommandEvent& evt)
{
   switch (evt.GetInt()) {
      case STS_GRID0_ON    : settingsMenu->Check(TMSET_GRID0,true );break;
      case STS_GRID0_OFF   : settingsMenu->Check(TMSET_GRID0,false);break;
      case STS_GRID1_ON    : settingsMenu->Check(TMSET_GRID1,true );break;
      case STS_GRID1_OFF   : settingsMenu->Check(TMSET_GRID1,false);break;
      case STS_GRID2_ON    : settingsMenu->Check(TMSET_GRID2,true );break;
      case STS_GRID2_OFF   : settingsMenu->Check(TMSET_GRID2,false);break;
      case STS_CELLMARK_OFF: settingsMenu->Check(TMSET_CELLMARK,false);break;
      case STS_CELLMARK_ON : settingsMenu->Check(TMSET_CELLMARK,true);break;
      case STS_CELLBOX_OFF : settingsMenu->Check(TMSET_CELLBOX,false);break;
      case STS_CELLBOX_ON  : settingsMenu->Check(TMSET_CELLBOX,true);break;
      case STS_TEXTMARK_OFF: settingsMenu->Check(TMSET_TEXTMARK,false);break;
      case STS_TEXTMARK_ON : settingsMenu->Check(TMSET_TEXTMARK,true);break;
      case STS_TEXTBOX_OFF : settingsMenu->Check(TMSET_TEXTBOX,false);break;
      case STS_TEXTBOX_ON  : settingsMenu->Check(TMSET_TEXTBOX,true);break;
      case STS_AUTOPAN_ON  : settingsMenu->Check(TMSET_AUTOPAN,true );break;
      case STS_AUTOPAN_OFF : settingsMenu->Check(TMSET_AUTOPAN,false);break;
      case STS_ANGLE_0     : settingsMenu->Check(TMSET_MARKER0  ,true );break;
      case STS_ANGLE_45    : settingsMenu->Check(TMSET_MARKER45 ,true );break;
      case STS_ANGLE_90    : settingsMenu->Check(TMSET_MARKER90 ,true );break;
      case STS_LONG_CURSOR : settingsMenu->Check(TMSET_CURLONG ,true );break;
      case STS_SHORT_CURSOR: settingsMenu->Check(TMSET_CURLONG ,false );break;
                    default: assert(false);
   }
};

void  tui::TopedFrame::OnMouseAccel(wxCommandEvent& evt) {
   wxString ost;
   if (0 == evt.GetInt())
      ost << wxT("unselect(");
   else if (1 == evt.GetInt())
      ost << wxT("select(");
   else return;
   ost << evt.GetString() << wxT(");");   
   _cmdline->parseCommand(ost);   
}

void tui::TopedFrame::OnzoomIn(wxCommandEvent& WXUNUSED(event)) {
   wxCommandEvent eventZOOM(wxEVT_CANVAS_ZOOM);
   eventZOOM.SetInt(ZOOM_IN);
	wxPostEvent(mS_canvas->canvas(), eventZOOM);
}

void tui::TopedFrame::OnzoomOut(wxCommandEvent& WXUNUSED(event)) {
   wxCommandEvent eventZOOM(wxEVT_CANVAS_ZOOM);
   eventZOOM.SetInt(ZOOM_OUT);
   wxPostEvent(mS_canvas->canvas(), eventZOOM);
}

void tui::TopedFrame::OnzoomEmpty(wxCommandEvent& WXUNUSED(event)) {
   wxCommandEvent eventZOOM(wxEVT_CANVAS_ZOOM);
   eventZOOM.SetInt(ZOOM_EMPTY);
   wxPostEvent(mS_canvas->canvas(), eventZOOM);
}

void tui::TopedFrame::OnpanLeft(wxCommandEvent& WXUNUSED(event)) {
   wxCommandEvent eventZOOM(wxEVT_CANVAS_ZOOM);
   eventZOOM.SetInt(ZOOM_LEFT);
   wxPostEvent(mS_canvas->canvas(), eventZOOM);
}

void tui::TopedFrame::OnpanRight(wxCommandEvent& WXUNUSED(event)) {
   wxCommandEvent eventZOOM(wxEVT_CANVAS_ZOOM);
   eventZOOM.SetInt(ZOOM_RIGHT);
   wxPostEvent(mS_canvas->canvas(), eventZOOM);
}

void tui::TopedFrame::OnpanUp(wxCommandEvent& WXUNUSED(event)) {
   wxCommandEvent eventZOOM(wxEVT_CANVAS_ZOOM);
   eventZOOM.SetInt(ZOOM_UP);
   wxPostEvent(mS_canvas->canvas(), eventZOOM);
}

void tui::TopedFrame::OnpanDown(wxCommandEvent& WXUNUSED(event)) {
   wxCommandEvent eventZOOM(wxEVT_CANVAS_ZOOM);
   eventZOOM.SetInt(ZOOM_DOWN);
   wxPostEvent(mS_canvas->canvas(), eventZOOM);
}

bool tui::TopedFrame::checkFileOverwriting(const wxString& fileName)
{
   bool ret=true;
   wxFileName checkedFileName(fileName);
   assert(checkedFileName.IsOk());
   if (checkedFileName.FileExists())
   {
      wxMessageDialog dlg1(this,
      wxT("File ") + fileName + wxT(" already exists. Overwrite ?"),
      wxT("Toped"),
      wxYES_NO | wxICON_QUESTION);
      if (wxID_NO==dlg1.ShowModal()) ret = false; else ret = true;
   }
   return ret;
}

void tui::TopedFrame::OnUncapturedMouseClick(wxCommandEvent& evt)
{
   telldata::ttpnt* p = static_cast<telldata::ttpnt*>(evt.GetClientData());
   delete p;
}
