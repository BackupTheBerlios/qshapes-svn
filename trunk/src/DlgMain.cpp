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

#include "ClipboardManager.h"
#include "DiagramView.h"
#include "DlgAbout.h"
#include "DlgMain.h"

#include <QAction>
#include <QActionGroup>
#include <QApplication>
#include <QCloseEvent>
#include <QComboBox>
#include <QCoreApplication>
#include <QFileDialog>
#include <QLabel>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QPainter>
#include <QPrinter>
#include <QPrintDialog>
#include <QWorkspace>
#include <QSettings>
#include <QSignalMapper>
#include <QStatusBar>
#include <QToolBar>

#include <qtcanvas.h>

#define VERSION_MAJ 1
#define VERSION_MIN 0

DlgMain::DlgMain(QWidget *parent)
        : QMainWindow(parent)
{
    // setup settings
    QCoreApplication::setOrganizationName("QShapes Team");
    QCoreApplication::setOrganizationDomain("www.qshapes.berlios.de");
    QCoreApplication::setApplicationName("QShapes");
    
    workspace = new QWorkspace (this);
    workspace->setScrollBarsEnabled (true);
    
    setCentralWidget(workspace);
    connect(workspace, SIGNAL(windowActivated(QWidget *)), this, SLOT(updateMenus()));
    
    windowMapper = new QSignalMapper(this);
    connect(windowMapper, SIGNAL(mapped(QWidget *)), workspace, SLOT(setActiveWindow(QWidget *)));

    // setup combo box
    m_pComboBox = new QComboBox;
    
    QStringList entries;
    
    for (int i = 25; i <= 300; i+=25)
    {
        QString temp;
        temp.sprintf ("%i",i);
        temp += "%";
        entries += temp;
    }

    m_pComboBox->insertItems (0, entries);
    m_pComboBox->setCurrentIndex(3); // start with 100% selected
    
    connect(m_pComboBox, SIGNAL(activated(QString)), this, SLOT(editZoomLevelChanged (QString)));
    
    m_pClipboardManager = new ClipboardManager ();
    connect(m_pClipboardManager, SIGNAL(pasteAvailable(bool)), this, SLOT(pasteAvailable (bool)));
            
    createActions();
    createMenus();
    createToolBars();
    createStatusBar();
    updateMenus();
    
    m_pPrinter = new QPrinter();
    
    setWindowTitle(tr("QShapes"));
    
//     vlayout = new QVBoxLayout;
//     vlayout->addWidget(this);
//     setLayout (vlayout);
}

void DlgMain::updateMenus()
{
    DiagramView* view = activeDiagramView();
    bool hasMdiChild = false;
    
    if (view != 0)
        hasMdiChild = true;
    
    fileSaveAct->setEnabled(hasMdiChild);
    fileSaveAsAct->setEnabled(hasMdiChild);
    fileExportAct->setEnabled(hasMdiChild);
    fileCloseAct->setEnabled(hasMdiChild);
    fileCloseAllAct->setEnabled(hasMdiChild);
    filePrintAct->setEnabled(hasMdiChild);
    
    if (m_pClipboardManager != NULL)
        editPasteAct->setEnabled (!m_pClipboardManager->empty());
    
    if (hasMdiChild)
    {
        editCutAct->setEnabled(view->isSelectionAvailable());
        editCopyAct->setEnabled(view->isSelectionAvailable());
        editUndoAct->setEnabled(view->isUndoAvailable());
        editRedoAct->setEnabled(view->isRedoAvailable());
        
        editEnableAntiAliasingAct->setEnabled(true);
        editEnableAntiAliasingAct->setChecked(view->highQualityRendering());
    }
    else
    {
        editCutAct->setEnabled(false);
        editCopyAct->setEnabled(false);
        editUndoAct->setEnabled(false);
        editRedoAct->setEnabled(false);
        editEnableAntiAliasingAct->setEnabled(false);
        editEnableAntiAliasingAct->setChecked(false);
        
        // it there's no window we've to disable paste action
        editPasteAct->setEnabled (false);
    }
    
    editZoomPlusAct->setEnabled (hasMdiChild);
    editZoomMinusAct->setEnabled (hasMdiChild);
    zoomLabel->setEnabled (hasMdiChild);
    m_pComboBox->setEnabled (hasMdiChild);
    
    browseCursorAct->setEnabled (hasMdiChild);
    browsePanAct->setEnabled (hasMdiChild);
    
    drawLineAct->setEnabled (hasMdiChild);
    drawProductBoxAct->setEnabled (hasMdiChild);
    drawSystemAct->setEnabled (hasMdiChild);
    drawSuperSystemAct->setEnabled (hasMdiChild);
    
    windowCascadeAct->setEnabled(hasMdiChild);
    windowTileAct->setEnabled(hasMdiChild);
    windowArrangeAct->setEnabled(hasMdiChild);
    windowNextAct->setEnabled(hasMdiChild);
    windowPreviousAct->setEnabled(hasMdiChild);
    windowSeparatorAct->setVisible(hasMdiChild);
}

