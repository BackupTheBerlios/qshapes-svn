/****************************************************************************
**
** Copyright (C) 1992-2005 Trolltech AS. All rights reserved.
**
** This file may be used under the terms of the GNU General Public
** License version 2.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of
** this file.  Please review the following information to ensure GNU
** General Public Licensing requirements will be met:
** http://www.trolltech.com/products/qt/opensource.html
**
** If you are unsure which license is appropriate for your use, please
** review the following information:
** http://www.trolltech.com/products/qt/licensing.html or contact the
** sales department at sales@trolltech.com.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/
#ifndef EXAMPLE_H
#define EXAMPLE_H

#include <qmainwindow.h>
#include <qtcanvas.h>
#include <qmenu.h>

class BouncyLogo : public QtCanvasSprite {
    void initPos();
    void initSpeed();
public:
    BouncyLogo(QtCanvas*);
    void advance(int);
    int rtti() const;
};


class FigureEditor : public QtCanvasView {
    Q_OBJECT

public:
    FigureEditor(QtCanvas *canvas, QWidget* parent = 0);
    void clear();

protected:
    void contentsMousePressEvent(QMouseEvent*);
    void contentsMouseMoveEvent(QMouseEvent*);
    void contentsMouseReleaseEvent(QMouseEvent *);

signals:
    void status(const QString&);

private:
    QtCanvasItem* moving;
    QPoint moving_start;
};

class Main : public QMainWindow {
    Q_OBJECT

public:
    Main(QtCanvas *, QWidget* parent = 0);
    ~Main();

private slots:
    void aboutQt();
    void newView();
    void clear();
    void init();

    void addSprite();
    void addCircle();
    void addHexagon();
    void addPolygon();
    void addSpline();
    void addText();
    void addLine();
    void addRectangle();
    void addMesh();
    void addLogo();
    void addButterfly();

    void enlarge();
    void shrink();
    void rotateClockwise();
    void rotateCounterClockwise();
    void zoomIn();
    void zoomOut();
    void mirror();
    void moveL();
    void moveR();
    void moveU();
    void moveD();

    void print();

private:
    QtCanvas *canvas;
    FigureEditor *editor;
};

#endif
