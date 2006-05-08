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

#ifndef DIAGRAMSUPPORT_H_
#define DIAGRAMSUPPORT_H_

class QPainter;
class QPoint;
class QRect;
class QString;

const int Margin = 5; //!< definine the size of hanlde's square sides (in pixel)

//! draw a square hanlde with center in center
void drawActiveHandle(QPainter &painter, const QPoint &center);

//! print x and y of a point, used only for debug
void printPoint(QPoint& point);

//! check if mouse is over a handle
bool collideHandle (QPoint& center, QPoint& mouse);

//! return true if point is inside rectangle, false otherwise
bool isPointContained (QPoint& point, QRect& rectangle);

#endif /*DIAGRAMSUPPORT_H_*/