void DlgMain::updateWindowMenu()
{
    windowMenu->clear();
    windowMenu->addAction(fileCloseAct);
    windowMenu->addAction(fileCloseAllAct);
    windowMenu->addSeparator();
    windowMenu->addAction(windowCascadeAct);
    windowMenu->addAction(windowTileAct);
    windowMenu->addAction(windowArrangeAct);
    windowMenu->addSeparator();
    windowMenu->addAction(windowNextAct);
    windowMenu->addAction(windowPreviousAct);
    windowMenu->addAction(windowSeparatorAct);

    QList<QWidget *> windows = workspace->windowList();
    windowSeparatorAct->setVisible(!windows.isEmpty());

    for (int i = 0; i < windows.size(); ++i)
    {
        DiagramView *child = qobject_cast<DiagramView*>(windows.at(i));

        QString text;
        if (i < 9)
            text = tr("&%1 %2").arg(i + 1).arg(child->userFriendlyCurrentFile());
        else
            text = tr("%1 %2").arg(i + 1).arg(child->userFriendlyCurrentFile());
        
        QAction *action  = windowMenu->addAction(text);
        action->setCheckable(true);
        action ->setChecked(child == activeDiagramView());
        connect(action, SIGNAL(triggered()), windowMapper, SLOT(map()));
        windowMapper->setMapping(action, child);
    }
}

DlgMain::~DlgMain()
{
    if (m_pClipboardManager != NULL)
        delete m_pClipboardManager;
    
    if (m_pComboBox != NULL)
        delete m_pComboBox;
    
    if (m_pPrinter != NULL)
        delete m_pPrinter;
}

void DlgMain::closeEvent(QCloseEvent *event)
{
    if (okToClearAll())
    {
        QList<QWidget*> windows = workspace->windowList();
    
        for (int i = 0; i < windows.size(); i++)
        {
            DiagramView* view = dynamic_cast<DiagramView*>(windows.at(i));
        
            if (view != 0)
                view->setModified (false);
        }
        
        workspace->closeAllWindows();
        
        if (activeDiagramView())
            event->ignore();
        else
        {
            //writeSettings();
            event->accept();
        }
    }
    else
        event->ignore();
}

