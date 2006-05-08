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

#ifndef SUPERSYSTEM_H
#define SUPERSYSTEM_H

#include "qtcanvas.h"
#include "DiagramBox.h"

/*! \class SuperSystem
    \brief The class describing the super system box item.
    \author Flavio Castelli

    SuperSystem is a common hexagon with some text inside<br>
    A beautiful ascii-art representation:<br>
<pre>
 /A----------B\
/              \
F               C 
\              /
 \-E.........F/
</pre>
<br>
m_height and m_width are referred to the surrounding rectangle
 */

class SuperSystem : public DiagramBox , public QtCanvasPolygonalItem
{
    public:
        enum { RTTI = 1003}; //!< value used for identifying subclasses of QtCanvasItem
        
        //! constructor
        SuperSystem(QtCanvas *canvas = 0  );
        
        //! constructor
        SuperSystem(SuperSystem *superSystem, QtCanvas *canvas = 0);
        
        //! destructor
        virtual ~SuperSystem();
        
        //! return the points surrounding the item
        QPolygon areaPoints() const;
        
        //! method used for identifying subclasses of QtCanvasItem
        int rtti() const { return RTTI; }
        
        //! associate an external file to the SuperSystem
        void associateFile (QString fileName);
        
        //! return the associated file's name
        QString getAssociatedFile() { return m_strExternalFile; }
        
        //! return true if item has an associated external file
        bool hasAssociatedFile();
        
        void moveBy ( double dx, double dy );
        
        QRect boundingRect() const;
        
        QRect getSurroundingRect() const;

        void setTopLeft (QPoint& topLeft);
        
        void setHeight (int height);
        void setWidth  (int width);
        void moveResizing (int mouseX, int mouseY, int width, int height);
        
        DiagramItem* clone ();
        
        void setText( QString newText);
        
        QString getToolTip();
        
        QString getType() {return QString ("SuperSystem"); }
        
        void changePen (QPen pen);
        
        void changeBrush (QBrush brush);
        
        void save (QDomDocument *document, QDomElement *parent);

        bool load (QDomNode* item);
        
    protected:
        //! function used to draw item's shape
        void drawShape(QPainter &painter);

    private:
        int m_width, m_height;
        QPoint m_topLeft;
        QString m_strExternalFile;
};

#endif
