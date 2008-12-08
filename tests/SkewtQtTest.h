/*
-------------------------------------------------------------------------
OBJECT NAME:	SkewtQtTest.h

FULL NAME:	Create a SkewtQtTest Window

COPYRIGHT:	University Corporation for Atmospheric Research, 2003-2008
-------------------------------------------------------------------------
*/

#ifndef SkewtQtTest_H
#define SkewtQtTest_H

#include <QtGui/QDialog>
#include <QtGui/QLabel>
#include <QtCore/QTimer>

#include "Skewt/Skewt.h"
#include "SkewTAdapterQt.h"

class SkewtQtTest : public QObject
{
  Q_OBJECT

public:
	SkewtQtTest(QDialog *parent, const char* name);
  virtual ~SkewtQtTest();

public slots:
  void timeOut();
  void unzoomSlot();

protected:
  void makeData();

  skewt::SkewTAdapterQt* _skewtAdapter;
  skewt::SkewT* _skewt;
  QLabel* _pointsLabel;
  QTimer _timer;
  
  // number of points in the initial profile
  int _n;
  // the total number of points being displayed.
  int _N;

  double* _pres;
  double* _tdry;
  double* _rh;
  double* _wspd;
  double* _wdir;
  double  _lastPres;
};

#endif
