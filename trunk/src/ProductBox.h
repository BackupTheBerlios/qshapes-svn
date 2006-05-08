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

#ifndef PRODUCTBOX_H
#define PRODUCTBOX_H

#include "SystemBox.h"

/*! \class ProductBox
    \brief The class describing the super system box item.
    \author Flavio Castelli

    ProductBox is a common rectangle with rounded corners and some text inside<br>
 */

class ProductBox : public SystemBox
{
    public:
        enum { RTTI = 1004}; //!< value used for identifying subclasses of QtCanvasItem
        
        //! constructor
        ProductBox(QtCanvas *canvas = 0  );
        
        //! constructor
        ProductBox(ProductBox *productBox, QtCanvas *canvas = 0);
        
        //! destructor
        virtual ~ProductBox();
        
        DiagramItem* clone ();
        
        QString getType() { return QString ("ProductBox"); }

    protected:
        //! function used to draw item's shape
        void drawShape(QPainter &painter);
};

#endif
