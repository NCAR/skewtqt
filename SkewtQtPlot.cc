#include "SkewtQtPlot.h"
#include <datastore/DataDomain.h>
#include <datastore/DataSource.h>
#include <datastore/DataSet.h>
#include <datastore/DataSetSelection.h>
#include "plotlib/DataSetSelectionDialog.h"
#include "PlotFactoryP.h"

#include <QMenu>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMouseEvent>
#include <QInputDialog>
#include <QtCore/QEvent>

#include "logx/Logging.h"

LOGGING("plotlib.SkewtQtPlot");

using namespace plotlib;
using namespace datastore;

const uint SkewtQtPlot::DefaultSymbolSize = 4;

const std::string SkewtQtPlot::P_AXIS = "pressure";
const std::string SkewtQtPlot::T_AXIS = "temperature";
const std::string SkewtQtPlot::DP_AXIS = "dewpoint";
const std::string SkewtQtPlot::WS_AXIS = "wind speed";
const std::string SkewtQtPlot::WD_AXIS = "wind direction";


REGISTER_MULTI_PLOT_TYPE(SkewT,"Skew-T",SkewtQtPlot);


/* -------------------------------------------------------------------- */
SkewtQtPlot::
SkewtQtPlot(const PlotCreateToken& pc) :
  Plot(pc),
  _pSkewTAdapter(0),
  _pSkewT(0),
  _prototypeSelection(0),
  _lastDataTime(DateTime::BEGINNING_OF_TIME)
{

  QSizePolicy MinMin(QSizePolicy::Minimum, QSizePolicy::Minimum);
  QSizePolicy ExpExp(QSizePolicy::Expanding, QSizePolicy::Expanding);

  if (layout()) {
    // get rid of the Plot::QWidget layout, if it has one,
    // so that we can select our own type.
    delete layout();
  }

  // create our layouts
  QVBoxLayout* mainLayout	= new QVBoxLayout;
  QBoxLayout* titleLayout	= new QHBoxLayout;
  _plotLayout			= new QVBoxLayout;
  QVBoxLayout* statusLayout	= new QVBoxLayout;
  QHBoxLayout* dataLayout	= new QHBoxLayout;
  QBoxLayout* pointCountLayout	= new QHBoxLayout;
  QBoxLayout* presLayout	= new QHBoxLayout;
  QBoxLayout* tdryLayout	= new QHBoxLayout;
  QBoxLayout* dpLayout		= new QHBoxLayout;
  QBoxLayout* wspdLayout	= new QHBoxLayout;
  QBoxLayout* wdirLayout	= new QHBoxLayout;

  // Set our layout for Plot::QWidget.
  this->setLayout(mainLayout);

  mainLayout->addLayout(titleLayout);
  mainLayout->addLayout(_plotLayout);
  mainLayout->addLayout(statusLayout);

  dataLayout->addLayout(pointCountLayout);
  dataLayout->addStretch();
  dataLayout->addLayout(presLayout);
  dataLayout->addStretch();
  dataLayout->addLayout(tdryLayout);
  dataLayout->addStretch();
  dataLayout->addLayout(dpLayout);
  dataLayout->addStretch();
  dataLayout->addLayout(wspdLayout);
  dataLayout->addStretch();
  dataLayout->addLayout(wdirLayout);

  statusLayout->addLayout(dataLayout);


  // create the skew-t artifacts: the skewt itself, and a graphics
  // adapter for Qt usage.
  newSkewT();

  // create the static label widgets
  QLabel* pointLabel = new QLabel( "Points:", 0);
  pointLabel->setAlignment(Qt::AlignRight);
  pointLabel->setSizePolicy(MinMin);

  QLabel* presLabel = new QLabel( "Pressure:", 0);
  presLabel->setAlignment(Qt::AlignRight);
  presLabel->setSizePolicy(MinMin);

  QLabel* tdryLabel = new QLabel( "Temperature:", 0);
  tdryLabel->setAlignment(Qt::AlignRight);
  tdryLabel->setSizePolicy(MinMin);

  QLabel* dpLabel = new QLabel( "Dewpoint: ", 0);
  dpLabel->setAlignment(Qt::AlignRight);
  dpLabel->setSizePolicy(MinMin);

  QLabel* WspdLabel = new QLabel( "Wind Speed: ", 0);
  WspdLabel->setAlignment(Qt::AlignRight);
  WspdLabel->setSizePolicy(MinMin);

  QLabel* WdirLabel = new QLabel( "Wind Dir: ", 0);
  WdirLabel->setAlignment(Qt::AlignRight);
  WdirLabel->setSizePolicy(MinMin);

  // create the status widgets that will be updated in real-time
  _title = new QLabel( "", 0);
  _title->setAlignment(Qt::AlignCenter);
  _title->setSizePolicy(MinMin);

  _pointCount = new QLabel( "", 0);
  _pointCount->setAlignment(Qt::AlignLeft);
  _pointCount->setSizePolicy(MinMin);

  _pressure = new QLabel( "", 0);
  _pressure->setAlignment(Qt::AlignLeft);
  _pressure->setSizePolicy(MinMin);

  _tdry = new QLabel( "", 0);
  _tdry->setAlignment(Qt::AlignLeft);
  _tdry->setSizePolicy(MinMin);

  _dp = new QLabel( "", 0);
  _dp->setAlignment(Qt::AlignLeft);
  _dp->setSizePolicy(MinMin);

  _wspd = new QLabel( "", 0);
  _wspd->setAlignment(Qt::AlignLeft);
  _wspd->setSizePolicy(MinMin);

  _wdir = new QLabel( "", 0);
  _wdir->setAlignment(Qt::AlignLeft);
  _wdir->setSizePolicy(MinMin);

  titleLayout->addStretch();
  titleLayout->addWidget(_title);
  titleLayout->addStretch();

  pointCountLayout->addWidget(pointLabel);
  pointCountLayout->addWidget(_pointCount);

  presLayout->addWidget(presLabel);
  presLayout->addWidget(_pressure);

  tdryLayout->addWidget(tdryLabel);
  tdryLayout->addWidget(_tdry);

  dpLayout->addWidget(dpLabel);
  dpLayout->addWidget(_dp);

  wspdLayout->addWidget(WspdLabel);
  wspdLayout->addWidget(_wspd);

  wdirLayout->addWidget(WdirLabel);
  wdirLayout->addWidget(_wdir);

  // Initialize the dataset selection prototype and try to fill
  // it with defaults.
  initDataSets(getDataSource());

  // Insert a default trace unless the creation token specified
  // an empty plot.
  if (! pc.empty())
  {
    if (! _prototypeSelection->valid()) {
      selectDataSets();
    } else {
      implInsertTrace(*_prototypeSelection);
    }
  }

} // END CTOR

