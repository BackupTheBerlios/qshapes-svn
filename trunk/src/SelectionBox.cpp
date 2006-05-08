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

#include "SelectionBox.h"

#include <qtcanvas.h>

SelectionBox::SelectionBox (QtCanvas *canvas)
    : SystemBox( canvas)
{
    // SelectionBox doesn't have text
    m_strText = "";
    
    m_pen.setStyle(Qt::DotLine);
    m_pen.setColor(QColor (74,72,65));
    setPen(m_pen);
    
    m_brush.setStyle(Qt::NoBrush);
    
    setBrush (m_brush);
    
    setSize (0,0);
    
    // start expanding
    m_handle = SystemBox::BOTTOMRIGHT;
}

SelectionBox::~SelectionBox()
{
}
