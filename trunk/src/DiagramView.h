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

#ifndef DIAGRAMVIEW_H_
#define DIAGRAMVIEW_H_

/*! \class DiagramView
    \brief DiagramView class provides an on-screen view of a DiagramItems.

    \author Flavio Castelli
 */

#include <qtcanvas.h>

class ActionManager;
class ClipboardManager;
class ConnectionCollector;
class DiagramItem;
class DiagramItemCollector;
class ItemConnection;
class QAction;
class QEvent;
class QPoint;
class SelectionBox;
class SelectionManager;
class SuperSystem;

class DiagramView : public QtCanvasView
{
    Q_OBJECT
    
    public:
        
        //! constructor
        DiagramView (QString fileName, ClipboardManager* clipboard, QtCanvas* canvas, QWidget* parent = 0);
        //! destructor
        virtual ~DiagramView();
        
        QString userFriendlyCurrentFile();
        void zoomPlus();
        void zoomMinus();
        void setZoomLevel (int zoomLevel);
        void setPanMode (bool status);
        bool isSelectionAvailable();
        bool isUndoAvailable();
        bool isRedoAvailable();
        void undo ();
        void redo ();
        bool save ();
        bool load ();
        QString generateReport();
        
        QString getFileName() { return m_strFileName;}
        void setFileName(QString fileName) { m_strFileName = fileName;}
        
        bool getMofified() { return m_bModified;}
        void setModified(bool status) { m_bModified = status;}
        
        QString strippedName(QString fullFileName);
        
        QtCanvas* getCanvas() { return m_pCanvas;}
        
    public slots:
        void associateFile();
        void openAssociateFile();
        void cut();
        void copy();
        void paste(QPoint* pastePoint = 0);
        void pasteTo();
        void del();
        void properties();
        void addProductBox();
        void addSystemBox();
        void addSuperSystem();
        void addLine();
        void bringToFront();
        void sendToBack();

    signals:
        void drawFigureAdded();
        void selectionAvailable (bool status);
        void zoomLevelUpdated (int zoolLevel);
        void undoAvailable (bool status);
        void redoAvailable (bool status);
        void associateFile (bool status);
        void openAssociateFile(QString  filename);
        
    protected:
        // mouse events
        void contentsContextMenuEvent(QContextMenuEvent *event);
        void contentsMousePressEvent(QMouseEvent *event);
        void contentsMouseMoveEvent(QMouseEvent *event);
        void contentsMouseReleaseEvent (QMouseEvent* event);
        void contentsMouseDoubleClickEvent(QMouseEvent *event);
        
        // keyboard event
        void keyPressEvent ( QKeyEvent * event );
        void keyReleaseEvent ( QKeyEvent * event );
        
        bool event(QEvent *event);
        void closeEvent(QCloseEvent *event);
        
    private:
        void createActions();
        void addItem(QtCanvasItem *item);
        void showNewItem(QtCanvasItem *item);
        void removeSelection();
        bool saveEmpty(QString filename);
        void autoScroll ();
        
        QtCanvas* m_pCanvas;
        ClipboardManager* m_pClipboardManager;
        DiagramItemCollector* m_pItemCollector;
        QtCanvasItem *m_pPendingItem;
        ActionManager *m_pActionManager;
//         QList<DiagramItem*> m_activeItems;
        ItemConnection* m_pItemConnection;
        ConnectionCollector* m_pConnCollector;
        SelectionBox* m_pSelectionBox;
        SelectionManager* m_pSelectionManager;
        QPoint m_lastPos;
        int m_iMinZ;
        int m_iMaxZ;
        int m_iZoomPercentage;
        bool m_bCtrl;
        bool m_bPan;
        bool m_bModified;
        QString m_strFileName;
        
        QAction *cutAct;
        QAction *copyAct;
        QAction *pasteAct;
        QAction *deleteAct;
        QAction *propertiesAct;
        QAction *addProductBoxAct;
        QAction *addSystemBoxAct;
        QAction *addSuperSystemAct;
        QAction *addLineAct;
        QAction *bringToFrontAct;
        QAction *sendToBackAct;
        QAction *associateFileAct;
        QAction *openAssociateFileAct;
};

#endif /*DIAGRAMVIEW_H_*/
