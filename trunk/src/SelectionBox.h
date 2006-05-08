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

#ifndef SELECTIONBOX_H
#define SELECTIONBOX_H

/*! \class SelectionBox
    \brief the most generic diagram box
    
    The box draw during selection operations
 */

class QtCanvas;

#include "SystemBox.h"

class SelectionBox : public SystemBox
{
    public:
        enum { RTTI = 1003 }; //!< value used for identifying subclasses of QtCanvasItem
        
        //! constructor
        SelectionBox(QtCanvas *canvas);
        
        //! desctructor
        virtual ~SelectionBox();
};

#endif
