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

#ifndef DIAGRAMBOX_H_
#define DIAGRAMBOX_H_

#include "ItemConnection.h"
#include "DiagramItem.h"

#include <QList>
#include <map>

class QPainter;
class QPoint;
class QPolygon;
class QRect;

/*! \class LtBoxX
    \brief class used for x-axis sorting operations
    
    This class is used for sorting the items connected to the one defined by m_refBoxID . The comparison is relative to the x-axis, starting from the left to right.
*/

class LtBoxX
{
    // class used during sort operations, return true if ref
    private:
        int m_refBoxID; //!< the reference box's ID

    public:
        
        //! constructor
        LtBoxX () {};
        
        //! destructor
        virtual ~LtBoxX () {};
        
        //! constructor
        LtBoxX (int ID);
        
        //! overload of the () operator
        bool operator () (ItemConnection* conn1, ItemConnection* conn2);
};

/*! \class LtBoxY
    \brief class used for y-axis sorting operations
    
    This class is used for sorting the items connected to the one defined by m_refBoxID . The comparison is relative to the y-axis, starting from the top to bottom.
 */

class LtBoxY
{
    // class used during sort operations, return true if ref
    private:
        int m_refBoxID; //!< the reference box's ID

    public:
        
        //! constructor
        LtBoxY () {};
        
        //! destructor
        virtual ~LtBoxY () {};
        
        //! constructor
        LtBoxY (int ID);

        //! overload of the () operator
        bool operator () (ItemConnection* conn1, ItemConnection* conn2);
};



/*! \class DiagramBox
    \brief the most generic diagram box
    
    This class defines the common methods of all diagram boxes.
 */

class DiagramBox : public DiagramItem
{
    public:
        
        //! constructor
        DiagramBox();
        
        //! constructor
        DiagramBox(DiagramBox* diagramBox);
        
        //! destructor
        virtual ~DiagramBox();
        
        //! enum used to define the box expansion handles
        enum HANDLE {   TOPLEFT,     //!< top left corner
                        TOPRIGHT,    //!< top right corner
                        BOTTOMRIGHT, //!< bottom right corner
                        BOTTOMLEFT,  //!< bottom left corner
                        MIDDLELEFT,  //!< middle point of the left side
                        MIDDLERIGHT, //!< middle point of the right side
                        MIDDLETOP,   //!< middle point of the upper side
                        MIDDLEBOTTOM,//!< middle point of the bottom side
                        NONE //!< no hanlde
                    };
        
        //! enum used to define the box sides
        enum SIDE   {   TOP     = 0, //!< top side
                        BOTTOM  = 1, //!< bottom side
                        LEFT    = 2, //!< left side
                        RIGHT   = 3, //!< right side
                        NOSIDE  = 4  //!< no side
                    };
                    
        //! enum used to define the box position, relative to another one
        /*!
            Soppose to have to DiagramBox: A and B.
            Look at this diagram:
<pre>
        |       |         
   1   |   2  |    3    
        |       |         
--------------------------
        |       |         
   4   |   A  |    5    
        |       |         
--------------------------
        |       |         
   6   |   7  |    8    
        |       |         
</pre>
        with this enum we define B's position, using A as reference.
        */
        enum RELPOS {   UPPERLEFT,      //!< B is in 1
                        UPPERMIDDLE,    //!< B is in 2
                        UPPERRIGHT,     //!< B is in 3
                        SAMELEFT,       //!< B is in 4
                        SAMERIGHT,      //!< B is in 5
                        LOWERLEFT,      //!< B is in 6
                        LOWERMIDDLE,    //!< B is in 7
                        LOWERRIGHT      //!< B is in 8
                    };
        
        //! method used to find the relative position of one box, using another one as reference
        DiagramBox::RELPOS getRelPos(DiagramBox* box);
        
        //! method used to find the relative position of one box, using a point as reference
	DiagramBox::RELPOS getRelPos(QPoint& point);
        
        //! return the middle point of the box
        void getMiddle  (QPoint& middle);
        
        //! return the top left point of the box
        void getTopLeft (QPoint& point);
        
