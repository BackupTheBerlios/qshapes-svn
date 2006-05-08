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

#include "SuperSystem.h"
#include "DiagramSupport.h"
#include "XmlSupport.h"

#include <QDomNode>
#include <QPolygon>
#include <QPainter>
#include <QPoint>

SuperSystem::SuperSystem(QtCanvas *canvas)
    : DiagramBox(),
      QtCanvasPolygonalItem (canvas)
{
    m_topLeft.setX(0);
    m_topLeft.setY(0);
    
    m_width = 100;
    m_height = 60;
    
    m_strText = "Super system";
    
    m_strExternalFile = QString();
}

SuperSystem::SuperSystem(SuperSystem *superSystem, QtCanvas *canvas)
    : DiagramBox(superSystem),
      QtCanvasPolygonalItem (canvas)
{
    QPoint topLeft;
    int height, width;
    
    superSystem->getTopLeft(topLeft);
    height = superSystem->getHeight();
    width = superSystem->getWidth();
    
    m_strExternalFile = superSystem->getAssociatedFile();
    
    moveResizing( topLeft.x(), topLeft.y(), width, height);
}

SuperSystem::~SuperSystem()
{
    hide();
}

QPolygon SuperSystem::areaPoints() const
{
    return DiagramBox::areaPoints();
}

void SuperSystem::drawShape(QPainter &painter)
{
    QPoint pointA (m_topLeft.x() + m_width /4, m_topLeft.y());
    QPoint pointB (m_topLeft.x() + 3 * m_width / 4, m_topLeft.y());
    QPoint pointC (m_topLeft.x() + m_width, m_topLeft.y() + m_height /2);
    QPoint pointD (pointB.x(), m_topLeft.y() + m_height);
    QPoint pointE (pointA.x(), pointD.y());
    QPoint pointF (m_topLeft.x(), pointC.y());
    
    QPointF points[6] = {
        pointA,
        pointB,
        pointC,
        pointD,
        pointE,
        pointF
    };

    painter.drawPolygon(points, 6);
    
    if (!m_strExternalFile.isEmpty())
    {
        QBrush brush;
        QBrush oriBrush = painter.brush();
        brush.setColor (Qt::red);
        brush.setStyle(Qt::SolidPattern);
        painter.setBrush(brush);
        painter.drawRect(m_topLeft.x() + m_width - 10, m_topLeft.y() + 10 , 5, 5);
        painter.setBrush(oriBrush);
    }
    
    // draw item's text and the handles
    finishDraw(painter, isActive());
}

void SuperSystem::associateFile (QString fileName)
{
    m_strExternalFile = fileName;
    invalidate();
    update();
    canvas()->update();
}

bool SuperSystem::hasAssociatedFile()
{
    return !m_strExternalFile.isEmpty();
}

void SuperSystem::changePen (QPen pen)
{
    m_pen = pen;
    setPen (m_pen);
    update();
}

void SuperSystem::changeBrush (QBrush brush)
{
    m_brush = brush;
    setBrush (m_brush);
    update();
}

QRect SuperSystem::boundingRect() const
{
    return QRect((int)x() - Margin, (int)y() - Margin,
                  m_width + 2 * Margin, m_height + 2 * Margin);
}

void SuperSystem::moveBy ( double dx, double dy )
{
    invalidate();
    
    m_topLeft.setX(m_topLeft.x() + (int) dx);
    m_topLeft.setY(m_topLeft.y() + (int) dy);
    
    QtCanvasPolygonalItem::moveBy (dx, dy);
    update();
}

QRect SuperSystem::getSurroundingRect() const
{
    return QRect(m_topLeft.x(), m_topLeft.y(), m_width, m_height);
}

void SuperSystem::setText (QString newText)
{
    m_strText = newText;
    update();
}

QString SuperSystem::getToolTip()
{
    QString tooltip = DiagramBox::getToolTip();
    
    if (m_strExternalFile.isEmpty())
        tooltip += QString("No associated file\n");
    else
    {
        tooltip += QString("Associated File: ");
        tooltip += m_strExternalFile;
    }
    
    return tooltip;
}

void SuperSystem::moveResizing (int mouseX, int mouseY, int width, int height)
{
    invalidate();
    
    m_topLeft.setX (mouseX);
    m_topLeft.setY (mouseY);
    m_height = height;
    m_width = width;
    update();
}

void SuperSystem::setHeight (int height)
{
    invalidate();
    
    m_height = height;

    update();
}

void SuperSystem::setWidth (int width)
{
    invalidate();
    
    m_width = width;

    update();
}

void SuperSystem::setTopLeft (QPoint& topLeft)
{
    invalidate();
    
    m_topLeft = topLeft;
    update();
}

DiagramItem* SuperSystem::clone ()
{
    SuperSystem* clone = new SuperSystem (this, this->canvas());
    return clone;
}

void SuperSystem::save (QDomDocument *document, QDomElement *parent)
{
    DiagramBox::save(document, parent);
    
    // append associated file
    createChildNode( document, parent, QString ("AssociatedFile"), m_strExternalFile);
}

bool SuperSystem::load (QDomNode* item)
{
    QString text = getNodeValue ( *item, QString ("AssociatedFile"));
    
    if (!text.isEmpty())
        m_strExternalFile = text;
    
    return DiagramBox::load( item);
}
