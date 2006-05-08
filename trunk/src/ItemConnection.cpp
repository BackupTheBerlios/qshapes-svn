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

#include "ItemConnection.h"
#include "DiagramBox.h"
#include "DiagramItemCollector.h"
#include "DiagramLine.h"
#include "XmlSupport.h"

#include <QPoint>
#include <QDomDocument>

ItemConnection::ItemConnection()
{
    m_startBox  = NULL;
    m_endBox    = NULL;
    m_line      = NULL;
}

ItemConnection::ItemConnection(DiagramBox* startBox, DiagramBox* endBox, DiagramLine* line)
    :   m_startBox (startBox),
        m_endBox (endBox),
        m_line (line)
{
}

ItemConnection::~ItemConnection()
{
    clean();
}

bool ItemConnection::operator== (ItemConnection* item2)
{
    if (m_ID == item2->getID())
        return true;
    
    return false;
}

void ItemConnection::clean()
{
    if (m_startBox != NULL)
        m_startBox->deleteConnection (m_ID);
    
    if (m_endBox != NULL)
        m_endBox->deleteConnection (m_ID);
}

void ItemConnection::makeConnection()
{
    DiagramBox::SIDE sideBegin, sideEnd;
    
    if ((m_startBox != NULL) && (m_endBox != NULL))
    {
        // (int*) cast aren't good, by now they'll work. TODO: fix them
        calcConnectionSides(m_startBox, m_endBox, (int*) &sideBegin, (int*) &sideEnd);
    }
    else if (m_startBox == NULL)
    {
        QPoint lineEdge = m_line->startPoint();
        calcConnectionSide(m_endBox, (int*) &sideEnd, lineEdge);
    }
    else if (m_endBox == NULL)
    {
        QPoint lineEdge = m_line->endPoint();
        calcConnectionSide(m_startBox, (int*) &sideBegin, lineEdge);
    }
    
    if (m_startBox != NULL)
        m_startBox->addConnection( this, sideBegin);
    
    if (m_endBox != NULL)
        m_endBox->addConnection( this, sideEnd);
}

DiagramBox* ItemConnection::getOtherBox (int ID)
{
    if ((m_startBox != NULL) && (m_startBox->getID() == ID))
        return m_endBox;
    else if ((m_endBox != NULL) && (m_endBox->getID() == ID))
        return m_startBox;
    else
        return NULL;
}

int ItemConnection::getLineID()
{
    return m_line->getID();
}

void ItemConnection::setLineMoving (bool state)
{
    if (m_line != NULL)
        m_line->setMoving( state);
}

/*!
    this method is used when one of the connection's DiagramBox is NULL
*/
void ItemConnection::getVirtualBox(QPoint& point)
{
    if (m_startBox == NULL)
        point = m_line->startPoint();
    else if (m_endBox == NULL)
        point = m_line->endPoint();
}

DiagramItem* ItemConnection::getByID(int ID)
{
    if ((m_startBox != NULL ) && (m_startBox->getID() == ID))
        return m_startBox;
    else if ((m_endBox != NULL) && (m_endBox->getID() == ID))
        return m_endBox;
    else if ((m_line != NULL) && (m_line->getID() == ID))
        return m_line;
    else
        return NULL;
}

void ItemConnection::updateConnection (int boxID, QPoint newPos)
{
    QPoint point;
    
    if ((m_startBox != NULL) && (m_startBox->getID() == boxID))
    {
        point = m_line->endPoint();
        m_line->setPoints (newPos.x(), newPos.y(), point.x(), point.y());
    }
    else if ((m_endBox != NULL) && (m_endBox->getID() == boxID))
    {
        point = m_line->startPoint();
        m_line->setPoints (point.x(), point.y(), newPos.x(), newPos.y());
    }
}

bool ItemConnection::containsID( int itemID)
{
    if ((m_endBox != NULL) && (m_endBox->getID() == itemID))
        return true;
    else if ((m_startBox != NULL)&& (m_startBox->getID() == itemID))
        return true;
    else if ((m_line != NULL) && (m_line->getID() == itemID))
        return true;
    
    return false;
}

void ItemConnection::deleteID (int ID)
{
    if ((m_startBox != NULL) && (m_startBox->getID() == ID))
         m_startBox = NULL;
    else if ((m_endBox != NULL) && (m_endBox->getID() == ID))
         m_endBox = NULL;
}

