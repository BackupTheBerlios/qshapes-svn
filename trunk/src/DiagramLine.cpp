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

#include "DiagramLine.h"
#include "DiagramSupport.h"
#include "LinePosAndSize.h"
#include "XmlSupport.h"

#include <math.h>
#include <QDomDocument>
#include <QtAlgorithms>
#include <QPainter>


DiagramLine::DiagramLine(QtCanvas *canvas)
    : QtCanvasLine(canvas),
    m_bMoveStart (false),
    m_bMoveEnd (true)
{
    setPoints(0, 0, 0, 0);
    m_strText = "line";
    
    m_lastGamma = 0;
    
    m_vert1.setX(-7);
    m_vert1.setY(-24);
    
    m_vert2.setX(7);
    m_vert2.setY(-24);
    
    m_brush.setStyle (Qt::SolidPattern);
    m_brush.setColor (m_pen.color());
}

DiagramLine::DiagramLine( DiagramLine *line, QtCanvas *canvas)
    : DiagramItem (line),
      QtCanvasLine (canvas),
      m_bMoveStart (false),
      m_bMoveEnd (true)
{
    // copy line's canvas if canvas is null
    if (canvas == 0)
        setCanvas (line->canvas());
    
    // copy only useful data
    QPoint start = line->startPoint();
    QPoint end = line->endPoint();
    
    setPoints (start.x(), start.y(), end.x(), end.y());
    
    m_lastGamma = 0;
    
    calculateTriangleVertices();
    
    m_brush.setStyle (Qt::SolidPattern);
    m_brush.setColor (m_pen.color());
}

DiagramLine::~DiagramLine()
{
    hide();
}

QString DiagramLine::getType()
{
    return QString ("DiagramLine");
}

DiagramItem* DiagramLine::clone ()
{
    DiagramLine* clone = new DiagramLine (this, this->canvas());
    
    return clone;
}

void DiagramLine::changePen (QPen pen)
{
    m_pen = pen;
    setPen (m_pen);
    m_brush.setColor (m_pen.color());
    update();
}

bool DiagramLine::isBegin (int mouseX, int mouseY)
{
    QPoint point = startPoint();
    QPoint mouse (mouseX, mouseY);

    if (collideHandle(point, mouse))
    {
        m_bMoveStart = true;
        m_bMoveEnd = false;

        return true;
    }

    return false;
}

bool DiagramLine::isEnd (int mouseX, int mouseY)
{
    QPoint point = endPoint();
    QPoint mouse (mouseX, mouseY);
    
    if (collideHandle(point, mouse))
    {
        m_bMoveStart = false;
        m_bMoveEnd = true;
        return true;
    }

    return false;
}

void DiagramLine::setText(QString newText)
{
    m_strText = newText;
    update();
}

bool DiagramLine::checkExpand (int mouseX, int mouseY)
{
    if (isBegin(mouseX,mouseY) || isEnd(mouseX, mouseY))
        return true;
    else
        return false;
}

void DiagramLine::expand(int mouseX, int mouseY)
{
    QPoint point;

    if (m_bMoveStart)
    {
        point = endPoint();
        setPoints (mouseX, mouseY, point.x(), point.y());
    }
    else
    {
        point = startPoint();
        setPoints (point.x(), point.y(), mouseX, mouseY);
    }
}

