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
 
#ifndef DIAGRAMLINE_H_
#define DIAGRAMLINE_H_

#include <qtcanvas.h>
#include "DiagramItem.h"

/*! \class DiagramLine
    \brief A diagram line
    \author Flavio Castelli

    DiagramLine is the common diagram's line
 */


class DiagramLine : public DiagramItem, public QtCanvasLine
{
    public:
        enum { RTTI = 1002 }; //!< value used for identifying subclasses of QtCanvasItem
        
        //! constructor
        DiagramLine(QtCanvas *canvas = 0);
        
        //! constructor
        DiagramLine(DiagramLine *line, QtCanvas *canvas = 0);
        
        //! destructor
        virtual ~DiagramLine();

        QPoint offset() const { return QPoint((int)x(), (int)y()); }
        
        //! function used to draw lines' shape
        void drawShape(QPainter &painter);
        
        //! return the area surrounding the line
        QPolygon  areaPoints() const;
        
        void changePen (QPen pen);
        
        void setMoving (bool state);
        
        QString getType();
        
        //!< useless method, line doesn't have a brush
        void changeBrush (QBrush brush) { m_brush = brush; }
        
        //! method used for identifying subclasses of QtCanvasItem
        int rtti() const { return RTTI; }
        
        //! check if the point defined by mouseX and mouseY is line's start point
        bool isBegin (int mouseX, int mouseY);
        
        //! check if the point defined by mouseX and mouseY is line's end point
        bool isEnd   (int mouseX, int mouseY);
        
        //! return true if line is expanding by start point
        bool isExpandingByBegin() {return m_bMoveStart;}
        
        //! return true if item is inside the rectangle defined by selection, false otherwise
        bool isContained (QRect& selection);
        
        PosAndSize* getPosAndSize();
        void setPosAndSize(PosAndSize* posAndSize);
        
        void expand (int mouseX, int mouseY);
        bool checkExpand (int mouseX, int mouseY);
        bool isExpanding() {return (m_bMoveStart || m_bMoveEnd);}
        void resetExpand();
        void getTopLeft (QPoint& point);
        
        DiagramItem* clone ();
        
        void setText (QString newText);
        
        void save (QDomDocument *document, QDomElement *parent);
        bool load (QDomNode* item);
        
    private:
        //! calculates triangle's vertices (the triangle is the edge of the arrow)
        void calculateTriangleVertices();
        
        double calculateGamma(QPoint start, QPoint end);
        
        bool lineInclinationChanged();
        
        bool m_bMoveStart; //!< boll variabile, true if line is expanding using the start point
        
        bool m_bMoveEnd; //!< boll variabile, true if line is expanding using the end point
        
        QPoint m_vert1,m_vert2; //!< arrow's edge vertices

        double m_lastGamma; //!< last line's inclination angle
};

#endif /*DIAGRAMLINE_H_*/
