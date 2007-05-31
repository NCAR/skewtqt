#include "SkewtQtPlot.h"
#include <datastore/DataDomain.h>
#include <datastore/DataSource.h>
#include <datastore/DataSet.h>
#include <datastore/DataSetSelection.h>
#include "plotlib/DataSetSelectWin.h"
#include "PlotFactoryP.h"

#include <qcursor.h>
#include <qinputdialog.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qprinter.h>
#include <qpushbutton.h>
#include <qpopupmenu.h>

#undef DEBUG
#include "logx/Logging.h"

LOGGING("plotlib.SkewtQtPlot");

using namespace plotlib;
using namespace datastore;

const uint SkewtQtPlot::DefaultSymbolSize = 4;

const std::string SkewtQtPlot::P_AXIS = "pressure";
const std::string SkewtQtPlot::T_AXIS = "temperature";
const std::string SkewtQtPlot::RH_AXIS = "relative humidity";
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

	// use the Plot widget to put everthing into.
	_topWidget		= new QWidget(this);

	// creat the layouts
	QBoxLayout* mainLayout	= new QVBoxLayout(_topWidget);

	_plotLayout		= new QVBoxLayout(mainLayout);
	QBoxLayout* statusLayout   = new QVBoxLayout(mainLayout);

	QBoxLayout* dataLayout	= new QHBoxLayout(statusLayout);

	QBoxLayout* pointCountLayout = new QHBoxLayout(dataLayout);
	QBoxLayout* presLayout	= new QHBoxLayout(dataLayout);
	QBoxLayout* tdryLayout	= new QHBoxLayout(dataLayout);
	QBoxLayout* rhLayout	= new QHBoxLayout(dataLayout);
	QBoxLayout* wspdLayout	= new QHBoxLayout(dataLayout);
	QBoxLayout* wdirLayout	= new QHBoxLayout(dataLayout);

	QBoxLayout* timeLayout	= new QHBoxLayout(statusLayout);

	QBoxLayout* startLayout	= new QHBoxLayout(timeLayout);

	QBoxLayout* stopLayout	= new QHBoxLayout(timeLayout);

	// create the skew-t artifacts: the skewt itself, and a graphics adapter for Qt usage.
	newSkewT();

	// create the static label widgets
	QLabel* pointLabel = new QLabel( "Points:", _topWidget);
	pointLabel->setAlignment(Qt::AlignRight);
	pointLabel->setSizePolicy(MinMin);

	QLabel* presLabel = new QLabel( "Pressure:", _topWidget);
	presLabel->setAlignment(Qt::AlignRight);
	presLabel->setSizePolicy(MinMin);

	QLabel* tdryLabel = new QLabel( "Temperature:", _topWidget);
	tdryLabel->setAlignment(Qt::AlignRight);
	tdryLabel->setSizePolicy(MinMin);

	QLabel* RHLabel = new QLabel( "RH: ", _topWidget);
	RHLabel->setAlignment(Qt::AlignRight);
	RHLabel->setSizePolicy(MinMin);

	QLabel* WspdLabel = new QLabel( "Wind Speed: ", _topWidget);
	WspdLabel->setAlignment(Qt::AlignRight);
	WspdLabel->setSizePolicy(MinMin);

	QLabel* WdirLabel = new QLabel( "Wind Dir: ", _topWidget);
	WdirLabel->setAlignment(Qt::AlignRight);
	WdirLabel->setSizePolicy(MinMin);

	QLabel* StartLabel = new QLabel( "Start: ", _topWidget);
	StartLabel->setAlignment(Qt::AlignRight);
	StartLabel->setSizePolicy(MinMin);

	QLabel* StopLabel = new QLabel( "Stop: ", _topWidget);
	StopLabel->setAlignment(Qt::AlignRight);
	StopLabel->setSizePolicy(MinMin);

	// create the status widgets that will be updated in real-time
	_pointCount = new QLabel( "", _topWidget);
	_pointCount->setAlignment(Qt::AlignLeft);
	_pointCount->setSizePolicy(MinMin);

	_pressure = new QLabel( "", _topWidget);
	_pressure->setAlignment(Qt::AlignLeft);
	_pressure->setSizePolicy(MinMin);

	_tdry = new QLabel( "", _topWidget);
	_tdry->setAlignment(Qt::AlignLeft);
	_tdry->setSizePolicy(MinMin);

	_RH = new QLabel( "", _topWidget);
	_RH->setAlignment(Qt::AlignLeft);
	_RH->setSizePolicy(MinMin);

	_wspd = new QLabel( "", _topWidget);
	_wspd->setAlignment(Qt::AlignLeft);
	_wspd->setSizePolicy(MinMin);

	_wdir = new QLabel( "", _topWidget);
	_wdir->setAlignment(Qt::AlignLeft);
	_wdir->setSizePolicy(MinMin);

	_startTime = new QLabel( "", _topWidget);
	_startTime->setAlignment(Qt::AlignLeft);
	_startTime->setSizePolicy(MinMin);

	_stopTime = new QLabel( "", _topWidget);
	_stopTime->setAlignment(Qt::AlignLeft);
	_stopTime->setSizePolicy(MinMin);

	pointCountLayout->addWidget(pointLabel);
	pointCountLayout->addWidget(_pointCount);

	presLayout->addWidget(presLabel);
	presLayout->addWidget(_pressure);

	tdryLayout->addWidget(tdryLabel);
	tdryLayout->addWidget(_tdry);

	rhLayout->addWidget(RHLabel);
	rhLayout->addWidget(_RH);

	wspdLayout->addWidget(WspdLabel);
	wspdLayout->addWidget(_wspd);

	wdirLayout->addWidget(WdirLabel);
	wdirLayout->addWidget(_wdir);

	startLayout->addWidget(StartLabel);
	startLayout->addWidget(_startTime);

	stopLayout->addWidget(StopLabel);
	stopLayout->addWidget(_stopTime);

	initDataSets(getDataSource());

} // END CTOR

