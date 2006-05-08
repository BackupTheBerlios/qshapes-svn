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

#include "SystemBox.h"
#include "DiagramSupport.h"

SystemBox::SystemBox(QtCanvas *canvas)
    : DiagramBox(),
      QtCanvasRectangle(canvas)
{
    setText("System");
    
    setSize (100, 60);
    move (0,0);
}

SystemBox::SystemBox (SystemBox *box, QtCanvas *canvas)
    : DiagramBox(box),
      QtCanvasRectangle(canvas)
{
    QPoint topLeft;
    int height, width;
    
    box->getTopLeft(topLeft);
    height = box->getHeight();
    width = box->getWidth();
    
    moveResizing( topLeft.x(), topLeft.y(), width, height);
}

SystemBox::~SystemBox()
{
    hide();
}

DiagramItem* SystemBox::clone ()
{
    SystemBox* clone = new SystemBox (this, this->canvas());
    
    return clone;
}

void SystemBox::setText(QString newText)
{
    m_strText = newText;
    update();
}

void SystemBox::changePen (QPen pen)
{
    m_pen = pen;
    setPen (m_pen);
    update();
}

void SystemBox::changeBrush (QBrush brush)
{
    m_brush = brush;
    setBrush (m_brush);
    update();
}

void SystemBox::drawShape(QPainter &painter)
{
    QtCanvasRectangle::drawShape(painter);
    
    finishDraw(painter, isActive());
}

QRect SystemBox::boundingRect() const
{
    return QRect((int)x() - Margin, (int)y() - Margin,
                 width() + 2 * Margin, height() + 2 * Margin);
}

QPolygon SystemBox::areaPoints() const
{
    return DiagramBox::areaPoints();
}

QRect SystemBox::getSurroundingRect() const
{
    return rect();
}

void SystemBox::moveResizing (int mouseX, int mouseY, int width, int height)
{
    move (mouseX, mouseY);
    setSize (width, height);
}

void SystemBox::setTopLeft (QPoint& topLeft)
{
    move (topLeft.x(), topLeft.y());
}

void SystemBox::setHeight (int height)
{
    setSize (width(), height);
}

void SystemBox::setWidth (int width)
{
    setSize (width, height());
}