        //! return the bottom left point of the box
        void getBottomLeft (QPoint& point);
        
        //! return the height of the box
        int  getHeight ();
        
        //! return the width of the box
        int  getWidth ();
        
        //! return the rectangle surrounding item
        virtual QRect getSurroundingRect() const = 0;
        
        //! set box's top left point
        virtual void setTopLeft (QPoint& topLeft) = 0;

        //! set box's height
        virtual void setHeight (int height) = 0;
        
        //! set box's width
        virtual void setWidth (int width) = 0;
        
        //! move top left point to (mouseX, mouseY), and changes the surrounding rectangle's height and width
        virtual void moveResizing (int mouseX, int mouseY, int width, int height) = 0;
        
        //! return the points surrounding the box
        QPolygon areaPoints() const;

        virtual void changePen (QPen pen) = 0;
        
        //! change item's brush
        virtual void changeBrush (QBrush brush) = 0;
        
        // expand stuff
        
        //! expands the box
        /*! expands the box according to its current handles.
            \sa HANDLE
        */
        void expand (int mouseX, int mouseY);
        
        //! check  if point (mouseX, mouseY) is one of the hanlde points of the DiagramBox
        bool checkExpand (int mouseX, int mouseY);
        
        //! return true if the box is expanding, otherwise return false
        bool isExpanding();
        
        //! set item's expanding state to false
        void resetExpand();
        
        //! return true if item is inside the rectangle defined by selection, false otherwise
        bool isContained (QRect& selection);
        
        void setMoving(bool state);
        
        // connection stuff
        
        //! add a new connection to items
        void addConnection (ItemConnection* connection, SIDE side);
        
        //! remove a connection
        void deleteConnection (int ID);
        
        //! update all items connections
        void updateConnections ();
        
        //! find again the correct side for each connection
        void recalcSideConnections (
            DiagramBox::SIDE side,
            QList<ItemConnection*>& connections,
            std::map<ItemConnection*,DiagramBox::SIDE>& movements);

        //! moves a connection to a new side
        void moveConnection (ItemConnection* connection, DiagramBox::SIDE newSide);
        
        //! return the side containing the connection
        DiagramBox::SIDE getConnectionSide (ItemConnection* connection);
        
        //! return the side containing the connection defined by connectionID
        DiagramBox::SIDE getConnectionSide (int connectionID);
        
        //! return the connection vector defined by side
        void getConnectionVector (QList<ItemConnection*>& vect, DiagramBox::SIDE side);
        
        //! return a new instance of the itself
        virtual DiagramItem* clone () = 0;
        
        //! set the text inside the box
        virtual void setText(QString newText) = 0;
        
        virtual QString getType() = 0;
        
        //! item's tooltip
        virtual QString getToolTip();
        
        //! add item details to an xml document
        virtual void save (QDomDocument *document, QDomElement *parent);
        
        //! restore item details from a QDomNode
        virtual bool load (QDomNode* item);
        
        PosAndSize* getPosAndSize();
        void setPosAndSize(PosAndSize* posAndSize);
        
    protected:
        //! draw text inside item and, if isActive is true, draw also the active hanldes
        void finishDraw (QPainter &painter, bool isActive);
        
        //! redraw all upper side connections
        virtual void updateTopConnections ();
        
        //! redraw all bottom side connections
        virtual void updateBottomConnections ();
        
        //! redraw all left side connections
        virtual void updateLeftConnections ();
        
        //! redraw all right side connections
        virtual void updateRightConnections ();

        //! redraw all connections defined by side
        void updateConnection (DiagramBox::SIDE side);

        HANDLE  m_handle; //!< variable containing the handle used for item's expansion \sa HANDLE
        
        QList<ItemConnection*> m_connTop; //!< vector of all the connection of the upper side
        QList<ItemConnection*> m_connBottom; //!< vector of all the connection of the lower side
        QList<ItemConnection*> m_connRight; //!< vector of all the connection of the right side
        QList<ItemConnection*> m_connLeft; //!< vector of all the connection of the left side
};

#endif /*DIAGRAMBOX_H_*/