double DiagramLine::calculateGamma(QPoint start, QPoint end)
{
    double startPointX = (double) start.x();
    double startPointY = (double) start.y();
    double endPointX = (double) end.x();
    double endPointY = (double) end.y();
    
    double gamma = atan (( startPointY - endPointY ) / ( startPointX - endPointX )) * 180 / 3.14159265;
    
    if (((startPointX < endPointX) && (startPointY == endPointY)) || (startPointX == endPointX == endPointX == endPointY))
        gamma = 0;
    else if ((startPointX < endPointX) && (startPointY < endPointY))
        ;// nop
    else if ((startPointX == endPointX) && (startPointY < endPointY))
        gamma = 90;
    else if ((startPointX > endPointX) && (startPointY < endPointY))
        gamma = 180 - fabs (gamma);
    else if ((startPointX > endPointX) && (startPointY == endPointY))
        gamma = 180;
    else if ((startPointX > endPointX) && (startPointY > endPointY))
        gamma = 180 + fabs (gamma);
    else if ((startPointX == endPointX) && (startPointY > endPointY))
        gamma = 270;
    else if ((startPointX < endPointX) && (startPointY > endPointY))
        gamma = 360 - fabs (gamma);
    else
        qDebug ("unknown case:\n\t-start point = (%i,%i)\n\t-end point = (%i,%i)\n", start.x(), start.y(), end.x(), end.y());
    
    gamma = 360 - gamma;
    
    return gamma;

}

bool DiagramLine::lineInclinationChanged()
{
    double newGamma = calculateGamma(startPoint(), endPoint());
    double delta = fabs( newGamma - m_lastGamma);
    
    if (delta > 6 )
        return true;
    
    return false;
}

void DiagramLine::calculateTriangleVertices()
{
//     qDebug ("Calculating triangle vertices\n");
    
    double startPointX = (double) startPoint().x();
    double startPointY = (double) startPoint().y();
    double endPointX = (double) endPoint().x();
    double endPointY = (double) endPoint().y();
    double AM = 24;
    double V1M = 7;
    double Mx, My;
    double V1x, V1y;
    double V2x, V2y;
            
    double m = ( startPointY - endPointY ) / ( startPointX - endPointX );
    double m_per = -1/m;
    
    double q = endPointY - (m*endPointX);
    
    double a = pow(m,2) + 1;
    double b = (-2*endPointX + 2*m*q -2*endPointY*m);
    double c = (-pow(AM,2) + pow(q,2) -2*endPointY*q + pow(endPointX,2) + pow(endPointY, 2) );
    
    double delta = sqrt(pow(b,2) - 4*a*c);
    double Mx1= (-b +delta)/(2*a);
    double Mx2= (-b -delta)/(2*a);
    
    if (((Mx1 <= endPointX) && (Mx1 >= startPointX)) || ((Mx1 >= endPointX) && (Mx1 <= startPointX)))
        Mx = Mx1;
    else
        Mx = Mx2;
    
    My = m *Mx  + q;
    
    double q2 = My - (m_per*Mx);
    
    double a2 = pow(m_per,2) + 1;
    double b2 = (-2*Mx + 2*m_per*q2 -2*My*m_per);
    double c2 = (-pow(V1M,2) + pow(q2,2) -2*My*q2 + pow(Mx,2) + pow(My,2));
    
    double delta2 = sqrt(pow(b2,2) - 4*a2*c2);
    
    V1x= (-b2 +delta2)/(2*a2);
    V2x= (-b2 -delta2)/(2*a2);
    
    V1y = m_per*V1x + q2;
    V2y = m_per*V2x + q2;
    
    m_vert1.setX ((int) V1x);
    m_vert1.setY ((int) V1y);
    
    m_vert2.setX ((int) V2x);
    m_vert2.setY ((int) V2y);
    
//     m_lastGamma = calculateGamma (startPoint(), endPoint());
}

void DiagramLine::drawShape(QPainter &painter)
{
    // draw line
    QtCanvasLine::drawShape(painter);
    
    //calculate triangle edges
    calculateTriangleVertices();

    // draw triangle
    QPointF points[3] = { endPoint(), m_vert1, m_vert2};
    
    QBrush oriBrush = painter.brush();
    painter.setBrush (m_brush);
    painter.drawPolygon(points, 3);
    painter.setBrush (oriBrush);
    
    QPen oriPen = painter.pen();
    QPen pen;
    pen.setColor (Qt::black);
    painter.setPen (pen);
    painter.drawText(boundingRect(), Qt::AlignCenter, m_strText);
    painter.setPen (oriPen);
    
    if (isActive())
    {
        drawActiveHandle(painter, startPoint() + offset());
        drawActiveHandle(painter, endPoint() + offset());
    }
}

