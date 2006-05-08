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

#include "DiagramBox.h"

#include <QtAlgorithms>
#include <QDomDocument>
#include <QPainter>
#include <QPoint>
#include <QPolygon>

#include "BoxPosAndSize.h"
#include "DiagramLine.h"
#include "DiagramSupport.h"
#include "XmlSupport.h"

DiagramBox::DiagramBox()
    : DiagramItem()
{
    m_handle = NONE;
    
    m_connTop.clear();
    m_connBottom.clear();
    m_connLeft.clear();
    m_connRight.clear();
    
    m_strText = "Box";
}

DiagramBox::DiagramBox(DiagramBox* diagramBox)
    : DiagramItem (diagramBox)
{
    if (diagramBox == NULL)
        m_strText = "Box";
    
    m_handle = NONE;
    
    m_connTop.clear();
    m_connBottom.clear();
    m_connLeft.clear();
    m_connRight.clear();
}

DiagramBox::~DiagramBox()
{
}

void DiagramBox::resetExpand()
{
    m_handle = NONE;
}

bool DiagramBox::isExpanding()
{
    if (m_handle != NONE)
        return true;

    return false;
}

void DiagramBox::setMoving(bool state)
{
    DiagramItem::setMoving (state);
    
    // updates all connection line state
    
    for (int i = 0; i < m_connTop.size(); i++)
        m_connTop.at(i)->setLineMoving(state);

    for (int i = 0; i < m_connBottom.size(); i++)
        m_connBottom.at(i)->setLineMoving(state);
    
    for (int i = 0; i < m_connLeft.size(); i++)
        m_connLeft.at(i)->setLineMoving(state);
    
    for (int i = 0; i < m_connRight.size(); i++)
        m_connRight.at(i)->setLineMoving(state);
}

QPolygon DiagramBox::areaPoints() const
{
//     F-----G       H-----I
//     |     |       |     |
//     |   A-----------B   |
//     E---|           |---L
//         |           |
//     P---|           |---M
//     |   C-----------D   |
//     |     |       |     |
//     Q-----R       O-----N
//
//     a pensarci bene è stato abbastanza inutile, basterebbero F I N Q
    
    const int Extra = Margin + 1;
    //QPolygon  points(12);
    QPolygon  points(4);
    int xp1, xp2, yp1, yp2;
    
    QRect rect = getSurroundingRect();
    rect.getCoords ( &xp1, &yp1, &xp2, &yp2 );

    QPoint pointA (xp1, yp1);
    QPoint pointB (xp2, yp1);
    QPoint pointC (xp1, yp2);
    QPoint pointD (xp2, yp2);
    
    QPoint pointE = pointA + QPoint(-Extra, +Extra);
    QPoint pointF = pointA + QPoint(-Extra, -Extra);
    QPoint pointG = pointA + QPoint(+Extra, -Extra);
    
    QPoint pointH = pointB + QPoint(-Extra, -Extra);
    QPoint pointI = pointB + QPoint(+Extra, -Extra);
    QPoint pointL = pointB + QPoint(+Extra, +Extra);

    QPoint pointM = pointD + QPoint(-Extra, +Extra);
    QPoint pointN = pointD + QPoint(+Extra, +Extra);
    QPoint pointO = pointD + QPoint(-Extra, -Extra);

    QPoint pointP = pointC + QPoint(-Extra, -Extra);
    QPoint pointQ = pointC + QPoint(-Extra, +Extra);
    QPoint pointR = pointC + QPoint(+Extra, +Extra);

//     points[0] = pointF;
//     points[1] = pointE; 
//     points[2] = pointP; 
//     points[3] = pointQ; 
//     points[4] = pointR; 
//     points[5] = pointO;
//     points[6] = pointN;
//     points[7] = pointM;
//     points[8] = pointL;
//     points[9] = pointI;
//     points[10] = pointH;
//     points[11] = pointG;
    
    points[0] = pointF;
    points[1] = pointI; 
    points[2] = pointN; 
    points[3] = pointQ; 
    
    return points;
}

void DiagramBox::finishDraw (QPainter &painter, bool isActive)
{
    QRect rect = getSurroundingRect();
    painter.drawText(rect,Qt::AlignCenter, m_strText);

    if(isActive)
    {
        QPoint middlePoint;
        getMiddle( middlePoint);
        int xp1, xp2, yp1, yp2;
    
        // top left
        xp1 = rect.topLeft().x();
        yp1 = rect.topLeft().y();
        
        // bottom right
        xp2 = rect.bottomRight().x();
        yp2 = rect.bottomRight().y();
        
        drawActiveHandle(painter, rect.topLeft());
        drawActiveHandle(painter, rect.topRight());
        drawActiveHandle(painter, rect.bottomLeft());
        drawActiveHandle(painter, rect.bottomRight());
        
        // MIDDLELEFT
        middlePoint.setX(xp1);
        middlePoint.setY(yp1 + (yp2 - yp1)/2);
        drawActiveHandle(painter, middlePoint);
        
        // MIDDLERIGHT
        middlePoint.setX(xp2);
        middlePoint.setY(yp1 + (yp2 - yp1)/2);
        drawActiveHandle(painter, middlePoint);
        
        // MIDDLETOP
        middlePoint.setX(xp1 + (xp2 - xp1)/2);
        middlePoint.setY(yp1);
        drawActiveHandle(painter, middlePoint);
        
        // MIDDLEBOTTOM
        middlePoint.setX(xp1 + (xp2 - xp1)/2);
        middlePoint.setY(yp2);
        drawActiveHandle(painter, middlePoint);
    }
}

