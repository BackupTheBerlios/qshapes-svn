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
#include <qdatetime.h>
#include <qmainwindow.h>
#include <qevent.h>
#include <qstatusbar.h>
#include <qmessagebox.h>
#include <qmenubar.h>
#include <qapplication.h>
#include <qpainter.h>
#include <qprinter.h>
#include <qlabel.h>
#include <qimage.h>
#include <qprogressdialog.h>
#include <qprintdialog.h>
#include "canvas.h"
#include <qdebug.h>

#include <stdlib.h>

// We use a global variable to save memory - all the brushes and pens in
// the mesh are shared.
static QBrush *tb = 0;
static QPen *tp = 0;

class EdgeItem;
class NodeItem;

class EdgeItem: public QtCanvasLine
{
public:
    EdgeItem(NodeItem*, NodeItem*, QtCanvas *canvas);
    void setFromPoint(int x, int y) ;
    void setToPoint(int x, int y);
    static int count() { return c; }
    void moveBy(double dx, double dy);
private:
    static int c;
};



class ImageItem: public QtCanvasRectangle
{
public:
    static const int RTTI;
    
    ImageItem(QImage img, QtCanvas *canvas);
    int rtti () const { return RTTI; }
    bool hit(const QPoint&) const;
protected:
    void drawShape(QPainter &);
private:
    QImage image;
    QPixmap pixmap;
};

const int ImageItem::RTTI = 984376;


ImageItem::ImageItem(QImage img, QtCanvas *canvas)
    : QtCanvasRectangle(canvas), image(img)
{
    setSize(image.width(), image.height());
    pixmap = QPixmap::fromImage(image);
}


void ImageItem::drawShape(QPainter &p)
{
    p.drawPixmap(int(x()), int(y()), pixmap);
}

bool ImageItem::hit(const QPoint &p) const
{
    int ix = p.x()-int(x());
    int iy = p.y()-int(y());
    if (!image.valid(ix , iy))
	return false;
    QRgb pixel = image.pixel(ix, iy);
    return qAlpha(pixel) != 0;
}

class NodeItem: public QtCanvasEllipse
{
public:
    NodeItem(QtCanvas *canvas);
    ~NodeItem() {}

    void addInEdge(EdgeItem *edge) { inList.append(edge); }
    void addOutEdge(EdgeItem *edge) { outList.append(edge); }

    void moveBy(double dx, double dy);

    //    QPoint center() { return boundingRect().center(); }
private:
    QList<EdgeItem*> inList;
    QList<EdgeItem*> outList;
};


int EdgeItem::c = 0;


void EdgeItem::moveBy(double, double)
{
    //nothing
}

EdgeItem::EdgeItem(NodeItem *from, NodeItem *to, QtCanvas *canvas)
    : QtCanvasLine(canvas)
{
    c++;
    setPen(*tp);
    setBrush(*tb);
    from->addOutEdge(this);
    to->addInEdge(this);
    setPoints(int(from->x()), int(from->y()), int(to->x()), int(to->y()));
    setZ(127);
}

void EdgeItem::setFromPoint(int x, int y)
{
    setPoints(x, y, endPoint().x(), endPoint().y());
}

void EdgeItem::setToPoint(int x, int y)
{
    setPoints(startPoint().x(), startPoint().y(), x, y);
}



void NodeItem::moveBy(double dx, double dy)
{
    QtCanvasEllipse::moveBy(dx, dy);

    for (int i = 0; i < inList.size(); ++i)
	inList.at(i)->setToPoint(int(x()), int(y()));

    for (int i = 0; i < outList.size(); ++i)
	outList.at(i)->setFromPoint(int(x()), int(y()));
}

NodeItem::NodeItem(QtCanvas *canvas)
    : QtCanvasEllipse(6, 6, canvas)
{
    setPen(*tp);
    setBrush(*tb);
    setZ(128);
}

FigureEditor::FigureEditor(QtCanvas* c, QWidget* parent)
    : QtCanvasView(c, parent), moving(0)
{
}

void FigureEditor::contentsMousePressEvent(QMouseEvent* e)
{
    QPoint p = inverseWorldMatrix().map(e->pos());
    QtCanvasItemList l = canvas()->collisions(p);
    moving = 0;
    for (QtCanvasItemList::Iterator it = l.begin(); it!= l.end(); ++it) {
	if ((*it)->rtti() == ImageItem::RTTI) {
	    ImageItem *item = (ImageItem*)(*it);
	    if (!item->hit(p))
		 continue;
	}
	moving = *it;
	moving_start = p;
	break;
    }
}