/*!
    if startBox is true m_startBox box is changed with newBox, otherwise is changed m_endBox
*/
void ItemConnection::changeBox (bool startBox, DiagramBox* newBox)
{
    DiagramBox* oldBox;
    DiagramBox::SIDE sideStart, sideEnd;
    
    if (startBox)
    {
        oldBox = m_startBox;
        m_startBox = newBox;
    }
    else
    {
        oldBox = m_endBox;
        m_endBox = newBox;
    }
    
    if (oldBox != NULL)
        // update oldBox connections
        oldBox->deleteConnection( m_ID);

    // update connections
    if ((m_startBox != NULL) && (m_endBox != NULL))
        calcConnectionSides(m_startBox, m_endBox, (int*) &sideStart, (int*) &sideEnd);
    else if (m_startBox == NULL)
    {
        QPoint virtualBox;
        getVirtualBox (virtualBox);
        
        calcConnectionSide(m_endBox, (int*) &sideEnd, virtualBox);
    }
    else if (m_endBox == NULL)
    {
        QPoint virtualBox;
        getVirtualBox (virtualBox);
        
        calcConnectionSide(m_startBox, (int*) &sideStart, virtualBox);
    }
    
    if (startBox)
    {
        if (m_startBox != NULL)
            m_startBox->addConnection ( this, sideStart);
        
        if (m_endBox != NULL)
            m_endBox->moveConnection ( this, sideEnd);
    }
    else
    {   // we've changed the endBox
        if (m_startBox != NULL)
            m_startBox->moveConnection( this, sideStart);
        
        if (m_endBox != NULL)
            m_endBox->addConnection( this, sideEnd);
    }
}

/*!
    if startBox is true newBox becames m_startBox, otherwise newBox becames m_endBox
 */
void ItemConnection::addBox (bool startBox, DiagramBox* newBox)
{
    DiagramBox::SIDE sideStart, sideEnd;
    
    if (startBox)
        m_startBox = newBox;
    else
        m_endBox = newBox;
    
    // m_startBox & m_endBox are always != NULL
    calcConnectionSides(m_startBox, m_endBox, (int*) &sideStart, (int*) &sideEnd);
    
    if (startBox)
    {
        // add the connection to the newBox
        m_startBox->addConnection(this, sideStart);
        m_endBox->moveConnection( this, sideEnd);
    }
    else
    {
        // add the connection to the newBox
        m_endBox->addConnection(this, sideEnd);
        m_startBox->moveConnection( this, sideStart);
    }
}

/*!
    A connection isn't valid if m_startBox and m_endBox are NULL, or m_line is NULL
 */
bool ItemConnection::isValid()
{
    if ((m_startBox == NULL) && (m_endBox == NULL))
        return false;
    else if (m_line == NULL)
        return false;
    
    return true;
}

bool ItemConnection::isStartBox (int ID)
{
    if ((m_startBox != NULL) && (m_startBox->getID() == ID))
        return true;
    
    return false;
}

/*!
    Each report line is composed in this way: Start box's name - connection line's  name -end box's name
 */
QString ItemConnection::generateReport ()
{
    if ((m_line == NULL) || (m_startBox == NULL))
        return QString(); // we don't save anything, connection isn't valid
    
    QString report;
    
    report += m_startBox->getText();
    report += " - ";
    
    report += m_line->getText();
    report += " - ";
    
    if (m_endBox == NULL)
        report += " NONE";
    else
        report += m_endBox->getText();
    
    report += "\n";
    
    return report;
}

void ItemConnection::save (QDomDocument *document, QDomElement *parent)
{
    if (m_line == NULL)
        return; // we don't save anything, connection isn't valid
    
    // save ID
    createChildNode( document, parent, QString ("ID"), m_ID);

    // save startBox ID
    int startBoxID = -1;
    
    if (m_startBox != NULL)
        startBoxID = m_startBox->getID();
        
    createChildNode( document, parent, QString ("StartBoxID"), startBoxID);
    
    // save endBox ID
    int endBoxID = -1;
    
    if (m_endBox != NULL)
        endBoxID = m_endBox->getID();
        
    createChildNode( document, parent, QString ("EndBoxID"), endBoxID);
    
    // save line ID
    createChildNode( document, parent, QString ("LineID"), m_line->getID());
}

