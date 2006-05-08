/****************************************************************************
**
** Copyright (C) 1992-2005 Trolltech AS. All rights reserved.
**
** This file may be used under the terms of the GNU General Public
** License version 2.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of
** this file.  Please review the following information to ensure GNU
** General Public Licensing requirements will be met:
** http://www.trolltech.com/products/qt/opensource.html
**
** If you are unsure which license is appropriate for your use, please
** review the following information:
** http://www.trolltech.com/products/qt/licensing.html or contact the
** sales department at sales@trolltech.com.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#include <qstatusbar.h>
#include <qmessagebox.h>
#include <qmenubar.h>
#include <qapplication.h>
#include <qdesktopwidget.h>
#include <qimage.h>

#include "canvas.h"

#include <stdlib.h>

extern QString butterfly_fn;
extern QString logo_fn;

int main(int argc, char** argv)
{
    QApplication app(argc,argv);

    if ( argc > 1 )
	butterfly_fn = argv[1];
    else
	butterfly_fn = ":/butterfly.png";

    if ( argc > 2 )
	logo_fn = argv[2];
    else
	logo_fn = ":/qtlogo.png";

    QtCanvas canvas(800,600);
    canvas.setAdvancePeriod(30);
    Main m(&canvas);
    m.resize(m.sizeHint());
    m.setWindowTitle("Qt Example - Canvas");
    if (QApplication::desktop()->width() > m.width() + 10
	&& QApplication::desktop()->height() > m.height() + 30)
	m.show();
    else
	m.showMaximized();

    QObject::connect(qApp, SIGNAL(lastWindowClosed()), qApp, SLOT(quit()));

    return app.exec();
}