void DlgMain::createActions()
{
    fileNewAct = new QAction(QIcon(":/images/filenew.png"),tr("&New"), this);
    fileNewAct->setShortcut(tr("Ctrl+N"));
    fileNewAct->setStatusTip(tr("Create a new file"));
    connect(fileNewAct, SIGNAL(triggered()), this, SLOT(fileNew()));
    
    fileOpenAct = new QAction(QIcon(":/images/fileopen.png"),tr("&Open..."), this);
    fileOpenAct->setShortcut(tr("Ctrl+O"));
    fileOpenAct->setStatusTip(tr("Open an existing file"));
    connect(fileOpenAct, SIGNAL(triggered()), this, SLOT(fileOpen()));
    
    for (int i = 0; i < MaxRecentFiles; ++i) {
        recentFileActs[i] = new QAction(this);
        recentFileActs[i]->setVisible(false);
        connect(recentFileActs[i], SIGNAL(triggered()),
                this, SLOT(openRecentFile()));
    }
    
    fileSaveAct = new QAction(QIcon(":/images/filesave.png"),tr("&Save"), this);
    fileSaveAct->setShortcut(tr("Ctrl+S"));
    fileSaveAct->setStatusTip(tr("Save the document to disk"));
    connect(fileSaveAct, SIGNAL(triggered()), this, SLOT(fileSave()));
    
    fileSaveAsAct = new QAction(QIcon(":/images/filesaveas.png"),tr("Save &As"), this);
    fileSaveAsAct->setStatusTip(tr("Save the document to disk with another name"));
    connect(fileSaveAsAct, SIGNAL(triggered()), this, SLOT(fileSaveAs()));
    
    fileExportAct = new QAction(tr("E&xport"), this);
    fileExportAct->setStatusTip(tr("Export to txt"));
    connect(fileExportAct, SIGNAL(triggered()), this, SLOT(fileExport()));
    
    fileCloseAct = new QAction(tr("&Close"), this);
    fileCloseAct->setShortcut(tr("Ctrl+W"));
    fileCloseAct->setStatusTip(tr("Close this document"));
    connect(fileCloseAct, SIGNAL(triggered()), this, SLOT(fileClose()));
    
    fileCloseAllAct = new QAction(tr("Close &All"), this);
    fileCloseAllAct->setStatusTip(tr("Close all the windows"));
    connect(fileCloseAllAct, SIGNAL(triggered()),
            workspace, SLOT(closeAllWindows()));

    
    fileExitAct = new QAction(tr("&Exit"), this);
    fileExitAct->setShortcut(tr("Ctrl+Q"));
    fileExitAct->setStatusTip(tr("Quit the program"));
    connect(fileExitAct, SIGNAL(triggered()), this, SLOT(fileExit()));
    
    filePrintAct = new QAction(QIcon(":/images/fileprint.png"),tr("&Print..."), this);
    filePrintAct->setShortcut(tr("Ctrl+P"));
    filePrintAct->setStatusTip(tr("Print the document"));
    connect(filePrintAct, SIGNAL(triggered()), this, SLOT(filePrint()));
    
    editUndoAct = new QAction(QIcon(":/images/undo.png"),tr("&Undo"), this);
    editUndoAct->setShortcut(tr("Ctrl+Z"));
    editUndoAct->setStatusTip(tr("Undo the last operation"));
    connect(editUndoAct, SIGNAL(triggered()), this, SLOT(editUndo()));
    
    editRedoAct = new QAction(QIcon(":/images/redo.png"),tr("&Redo"), this);
    editRedoAct->setShortcut(tr("Ctrl+Y"));
    editRedoAct->setStatusTip(tr("Redo the last operation"));
    connect(editRedoAct, SIGNAL(triggered()), this, SLOT(editRedo()));
    
    editCutAct = new QAction(QIcon(":/images/editcut.png"),tr("Cu&t"), this);
    editCutAct->setShortcut(tr("Ctrl+X"));
    editCutAct->setStatusTip(tr("Cut the current selection's contents to the "
            "clipboard"));
    connect(editCutAct, SIGNAL(triggered()), this, SLOT(editCut()));
    
    editCopyAct = new QAction(QIcon(":/images/editcopy.png"),tr("&Copy"), this);
    editCopyAct->setShortcut(tr("Ctrl+C"));
    editCopyAct->setStatusTip(tr("Copy the current selection's contents to the "
            "clipboard"));
    connect(editCopyAct, SIGNAL(triggered()), this, SLOT(editCopy()));
    
    editPasteAct = new QAction(QIcon(":/images/editpaste.png"),tr("&Paste"), this);
    editPasteAct->setShortcut(tr("Ctrl+V"));
    editPasteAct->setStatusTip(tr("Paste the clipboard's contents into the current "
            "selection"));
    connect(editPasteAct, SIGNAL(triggered()), this, SLOT(editPaste()));
    
    zoomLabel = new QLabel();
    zoomLabel->setPixmap(QPixmap(":/images/zoom.png"));
    
    editZoomPlusAct = new QAction(QIcon(":/images/zoom+.png"),tr("Increase zoom"), this);
    editZoomPlusAct->setStatusTip(tr("Increase zoom"));
    connect(editZoomPlusAct, SIGNAL(triggered()), this, SLOT(editZoomPlus()));
    
    editZoomMinusAct = new QAction(QIcon(":/images/zoom-.png"),tr("Decrease zoom"), this);
    editZoomMinusAct->setStatusTip(tr("Decrease zoom"));
    connect(editZoomMinusAct, SIGNAL(triggered()), this, SLOT(editZoomMinus()));
    
    editEnableAntiAliasingAct = new QAction(tr("&Enable Antialising"), this);
    editEnableAntiAliasingAct->setCheckable(true);
    editEnableAntiAliasingAct->setStatusTip(tr("Enable antialiasing"));
    connect(editEnableAntiAliasingAct, SIGNAL(toggled(bool)), this, SLOT(editEnableAntiAliasing(bool)));
    
    browseCursorAct = new QAction(QIcon(":/images/browse_cursor.xpm"),tr("Select tool"), this);
    browseCursorAct->setCheckable(true);
    browseCursorAct->setStatusTip(tr("Select tool"));
    connect(browseCursorAct, SIGNAL(toggled(bool)), this, SLOT(browseCursor()));
    
    browsePanAct = new QAction(QIcon(":/images/browse_pan.xpm"),tr("Pan document"), this);
    browsePanAct->setCheckable(true);
    browsePanAct->setStatusTip(tr("Pan document"));
    connect(browsePanAct, SIGNAL(toggled(bool)), this, SLOT(browsePan()));
    
    browseGroup = new QActionGroup (this);
    
    browseGroup->addAction (browseCursorAct);
    browseGroup->addAction (browsePanAct);
    
    drawLineAct = new QAction(QIcon(":/images/draw_connector.png"),tr("Insert a line"), this);
    drawLineAct->setCheckable(true);
    drawLineAct->setStatusTip(tr("Insert a line"));
    connect(drawLineAct, SIGNAL(toggled(bool)), this, SLOT(drawLine()));
    
    drawProductBoxAct = new QAction(QIcon(":/images/draw_productbox.xpm"),tr("Insert a product box"), this);
    drawProductBoxAct->setCheckable(true);
    drawProductBoxAct->setStatusTip(tr("Insert a product box"));
    connect(drawProductBoxAct, SIGNAL(toggled(bool)), this, SLOT(drawProductBox()));
    
    drawSystemAct = new QAction(QIcon(":/images/draw_system.xpm"),tr("Insert system box"), this);
    drawSystemAct->setCheckable(true);
    drawSystemAct->setStatusTip(tr("Insert a system box"));
    connect(drawSystemAct, SIGNAL(toggled(bool)), this, SLOT(drawSystem()));

    drawSuperSystemAct = new QAction(QIcon(":/images/draw_supersystem.xpm"),tr("Insert super system box"), this);
    drawSuperSystemAct->setCheckable(true);
    drawSuperSystemAct->setStatusTip(tr("Insert super system box"));
    connect(drawSuperSystemAct, SIGNAL(toggled(bool)), this, SLOT(drawSuperSystem()));
    
    drawGroup = new QActionGroup (this);
    
    drawGroup->addAction (drawLineAct);
    drawGroup->addAction (drawSystemAct);
    drawGroup->addAction (drawSuperSystemAct);
    drawGroup->addAction (drawProductBoxAct);
    
    windowCascadeAct = new QAction(tr("&Cascade"), this);
    windowCascadeAct->setStatusTip(tr("Cascade the windows"));
    connect(windowCascadeAct, SIGNAL(triggered()), workspace, SLOT(cascade()));
    
    windowTileAct = new QAction(tr("&Tile"), this);
    windowTileAct->setStatusTip(tr("Tile the windows"));
    connect(windowTileAct, SIGNAL(triggered()), workspace, SLOT(tile()));

    windowArrangeAct = new QAction(tr("Arrange &icons"), this);
    windowArrangeAct->setStatusTip(tr("Arrange the icons"));
    connect(windowArrangeAct, SIGNAL(triggered()), workspace, SLOT(arrangeIcons()));

    windowNextAct = new QAction(tr("Ne&xt"), this);
    windowNextAct->setShortcut(tr("Ctrl+F6"));
    windowNextAct->setStatusTip(tr("Move the focus to the next window"));
    connect(windowNextAct, SIGNAL(triggered()),
            workspace, SLOT(activateNextWindow()));

    windowPreviousAct = new QAction(tr("Pre&vious"), this);
    windowPreviousAct->setShortcut(tr("Ctrl+Shift+F6"));
    windowPreviousAct->setStatusTip(tr("Move the focus to the previous "
            "window"));
    connect(windowPreviousAct, SIGNAL(triggered()),
            workspace, SLOT(activatePreviousWindow()));

    windowSeparatorAct = new QAction(this);
    windowSeparatorAct->setSeparator(true);
    
    aboutAct = new QAction(tr("&About"), this);
    aboutAct->setStatusTip(tr("Show the application's About box"));
    connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));
    
    aboutQtAct = new QAction(tr("About &Qt"), this);
    aboutQtAct->setStatusTip(tr("Show the Qt library's About box"));
    connect(aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
}

