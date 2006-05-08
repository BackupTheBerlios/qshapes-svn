
QtCanvas for Qt 4.1.0
---------------------

This module may be used with the Qt Open Source Edition under the
same license terms as that.

QtCanvas is Qt 3's canvas module with a Qt 4 style API.

QtCanvas is a port to Qt 4 of Qt 3's QCanvas module. It is
intended to be used in porting projects from Qt 3 to Qt 4.
QtCanvas differs from the Qt3Canvas classes of the Qt3Support
module in that the API has been updated to Qt 4 style. Hence, it
facilitates "clean" Qt 4 ports which do not require Qt3Support.

It should be noted that this module is not recommended as the
long-term choice for new Qt 4 projects, as Qt 4.2 is expected to
provide a replacement for QCanvas, based on Qt 4's powerful new 2D
paint system.

QtCanvas provides a highly optimized 2D graphic area that can
contain an arbitrary number of items. Canvas items can have an
arbitrary shape, size and content, can be freely moved around in
the canvas, and can be checked for collisions. Canvas items can be
set to move across the canvas automatically and animated canvas
items are supported. Several item types are provided: ellipses,
line segments, pixmaps, polygons, splines, sprites and texts.
Custom items can be created using subclassing.

QtCanvas module uses a document/view model. The QtCanvasView class
is used to show a particular view of a canvas. Multiple views can
operate on the same canvas at the same time. Every view can use an
arbitrary transformation matrix on the canvas which makes it easy
to implement features such as zooming.

Installation instructions:

To build the example:
---------------------

	cd qtcanvas/example
	qmake
	make

    Or if you are using Microsoft Visual Studio:

	cd qtcanvas\example
	qmake
	nmake

Using QtCanvas in your project
------------------------------

The directory src includes all the source code for the component. To
use this component in your project:

1.  Add the following line

	include(your-install-dir/qtcanvas/src/qtcanvas.pri)

    to your project's .pro file.

2.  Run qmake on the project's .pro file.

This adds the QtCanvas's sources and headers to the SOURCES and HEADERS
qmake variables respectively, and update INCLUDEPATH to contain the
package's src directory. Additionally, the .pri file may include some
dependencies needed by the package.

To include the header file from the package in your sources, you can
now simply use:

    #include <qtcanvas.h>


Installing the documentation
----------------------------

The HTML documentation for the package's classes is located in the
your-install-dir/doc/html/. You can open these files with any web
browser.

To install the documentation into Qt Assistant:

1.  The directory your-install-dir/doc/html contains a file with the
    name canvas.dcf.

2.  Close any running instances of assistant.

3.  Execute the following commands in a shell, command prompt or terminal
    window:

	cd your-install-dir/qtcanvas/doc/html/
	assistant -addContentFile canvas.dcf

The next time you start Qt Assistant, you can access the package's
documentation.


Removing the documentation from Qt Assistant
--------------------------------------------

1.  The directory your-install-dir/doc/html contains a file with the
    name canvas.dcf.

2.  Close any running instances of assistant.

3.  Execute the following commands in a shell, command prompt or terminal
    window:

	cd your-install-dir/qtcanvas/doc/html/
	assistant -removeContentFile canvas.dcf