/* -------------------------------------------------------------------- */
SkewtQtPlot::~SkewtQtPlot()
{
  delete _pSkewT;
  delete _pSkewTAdapter;
  delete _prototypeSelection;

} // END DTOR


/* -------------------------------------------------------------------- */
QPixmap
SkewtQtPlot::
getPixmap()
{
  // grab the skewt pixmap, add a title above it,
  // and return as a pixmap.
  // The title will be the domain extent

  QFont font = QFont("Arial", 12);
  QFontMetrics metrics(font);

  // get the skewt pixmap
  QPixmap skewt = skewtWidget()->grab();

  // set the number of pixels above and below the title text
  int titlePad = 5;

  // set the title string
  QString title = getDomain()->simpleString().c_str();

  // figure how many vertical pixels we need to display the title
  int textHeight = metrics.boundingRect(title).height();

  // determine final pixmap dimensions
  int finalWidth = skewt.width();
  int titleHeight = textHeight + 2*titlePad;
  int finalHeight = skewt.height() + titleHeight;

  // allocate the final pixmap
  QPixmap p(finalWidth, finalHeight);

  // fill it with white
  p.fill();

  // a painter is used for the drawing operations
  QPainter painter(&p);

  // render the title string at the top
  painter.setPen(Qt::black);
  painter.setFont(font);
  painter.drawText(QRect(0,0,finalWidth, titlePad+textHeight),
	  Qt::AlignCenter | Qt::AlignTop, title);

  // copy the skewt into the bottom of the pixmap
  painter.drawPixmap(0, titleHeight, skewt, 0, 0, 0, 0);

  // return it
  return p;
}

/* -------------------------------------------------------------------- */
void
SkewtQtPlot::newSkewT()
{
  DLOG << "newSkewT";

  int symbolSize = DefaultSymbolSize;

  // get rid of the existing plot
  if (_pSkewT)
    delete _pSkewT;
  if (_pSkewTAdapter)
  {
    symbolSize = _pSkewTAdapter->getSymbolSize();
    delete _pSkewTAdapter;
  }

  QSizePolicy ExpExp(QSizePolicy::Expanding, QSizePolicy::Expanding);

  _pSkewTAdapter = new skewt::SkewTAdapterQt(this, -1, -1, symbolSize);
  _pSkewTAdapter->show();
  _pSkewTAdapter->setSizePolicy(ExpExp);
  _pSkewTAdapter->installEventFilter(this);

  _pSkewT = new skewt::SkewT(*_pSkewTAdapter, 0, 0, 0, 0, 0, 0, 30, "", "", "");

  // Tell the adapter who the skewt is, so that it can be called by the adapter
  // when the adapter detects a zoom.
  _pSkewTAdapter->setSkewT(_pSkewT);

  _plotLayout->addWidget(_pSkewTAdapter);

  _lastDataTime = DateTime::BEGINNING_OF_TIME;

  // draw the display
  _pSkewT->draw();
}

