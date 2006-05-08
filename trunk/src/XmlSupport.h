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

#ifndef XMLSUPPORT_H_
#define XMLSUPPORT_H_

class QDomDocument;
class QDomElement;
class QDomNode;
class QPoint;
class QString;

#include <QList>

//! create a new xml node, with a QDomText associated
void createChildNode (QDomDocument* document, QDomElement* parent, QString tagName, QString value);

//! create a new xml node, with a QDomText associated
void createChildNode (QDomDocument* document, QDomElement* parent, QString tagName, int value);

//! return the value associated to the corresponding tag name converted to int, return -1 if nothing is found or some error occurred
int getNodeNumValue (QDomNode node, QString tagName, bool* check);

//! return the value associated to the corresponding tag name, return an empty string if nothing is found
QString getNodeValue (QDomNode node, QString tagName);

//! return the coordinates of a point stored in a QDomNode
QPoint getNodePoint (QDomNode node, QString tagName, bool* check);

//! return the type of item (SystemBox, DiagramLine,...) defined by the node
int nodeItemType (QDomNode item);

//! populate a vector with all connections ID contained by a side
void readConnectionID (QDomElement connections, QString side, QList<int>& connectionsID);

#endif // XMLSUPPORT_H_
