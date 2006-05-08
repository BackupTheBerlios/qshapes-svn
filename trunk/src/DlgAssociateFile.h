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

#ifndef DLGASSOCIATEFILE_H
#define DLGASSOCIATEFILE_H

#include "ui_FormAssociateFile.h"

class QString;

/*! \class DlgAssociateFile
    \brief Dialog for external file association
    \author Flavio Castelli

    The dialog used to associate a new or already existing file to a SuperSystem box. \sa SuperSystem
*/

class DlgAssociateFile : public QDialog, private Ui::DlgAssociateFile
{
    Q_OBJECT

    public:
        DlgAssociateFile(QString* filename, bool* newFile, QWidget *parent = 0);

        virtual ~DlgAssociateFile();
        
    private slots:
        void radioAssociateFile ();
        void radioNewFile ();
        void openFile();
        void createFile();
        void accept();

    private:
        void createActions ();
        
        QString* m_pFilename;
        QString m_strFilename;
        bool* m_pNewFile;
};

#endif