QString DiagramBox::getToolTip()
{
    QString tooltip = DiagramItem::getToolTip();
    QString connNumber;
    tooltip += connNumber.number(m_connTop.size() + m_connBottom.size() + m_connLeft.size() + m_connRight.size());
    tooltip += QString(" connections\n");
    
    return tooltip;
}

bool DiagramBox::checkExpand (int mouseX, int mouseY)
{
    QPoint point;
    QPoint mouse (mouseX, mouseY);    
    // p1 = top left point
    // p2 = bottom right point
    
    int xp1, xp2, yp1, yp2;
    
    getSurroundingRect().getCoords ( &xp1, &yp1, &xp2, &yp2 );
    
    //TOPLEFT
    point.setX(xp1);
    point.setY(yp1);
    
    if (collideHandle(point, mouse))
    {
        m_handle = TOPLEFT;
        return true;
    }
    
    //TOPRIGHT
    point.setX(xp2);
    point.setY(yp1);
    
    if (collideHandle(point, mouse))
    {
        m_handle = TOPRIGHT;
        return true;
    }

    //BOTTOMRIGHT
    point.setX(xp2);
    point.setY(yp2);
    
    if (collideHandle(point, mouse))
    {
        m_handle = BOTTOMRIGHT;
        return true;
    }
    
    //BOTTOMLEFT
    point.setX(xp1);
    point.setY(yp2);
    
    if (collideHandle(point, mouse))
    {
        m_handle = BOTTOMLEFT;
        return true;
    }

    //MIDDLELEFT
    point.setX(xp1);
    point.setY(yp1 + (yp2 - yp1)/2);
    
    if (collideHandle(point, mouse))
    {
        m_handle = MIDDLELEFT;
        return true;
    }
    
    //MIDDLERIGHT
    point.setX(xp2);
    point.setY(yp1 + (yp2 - yp1)/2);
    
    if (collideHandle(point, mouse))
    {
        m_handle = MIDDLERIGHT;
        return true;
    }
    
    //MIDDLETOP
    point.setX(xp1 + (xp2 - xp1)/2);
    point.setY(yp1);
    
    if (collideHandle(point, mouse))
    {
        m_handle = MIDDLETOP;
        return true;
    }
    
    //MIDDLEBOTTOM
    point.setX(xp1 + (xp2 - xp1)/2);
    point.setY(yp2);
    
    if (collideHandle(point, mouse))
    {
        m_handle = MIDDLEBOTTOM;
        return true;
    }
    
    m_handle = NONE;
    
    return false;
}

