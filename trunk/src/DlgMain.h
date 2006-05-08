/*
 * This file is part of QShapes project
 * Copyright (C) 2006 QShapes development team
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#ifndef DLGMAIN_
#define DLGMAIN_

#include <QMainWindow>

class QAction;
class QActionGroup;
class ClipboardManager;
class DiagramView;
class QComboBox;
class QLabel;
class QMenu;
class QPrinter;
class QWorkspace;
class QSignalMapper;
class QString;
class QToolBar;

/*! \class DlgMain
    \brief The main app window
    \author Flavio Castelli

    DlgMain is the MDI window showing all DiagramView's instances. \sa DiagramView
 */

class DlgMain : public QMainWindow
{
    Q_OBJECT
    
    public:
        DlgMain (QWidget *parent = 0);
        virtual ~DlgMain();
        
        void init (char* filename);
        
    protected:
        void closeEvent(QCloseEvent *event);

    private slots:
        void fileNew();
        void fileOpen(QString filename = QString());
        void openRecentFile();
        void fileSave();
        void fileSaveAs();
        void fileExport();
        void fileClose();
        void fileExit();
        void filePrint();
        void editUndo();
        void undoAvailable (bool status);
        void editRedo();
        void redoAvailable (bool status);
        void editCut();
        void editCopy();
        void selectionAvailable (bool status);
        void editPaste();
        void pasteAvailable (bool status);
        void editEnableAntiAliasing(bool status);
        void editZoomPlus();
        void editZoomMinus();
        void editZoomLevelUpdated (int zoolLevel);
        void editZoomLevelChanged (QString zoolLevel);
        void browseCursor();
        void browsePan();
        void drawFigureAdded();
        void drawLine();
        void drawProductBox();
        void drawSystem();
        void drawSuperSystem();
        void updateMenus();
        void updateWindowMenu();
        void associateFile (bool status);
        void about();
        
    private:
        void createActions();
        void createToolBars();
        void createMenus();
        void createStatusBar();
        void updateRecentFileActions();
        
        bool okToClear (DiagramView* view = NULL);
        bool okToClearAll ();
        
        enum { MaxRecentFiles = 5 };
        QAction *recentFileActs[MaxRecentFiles];
        
        DiagramView* activeDiagramView();
        DiagramView* createDiagramView(QString fileName);

        QPrinter* m_pPrinter;
        
        QMenu *fileMenu;
        QMenu *editMenu;
        QMenu *windowMenu;
        QMenu *helpMenu;
        
        QWorkspace *workspace;
        QSignalMapper *windowMapper;
        
        QToolBar *fileToolBar;
        QToolBar *editToolBar;
        QToolBar *drawToolBar;
        QToolBar *browseToolBar;
        
        QActionGroup *drawGroup;
        QActionGroup *browseGroup;
        
        QAction *aboutAct;
        QAction *aboutQtAct;
        QAction *fileNewAct;
        QAction *fileOpenAct;
        QAction *fileSaveAct;
        QAction *fileSaveAsAct;
        QAction *fileExportAct;
        QAction *filePrintAct;
        QAction *fileCloseAct;
        QAction *fileCloseAllAct;
        QAction *fileExitAct;
        QAction *editUndoAct;
        QAction *editRedoAct;
        QAction *editCutAct;
        QAction *editCopyAct;
        QAction *editPasteAct;
        QAction *editEnableAntiAliasingAct;
        QAction *editZoomPlusAct;
        QAction *editZoomMinusAct;
        QAction *browseCursorAct;
        QAction *browsePanAct;
        QAction *drawProductBoxAct;
        QAction *drawSystemAct;
        QAction *drawSuperSystemAct;
        QAction *drawLineAct;
        QAction *separatorAct;
        QAction *windowCascadeAct;
        QAction *windowTileAct;
        QAction *windowArrangeAct;
        QAction *windowNextAct;
        QAction *windowPreviousAct;
        QAction *windowSeparatorAct;
        
        QLabel* zoomLabel;
        
        QComboBox* m_pComboBox;
        
        ClipboardManager *m_pClipboardManager;
}; 

#endif /*DLGMAIN_*/