/* -------------------------------------------------------------------- */
void
SkewtQtPlot::unzoomSlot()
{
  QApplication::setOverrideCursor(Qt::WaitCursor);
  _pSkewT->unzoom();
  QApplication::restoreOverrideCursor();
}

/* -------------------------------------------------------------------- */
void
SkewtQtPlot::changeTraceWidth()
{
  uint width = _pSkewTAdapter->getSymbolSize();

  bool  ok;

  width = QInputDialog::getInt(this,
			       "Set symbol size", "Symbol size (pixels):",
			       width, 0, 100, 1, &ok );

  if (ok)
  {
    QApplication::setOverrideCursor(Qt::WaitCursor);
    DLOG << "changeTraceWidth, new width = " << width;
    _pSkewTAdapter->setSymbolSize(width);
    newSkewT();
    replot(DataNotice());
    QApplication::restoreOverrideCursor();
  }
}

/* -------------------------------------------------------------------- */
void
SkewtQtPlot::
replot(datastore::DataNotice dn)
{
  if (! dn.sourceUpdate())
  {
    newSkewT();
  }

  const datastore::DataDomain& dom = *getDomain();
  const datastore::DateTime *times;
  const double	*pres;
  const double	*tdry;
  const double	*dp;
  const double	*wspd;
  const double	*wdir;

  traces_t dlist = getTraces();
  DLOG << "replot, nTraces = " << dlist.size();

  // we asked for 5 datasets per trace; we had better get them,
  // and we only do one trace at present.
  if (dlist.size() != 1 || dlist[0]->_axisList.size() != 5)
    return;

  int nData = 0;
  // iterate over all of the variables, collecting the data.
  // If we don't get the same number of points for each trace,
  // just abandon this update.
  for (size_t i = 0; i < dlist[0]->_axisList.size(); i++)
  {
    DataSet& ds = dlist[0]->_axisList[i];

    // grab the data for this variable
    int n = 0;
    switch (i) {
      case 0:
        n = ds.getTimeDataPairs(dom, &times, &pres);
        break;
      case 1:
        n = ds.getTimeDataPairs(dom, &times, &tdry);
        break;
      case 2:
        n = ds.getTimeDataPairs(dom, &times, &dp);
        break;
      case 3:
        n = ds.getTimeDataPairs(dom, &times, &wspd);
        break;
      case 4:
        n = ds.getTimeDataPairs(dom, &times, &wdir);
        break;
    }

    if (i == 0) {
      // save the number of data points
      nData = n;
    } else {
      if (n != nData) {
        // if succeeding variables don't have the same number of data points,
        // just ignore this update.
        return;
      }
    }
  }

  // if we didn't get any data, return
  if (nData == 0)
    return;

  // find the oldest new data that hasn't been plotted
  int index;
  for (index = nData-1; index >=0; index--) {
    if (times[index] <= _lastDataTime)
      break;
  }
  index++;

  if (index < nData) {
    // plot data values for times that we haven't seen previously.
    for (int k = index; k < nData; k++) {
      if (
        !netcdfFillValue(pres[k]) &&
        !netcdfFillValue(tdry[k]) &&
        !netcdfFillValue(dp[k])&&
        !netcdfFillValue(wspd[k]) && 
        !netcdfFillValue(wdir[k])) {
          _pSkewT->drawTdry(pres[k], tdry[k]);
          _pSkewT->drawDp(pres[k], dp[k]);
          _pSkewT->drawWind(pres[k], wspd[k], wdir[k]);
      }
    }

    _title->setText(getDomain()->simpleString().c_str());

    _pointCount->setNum(nData);

    // update the status displays with the most recent data
    _pressure->setText(QString("%1").arg(pres[nData-1], 0, 'f', 1 ));
    _tdry->setText(QString("%1").arg(tdry[nData-1], 0, 'f', 1 ));
    _dp->setText(QString("%1").arg(dp[nData-1],	0, 'f', 0 ));
    _wspd->setText(QString("%1").arg(wspd[nData-1], 0, 'f', 1 ));
    _wdir->setText(QString("%1").arg(wdir[nData-1], 0, 'f', 0 ));

    _lastDataTime = times[nData-1];
  }
}