void DiagramBox::expand (int mouseX, int mouseY)
{
    int xp1, xp2, yp1, yp2;
    int height, width;
    getSurroundingRect().getCoords ( &xp1, &yp1, &xp2, &yp2 );
    
    switch (m_handle)
    {
        case TOPLEFT:
//             printf ("expanding: m_handle = TOPLEFT\n");
            if ((mouseY <= yp2) && (mouseX <= xp2))
            {   // two cases:
                // 1) expansion in act (towards x axis)
                // 2) rect is collapsing, in the worst case now it's only a point
                
                width  = xp2 - mouseX + 1;
                height = yp2 - mouseY + 1;
                
                moveResizing( mouseX, mouseY, width, height);
            }
            else if ((mouseY <= yp2) && (mouseX >= xp2))
            {
                width  = mouseX - xp2 + 1;
                height = yp2 - mouseY + 1;
                
                moveResizing( xp2, mouseY, width, height);
                
                // update hanlder
                m_handle = TOPRIGHT;
            }
            else if ((mouseY >= yp2) && (mouseX >= xp2))
            {   // rec collapsed and is expanding in opposite direction
                width  = mouseX - xp2 + 1;
                height = mouseY - yp2 + 1;
                
                moveResizing( xp2, yp2, width, height);
                
                // update hanlder
                m_handle = BOTTOMRIGHT;
            }
            else if ((mouseY >= yp2) && (mouseX <= xp2))
            {
                width = xp2 - mouseX +1;
                height = mouseY - yp2 + 1;
                
                moveResizing( mouseX, yp2, width, height);
                
                m_handle = BOTTOMLEFT;
            }
            break;
        case TOPRIGHT:
//             printf ("expanding: m_handle = TOPRIGHT\n");
            if ((mouseY <= yp2) && (mouseX >= xp1))
            {   // two cases:
                // 1) expansion in act (towards x axis)
                // 2) rect is collapsing, in the worst case now it's only a point
                width  = mouseX - xp1 + 1;
                height = yp2 - mouseY + 1;
                
                moveResizing( xp1, mouseY, width, height);
            }
            else if ((mouseY <= yp2) && (mouseX < xp1))
            {   
                width  = xp1 - mouseX + 1;
                height = yp2 - mouseY + 1;
                
                moveResizing( mouseX, mouseY, width, height);
                
                // update hanlder
                m_handle = TOPLEFT;
            }
            else if ((mouseY >= yp2) && (mouseX >= xp1))
            {   
                width  = mouseX - xp1 + 1;
                height = mouseY - yp2 + 1;
                
                moveResizing( xp1, yp2, width, height);
                
                // update hanlder
                m_handle = BOTTOMRIGHT;
            }
            else if ((mouseY >= yp2) && (mouseX <= xp1))
            {   // rec collapsed and is expanding in opposite direction
                width  = xp1 - mouseX + 1;
                height = mouseY - yp2 + 1;
                
                moveResizing( mouseX, yp2, width, height);
                // update hanlder
                m_handle = BOTTOMLEFT;
            }
            break;
        case BOTTOMRIGHT:
//             printf ("expanding: m_handle = BOTTOMRIGHT\n");
            if ((mouseY >= yp1) && (mouseX >= xp1))
            {   // two cases:
                // 1) expansion in act
                // 2) rect is collapsing, in the worst case now it's only a point
                width  = mouseX - xp1 + 1;
                height = mouseY - yp1 + 1;
                
                setHeight( height);
                setWidth( width);
            }
            else if ((mouseY >= yp1) && (mouseX < xp1))
            {   
                width  = xp1 - mouseX + 1;
                height = mouseY - yp1 + 1;
                
                moveResizing( mouseX, yp1, width, height);
                // update hanlder
                m_handle = BOTTOMLEFT;
            }
            else if ((mouseY <= xp1) && (mouseX <= xp1))
            {   // rec collapsed and is expanding in opposite direction
                width  = xp1 - mouseX + 1;
                height = yp1 - mouseY + 1;
                
                moveResizing( mouseX, mouseY, width, height);
                // update hanlder
                m_handle = TOPLEFT;
            }
            else if ((mouseY <= yp1) && (mouseX >= xp1))
            {
                width = mouseX - xp1 +1;
                height = yp1 - mouseY + 1;
                
                moveResizing( xp1, mouseY, width, height);
                // update hanlder
                m_handle = TOPRIGHT;
            }
            break;
        case BOTTOMLEFT:
//             printf ("expanding: m_handle = BOTTOMLEFT\n");
            if ((mouseY >= yp1) && (mouseX <= xp2))
            {   // two cases:
                // 1) expansion in act
                // 2) rect is collapsing, in the worst case now it's only a point
                width  = xp2 - mouseX + 1;
                height = mouseY - yp1 + 1;
                
                moveResizing( mouseX, yp1, width, height);
            }
            else if ((mouseY >= yp1) && (mouseX > xp2))
            {
                width  = mouseX - xp2 + 1;
                height = mouseY - yp1 + 1;
                
                moveResizing( xp2, yp1, width, height);
                
                // update hanlder
                m_handle = BOTTOMRIGHT;
            }
            else if ((mouseY <= yp1) && (mouseX <= xp2))
            {   // rec collapsed and is expanding in opposite direction
                width  = xp2 - mouseX + 1;
                height = yp1 - mouseY + 1;
                
                moveResizing( mouseX, mouseY, width, height);
                // update hanlder
                m_handle = TOPLEFT;
            }
            else if ((mouseY <= yp2) && (mouseX >= xp2))
            {
                width = mouseX - xp2 + 1;
                height = yp1 - mouseY +1;
                
                moveResizing( xp2, mouseY, width, height);
                // update handler
                m_handle = TOPRIGHT;
            }
            break;
        case MIDDLELEFT:
            // we ignore mouseY component
            if (mouseX <= xp2)
            {   // two cases:
                // 1) expansion in act
                // 2) rect is collapsing, in the worst case now it's only a point
                width  = xp2 - mouseX + 1;
                height = getHeight();
                
                moveResizing( mouseX, yp1, width, height);
            }
            else
            {   // rec collapsed and is expanding in opposite direction
                width  = mouseX - xp2 + 1;
                height = getHeight();
                
                moveResizing( xp2, yp1, width, height);
                // update hanlder
                m_handle = MIDDLERIGHT;
            }
            break;
        case MIDDLERIGHT:
            // we ignore mouseY component
            if (mouseX >= xp1)
            {   // two cases:
                // 1) expansion in act
                // 2) rect is collapsing, in the worst case now it's only a point
                width  = mouseX - xp1 + 1;
                height = getHeight();
                
                moveResizing( xp1, yp1, width, height);
            }
            else
            {   // rec collapsed and is expanding in opposite direction
                width  = xp1 - mouseX + 1;
                height = getHeight();
                
                moveResizing( mouseX, yp1, width, height);
                
                // update hanlder
                m_handle = MIDDLELEFT;
            }
            break;
        case MIDDLETOP:
            // we ignore mouseX component
            if (mouseY <= yp2)
            {   // two cases:
                // 1) expansion in act
                // 2) rect is collapsing, in the worst case now it's only a point
                width  = getWidth();
                height = yp2 - mouseY +1;
                
                moveResizing( xp1, mouseY, width, height);
            }
            else
            {   // rec collapsed and is expanding in opposite direction
                width  = getWidth();
                height = mouseY - yp2 +1;
                
                moveResizing( xp1, yp2, width, height);
                // update hanlder
                m_handle = MIDDLEBOTTOM;
            }
            break;
        case MIDDLEBOTTOM:
            // we ignore mouseX component
            if (mouseY >= yp1)
            {   // two cases:
                // 1) expansion in act
                // 2) rect is collapsing, in the worst case now it's only a point
                width  = getWidth();
                height = mouseY - yp1 +1;
                
                moveResizing( xp1, yp1, width, height);
            }
            else
            {   // rec collapsed and is expanding in opposite direction
                width  = getWidth();
                height = yp1 - mouseY +1;
                
                moveResizing( xp1, mouseY, width, height);
                // update hanlder
                m_handle = MIDDLETOP;
            }
            break;
        case NONE:
            break;
    }
}


