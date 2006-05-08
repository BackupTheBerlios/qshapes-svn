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

#ifndef LINEPOSANDSIZE_H
#define LINEPOSANDSIZE_H

/*! 
    \class LinePosAndSize
    \brief Class used to store DiagramLine's position and size
    \author Flavio Castelli
 */
class QPoint;

#include "PosAndSize.h"

class LinePosAndSize : public PosAndSize
{
    public:
        //! constructor
        LinePosAndSize();

        //! constructor
        LinePosAndSize(QPoint startPoint, QPoint endPoint);
        
        //! destructor
        ~LinePosAndSize();
        
        //! return startPoint
        QPoint getStartPoint() { return m_startPoint; }
        
        //! return endPoint
        QPoint getEndPoint() { return m_endPoint; }
};

#endif
