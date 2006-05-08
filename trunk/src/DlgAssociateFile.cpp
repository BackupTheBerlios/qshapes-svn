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

#include "DlgAssociateFile.h"

#include <QFileDialog>
#include <QFile>
#include <QMessageBox>
#include <QRegExp>
#include <QSettings>

DlgAssociateFile::DlgAssociateFile(QString* filename, bool* newFile, QWidget *parent)
    : QDialog(parent, Qt::Dialog)
{
    setupUi (this);
    m_pFilename = filename;
    m_strFilename = "";
    m_pNewFile = newFile;
    *m_pNewFile = true;
    
    createActions();
}


DlgAssociateFile::~DlgAssociateFile()
{
}

void DlgAssociateFile::createActions ()
{
    connect(btnOpenFile, SIGNAL(clicked()), this, SLOT(openFile()));
    connect(btnCreateFile, SIGNAL(clicked()), this, SLOT(createFile()));
    connect(radioBtnAssociate, SIGNAL(clicked()), this, SLOT(radioAssociateFile()));
    connect(radioBtnNewFile, SIGNAL(clicked()), this, SLOT(radioNewFile()));
}


void DlgAssociateFile::radioAssociateFile ()
{
    btnOpenFile->setEnabled(true);
    btnCreateFile->setEnabled(false);
    *m_pNewFile = false;
    m_strFilename = "";
}

void DlgAssociateFile::radioNewFile ()
{
    btnOpenFile->setEnabled(false);
    btnCreateFile->setEnabled(true);
    *m_pNewFile = true;
    m_strFilename = "";
}

void DlgAssociateFile::createFile()
{
    m_strFilename = QFileDialog::getSaveFileName( this, "QShapes -- Associate new file", QString(), "QShapes file (*.qsh *.QSH)");
    
    if ( ! m_strFilename.isEmpty() )
    {
            //if needed adds .qsh extension
        QRegExp ext ( "(\\.qsh)$" );
        ext.setCaseSensitivity (Qt::CaseInsensitive);
        
        if (ext.indexIn(m_strFilename) == -1)
            m_strFilename += ".qsh"; // add extension
    }
}

void DlgAssociateFile::openFile()
{
    QSettings settings;
    QString lastDir = settings.value("lastDir","").toString();
    
    //check if last dir still exists
    QDir dir (lastDir);
    if (!dir.exists())
        lastDir = "";

    m_strFilename = QFileDialog::getOpenFileName( this, "QShapes -- Associate file", lastDir, "QShapes file (*.qsh *.QSH);;All files (*.*)");
    
    try
    {
        if (m_strFilename.isEmpty())
            throw (QString ("You've specified an invalid filename"));

        if (!QFile::exists( m_strFilename ))
            throw (QString ("Selected filename doesn't exist!"));
    }
    catch (QString &msg)
    {
        msg = "File association abandoned\n" + msg;
        QMessageBox::critical (this, "Error", msg);
        m_strFilename = "";
    }
    catch (...)
    {
        QMessageBox::critical (this, "Error", "General error");
        m_strFilename = "";
    }
}

void DlgAssociateFile::accept()
{
    if (!m_strFilename.isEmpty())
    {
        *m_pFilename = m_strFilename;
        QDialog::accept();
    }
    else
        QMessageBox::critical (this, "Error", "No file specified!");
}