/*! the connection is added to the side defined by the user.
            \sa SIDE
*/
void DiagramBox::addConnection (ItemConnection* connection, SIDE side)
{
    if ((connection == NULL) || (!connection->isValid()))
        return;

    switch (side)
    {
        case DiagramBox::BOTTOM:
            m_connBottom.push_back(connection);
            updateBottomConnections();
            break;
        case DiagramBox::TOP:
            m_connTop.push_back (connection);
            updateTopConnections();
            break;
        case DiagramBox::LEFT:
            m_connLeft.push_back(connection);
            updateLeftConnections();
            break;
        case DiagramBox::RIGHT:
            m_connRight.push_back(connection);
            updateRightConnections();
            break;
        case DiagramBox::NOSIDE:
            qDebug ("ERROR: DiagramBox::addConnection -> DiagramBox::NOSIDE\n");
            
            // we've to remove this connection
            connection->deleteID( m_ID);
            break;
    }
}

/*!
    delete the connection and redraw all connections
*/
void DiagramBox::deleteConnection (int ID)
{
    QList<ItemConnection*>::iterator iter;
    QList<ItemConnection*>::iterator last;
    QList<ItemConnection*>* actualVector = NULL;
    
    DiagramBox::SIDE actualSide = getConnectionSide(ID);
    
    switch (actualSide)
    {
        case DiagramBox::TOP:
            iter = m_connTop.begin();
            last = m_connTop.end();
            actualVector = &m_connTop;
            break;
        case DiagramBox::BOTTOM:
            iter = m_connBottom.begin();
            last = m_connBottom.end();
            actualVector = &m_connBottom;
            break;
        case DiagramBox::LEFT:
            iter = m_connLeft.begin();
            last = m_connLeft.end();
            actualVector = &m_connLeft;
            break;
        case DiagramBox::RIGHT:
            iter = m_connRight.begin();
            last = m_connRight.end();
            actualVector = &m_connRight;
            break;
        case DiagramBox::NOSIDE:
            return;
    }

    for ( ; iter != last; iter++)
    {
        if ((*iter)->getID() == ID)
        {
            actualVector->erase (iter);
            updateConnection (actualSide);
            return;
        }
    }
}

void DiagramBox::updateConnections ()
{
    std::map<ItemConnection*, DiagramBox::SIDE> movements;
    std::map<ItemConnection*, DiagramBox::SIDE>::iterator iter;
    
    // update all top connections
    recalcSideConnections(DiagramBox::TOP, m_connTop, movements);
    
    // update all bottom connections
    recalcSideConnections(DiagramBox::BOTTOM, m_connBottom, movements);
    
    // update all left connections
    recalcSideConnections(DiagramBox::LEFT, m_connLeft, movements);
    
    // update all right connections
    recalcSideConnections(DiagramBox::RIGHT, m_connRight, movements);
    
    // apply changes
    
    // move items to top side
    for (iter = movements.begin(); iter != movements.end(); iter++)
    {
        switch (iter->second)
        {
            case DiagramBox::BOTTOM:
                m_connBottom.push_back(iter->first);
                break;
            case DiagramBox::TOP:
                m_connTop.push_back (iter->first);
                break;
            case DiagramBox::LEFT:
                m_connLeft.push_back(iter->first);
                break;
            case DiagramBox::RIGHT:
                m_connRight.push_back(iter->first);
                break;
            case DiagramBox::NOSIDE:
                qDebug ("ERROR: DiagramBox::updateConnections -> DiagramBox::NOSIDE\n");
                
                // we've to delete this box from the connection
                (iter->first)->deleteID (m_ID);
                break;
        }
    }
    
    // redraw all sides
    updateTopConnections();
    updateBottomConnections();
    updateLeftConnections();
    updateRightConnections();
}


