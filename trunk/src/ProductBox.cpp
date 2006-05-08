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

#include "ProductBox.h"
#include "DiagramSupport.h"

#include <QPolygon>
#include <QPainter>
#include <QPoint>

ProductBox::ProductBox(QtCanvas *canvas)
    : SystemBox(canvas)
{
    m_strText = "Product";
}

ProductBox::ProductBox(ProductBox *productBox, QtCanvas *canvas)
    : SystemBox(productBox, canvas)
{
}

ProductBox::~ProductBox()
{
}

void ProductBox::drawShape(QPainter &painter)
{
    QRect rectangle = rect();

    painter.drawRoundRect(rectangle, 70, 70);
    
    finishDraw(painter, isActive());
}

DiagramItem* ProductBox::clone ()
{
    ProductBox* clone = new ProductBox (this, this->canvas());
    return clone;
}