void FigureEditor::clear()
{
    QtCanvasItemList list = canvas()->allItems();
    qDeleteAll(list);
}

void FigureEditor::contentsMouseMoveEvent(QMouseEvent* e)
{
    if (moving) {
 	QPoint p = inverseWorldMatrix().map(e->pos());
 	moving->moveBy(p.x() - moving_start.x(), p.y() - moving_start.y());
	moving_start = p;
	canvas()->update();
    }
}

void FigureEditor::contentsMouseReleaseEvent(QMouseEvent *)
{
    moving = 0;
}


BouncyLogo::BouncyLogo(QtCanvas* canvas) :
    QtCanvasSprite(0, canvas)
{
    static QtCanvasPixmapArray logo(":/qt-trans.xpm");
    setSequence(&logo);
    setAnimated(true);
    initPos();
}


const int logo_rtti = 1234;

int BouncyLogo::rtti() const
{
    return logo_rtti;
}

void BouncyLogo::initPos()
{
    initSpeed();
    int trial = 1000;
    do {
	move(rand() % canvas()->width(), rand() % canvas()->height());
	advance(0);
    } while (trial-- && xVelocity() == 0.0 && yVelocity() == 0.0);
}

void BouncyLogo::initSpeed()
{
    const double speed = 4.0;
    double d = (double)(rand() % 1024) / 1024.0;
    setVelocity(d*speed*2-speed, (1-d)*speed*2-speed);
}

void BouncyLogo::advance(int stage)
{
    switch (stage) {
      case 0: {
	double vx = xVelocity();
	double vy = yVelocity();

	if (vx == 0.0 && vy == 0.0) {
	    // stopped last turn
	    initSpeed();
	    vx = xVelocity();
	    vy = yVelocity();
	}

	double nx = x() + vx;
	double ny = y() + vy;

	if (nx < 0 || nx >= canvas()->width())
	    vx = -vx;
	if (ny < 0 || ny >= canvas()->height())
	    vy = -vy;

	for (int bounce = 0; bounce<4; bounce++) {
	    QtCanvasItemList l = collisions(false);
	    for (QtCanvasItemList::Iterator it = l.begin(); it!= l.end(); ++it) {
		QtCanvasItem *hit = *it;
		if (hit->rtti() == logo_rtti && hit->collidesWith(this)) {
		    switch (bounce) {
		      case 0:
			vx = -vx;
			break;
		      case 1:
			vy = -vy;
			vx = -vx;
			break;
		      case 2:
			vx = -vx;
			break;
		      case 3:
			// Stop for this turn
			vx = 0;
			vy = 0;
			break;
		    }
		    setVelocity(vx, vy);
		    break;
		}
	    }
	}

	if (x()+vx < 0 || x()+vx >= canvas()->width())
	    vx = 0;
	if (y()+vy < 0 || y()+vy >= canvas()->height())
	    vy = 0;

	setVelocity(vx, vy);
      } break;
      case 1:
	QtCanvasItem::advance(stage);
	break;
    }
}

static uint mainCount = 0;
static QImage *butterflyimg;
static QImage *logoimg;