bool ItemConnection::load (QDomNode connection, DiagramItemCollector* itemCollector)
{
    int value;
    bool check;
    DiagramBox* box;
    DiagramLine* line;
    
    // restore ID
    value = getNodeNumValue ( connection, QString ("ID"), &check);
    
    if (!check)
        return false;
    
    m_ID = value;
    
    // restore start box ID
    value = getNodeNumValue ( connection, QString ("StartBoxID"), &check);
    
    if (!check)
        return false;
    
    if (value != -1)
    {
        box = dynamic_cast<DiagramBox*>(itemCollector->getItemByID( value));
        if (box != 0)
            m_startBox = box;
        else
            return false;
    }
    
    // restore end box ID
    value = getNodeNumValue ( connection, QString ("EndBoxID"), &check);
    
    if (!check)
        return false;
    
    if (value != -1)
    {
        box = dynamic_cast<DiagramBox*>(itemCollector->getItemByID( value));
        if (box != 0)
            m_endBox = box;
        else
            return false;
    }

    
    // restore line ID
    value = getNodeNumValue ( connection, QString ("LineID"), &check);
    
    if (!check)
        return false;
    
    if (value != -1)
    {
        line = dynamic_cast<DiagramLine*>(itemCollector->getItemByID( value));
        if (box != 0)
            m_line = line;
        else
            return false;
    }
    else
        return false;
    
    return true;
}

//////////////////////////////////////////////
// Support functions for connections        //
//////////////////////////////////////////////

void calcConnectionSides(DiagramBox* box1, DiagramBox* box2, int* side1, int* side2)
{
    if ((box1 == NULL) || (box2 == NULL))
        return;
    
    switch (box1->getRelPos(box2))
    {
        case DiagramBox::UPPERLEFT:
            *side1 = DiagramBox::BOTTOM;
            *side2 = DiagramBox::LEFT;
            break;
        case DiagramBox::UPPERMIDDLE:
            *side1 = DiagramBox::BOTTOM;
            *side2 = DiagramBox::TOP;
            break;
        case DiagramBox::UPPERRIGHT:
            *side1 = DiagramBox::BOTTOM;
            *side2 = DiagramBox::RIGHT;
            break;
        case DiagramBox::SAMELEFT:
            *side1 = DiagramBox::RIGHT;
            *side2 = DiagramBox::LEFT;
            break;
        case DiagramBox::SAMERIGHT:
            *side1 = DiagramBox::LEFT;
            *side2 = DiagramBox::RIGHT;
            break;
        case DiagramBox::LOWERLEFT:
            *side1 = DiagramBox::TOP;
            *side2 = DiagramBox::LEFT;
            break;
        case DiagramBox::LOWERMIDDLE:
            *side1 = DiagramBox::TOP;
            *side2 = DiagramBox::BOTTOM;
            break;
        case DiagramBox::LOWERRIGHT:
            *side1 = DiagramBox::TOP;
            *side2 = DiagramBox::RIGHT;
            break;
        default:
            qDebug ("no value!!!\n");
            break;
    }
}

void calcConnectionSide (DiagramBox* box, int* side, QPoint& lineEdge)
{
    if (box == NULL)
        return;
    
    switch (box->getRelPos(lineEdge))
    {
        case DiagramBox::UPPERLEFT:
            *side = DiagramBox::BOTTOM;
            break;
        case DiagramBox::UPPERMIDDLE:
            *side = DiagramBox::BOTTOM;
            break;
        case DiagramBox::UPPERRIGHT:
            *side = DiagramBox::BOTTOM;
            break;
        case DiagramBox::SAMELEFT:
            *side = DiagramBox::RIGHT;
            break;
        case DiagramBox::SAMERIGHT:
            *side = DiagramBox::LEFT;
            break;
        case DiagramBox::LOWERLEFT:
            *side = DiagramBox::TOP;
            break;
        case DiagramBox::LOWERMIDDLE:
            *side = DiagramBox::TOP;
            break;
        case DiagramBox::LOWERRIGHT:
            *side = DiagramBox::TOP;
            break;
        default:
            qDebug ("no value!!!\n");
            break;
    }
}
