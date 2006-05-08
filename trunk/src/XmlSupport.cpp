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

#include "XmlSupport.h"

#include "DiagramLine.h"
#include "ProductBox.h"
#include "SystemBox.h"
#include "SuperSystem.h"

#include <QDomDocument>
#include <QDomElement>
#include <QDomText>
#include <QPoint>
#include <QString>

void createChildNode (QDomDocument* document, QDomElement* parent, QString tagName, QString value)
{
    QDomElement child = document->createElement(tagName);
    QDomText text = document->createTextNode(value);
    
    child.appendChild(text);
    parent->appendChild(child);
}

void createChildNode (QDomDocument* document, QDomElement* parent, QString tagName, int value)
{
    QString string;
    string.setNum (value);
    
    createChildNode (document, parent, tagName, string);
}

QString getNodeValue (QDomNode node, QString tagName)
{
    QDomElement element = node.firstChildElement(tagName);
    
    if (element.isNull())
        return QString();

    return element.text();
}

int getNodeNumValue (QDomNode node, QString tagName, bool* check)
{
    QDomElement element = node.firstChildElement(tagName);
    
    if (element.isNull())
    {
        *check = false;
        return -1;
    }

    QString text = element.text();
    
    int value = text.toInt (check);
            
    if (*check)
        return value;
    else
        return -1;
}

QPoint getNodePoint (QDomNode node, QString tagName, bool* check)
{
    int x, y;
    QPoint point (0,0);
    
    QDomElement nodePoint = node.firstChildElement(tagName);
    
    if (nodePoint.isNull())
    {
        *check = false;
        return point;
    }

    x = getNodeNumValue (nodePoint, QString("x"), check);
    
    if (!check)
        return point;
    
    y = getNodeNumValue (nodePoint, QString("y"), check);
    
    if (!check)
        return point;
    
    point.setX(x);
    point.setY(y);
    
    return point;
}

void readConnectionID (QDomElement connections, QString side, QList<int>& connectionsID)
{
    connectionsID.clear();
    
    QDomElement element = connections.firstChildElement(side);
        
    if (!element.isNull())
    {
        QDomElement connection;
            
        connection = element.firstChildElement("ConnectionID");
            
        while (!connection.isNull())
        {
            int connectionID;
            bool check;
            
            connectionID = connection.text().toUInt(&check);
            
            if (check)
                connectionsID.push_back(connectionID);
                
            connection = connection.nextSiblingElement("ConnectionID");
        }
    }
}

int nodeItemType (QDomNode item)
{
    QDomElement typeElement = item.firstChildElement(QString ("Type"));
    
    if (typeElement.isNull())
    {
        qDebug ("no type found\n");
        return -1;
    }
    else
    {
        QString type = typeElement.text();
        
        if (type == QString("DiagramLine"))
            return DiagramLine::RTTI;
        else if (type == QString("SystemBox"))
            return SystemBox::RTTI;
        else if (type == QString("SuperSystem"))
            return SuperSystem::RTTI;
        else if (type == QString("ProductBox"))
                return ProductBox::RTTI;
        else
        {
            qDebug ("unknown type: |%s|\n", type.toStdString().c_str());
            return -1;
        }
    }
}