void DlgMain::createMenus()
{
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(fileNewAct);
    fileMenu->addAction(fileOpenAct);
    fileMenu->addAction(fileSaveAct);
    fileMenu->addAction(fileSaveAsAct);
    fileMenu->addAction(fileExportAct);
    fileMenu->addAction(filePrintAct);
    
    separatorAct = fileMenu->addSeparator();
    for (int i = 0; i < MaxRecentFiles; ++i)
        fileMenu->addAction(recentFileActs[i]);
    
    updateRecentFileActions();
    
    fileMenu->addSeparator();
    fileMenu->addAction(fileCloseAct);
    fileMenu->addAction(fileExitAct);
    
    editMenu = menuBar()->addMenu(tr("&Edit"));
    editMenu->addAction(editUndoAct);
    editMenu->addAction(editRedoAct);
    editMenu->addSeparator();
    editMenu->addAction(editCutAct);
    editMenu->addAction(editCopyAct);
    editMenu->addAction(editPasteAct);
    editMenu->addSeparator();
//     editMenu->addAction(editInsertRectAct);
    editMenu->addAction(editEnableAntiAliasingAct);
    
    windowMenu = menuBar()->addMenu(tr("&Window"));
    connect(windowMenu, SIGNAL(aboutToShow()), this, SLOT(updateWindowMenu()));

    helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(aboutAct);
    helpMenu->addAction(aboutQtAct);
}

