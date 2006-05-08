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

#ifndef ITEMCONNECTION_H_
#define ITEMCONNECTION_H_

/*! \class ItemConnection 
    \brief A connection between two diagram items

    This class contains all the informations regarding a connection between two items.
*/

class DiagramBox;
class DiagramLine;
class DiagramItem;
class DiagramItemCollector;
class QDomDocument;
class QDomElement;
class QDomNode;
class QPoint;

#include <QString>

class ItemConnection
{
    public:
        //! Class constructor
        ItemConnection();
        
        //! Class constructor
        ItemConnection(DiagramBox* startBox, DiagramBox* endBox, DiagramLine* line);
        
        //! Class destructor
        virtual ~ItemConnection();
        
        //! clean-up code
        void clean();
        
        //! set the connection ID
        void setID (int ID) {m_ID = ID;}
        
        //! return the connection ID
        int getID () { return m_ID;}
        
        //! set NULL the box defined by ID
        void deleteID (int ID);
        
        //! change a connection box with a new one
        void changeBox (bool startBox, DiagramBox* newBox);
        
        //! add a new box
        void addBox (bool startBox, DiagramBox* newBox);
        
        //! return true if one of the connection member has ID equal to itemID
        bool containsID(int itemID);

        //! return true if connection is valid
        bool isValid();
        
        //! return true if the box defined by ID is the startBox, false otherwise
        bool isStartBox (int ID);
        
        //! set the first item of the connection
        void setStartBox(DiagramBox* startBox) {m_startBox = startBox;}
        
        //! return the first item of the connection
        DiagramBox* getStartBox() { return m_startBox;}
        
        //! set the last item of the connection
        void setEndBox(DiagramBox* endBox) {m_endBox = endBox;}
        
        //! return the last item of the connection
        DiagramBox* getEndBox() { return m_endBox;}
        
        //! set the line that joins connection's items
        void setLine(DiagramLine* line) {m_line = line;}
        
        //! updates line moving state
        void setLineMoving (bool state);
        
        //! setup the connection for the first time
        void makeConnection();
        
        //! return the other box present in the connection (opposed to the box defined by ID)
        DiagramBox* getOtherBox (int ID);
        
        //! return the ID of the line present in the connection
        int getLineID ();
        
        //! return the line present in the connection
        DiagramLine* getLine () { return m_line; }
        
        //! return the box with the specified ID
        DiagramItem* getByID(int ID);
        
        //! return the edge of the connection line opposed to the unique non-null DiagramBox
        void getVirtualBox(QPoint& point);
        
        //! updated line's point starting from a box
        void updateConnection (int boxID, QPoint newPos);
        
        //! add connecion details to an xml document
        void save (QDomDocument *document, QDomElement *parent);
        
        //! restore a connection from an xml document
        bool load (QDomNode connection, DiagramItemCollector* itemCollector);
        
        //! generate a text report
        QString generateReport();
        
        bool operator== (ItemConnection* item2);
        
    private:
        int m_ID; //!< connection's ID
        DiagramBox* m_startBox; //!< connection's start item
        DiagramBox* m_endBox; //!< connection's end item
        DiagramLine* m_line; //!< connection's line
};


///////////////////////////////////
// Support classes and functions //
///////////////////////////////////

/*!
    takes two DiagramBox and put into box1 and box2 the appropriate connetion side
 */
void calcConnectionSides(DiagramBox* box1, DiagramBox* box2, int* side1, int* side2);

/*!
    takes a DiagramBox and a QPoint, calculates the right connection side
*/
void calcConnectionSide (DiagramBox* box, int* side, QPoint& lineEdge);

#endif /*ITEMCONNECTION_H_*/
