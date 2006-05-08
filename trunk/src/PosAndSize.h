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

#ifndef POSANDSIZE_H
#define POSANDSIZE_H

/*! 
    \class PosAndSize
    \brief Class used to store diagram item's position and size
    \author Flavio Castelli
 */

#include <QPoint>

class PosAndSize
{
    public:
        PosAndSize();
        virtual ~PosAndSize();
    
        bool operator != (PosAndSize* item2);
        
    protected:
        QPoint m_topLeft; //!< box top left point
        int m_iWidth; //!< box width
        int m_iHeight; //!< box height
        QPoint m_startPoint; //! line's start point
        QPoint m_endPoint; //! line's end point
};

#endif