QPolygon DiagramLine::areaPoints() const
{
    const int Extra = Margin + 1;
    QPolygon  points(6);
    
    // calculate other points
    QPoint pointA = startPoint() + offset();
    QPoint pointB = endPoint() + offset();
    
    if (pointA.x() > pointB.x())
        qSwap(pointA, pointB);
    
    points[0] = pointA + QPoint(-Extra, -Extra);
    points[1] = pointA + QPoint(-Extra, +Extra);
    points[3] = pointB + QPoint(+Extra, +Extra);
    points[4] = pointB + QPoint(+Extra, -Extra);
    if (pointA.y() > pointB.y())
    {
        points[2] = pointA + QPoint(+Extra, +Extra);
        points[5] = pointB + QPoint(-Extra, -Extra);
    } else
    {
        points[2] = pointB + QPoint(-Extra, +Extra);
        points[5] = pointA + QPoint(+Extra, -Extra);
    }
    
    return points;
}

void DiagramLine::setMoving (bool state)
{
    DiagramItem::setMoving (state);
    
    if (!state)
        update();
}

void DiagramLine::resetExpand()
{
    m_bMoveStart = false;
    m_bMoveEnd = false;
    
    update();
}

bool DiagramLine::isContained (QRect& selection)
{
    QPoint begin, end;
    
    begin = startPoint();
    end = endPoint();
    
    if (isPointContained( begin, selection) && isPointContained( end, selection))
        return true;
    else
        return false;
}

/*!
    the upper left point of the rectangle with this line as diagonal
*/
void DiagramLine::getTopLeft (QPoint& point)
{
    QPoint begin, end;
    
    begin = startPoint();
    end = endPoint();
    
    if (begin.y() <= end.y())
        point.setY( begin.y());
    else
        point.setY( end.y());
    
    if (begin.x() <= end.x())
        point.setX( begin.x());
    else
        point.setX( end.x());
}

PosAndSize* DiagramLine::getPosAndSize()
{
    LinePosAndSize* posAndSize = new LinePosAndSize (startPoint(), endPoint());
    
    return posAndSize;
}

void DiagramLine::setPosAndSize(PosAndSize* posAndSize)
{
    LinePosAndSize* linePosAndSize = dynamic_cast<LinePosAndSize*> (posAndSize);
    
    if (linePosAndSize != 0)
    {
        QPoint start, end;
    
        start = linePosAndSize->getStartPoint();
        end = linePosAndSize->getEndPoint();
    
        setPoints (start.x(), start.y(), end.x(), end.y());
    }
}

void DiagramLine::save (QDomDocument *document, QDomElement *parent)
{
    DiagramItem::save (document, parent);
    
    // save position
    QDomElement position = document->createElement ("Position");
    parent->appendChild(position);
    
    QDomElement start = document->createElement ("StartPoint");
    position.appendChild(start);
    
    // save start point
    QPoint point = startPoint();
    
    createChildNode( document, &start, QString ("x"), point.x());
    createChildNode( document, &start, QString ("y"), point.y());
    
    // save end point
    point = endPoint();
    
    QDomElement end = document->createElement ("EndPoint");
    position.appendChild(end);
    
    createChildNode( document, &end, QString ("x"), point.x());
    createChildNode( document, &end, QString ("y"), point.y());
}

bool DiagramLine::load (QDomNode* item)
{
    bool check;
    
    if (!DiagramItem::load(item))
        return false;
    
    // restore startPoint & endPoint
    
    QDomNode position = item->firstChildElement("Position");
    
    if (position.isNull())
        return false;
    
    QPoint start = getNodePoint (position, QString ("StartPoint"), &check);
          
    if (!check)
        return false;
    
    QPoint end = getNodePoint (position, QString ("EndPoint"), &check);
          
    if (!check)
        return false;
    
    setPoints (start.x(), start.y(), end.x(), end.y());
    
    return true;
}