/*!
    examines all connections contained in vector connections.
    These ItemConnection instances are in DiagramBox's side defined by side.
    When a ItemConnection is moved it's stored into the map movements and removed from its old vector.
    movements is a std::map with key ItemConnection and value RELPOS (the new connection side)
*/
void DiagramBox::recalcSideConnections (
        DiagramBox::SIDE side,
        QList<ItemConnection*>& connections,
        std::map<ItemConnection*, DiagramBox::SIDE>& movements
                                       )
{
    typedef QList<ItemConnection*>::iterator ITEMCONNITER;
    
    QList<ItemConnection*>::iterator iter;
    QList<ITEMCONNITER> toErase;
    DiagramBox *otherBox = NULL;
    
    for (iter = connections.begin(); iter != connections.end(); iter++)
    {
        DiagramBox::SIDE thisSide, otherSide;
        
        otherBox = (*iter)->getOtherBox(m_ID);
        
        if (otherBox == NULL)
        {
            // on the other side of the connection there's a point
            QPoint virtualPoint;
            (*iter)->getVirtualBox (virtualPoint);
            
            calcConnectionSide (this, (int*) &thisSide, virtualPoint);
        }
        else
        {
            //TODO: fix (int*) cast, find a better solution
            calcConnectionSides(this, otherBox, (int*) &thisSide, (int*) &otherSide);
        }
        
        // looking for movements
        if (thisSide != side)
        {
            // connection side has to be changed
            movements.insert (std::make_pair (*iter, thisSide));
            toErase.push_back(iter);
        }
        
        // update also the other box
        if (otherBox != NULL)
            otherBox->moveConnection(*iter, otherSide);
    }
    
    // delete all ItemConnection that have been moved to a new side
    for (int i = 0; i < toErase.size(); i++)
        connections.erase(toErase[i]);
}

void DiagramBox::moveConnection (ItemConnection* connection, DiagramBox::SIDE newSide)
{
    // check if this box has this connection
    if (!connection->containsID( m_ID))
        return;
    
    DiagramBox::SIDE actualSide = getConnectionSide (connection);
    
    if (actualSide == newSide)
        return;

    QList<ItemConnection*>::iterator iter;
    QList<ItemConnection*>::iterator last;
    QList<ItemConnection*>* actualVector = NULL;
    
    switch (actualSide)
    {
        case DiagramBox::TOP:
            iter = m_connTop.begin();
            last = m_connTop.end();
            actualVector = &m_connTop;
            break;
        case DiagramBox::BOTTOM:
            iter = m_connBottom.begin();
            last = m_connBottom.end();
            actualVector = &m_connBottom;
            break;
        case DiagramBox::LEFT:
            iter = m_connLeft.begin();
            last = m_connLeft.end();
            actualVector = &m_connLeft;
            break;
        case DiagramBox::RIGHT:
            iter = m_connRight.begin();
            last = m_connRight.end();
            actualVector = &m_connRight;
            break;
        case DiagramBox::NOSIDE:
            qDebug ("ERROR: DiagramBox::moveConnection -> DiagramBox::NOSIDE");
            
            //delete this box from the connection
            connection->deleteID( m_ID);
            break;
    }
    
    for (; iter != last; iter++)
    {
        if ((*iter)->getID() == connection->getID())
        {
            // remove from actual vector
            if (actualVector != NULL)
                actualVector->erase(iter);
            
            // add the connection to the new side
            switch (newSide)
            {
                case DiagramBox::TOP:
                    m_connTop.push_back (connection);
                    updateTopConnections();
                    updateConnection (actualSide);
                    break;
                case DiagramBox::BOTTOM:
                    m_connBottom.push_back (connection);
                    updateBottomConnections();
                    updateConnection (actualSide);
                    break;
                case DiagramBox::LEFT:
                    m_connLeft.push_back (connection);
                    updateLeftConnections();
                    updateConnection (actualSide);
                    break;
                case DiagramBox::RIGHT:
                    m_connRight.push_back (connection);
                    updateRightConnections();
                    updateConnection (actualSide);
                    break;
                case DiagramBox::NOSIDE:
                    qDebug ("ERROR: DiagramBox::moveConnection -> DiagramBox::NOSIDE");
                    //delete this box from the connection
                    connection->deleteID( m_ID);
                    break;
            }
        }
    }
}

void DiagramBox::updateConnection (DiagramBox::SIDE side)
{
    switch (side)
    {
        case DiagramBox::TOP:
            updateTopConnections();
            break;
        case DiagramBox::BOTTOM:
            updateBottomConnections();
            break;
        case DiagramBox::LEFT:
            updateLeftConnections();
            break;
        case DiagramBox::RIGHT:
            updateRightConnections();
            break;
        case DiagramBox::NOSIDE:
            break;
    }
}

