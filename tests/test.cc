/*
-------------------------------------------------------------------------
OBJECT NAME:	test.cc

FULL NAME:	Main launcher for SkewtQtTest programs

TYPE:		Qt

COPYRIGHT:	University Corporation for Atmospheric Research, 2003-2004
-------------------------------------------------------------------------
*/

#include <q3vbox.h>
#include <qapplication.h>
#include "SkewtQtTest.h"

int main(int argc, char *argv[])
{

  QApplication app(argc, argv);

  Q3VBox mainWindow;
  mainWindow.show();

  SkewtQtTest skewtTest(&mainWindow, "skewtTest");

  app.setMainWidget(&mainWindow);
  app.exec();

  return(0);

}	// END MAIN

