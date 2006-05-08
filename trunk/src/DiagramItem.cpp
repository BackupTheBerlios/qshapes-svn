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

#include "DiagramItem.h"
#include "XmlSupport.h"

#include <QDomElement>

DiagramItem::DiagramItem()
{
    m_bMoving = false;
    m_ID = 0;
    m_strText = "unamed";
    m_pen = QPen();
    m_brush = QBrush();
}

DiagramItem::DiagramItem (DiagramItem *diagramItem)
{
    if (diagramItem != NULL)
    {
        m_ID = diagramItem->getID();
        m_pen = diagramItem->getPen();
        m_brush = diagramItem->getBrush();
        m_strText = diagramItem->getText();
        m_strToolTip = diagramItem->getToolTip();
        m_bMoving = diagramItem->getMoving();
    }
    else
    {
        m_bMoving = false;
        m_ID = 0;
        m_strText = "unamed";
        m_pen = QPen();
        m_brush = QBrush();
    }
}

DiagramItem::~DiagramItem()
{
}

bool DiagramItem::operator== (DiagramItem* item2)
{
    if (m_ID == item2->getID())
        return true;
    
    return false;
}

void DiagramItem::setMoving(bool state)
{
    m_bMoving = state;
}

void DiagramItem::setToolTip( QString message)
{
    m_strToolTip = message;
}

QString DiagramItem::getToolTip()
{
    m_strToolTip.sprintf ("ID: %i\nName: ",m_ID);
    m_strToolTip += m_strText;
    m_strToolTip += "\n";
    
    return m_strToolTip;
}

QPen DiagramItem::getPen()
{
    return m_pen;
}

void DiagramItem::save (QDomDocument *document, QDomElement *parent)
{
    // save ID
    createChildNode( document, parent, QString ("ID"), m_ID);
    
    // save type
    createChildNode( document, parent, QString ("Type"), getType());
    
    // save text
    createChildNode( document, parent, QString ("Text"), m_strText);
    
    // save pen
    QDomElement pen = document->createElement ("Pen");
    parent->appendChild(pen);
    createChildNode( document, &pen, QString ("Color"), m_pen.color().name());
    createChildNode( document, &pen, QString ("Style"), (int) m_pen.style());
    createChildNode( document, &pen, QString ("Width"), m_pen.width());
    
        // save brush
    QDomElement brush = document->createElement ("Brush");
    parent->appendChild(brush);
    createChildNode( document, &brush, QString ("Color"), m_brush.color().name());
    createChildNode( document, &brush, QString ("Style"), (int) m_brush.style());
}

bool DiagramItem::load (QDomNode* item)
{
    int ID;
    bool check;
    QString text;
    
    // restore ID
    ID = getNodeNumValue ( *item, QString ("ID"), &check);

    if (!check)
        return false;

    m_ID = ID;
    
    // restore text
    text = getNodeValue ( *item, QString ("Text"));
    m_strText = text;
    
    // restore brush
    QDomElement brush = item->firstChildElement(QString("Brush"));
    
    if (!brush.isNull())
    {
        // restore color
        QColor color;
        color.setNamedColor (getNodeValue (brush, QString("Color")));
        m_brush.setColor (color);
        
        // restore style
        int style = getNodeNumValue (brush, QString ("Style"), &check);
        
        if (check)
            m_brush.setStyle ((Qt::BrushStyle) style);
        
        changeBrush( m_brush);
    }
    
    // restore pen
    QDomElement pen = item->firstChildElement(QString("Pen"));
    
    if (!pen.isNull())
    {
        // restore color
        QColor color;
        color.setNamedColor (getNodeValue (pen, QString("Color")));
        m_pen.setColor(color);
        
        // restore style
        int style = getNodeNumValue (pen, QString ("Style"), &check);
        
        if (check)
            m_pen.setStyle ((Qt::PenStyle) style);
        
        // restore width
        int width = getNodeNumValue (pen, QString ("Width"), &check);
        
        if (check)
            m_pen.setWidth (width);
        
        changePen (m_pen);
    }
    
    return true;
}