void DiagramBox::updateTopConnections()
{
    int offset;
    QPoint anchor;
    
    LtBoxX qSortOrdering (m_ID);

    qSort (m_connTop.begin(), m_connTop.end(), qSortOrdering);

    offset = getWidth() / (m_connTop.size() + 1);

    getTopLeft(anchor);
    
    for (int i = 0; i < m_connTop.size(); i++)
    {
        anchor.setX(anchor.x() + offset);
        m_connTop[i]->updateConnection(m_ID, anchor);
    }
}

void DiagramBox::updateBottomConnections ()
{
    int offset;
    QPoint anchor;
    
    LtBoxX qSortOrdering (m_ID);
    qSort (m_connBottom.begin(), m_connBottom.end(), qSortOrdering);
    
    offset = getWidth() / (m_connBottom.size() + 1);
    
    getBottomLeft(anchor);
    
    for (int i = 0; i < m_connBottom.size(); i++)
    {
        anchor.setX(anchor.x() + offset);
        m_connBottom[i]->updateConnection(m_ID, anchor);
    }
}

void DiagramBox::updateLeftConnections ()
{
    int offset;
    QPoint anchor;

    LtBoxY qSortOrdering (m_ID);

    qSort (m_connLeft.begin(), m_connLeft.end(), qSortOrdering);
    
    offset = getHeight()/ (m_connLeft.size() + 1);
    getTopLeft(anchor);
    
    for (int i = 0; i < m_connLeft.size(); i++)
    {
        anchor.setY(anchor.y() + offset);
        m_connLeft[i]->updateConnection(m_ID, anchor);
    }
}

void DiagramBox::updateRightConnections ()
{
    int offset;
    QPoint anchor;

    LtBoxY qSortOrdering (m_ID);

    qSort (m_connRight.begin(), m_connRight.end(), qSortOrdering);
    
    offset = getHeight() / (m_connRight.size() + 1);
    getTopLeft(anchor);
    
    anchor.setX(anchor.x() + getWidth());
    
    for (int i = 0; i < m_connRight.size(); i++)
    {
        anchor.setY(anchor.y() + offset);
        m_connRight[i]->updateConnection(m_ID, anchor);
    }
}

DiagramBox::SIDE DiagramBox::getConnectionSide (ItemConnection* connection)
{
    QList<ItemConnection*>::iterator iter;
    QList<ItemConnection*>::iterator last;
    
    for (int side = 0; side < 4; side++)
    {
        switch ((DiagramBox::SIDE) side)
        {
            case DiagramBox::BOTTOM:
                iter = m_connBottom.begin();
                last  = m_connBottom.end();
                break;
            case DiagramBox::TOP:
                iter = m_connTop.begin();
                last  = m_connTop.end();
                break;
            case DiagramBox::LEFT:
                iter = m_connLeft.begin();
                last  = m_connLeft.end();
                break;
            case DiagramBox::RIGHT:
                iter = m_connRight.begin();
                last  = m_connRight.end();
                break;
            case DiagramBox::NOSIDE:
                break;
        }
        
        for (; iter != last; iter++)
        {
            if ((*iter)->getID() == connection->getID())
                return (DiagramBox::SIDE) side;
        }
    }
    
    return DiagramBox::NOSIDE;
}

DiagramBox::SIDE DiagramBox::getConnectionSide (int connectionID)
{
    QList<ItemConnection*>::iterator iter;
    QList<ItemConnection*>::iterator last;
    
    for (int side = 0; side < 4; side++)
    {
        switch ((DiagramBox::SIDE) side)
        {
            case DiagramBox::BOTTOM:
                iter = m_connBottom.begin();
                last  = m_connBottom.end();
                break;
            case DiagramBox::TOP:
                iter = m_connTop.begin();
                last  = m_connTop.end();
                break;
            case DiagramBox::LEFT:
                iter = m_connLeft.begin();
                last  = m_connLeft.end();
                break;
            case DiagramBox::RIGHT:
                iter = m_connRight.begin();
                last  = m_connRight.end();
                break;
            case DiagramBox::NOSIDE:
                break;
        }
        
        for (; iter != last; iter++)
        {
            if ((*iter)->getID() == connectionID)
                return (DiagramBox::SIDE) side;
        }
    }
    
    return DiagramBox::NOSIDE;
}

//////////////////////
//  Support classes //
//////////////////////

LtBoxX::LtBoxX (int ID)
    : m_refBoxID (ID)
{
}

/*!
    oveloaded operator used during qSort operations
 */
