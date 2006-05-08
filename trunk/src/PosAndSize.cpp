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

#include "PosAndSize.h"

PosAndSize::PosAndSize()
{
    m_startPoint = QPoint (0,0);
    m_endPoint = QPoint (0,0);
    m_topLeft =  QPoint (0,0);
    m_iWidth = 0;
    m_iHeight = 0;
}


PosAndSize::~PosAndSize()
{
}

bool PosAndSize::operator != (PosAndSize* item2)
{
    if (m_iWidth != item2->m_iWidth)
        return true;
    else if (m_iHeight != item2->m_iHeight)
        return true;
    else if (m_startPoint != item2->m_startPoint)
        return true;
    else if (m_endPoint != item2->m_endPoint)
        return true;
    else if (m_topLeft != item2->m_topLeft)
        return true;
    
    return false;
}