Main::Main(QtCanvas *c, QWidget* parent)
    : QMainWindow(parent),
      canvas(c)
{
    editor = new FigureEditor(canvas, this);
    QMenuBar* menu = menuBar();


    QMenu* file = new QMenu("&File", menu);
    file->addAction("&Fill canvas", this, SLOT(init()), Qt::CTRL+Qt::Key_F);
    file->addAction("&Erase canvas", this, SLOT(clear()), Qt::CTRL+Qt::Key_E);
    file->addAction("&New view", this, SLOT(newView()), Qt::CTRL+Qt::Key_N);
    file->addSeparator();
    file->addAction("&Print...", this, SLOT(print()), Qt::CTRL+Qt::Key_P);
    file->addSeparator();
    file->addAction("E&xit", qApp, SLOT(quit()), Qt::CTRL+Qt::Key_Q);
    menu->addMenu(file);

    QMenu* edit = new QMenu("&Edit", menu);
    edit->addAction("Add &Circle", this, SLOT(addCircle()), Qt::ALT+Qt::Key_C);
    edit->addAction("Add &Hexagon", this, SLOT(addHexagon()), Qt::ALT+Qt::Key_H);
    edit->addAction("Add &Polygon", this, SLOT(addPolygon()), Qt::ALT+Qt::Key_P);
    edit->addAction("Add Spl&ine", this, SLOT(addSpline()), Qt::ALT+Qt::Key_I);
    edit->addAction("Add &Text", this, SLOT(addText()), Qt::ALT+Qt::Key_T);
    edit->addAction("Add &Line", this, SLOT(addLine()), Qt::ALT+Qt::Key_L);
    edit->addAction("Add &Rectangle", this, SLOT(addRectangle()), Qt::ALT+Qt::Key_R);
    edit->addAction("Add &Sprite", this, SLOT(addSprite()), Qt::ALT+Qt::Key_S);
    edit->addAction("Create &Mesh", this, SLOT(addMesh()), Qt::ALT+Qt::Key_M);
    edit->addAction("Add &Alpha-blended image", this, SLOT(addButterfly()), Qt::ALT+Qt::Key_A);
    menu->addMenu(edit);

    QMenu* view = new QMenu("&View", menu);
    view->addAction("&Enlarge", this, SLOT(enlarge()), Qt::SHIFT+Qt::CTRL+Qt::Key_Plus);
    view->addAction("Shr&ink", this, SLOT(shrink()), Qt::SHIFT+Qt::CTRL+Qt::Key_Minus);
    view->addSeparator();
    view->addAction("&Rotate clockwise", this, SLOT(rotateClockwise()), Qt::CTRL+Qt::Key_PageDown);
    view->addAction("Rotate &counterclockwise", this, SLOT(rotateCounterClockwise()), Qt::CTRL+Qt::Key_PageUp);
    view->addAction("&Zoom in", this, SLOT(zoomIn()), Qt::CTRL+Qt::Key_Plus);
    view->addAction("Zoom &out", this, SLOT(zoomOut()), Qt::CTRL+Qt::Key_Minus);
    view->addAction("Translate left", this, SLOT(moveL()), Qt::CTRL+Qt::Key_Left);
    view->addAction("Translate right", this, SLOT(moveR()), Qt::CTRL+Qt::Key_Right);
    view->addAction("Translate up", this, SLOT(moveU()), Qt::CTRL+Qt::Key_Up);
    view->addAction("Translate down", this, SLOT(moveD()), Qt::CTRL+Qt::Key_Down);
    view->addAction("&Mirror", this, SLOT(mirror()), Qt::CTRL+Qt::Key_Home);
    view->addSeparator();
    QAction *hq = view->addAction("&High quality rendering");
    connect(hq, SIGNAL(toggled(bool)), editor, SLOT(setHighQualityRendering(bool)));
    hq->setChecked(false);
    hq->setCheckable(true);
    menu->addMenu(view);

    menu->addSeparator();

    QMenu* help = new QMenu("&Help", menu);
    help->addAction("&About Qt", this, SLOT(aboutQt()), Qt::Key_F1);
    menu->addMenu(help);

    statusBar();

    setCentralWidget(editor);

    QtCanvasItemList list = canvas->allItems();
    if (list.isEmpty()) 
        init();
}

void Main::init()
{
    clear();

    static int r = 24;
    srand(++r);

    mainCount++;
    butterflyimg = 0;
    logoimg = 0;

    int i;
    for (i = 0; i < canvas->width() / 56; i++)
        addButterfly();

    for (i = 0; i < canvas->width() / 85; i++) 
        addHexagon();

    for (i = 0; i < canvas->width() / 128; i++) 
        addLogo();
}

Main::~Main()
{
    if (!--mainCount) {
	delete[] butterflyimg;
	butterflyimg = 0;
	delete[] logoimg;
	logoimg = 0;
    }
}

void Main::newView()
{
    // Open a new view... have it delete when closed.
    Main *m = new Main(canvas, 0);
    m->show();
}

void Main::clear()
{
    editor->clear();
}

void Main::aboutQt()
{
    QMessageBox::aboutQt(this, "Qt Canvas Example");
}

void Main::enlarge()
{
    canvas->resize(canvas->width()*4/3, canvas->height()*4/3);
}

void Main::shrink()
{
    canvas->resize(canvas->width()*3/4, canvas->height()*3/4);
}

void Main::rotateClockwise()
{
    QMatrix m = editor->worldMatrix();
    m.rotate(22.5);
    editor->setWorldMatrix(m);
}

void Main::rotateCounterClockwise()
{
    QMatrix m = editor->worldMatrix();
    m.rotate(-22.5);
    editor->setWorldMatrix(m);
}

void Main::zoomIn()
{
    QMatrix m = editor->worldMatrix();
    m.scale(2.0, 2.0);
    editor->setWorldMatrix(m);
}

void Main::zoomOut()
{
    QMatrix m = editor->worldMatrix();
    m.scale(0.5, 0.5);
    editor->setWorldMatrix(m);
}