bool LtBoxX::operator () (ItemConnection* conn1, ItemConnection* conn2)
{
    DiagramBox* box1 = conn1->getOtherBox(m_refBoxID);
    DiagramBox* box2 = conn2->getOtherBox(m_refBoxID);
    
    QPoint middle1, middle2;
    
    if (box1 != NULL)
        box1->getMiddle(middle1);
    else
        conn1->getVirtualBox( middle1);
    
    if (box2 != NULL)
        box2->getMiddle(middle2);
    else
        conn2->getVirtualBox( middle2);
    
    if (middle1.x() <= middle2.x())
        return true;
    
    return false;
}

LtBoxY::LtBoxY (int ID)
    : m_refBoxID (ID)
{
}

/*!
    oveloaded operator used during qSort operations
*/
bool LtBoxY::operator () (ItemConnection* conn1, ItemConnection* conn2)
{
    DiagramBox* box1 = conn1->getOtherBox(m_refBoxID);
    DiagramBox* box2 = conn2->getOtherBox(m_refBoxID);
    
    QPoint middle1, middle2;
    
    if (box1 != NULL)
        box1->getMiddle(middle1);
    else
        conn1->getVirtualBox( middle1);
    
    if (box2 != NULL)
        box2->getMiddle(middle2);
    else
        conn2->getVirtualBox( middle2);
    
    if (middle1.y() <= middle2.y())
        return true;
    
    return false;
}


DiagramBox::RELPOS DiagramBox::getRelPos(DiagramBox* box){
    
    int xp1, xp2, yp1, yp2;

    QPoint topleft;
    getTopLeft(topleft);
    
    xp1=topleft.x();
    yp1=topleft.y();
    
    xp2=xp1 + getWidth();
    yp2=yp1 + getHeight();
    
    
    QPoint middleBox;
        
    box->getMiddle(middleBox);
        
    if (middleBox.y() < yp1)
    {
        if (middleBox.x() <= xp1)
            return LOWERRIGHT;
        else if (middleBox.x() >= xp2)
            return LOWERLEFT;
        else
            return LOWERMIDDLE;
    }
    else if ((middleBox.y() >= yp1) && (middleBox.y() <= yp2))
    {
        if (middleBox.x() <= xp1)
            return SAMERIGHT;
        else if (middleBox.x() >= yp2)
            return SAMELEFT;
        else return SAMELEFT;
    }
    else // middleBox.y() > yp2
    {
        if (middleBox.x() <= xp1)
            return UPPERRIGHT;
        else if (middleBox.x() >= xp2)
            return UPPERLEFT;
        else
            return UPPERMIDDLE;
    }
        
    //by default return UPPERMIDDLE
    qDebug ("ERROR: DiagramBox::getRelPos -> reached end of method, return a default value\n");
    return UPPERMIDDLE;
}

DiagramBox::RELPOS DiagramBox::getRelPos(QPoint& point )
{
    int xp1, xp2, yp1, yp2;

    QPoint topleft;
    getTopLeft(topleft);
    
    xp1 = topleft.x();
    yp1 = topleft.y();
    
    xp2 = xp1 + getWidth();
    yp2 = yp1 + getHeight();
    
    if (point.y() < yp1)
    {
	if (point.x() <= xp1)
	    return LOWERRIGHT;
	else if (point.x() >= xp2)
	    return LOWERLEFT;
	else
	    return LOWERMIDDLE;
    }
    else if ((point.y() >= yp1) && (point.y() <= yp2))
    {
	if (point.x() < xp1)
	    return SAMERIGHT;
	else if (point.x() > yp2)
	    return SAMELEFT;
        else return SAMELEFT;
    }
    else // point.y() > yp2
    {
	if (point.x() <= xp1)
	    return UPPERRIGHT;
	else if (point.x() >= xp2)
	    return UPPERLEFT;
	else
	    return UPPERMIDDLE;
    }
    
    //by default return UPPERMIDDLE
    qDebug ("ERROR: DiagramBox::getRelPos (QPoint)-> reached end of method, return a default value\n");
    return UPPERMIDDLE;
}

void DiagramBox::getConnectionVector (QList<ItemConnection*>& vect, DiagramBox::SIDE side)
{
    QList<ItemConnection*>::iterator iter;
    QList<ItemConnection*>::iterator last;
    
    switch (side)
    {
        case DiagramBox::BOTTOM:
            iter = m_connBottom.begin();
            last  = m_connBottom.end();
            break;
        case DiagramBox::TOP:
            iter = m_connTop.begin();
            last  = m_connTop.end();
            break;
        case DiagramBox::LEFT:
            iter = m_connLeft.begin();
            last  = m_connLeft.end();
            break;
        case DiagramBox::RIGHT:
            iter = m_connRight.begin();
            last  = m_connRight.end();
            break;
        case DiagramBox::NOSIDE:
            iter = m_connBottom.end();
            last = m_connBottom.end();
            break;
    }
    
    for ( ; iter != last; iter++)
        vect.push_back (*iter);
}