void DlgMain::createToolBars()
{
    fileToolBar = addToolBar(tr("File"));
    fileToolBar->addAction(fileNewAct);
    fileToolBar->addAction(fileOpenAct);
    fileToolBar->addAction(fileSaveAct);
    fileToolBar->addAction(fileSaveAsAct);
    fileToolBar->addAction(filePrintAct);
    
    editToolBar = addToolBar(tr("Edit"));
    editToolBar->addAction(editCutAct);
    editToolBar->addAction(editCopyAct);
    editToolBar->addAction(editPasteAct);
    editToolBar->addSeparator();
    editToolBar->addAction(editUndoAct);
    editToolBar->addAction(editRedoAct);
    editToolBar->addSeparator();
    editToolBar->addWidget(zoomLabel);
    editToolBar->addWidget(m_pComboBox);
    editToolBar->addAction(editZoomPlusAct);
    editToolBar->addAction(editZoomMinusAct);
    
    browseToolBar = addToolBar(tr("Browse"));
    browseToolBar->addAction (browseCursorAct);
    browseToolBar->addAction (browsePanAct);
    
    drawToolBar = addToolBar(tr("Draw"));
    drawToolBar->addAction(drawLineAct);
    drawToolBar->addAction(drawProductBoxAct);
    drawToolBar->addAction(drawSystemAct);
    drawToolBar->addAction(drawSuperSystemAct);
}

void DlgMain::createStatusBar()
{
    statusBar()->showMessage(tr("Ready"));
}

void DlgMain::openRecentFile()
{
    QAction *action = qobject_cast<QAction *>(sender());
    
    if (action)
    {
        QString filename = action->data().toString();
        fileOpen (filename);
    }
}

DiagramView* DlgMain::activeDiagramView()
{
    return qobject_cast<DiagramView*>(workspace->activeWindow());
}