/* -------------------------------------------------------------------- */
SkewtQtPlot::~SkewtQtPlot()
{
	delete _pSkewT;
	delete _pSkewTAdapter;
	delete _prototypeSelection;

} // END DTOR


void
SkewtQtPlot::
setupPlotWindow(PlotWindow*)
{
  // There is no menu modification to do here.
  // The skew-t plot does not add a menu entry to create more plots.
}


QPixmap
SkewtQtPlot::
getPixmap()
{
  return QPixmap::grabWidget(this->skewtWidget());
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

	_pSkewTAdapter = new skewt::SkewTAdapterQt(_topWidget, symbolSize);
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
	_pSkewT->unzoom();
}

/* -------------------------------------------------------------------- */
void
SkewtQtPlot::changeTraceWidth()
{
  uint width = _pSkewTAdapter->getSymbolSize();

  bool  ok;

  width = QInputDialog::getInteger(
		"Set symbol size", "Symbol size (pixels):",
		width, 0, 100, 1, &ok, this );

  if (ok)
  {
    DLOG << "changeTraceWidth, new width = " << width;
    _pSkewTAdapter->setSymbolSize(width);
    newSkewT();
    replot(DataNotice());
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
  const double	*RH;
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
	for (size_t i = 0; i < dlist[0]->_axisList.size(); i++) {

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
		n = ds.getTimeDataPairs(dom, &times, &RH);
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
			_pSkewT->drawTdry(pres[k], tdry[k]);
			_pSkewT->drawDp(pres[k], tdry[k], RH[k]);
			_pSkewT->drawWind(pres[k], wspd[k], wdir[k]);
		}

		_startTime->setText(times[0].simpleString().c_str());
		_stopTime->setText (times[nData-1].simpleString().c_str());

		_pointCount->setNum(nData);

		// update the status displays with the most recent data
		_pressure->setText(QString("%1").arg(pres[nData-1], 0, 'f', 1 ));
		_tdry->setText(QString("%1").arg(tdry[nData-1], 0, 'f', 1 ));
		_RH->setText(QString("%1").arg(RH[nData-1],	0, 'f', 0 ));
		_wspd->setText(QString("%1").arg(wspd[nData-1], 0, 'f', 1 ));
		_wdir->setText(QString("%1").arg(wdir[nData-1], 0, 'f', 0 ));

		_lastDataTime = times[nData-1];
	}
}