bool DiagramBox::isContained (QRect& selection)
{
    QPoint topLeft, bottomLeft, topRight, bottomRight;
    
    getTopLeft (topLeft);
    getBottomLeft (bottomLeft);
    
    topRight.setX (topLeft.x() + getWidth());
    topRight.setY (topLeft.y());
    bottomRight.setX(topRight.x());
    bottomRight.setY (bottomLeft.y());
    
    if (isPointContained( topLeft, selection) && isPointContained( topRight, selection) && isPointContained( bottomLeft, selection) && isPointContained( bottomRight, selection))
        return true;
    else
        return false;
}

PosAndSize* DiagramBox::getPosAndSize()
{
    QPoint topLeft;
    getTopLeft(topLeft);
    
    BoxPosAndSize* posAndSize = new BoxPosAndSize (topLeft, getHeight(), getWidth());
    
    return posAndSize;
}

void DiagramBox::setPosAndSize(PosAndSize* posAndSize)
{
    BoxPosAndSize *boxPosAndSize;
    
    boxPosAndSize = dynamic_cast<BoxPosAndSize*> (posAndSize);
    
    if (boxPosAndSize != 0)
    {
        QPoint topLeft = boxPosAndSize->getTopLeft();
        int height = boxPosAndSize->getHeight();
        int width = boxPosAndSize->getWidth();
        
        setTopLeft (topLeft);
        setHeight (height);
        setWidth (width);
    }
}

void DiagramBox::getTopLeft (QPoint& point)
{
    QRect rect = getSurroundingRect();
    
    point = rect.topLeft();
}

void DiagramBox::getBottomLeft (QPoint& point)
{
    QRect rect = getSurroundingRect();
    
    point = rect.bottomLeft();
}

int DiagramBox::getHeight()
{
    QRect rect = getSurroundingRect();
    
    return rect.height();
}

int DiagramBox::getWidth()
{
    QRect rect = getSurroundingRect();
    
    return rect.width();
}

void DiagramBox::getMiddle (QPoint& middle)
{
    int xp1, xp2, yp1, yp2;
    
    QRect rect = getSurroundingRect();
    
    rect.getCoords ( &xp1, &yp1, &xp2, &yp2 );
    
    middle.setX (xp1 + ((xp2 - xp1)/2));
    middle.setY (yp1 + ((yp2 - yp1)/2));
}

void DiagramBox::save (QDomDocument *document, QDomElement *parent)
{
    DiagramItem::save(document, parent);
    
    // save position
    QDomElement position = document->createElement ("Position");
    parent->appendChild(position);
    
    QDomElement point = document->createElement ("TopLeft");
    position.appendChild(point);
    
    QPoint topLeft;
    
    // save topLeft point
    getTopLeft (topLeft);
    
    createChildNode( document, &point, QString ("x"), topLeft.x());
    createChildNode( document, &point, QString ("y"), topLeft.y());
    
    // save height & width
    createChildNode( document, parent, QString ("Height"), getHeight());
    createChildNode( document, parent, QString ("Width"), getWidth());
    
    // save connections
    QList<ItemConnection*>::iterator connIter;
    
    QDomElement connections = document->createElement ("ConnectionsID");
    parent->appendChild(connections);
    
    // save Top connections
    QDomElement connTop = document->createElement ("Top");
    connections.appendChild(connTop);
    
    for (connIter = m_connTop.begin(); connIter != m_connTop.end(); connIter++)
        createChildNode( document, &connTop, QString ("ConnectionID"), (*connIter)->getID());
    
    // save bottom connections
    QDomElement connBottom = document->createElement ("Bottom");
    connections.appendChild(connBottom);
    
    for (connIter = m_connBottom.begin(); connIter != m_connBottom.end(); connIter++)
        createChildNode( document, &connBottom, QString ("ConnectionID"), (*connIter)->getID());
    
    // save left connections
    QDomElement connLeft = document->createElement ("Left");
    connections.appendChild(connLeft);
    
    for (connIter = m_connLeft.begin(); connIter != m_connLeft.end(); connIter++)
        createChildNode( document, &connLeft, QString ("ConnectionID"), (*connIter)->getID());
    
    // save right connections
    QDomElement connRight = document->createElement ("Right");
    connections.appendChild(connRight);
    
    for (connIter = m_connRight.begin(); connIter != m_connRight.end(); connIter++)
        createChildNode( document, &connRight, QString ("ConnectionID"), (*connIter)->getID());
}

bool DiagramBox::load (QDomNode* item)
{
    int height, width;
    QPoint topLeft;
    bool check;
    QString text;
    
    if (!DiagramItem::load (item))
        return false;
    
    // restore height
    height = getNodeNumValue ( *item, QString ("Height"), &check);

    if (!check)
        return false;
    
    // restore width
    width = getNodeNumValue ( *item, QString ("Width"), &check);

    if (!check)
        return false;
    
    // restore position
    QDomNode position = item->firstChildElement("Position");
    
    if (position.isNull())
        return false;
    
    topLeft = getNodePoint (position, QString ("TopLeft"), &check);
          
    if (!check)
        return false;
    
    moveResizing (topLeft.x(), topLeft.y(), width, height);
    
    return true;
}