DiagramView* DlgMain::createDiagramView(QString fileName)
{
    //TODO select size from dialog (A4, ...)
    QtCanvas* canvas = new QtCanvas(1024, 768);
    
    DiagramView* view = new DiagramView (fileName, m_pClipboardManager, canvas, workspace);
    
    workspace->addWindow(view);

    connect(view, SIGNAL(drawFigureAdded()), this, SLOT(drawFigureAdded()));

    connect(view, SIGNAL(selectionAvailable(bool)), this, SLOT(selectionAvailable(bool)));

    connect(view, SIGNAL(zoomLevelUpdated(int)), this, SLOT(editZoomLevelUpdated(int)));
    
    connect(view, SIGNAL(undoAvailable(bool)), this, SLOT(undoAvailable(bool)));
    connect(view, SIGNAL(redoAvailable(bool)), this, SLOT(redoAvailable(bool)));
    
    connect(view, SIGNAL(associateFile(bool)), this, SLOT(associateFile(bool)));
    
    connect(view, SIGNAL(openAssociateFile( QString )), this, SLOT(fileOpen( QString)));
    
    selectionAvailable( false);
    
    return view;
}

void DlgMain::fileNew()
{
    QList<QWidget *> windows = workspace->windowList();
    
    QString fileName (tr("Untitled"));
    fileName += QString::number(windows.size());
    
    DiagramView *view = createDiagramView(fileName);
    view->show();
    
    browseCursorAct->setChecked(true);
}

void DlgMain::init (char* filename)
{
    fileOpen (QString (filename));
}

void DlgMain::fileOpen(QString filename)
{
    QSettings settings;
    QString lastDir = settings.value("lastDir","").toString();
    
    //check if last dir still exists
    QDir dir (lastDir);
    if (!dir.exists())
        lastDir = "";

    if (filename.isEmpty())
        filename = QFileDialog::getOpenFileName( this, "QShapes -- File Open", lastDir, "QShapes file (*.qsh *.QSH);;All files (*.*)");
    
    try
    {
        if (filename.isEmpty())
            throw (QString ("You've specified an invalid filename"));
            
        if (!QFile::exists( filename ))
            throw (QString ("Selected filename doesn't exist!"));
        
        DiagramView* view = createDiagramView (filename);
        
        if (view->load())
        {
            statusBar()->showMessage ("File opened successfully",2000);
            view->show();
            
            // update recent file list
            QSettings settings;
            QStringList files = settings.value("recentFileList").toStringList();
            files.removeAll(filename);
            files.prepend(filename);
            while (files.size() > MaxRecentFiles)
                files.removeLast();

            settings.setValue("recentFileList", files);

            foreach (QWidget *widget, QApplication::topLevelWidgets())
            {
                DlgMain*mainWin = qobject_cast<DlgMain *>(widget);
                if (mainWin)
                    mainWin->updateRecentFileActions();
            }
            
            browseCursorAct->setChecked(true);
            
            // update last dir
            QFileInfo fileInfo (filename);
            settings.setValue("lastDir", fileInfo.dir().path());
        }
        else
            delete view;
    }
    catch (QString &msg)
    {
        msg = "File Open abandoned\n" + msg;
        QMessageBox::critical (this, "Error", msg);
        
        fileNewAct->setEnabled(true);
        fileOpenAct->setEnabled(true);
    }
    catch (...)
    {
        QMessageBox::critical (this, "Error", "General error");
    }
}

void DlgMain::updateRecentFileActions()
{
    QSettings settings;
    QStringList files = settings.value("recentFileList").toStringList();

    int numRecentFiles = qMin(files.size(), (int)MaxRecentFiles);

    for (int i = 0; i < numRecentFiles; ++i)
    {
        QString text = tr("&%1 %2").arg(i + 1).arg(QFileInfo(files[i]).fileName());
        recentFileActs[i]->setText(text);
        recentFileActs[i]->setData(files[i]);
        recentFileActs[i]->setVisible(true);
        text = tr("Open \"%1\"").arg(files[i]);
        recentFileActs[i]->setStatusTip(text);
    }
    
    for (int j = numRecentFiles; j < MaxRecentFiles; ++j)
        recentFileActs[j]->setVisible(false);

    separatorAct->setVisible(numRecentFiles > 0);
}

void DlgMain::fileSave()
{
    DiagramView *view = activeDiagramView();
    
    if (view != 0)
    {
        QString filename = view->getFileName();
        
        if ((filename.isEmpty()) || (filename.contains("untitled", Qt::CaseInsensitive)))
        {
            fileSaveAs();
            return;
        }
        
        if (view->save())
        {
            view->setModified (false);
            
            QString title;
        
            title = "QShapes - ";
            title += view->strippedName(view->getFileName());
            
            view->setWindowTitle (title);
            
            statusBar()->showMessage ("File successfully saved", 2000);
        }
        else
            statusBar()->showMessage ("File NOT saved", 2000);
    }
}