/* -------------------------------------------------------------------- */
void 
SkewtQtPlot::print(QPrinter *printer) {
	_pSkewTAdapter->print(printer);
}

/* -------------------------------------------------------------------- */
QWidget*
SkewtQtPlot::skewtWidget() {
	return _pSkewTAdapter;
}


/* -------------------------------------------------------------------- */
void
SkewtQtPlot::initDataSets(const datastore::DataSource *ds) {

	// Just in case initDataSets is called more than once
	delete _prototypeSelection;
	_prototypeSelection = new DataSetSelection();

	_prototypeSelection->_filter.setRankFilter(DataSetFilter::Op_EQ, 1);
	AxisSelectMap& axisList = _prototypeSelection->_axisList;
	// The order of definition here determines the order that that
	// the data sets are delivered in the replot!
	axisList[P_AXIS]  = 0;
	axisList[T_AXIS]  = 0;
	axisList[RH_AXIS] = 0;
	axisList[WS_AXIS] = 0;
	axisList[WD_AXIS] = 0;

	datastore::DataSetList sets;
	ds->getDataSets(sets, DataSetFilter(), /*recurse*/true);

	if (sets.size() == 0)
		return;

	for (size_t j = 0; j < sets.size(); ++j) {
		std::string name	 = sets[j]->getVariableName().c_str();
		std::string longName = sets[j]->getLongName().c_str();
		std::string units    = sets[j]->getUnits().c_str();

		if (!name.compare("PSX")) {
			axisList[P_AXIS] = sets[j];
		} else if (!name.compare("ATX")) {
			axisList[T_AXIS] = sets[j];
		} else if (!name.compare("RHUM")) {
			axisList[RH_AXIS] = sets[j];
		} else if (!name.compare("WS")) {
			axisList[WS_AXIS] = sets[j];
		} else if (!name.compare("WD")) {
			axisList[WD_AXIS] = sets[j];
		}
	}

	if (! _prototypeSelection->valid()) {
		selectDataSets();
	} else {
		implInsertTrace(*_prototypeSelection);
	}
}


datastore::DataSetSelection*
SkewtQtPlot::
createSelection(const std::string& kind)
{
	return _prototypeSelection->clone();
}


/* -------------------------------------------------------------------- */
void
SkewtQtPlot::selectDataSets() {

	// Always limit traces to one, for now.
	traces_t dlist = getTraces();
	if (dlist.size() == 1)
	{
		dlist[0]->_action = SA_REPLACE;
		popupDataSetSelectWin(*(dlist[0]));
	}
	else
	{
		boost::scoped_ptr<DataSetSelection> dss(createSelection());
		dss->_action = SA_INSERT;
		popupDataSetSelectWin(*dss);
	}
}


/* -------------------------------------------------------------------- */
void
SkewtQtPlot::
implInsertTrace(const datastore::DataSetSelection& dss) {

	// register the data sets that will provide fields for plotting.
	// Also label the plot with the variable names.

	AxisSelectMap axis = dss._axisList;
	// create a key which identifies the variables used in the plot
	std::string key = "|cpres=";
	key += axis[P_AXIS].getVariableName();
	key += " tdry=";
	key += axis[T_AXIS].getVariableName();
	key += " rh=";
	key += axis[RH_AXIS].getVariableName();
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
implReplaceTrace(const datastore::DataSetSelection& dss) {

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
	QPopupMenu popup(this);

	popup.insertItem("Change Variables...", this, SLOT(selectDataSets()));
	popup.insertItem("Change Trace Width...", this, SLOT(changeTraceWidth()));
	popup.insertItem("Undo Zoom", this, SLOT(unzoomSlot()));
	popup.exec(QCursor::pos());
}

/* -------------------------------------------------------------------- */
bool 
SkewtQtPlot::eventFilter(QObject *object, QEvent *e)
{

	// catch the right mouse button press, and popup
	// the context menu.

	if (e->type() == QEvent::MouseButtonPress) {
		ButtonState b = ((QMouseEvent*)e)->button();
		if ( b == Qt::RightButton)
		{
			popupPlotContextMenu();
			return true;
		}
	}
	return QObject::eventFilter(object, e);
}