/* -------------------------------------------------------------------- */
void
SkewtQtPlot::print(QPrinter *printer)
{
  // set the title string
  std::string title = getDomain()->simpleString().c_str();

  _pSkewTAdapter->print(printer, title);
}

/* -------------------------------------------------------------------- */
QWidget*
SkewtQtPlot::skewtWidget()
{
  return _pSkewTAdapter;
}


/* -------------------------------------------------------------------- */
void
SkewtQtPlot::initDataSets(const datastore::DataSource *ds)
{
  // Just in case initDataSets is called more than once
  delete _prototypeSelection;
  _prototypeSelection = new DataSetSelection();

  _prototypeSelection->_filter.setRankFilter(DataSetFilter::Op_EQ, 1);
  AxisSelectMap& axisList = _prototypeSelection->_axisList;
  // The order of definition here determines the order that that
  // the data sets are delivered in the replot!
  axisList[P_AXIS]  = 0;
  axisList[T_AXIS]  = 0;
  axisList[DP_AXIS] = 0;
  axisList[WS_AXIS] = 0;
  axisList[WD_AXIS] = 0;

  datastore::DataSetList sets;
  ds->getDataSets(sets, DataSetFilter(), /*recurse*/true);

  DataSet *set;

  if ((set = FindVariable(sets, "PSXC")) == 0)
    set = FindVariable(sets, "PSFDC");
  axisList[P_AXIS] = set;
  axisList[T_AXIS] = FindVariable(sets, "ATX");
  axisList[DP_AXIS] = FindVariable(sets, "DPXC");
  axisList[WS_AXIS] = FindVariable(sets, "WSC");
  axisList[WD_AXIS] = FindVariable(sets, "WDC");
}


datastore::DataSetSelection*
SkewtQtPlot::
createSelection(const std::string& kind)
{
  return _prototypeSelection->clone();
}


/* -------------------------------------------------------------------- */
void
SkewtQtPlot::
implInsertTrace(const datastore::DataSetSelection& dss)
{
  // register the data sets that will provide fields for plotting.
  // Also label the plot with the variable names.

  AxisSelectMap axis = dss._axisList;
  // create a key which identifies the variables used in the plot
  std::string key = "|cpres=";
  key += axis[P_AXIS].getVariableName();
  key += " tdry=";
  key += axis[T_AXIS].getVariableName();
  key += " dp=";
  key += axis[DP_AXIS].getVariableName();
  key += " wspd=";
  key += axis[WS_AXIS].getVariableName();
  key += " wdir=";
  key += axis[WD_AXIS].getVariableName();

  _pSkewTAdapter->Text(key, 0.5, 0.01, 0);

  Plot::implInsertTrace (dss);
}

/* -------------------------------------------------------------------- */
void
SkewtQtPlot::
implReplaceTrace(const datastore::DataSetSelection& dss)
{
  // Replace the data sets, and redraw the graph with the new data sets.

  // sanity check; if we don't gt exactly 5, then there is a problem
  if (dss._axisList.size() != 5)
    return;

  // get rid of the existing skewt plot, and start a fresh blank one.
  // State associated with the data being fed to the skewt (e.g. the
  // running number of data points, etc. will also be reset.
  newSkewT();

  removeAllTraces();
  implInsertTrace(dss);
}

/* -------------------------------------------------------------------- */
void SkewtQtPlot::popupPlotContextMenu()
{
  QMenu popup(this);

  popup.addAction("Change Variables...", this, SLOT(selectDataSets()));
  popup.addAction("Change Trace Width...", this, SLOT(changeTraceWidth()));
  popup.addAction("Undo Zoom", this, SLOT(unzoomSlot()));
  popup.exec(QCursor::pos());
}

/* -------------------------------------------------------------------- */
bool
SkewtQtPlot::eventFilter(QObject *object, QEvent *e)
{
  // catch the right mouse button press, and popup
  // the context menu.
  if (e->type() == QEvent::MouseButtonPress)
  {
    Qt::MouseButton b = ((QMouseEvent*)e)->button();
    if (b == Qt::RightButton)
    {
      popupPlotContextMenu();
      return true;
    }
  }
  return QObject::eventFilter(object, e);
}

/* -------------------------------------------------------------------- */
bool
SkewtQtPlot::netcdfFillValue(double val) {
  return (lround(val) == -32767);
}