void DlgMain::fileSaveAs()
{
    DiagramView *view = activeDiagramView();
    
    if (view != 0)
    {
        QString filename = QFileDialog::getSaveFileName( this, "QShapes -- File Save As", QString(), "QShapes file (*.qsh *.QSH)");
    
        if ( ! filename.isEmpty() )
        {
            //if needed adds .qsh extension
            QRegExp ext ( "(\\.qsh)$" );
            ext.setCaseSensitivity (Qt::CaseInsensitive);
        
            if (ext.indexIn(filename) == -1)
                filename+=".qsh"; // add extension

                view->setFileName(filename);
                
                fileSave();
        }
    }
}

void DlgMain::fileExport()
{
    DiagramView *view = activeDiagramView();
    
    if (view != 0)
    {
        QString filename = QFileDialog::getSaveFileName( this, "QShapes -- File Export", QString(), "Text file (*.txt *.TXT)");
    
        if ( ! filename.isEmpty() )
        {
            //if needed adds .qsh extension
            QRegExp ext ( "(\\.txt)$" );
            ext.setCaseSensitivity (Qt::CaseInsensitive);
        
            if (ext.indexIn(filename) == -1)
                filename+=".txt"; // add extension

            QString report = view->generateReport();
            
            if (!report.isEmpty())
            {
                QFile file (filename);

                file.open (QIODevice::WriteOnly | QIODevice::Truncate);

                int result = file.write (report.toAscii(), report.length());
    
                file.close();
    
                if (result == -1)
                    QMessageBox::critical (this, "Error", "An error occurred, unable to export file");
                else
                    statusBar()->showMessage ("File successfully exported", 2000);
            }
            else
                QMessageBox::critical (this, "Error", "Report is empty, maybe you've to check your file");
        }
    }
}

bool DlgMain::okToClear (DiagramView* view)
{
    if (view == NULL)
        view = activeDiagramView();
    
    if (view != 0)
    {
        if (view->getMofified())
        {
            QString message = view->getFileName();
            
            message += QString(" has been modified\nAre you sure to close this file?");
            
            if (QMessageBox::warning (this, "Warning",message,QMessageBox::Yes, QMessageBox::No, QMessageBox::NoButton) == QMessageBox::Yes)
            {
                view->setModified (false);
                return true;
            }
            
            return false;
        }
            
        //nothing changed, we can clear all
        return true;
    }
    else
        return true;
}

bool DlgMain::okToClearAll ()
{
    QList<QWidget *> windows = workspace->windowList();

    for (int i = 0; i < windows.size(); ++i)
    {
        DiagramView* view = dynamic_cast<DiagramView*>(windows.at(i));
        
        if ((view != 0) && (!okToClear(view)))
            return false;
    }
    
    return true;
}

void DlgMain::fileClose()
{
    if (okToClear())
    {
        DiagramView* view = dynamic_cast<DiagramView*>(workspace->activeWindow());
        
        view->setModified (false);
        
        workspace->closeActiveWindow();
    }
}

void DlgMain::fileExit()
{
    if (okToClear())
    {
        workspace->closeAllWindows();
        close();
    }
}

void DlgMain::filePrint()
{
    DiagramView *view = activeDiagramView();
    
    if (view != 0)
    {
        QPrintDialog dlg( m_pPrinter, this);
        
        if (dlg.exec() == QDialog::Accepted)
        {
            QtCanvas* canvas = view->getCanvas();
            
            QPainter painter(m_pPrinter);
            
//             painter.setRenderHint(QPainter::Antialiasing , true);
//             painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
            
            QRect rect = painter.viewport();
            QSize size = canvas->size();
            size.scale(rect.size(), Qt::KeepAspectRatio);
            painter.setViewport(rect.x(), rect.y(),
                                size.width(), size.height());
            painter.setWindow(canvas->rect());
            painter.drawRect(painter.window());
            painter.setClipRect(painter.viewport());
            QtCanvasItemList items = canvas->collisions(canvas->rect());
            QtCanvasItemList::const_iterator it = items.end();
            while (it != items.begin()) {
                --it;
                (*it)->draw(painter);
            }
        }

    }
}

