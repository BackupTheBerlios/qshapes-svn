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

#include "DiagramSupport.h"

#include <QPainter>
#include <QPoint>
#include <QRect>

//	A = (x-margin; y-margin)
//	B = (A.x+width; A.y)
//	C = (A.x+width; A.y + height)
//	D = (A.x; A.y + height)
// 
// (0,0)------------------
//  |       A-------B
//  |       |       |
//  |       D-------C
//  |
//  |

void drawActiveHandle(QPainter &painter, const QPoint &center)
{
    painter.setPen(Qt::black);
	painter.setBrush(Qt::gray);
    painter.drawRect(center.x() - Margin, center.y() - Margin,
                     2 * Margin + 1, 2 * Margin + 1);
}

void printPoint(QPoint& point)
{
	qDebug ("(%i,%i)\n",point.x(), point.y());	
}

/*!
    the handle is a square with side defined by Margin
    \sa Margin
*/
bool collideHandle (QPoint& center, QPoint& mouse)
{
	QPoint A (center.x() - Margin, center.y() - Margin);
	QPoint B (A.x() + 2 * Margin + 1, A.y());
	//QPoint C (A.x() + 2 * Margin + 1, A.y() + 2 * Margin + 1);
	QPoint D (A.x() , A.y() + 2 * Margin + 1);

//	printf ("mouse point: ");
//	printPoint(mouse);
//	printf ("A: ");
//	printPoint(A);
//	printf ("B: ");
//	printPoint(B);
//	printf ("C: ");
//	printPoint(C);
//	printf ("D: ");
//	printPoint(D);	
	
	if ((mouse.x() > A.x()) && (mouse.x() < B.x()) && // x constraint
		(mouse.y() > A.y() && (mouse.y() < D.y()))) // y constraint
			return true;

	return false;
}

bool isPointContained (QPoint& point, QRect& rectangle)
{
    int xp1, xp2, yp1, yp2;

    rectangle.getCoords ( &xp1, &yp1, &xp2, &yp2 );
    
    if (((point.x() >= xp1) && (point.x() <= xp2)) // x condition 1
          && ((point.y() >= yp1) && (point.y() <= yp2))) // y condition 1
        return true;
    else
        return false;
}