void Main::mirror()
{
    QMatrix m = editor->worldMatrix();
    m.scale(-1, 1);
    editor->setWorldMatrix(m);
}

void Main::moveL()
{
    QMatrix m = editor->worldMatrix();
    m.translate(-16, 0);
    editor->setWorldMatrix(m);
}

void Main::moveR()
{
    QMatrix m = editor->worldMatrix();
    m.translate(+16, 0);
    editor->setWorldMatrix(m);
}

void Main::moveU()
{
    QMatrix m = editor->worldMatrix();
    m.translate(0, -16);
    editor->setWorldMatrix(m);
}

void Main::moveD()
{
    QMatrix m = editor->worldMatrix();
    m.translate(0, +16);
    editor->setWorldMatrix(m);
}

void Main::print()
{
    QPrinter printer;
    QPrintDialog dlg(&printer, this);
    if (dlg.exec() == QDialog::Accepted) {
 	QPainter pp(&printer);
 	canvas->drawArea(QRect(0, 0, canvas->width(), canvas->height()), &pp, false);
    }
}


void Main::addSprite()
{
    QtCanvasItem* i = new BouncyLogo(canvas);
    i->setZ(rand() % 256);
    i->show();
}

QString butterfly_fn;
QString logo_fn;


void Main::addButterfly()
{
    if (butterfly_fn.isEmpty())
	return;
    if (!butterflyimg) {
	butterflyimg = new QImage[4];
	butterflyimg[0].load(butterfly_fn);
	butterflyimg[1] = butterflyimg[0].scaled(int(butterflyimg[0].width()*0.75),
                                                 int(butterflyimg[0].height()*0.75),
                                                 Qt::KeepAspectRatio, Qt::SmoothTransformation);
	butterflyimg[2] = butterflyimg[0].scaled(int(butterflyimg[0].width()*0.5),
                                                 int(butterflyimg[0].height()*0.5),
                                                 Qt::KeepAspectRatio, Qt::SmoothTransformation);
	butterflyimg[3] = butterflyimg[0].scaled(int(butterflyimg[0].width()*0.25),
                                                 int(butterflyimg[0].height()*0.25),
                                                 Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }
    QtCanvasPolygonalItem* i = new ImageItem(butterflyimg[rand() % 4], canvas);
    i->move(rand() % (canvas->width() - butterflyimg->width()),
	    rand() % (canvas->height() - butterflyimg->height()));
    i->setZ(rand() % 256 + 250);
    i->show();
}

void Main::addLogo()
{
    if (logo_fn.isEmpty())
	return;
    if (!logoimg) {
	logoimg = new QImage[4];
	logoimg[0].load(logo_fn);
	logoimg[1] = logoimg[0].scaled(int(logoimg[0].width()*0.75),
                                       int(logoimg[0].height()*0.75),
                                       Qt::KeepAspectRatio, Qt::SmoothTransformation);
	logoimg[2] = logoimg[0].scaled(int(logoimg[0].width()*0.5),
                                       int(logoimg[0].height()*0.5),
                                       Qt::KeepAspectRatio, Qt::SmoothTransformation);
	logoimg[3] = logoimg[0].scaled(int(logoimg[0].width()*0.25),
                                        int(logoimg[0].height()*0.25),
                                       Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }
    QtCanvasPolygonalItem* i = new ImageItem(logoimg[rand() % 4], canvas);
    i->move(rand() % (canvas->width() - logoimg->width()),
	    rand() % (canvas->height() - logoimg->width()));
    i->setZ(rand() % 256 + 256);
    i->show();
}



void Main::addCircle()
{
    QtCanvasPolygonalItem* i = new QtCanvasEllipse(50, 50, canvas);
    i->setBrush(QColor(rand() % 32 * 8, rand() % 32 * 8, rand() % 32 * 8));
    i->move(rand() % canvas->width(), rand() % canvas->height());
    i->setZ(rand() % 256);
    i->show();
}

void Main::addHexagon()
{
    QtCanvasPolygon* i = new QtCanvasPolygon(canvas);
    const int size = canvas->width() / 25;
    QPolygon pa(6);
    pa[0] = QPoint(2*size, 0);
    pa[1] = QPoint(size, -size*173/100);
    pa[2] = QPoint(-size, -size*173/100);
    pa[3] = QPoint(-2*size, 0);
    pa[4] = QPoint(-size, size*173/100);
    pa[5] = QPoint(size, size*173/100);
    i->setPoints(pa);
    i->setBrush(QColor(rand() % 32*8, rand() % 32*8, rand() % 32*8));
    i->move(rand() % canvas->width(), rand() % canvas->height());
    i->setZ(rand() % 256);
    i->show();
}

void Main::addPolygon()
{
    QtCanvasPolygon* i = new QtCanvasPolygon(canvas);
    const int size = canvas->width()/2;
    QPolygon pa(6);
    pa[0] = QPoint(0, 0);
    pa[1] = QPoint(size, size/5);
    pa[2] = QPoint(size*4/5, size);
    pa[3] = QPoint(size/6, size*5/4);
    pa[4] = QPoint(size*3/4, size*3/4);
    pa[5] = QPoint(size*3/4, size/4);
    i->setPoints(pa);
    i->setBrush(QColor(rand() % 32*8, rand() % 32*8, rand() % 32*8));
    i->move(rand() % canvas->width(), rand() % canvas->height());
    i->setZ(rand() % 256);
    i->show();
}

void Main::addSpline()
{
    QtCanvasSpline* i = new QtCanvasSpline(canvas);
    const int size = canvas->width()/6;
    QPolygon pa(12);
    pa[0] = QPoint(0, 0);
    pa[1] = QPoint(size/2, 0);
    pa[2] = QPoint(size, size/2);
    pa[3] = QPoint(size, size);
    pa[4] = QPoint(size, size*3/2);
    pa[5] = QPoint(size/2, size*2);
    pa[6] = QPoint(0, size*2);
    pa[7] = QPoint(-size/2, size*2);
    pa[8] = QPoint(size/4, size*3/2);
    pa[9] = QPoint(0, size);
    pa[10] = QPoint(-size/4, size/2);
    pa[11] = QPoint(-size/2, 0);
    i->setControlPoints(pa);
    i->setBrush(QColor(rand() % 32*8, rand() % 32*8, rand() % 32*8));
    i->move(rand() % canvas->width(), rand() % canvas->height());
    i->setZ(rand() % 256);
    i->show();
}

void Main::addText()
{
    QtCanvasText* i = new QtCanvasText(canvas);
    i->setText("QtCanvasText");
    i->move(rand() % canvas->width(), rand() % canvas->height());
    i->setZ(rand() % 256);
    i->show();
}

void Main::addLine()
{
    QtCanvasLine* i = new QtCanvasLine(canvas);
    i->setPoints(rand() % canvas->width(), rand() % canvas->height(), 
		  rand() % canvas->width(), rand() % canvas->height());
    i->setPen(QPen(QColor(rand() % 32*8, rand() % 32*8, rand() % 32*8), 6));
    i->setZ(rand() % 256);
    i->show();
}

void Main::addMesh()
{
    int x0 = 0;
    int y0 = 0;

    if (!tb) tb = new QBrush(Qt::red);
    if (!tp) tp = new QPen(Qt::black);

    int nodecount = 0;

    int w = canvas->width();
    int h = canvas->height();

    const int dist = 30;
    int rows = h / dist;
    int cols = w / dist;

    QVector<NodeItem*> lastRow(cols);
    for (int j = 0; j < rows; j++) {
	int n = j % 2 ? cols-1 : cols;
	NodeItem *prev = 0;
	for (int i = 0; i < n; i++) {
	    NodeItem *el = new NodeItem(canvas);
	    nodecount++;
	    int r = rand();
	    int xrand = r % 20;
	    int yrand = (r/20) % 20;
	    el->move(xrand + x0 + i*dist + (j % 2 ? dist/2 : 0), 
		      yrand + y0 + j*dist);

	    if (j > 0) {
		if (i < cols-1)
		    (new EdgeItem(lastRow[i], el, canvas))->show();
		if (j % 2)
		    (new EdgeItem(lastRow[i+1], el, canvas))->show();
		else if (i > 0)
		    (new EdgeItem(lastRow[i-1], el, canvas))->show();
	    }
	    if (prev) {
		(new EdgeItem(prev, el, canvas))->show();
	    }
	    if (i > 0) lastRow[i-1] = prev;
	    prev = el;
	    el->show();
	}
	lastRow[n-1] = prev;
    }
}

void Main::addRectangle()
{
    QtCanvasPolygonalItem *i = new QtCanvasRectangle(rand() % canvas->width(), rand() % canvas->height(), 
                                                     canvas->width()/5, canvas->width()/5,
                                                     canvas);
    int z = rand() % 256;
    i->setBrush(QColor(z, z, z));
    i->setPen(QPen(QColor(rand() % 32 * 8, rand() % 32*8, rand() % 32*8), 6));
    i->setZ(z);
    i->show();
}