void DlgMain::associateFile (bool status)
{
    if (status)
        statusBar()->showMessage ("File associated successfully to supersystem", 2000);
    else
        statusBar()->showMessage ("Error: unable to associate external file to supersystem", 2000);
}

void DlgMain::editUndo()
{
    DiagramView *view = activeDiagramView();
    
    if (view != 0)
        view->undo();
}

void DlgMain::undoAvailable(bool status)
{
    editUndoAct->setEnabled (status);
    
    DiagramView *view = activeDiagramView();
    
    if (view != 0)
        view->setModified (true);
}

void DlgMain::redoAvailable(bool status)
{
    editRedoAct->setEnabled (status);
}

void DlgMain::editRedo()
{
    DiagramView *view = activeDiagramView();
    
    if (view != 0)
        view->redo();
}

void DlgMain::selectionAvailable(bool status)
{
    editCopyAct->setEnabled (status);
    editCutAct->setEnabled (status);
}

void DlgMain::editCut()
{
    DiagramView *view = activeDiagramView();
    
    if (view != 0)
        view->cut();
}

void DlgMain::editCopy()
{
    DiagramView *view = activeDiagramView();
    
    if (view != 0)
        view->copy();
}

void DlgMain::editPaste()
{
    DiagramView *view = activeDiagramView();
    
    if (view != 0)
        view->paste();
}

void DlgMain::pasteAvailable (bool status)
{
    editPasteAct->setEnabled (status);
}

void DlgMain::editZoomLevelUpdated( int zoomLevel)
{
    m_pComboBox->setCurrentIndex (zoomLevel / 25 -1);
}

void DlgMain::editZoomLevelChanged( QString zoomLevel)
{
    DiagramView *view = activeDiagramView();
        
    if (view != 0)
    {
        // zoomLevel format is %i\% --> remove last %
        zoomLevel.remove (zoomLevel.length() -1, zoomLevel.length() -1);
        int value;
        bool check;
        
        value = zoomLevel.toInt(&check);
        
        if (check)
            view->setZoomLevel (value);
    }
}

void DlgMain::editZoomPlus()
{
    DiagramView *view = activeDiagramView();
    
    if (view != 0)
        view->zoomPlus();
}

void DlgMain::editZoomMinus()
{
    DiagramView *view = activeDiagramView();
    
    if (view != 0)
        view->zoomMinus();
}

void DlgMain::browseCursor()
{
    DiagramView *view = activeDiagramView();
    
    if (view != 0)
        view->setPanMode(false);
}

void DlgMain::browsePan()
{
    DiagramView *view = activeDiagramView();
    
    if (view != 0)
        view->setPanMode(true);
}

void DlgMain::drawSystem()
{
    if (drawSystemAct->isChecked())
    {
        DiagramView *view = activeDiagramView();
        
        if (view != 0)
            view->addSystemBox();
    }
}

void DlgMain::drawLine()
{
    if (drawLineAct->isChecked())
    {
        DiagramView *view = activeDiagramView();
        
        if (view != 0)
            view->addLine();
    }
}

void DlgMain::drawProductBox()
{
    if(drawProductBoxAct->isChecked())
    {
        DiagramView *view = activeDiagramView();
        
        if(view !=0)
            view->addProductBox();
    }
}

void DlgMain::drawSuperSystem()
{
    if(drawSuperSystemAct->isChecked())
    {
        DiagramView *view = activeDiagramView();
        
        if(view !=0)
            view->addSuperSystem();
    }
}

void DlgMain::drawFigureAdded()
{
    drawSystemAct->setChecked (false);
    drawLineAct->setChecked (false);
    drawSuperSystemAct->setChecked (false);
    drawProductBoxAct->setChecked(false);
}

void DlgMain::editEnableAntiAliasing(bool status)
{
    DiagramView *view = activeDiagramView();
    
    if (view != 0)
        view->setHighQualityRendering(status);
}

void DlgMain::about()
{
    DlgAbout dlg (VERSION_MAJ, VERSION_MIN, this);
    dlg.exec();
}
