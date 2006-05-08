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

#ifndef BOXPOSANDSIZE_H
#define BOXPOSANDSIZE_H

/*! 
    \class BoxPosAndSize
    \brief Class used to store DiagramBox's position and size
    \author Flavio Castelli
 */
 
#include "PosAndSize.h"

class QPoint;

class BoxPosAndSize : public PosAndSize
{
    public:
        //! constructor
        BoxPosAndSize();
        BoxPosAndSize (QPoint& topLeft, int height, int width);
        
        //! destructor
        ~BoxPosAndSize();
        
        //! return item's height
        int getHeight () { return m_iHeight; }
        
        //! return item's width
        int getWidth () { return m_iWidth; }
        
        //! return box's top left point
        QPoint getTopLeft () { return m_topLeft; }

};

#endif
