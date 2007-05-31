/*
-------------------------------------------------------------------------
OBJECT NAME:	SkewtQtTest.h

FULL NAME:	Create a SkewtQtTest Window

TYPE:		

DESCRIPTION:	

NOTES:		

COPYRIGHT:	University Corporation for Atmospheric Research, 2003
-------------------------------------------------------------------------
*/

#ifndef SkewtQtTest_H
#define SkewtQtTest_H

#include <qapplication.h>
#include <qmainwindow.h>
#include <qpopupmenu.h>
#include <qdialog.h>
#include <qtimer.h>

#include "Skewt/Skewt.h"
#include "SkewTAdapterQt.h"

class SkewtQtTest : public QObject
{
  Q_OBJECT

public:
	SkewtQtTest(QWidget *parent, const char* name);
  virtual ~SkewtQtTest();

  public slots:
    void            timeOut();
    void            unzoomSlot();

protected:

  void              makeData();

  skewt::SkewTAdapterQt*   _skewtAdapter;
  skewt::SkewT*            _skewt;
  QTimer            _timer;
  
  int _n;
  double*           _pres;
  double*           _tdry;
  double*           _rh;
  double*           _wspd;
  double*           _wdir;
  double            _lastPres;

  

};

#endif
