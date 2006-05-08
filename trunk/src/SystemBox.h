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

#ifndef SYSTEMBOX_H_
#define SYSTEMBOX_H_

#include <qtcanvas.h>
#include "DiagramBox.h"

/*! \class SystemBox
    \brief The class describing the system box item.
    \author Flavio Castelli

    SystemBox is a common rectangle with some text inside
 */

class SystemBox : public DiagramBox, public QtCanvasRectangle
{
    public:
        enum { RTTI = 1001 }; //!< value used for identifying subclasses of QtCanvasItem
    
        //! constructor
        SystemBox(QtCanvas *canvas = 0);
        
        //! constructor
        SystemBox(SystemBox *box, QtCanvas *canvas = 0);
        
        //! desctructor
        virtual ~SystemBox();
        
        QString getType() { return QString ("SystemBox"); }
        
        //! function used to draw item's shape
        void drawShape(QPainter &painter);
        
        QRect boundingRect() const;
        
        //! method used for identifying subclasses of QtCanvasItem
        int rtti() const { return RTTI; }
        
        //! return the area surrounding the line
        QPolygon areaPoints() const;
        
        void changePen (QPen pen);
        
        void changeBrush (QBrush brush);
        // specific methods
        
        //! return the rectangle surrounding the item
        QRect getSurroundingRect() const;
        
        //! set box's top left point
        void setTopLeft (QPoint& topLeft);

        //! set box's height
        void setHeight (int height);
        
        //! set box's width
        void setWidth (int width);
        
        void moveResizing (int mouseX, int mouseY, int width, int height);
        void setText(QString newText);
        
        DiagramItem* clone ();
};

#endif /*SYSTEMBOX_H_*/
