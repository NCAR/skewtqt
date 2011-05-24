// -*- C++ -*-

#ifndef _plotlib_skewt_SkewtQtPlot_h_
#define _plotlib_skewt_SkewtQtPlot_h_

#include "plotlib/Plot.h"
#include "datastore/DateTime.h"

#include "SkewT/SkewT.h"
#include "SkewTAdapterQt.h"

#include <datastore/DateTime.h>

#include <set>

class QLabel;
class QBoxLayout;

/* -------------------------------------------------------------------- */
namespace plotlib
{

	/**
	* Plot sub-class for a SkewT-LogP display.
	*
	* SkewtQtPlot renders the SkewT generic plot, using the combination of
	* SkewT and SkewTAdapterQt.  It responds to data 
	* notifications via the replot mechanism, which add new data points in 
	* real-time or post processing.
	* 
	* When SkewtQtPlot is created, it builds these main items:
	*<ul>
	*<li> A QWidget, which will contain all other graphics components
	*<li> A number of nested layouts. One branch contains the actual
	* plot. The other one contains status displays of interesting
	* details, such as the number of data points on the graph, etc.
	*<li> A SkewtQtAdapter, which uses Qt to handle all graphics activities.
	*<li> A SkewT, which renders a "virtual" skewt-logp plot onto the SkewtQtAdapter.
	*</ul> 
	*
	* SkewtQtPlot depends heavily upon SkewT. SkewT was not designed to have data 
	* removed from it. When this needs to be done, such as when a new set 
	* of variables is chosen, the whole SkewT and SkewTAdapterQt combination 
	* are deleted, and new ones constructed. @see newSkewT.
	*
	* SkewT maintains its own copy of the data that is bing plotted. 
	* There is not a two way connection between SkewT and its data provider (in this case 
	* SkewtQtPlot). The implication is that SkewtQtPlot is completely in charge
	* of the data that is being displayed in SkewT. SkewT is not able to query the
	* datastore, or "pull" data from it.
	*
	* <b>Data management</b>
	*
	* One unique aspect of SkewT plotting is that multiple variables must exist at the
	* same time in order to plot a point on the graph: a temperature point requires
	* a pressure and a temperature, a dew point requires a pressure, temperature and 
	* relative humidity, and a wind barb needs pressure, wind speed and direction.
	* SkewtQtPlot has an unsophisticated approach to this: it expects pressure, temperature,
	* relative humidity, wind speed and wind direction to all be available in 
	* each data notification. Future refinements could eliminate this restricition, if
	* it becomes warranted.
	*
	* As part of the plotting/datastore mechanism, replot() is called by the 
	* datastore infrastructure when new data is available. SkewtQtPlot verifies
	* that all five necessary variables are present. It next verifies that data
	* for this timestamp have not already been plotted. If both tests pass, then
	* the three SkewT add data functions are called to plot a temperature, dewpoint
	* and wind barb.
	* 
	* <b>Plot context menu</b>
	*
	* A single context menu will popup from a right mouse clicked anywhere
	* over the SkewT (actually, over the SkewTAdapterQT). This is accomplished by
	* attaching SkewtQtPlot to SkewTAdapterQt as an event filter .
	* This is pretty nice, since the event filter is contained within SkewtQtPlot,
	* rather than having this logic within the adapter. @see eventFilter().
	*
	* <b>Zooming</b>
	*
	* Zooming is handled by SkewTAdapterQt and SkewT. A rubberband bounding
	* box is drawn by left click dragging a box in the desired area. An unzoom 
	* back to full scale display is acomplished via a choice in the context menu, 
	* or a single mouse click on the plot.
	* 
	* <b>DataSet selection</b>
	*
	* Five DataSet s are used for SkewtQtPlot operation. At construction time, an
	* attempt is made to find a default collection of variables.
	* If they are successfully located, the plotting proceeds with these. If not, 
	* a dataSet selection dialog is presented for the user to utilize in choosing
	* appropriate sets. @see initDataSets
	*
	* Using the context menu, the user can choose to change the collection of 
	* DataSet s. The DataSet selection dialog will be displayed, and once the
	* choices have been made, the existing plot will be destroyed, and a new one
	* created with the new variables.
	*
	*/
	class SkewtQtPlot : public plotlib::Plot
	{
		Q_OBJECT

	public:
		/// Ctor
		/// @param pc Plot information to pass up to base classes.
		SkewtQtPlot(const PlotCreateToken& pc);

		/// Dtor
		~SkewtQtPlot();

  virtual QSize
  sizeHint() const
  {
    return QSize(600, 400);
  }

  virtual QSizePolicy
  sizePolicy() const
  {
    return QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  }

   virtual QPixmap
   getPixmap();

    /// Create a new skewt plot; one which does not have any data displayed yet. If
    /// a SkewtT and its SkewTQtAdapter already exist, delete them first. 
    void newSkewT();
  
    /// this function is called when there is new data to be plotted. It assumes that
    /// the data update is going to deliver 5 traces (variables), and they
    /// will be in the order: pres, tdry, dp, wspd and wdiir. It also expects that
    /// the same number of data points are delivered for each trace.
    ///
    /// If each trace does have the same length, then the timetags are ASSUMED
    /// to be equivalent. A set of already plotted timetags is maintained within
    /// SkewQtPlot. The timetags for the first trace (pres) are used to see if
    /// each data point has already been plotted. If not, all of the variables are
    /// processed at that timetag, and the timetag is added to the set of 
    /// already plotted times.
    /// @param dn The data domain of the new data
    ///
    void replot(datastore::DataNotice dn);

		/// Make an initial guess at the data sets to use. An attempt is 
		/// made to fill _presSet, _tdrySet, _DPset, _wspdSet and
		/// _wdirSet with default DataSet values. If the default
		/// sets cannot be located, then selectDataSets() is called in order
		/// to let the user to complete the selection. If the default DataSet s are
		/// located, then applyDataSets() is called to make them active.
		/// @param ds The datasource.
		void initDataSets(const datastore::DataSource *ds);

		virtual void print(QPrinter *printer);

		/// Return the widget that has just the skew-t plot in it. 
		/// This can be useful, say if you want to print or save a 
		/// graphics file from just the skewt area of SkewtQtPlot.
		QWidget* skewtWidget();

	protected slots:
		///
		/// Calls the SkewT::unzoom() function
		///
		void unzoomSlot();

		/// Replace the existing skewt DataSet s with a new selection of them.
		/// The skewt plot is reinitialized, and the new DataSet s are drawn, but using 
		/// the existing time domain.
		/// @param dss A selection of DataSets.
		void
			implReplaceTrace(const datastore::DataSetSelection& dss);

		/// Popup a context menu for the skewt plot area.
		void popupPlotContextMenu();

		datastore::DataSetSelection*
			createSelection(const std::string& kind = "");

		/// Edit/change line width.  Popup dialog with current width and allow editing.
		void changeTraceWidth();


	protected:
		/// Remove previously registered trace and replace it with
		/// the given DataSet selection @p dss. 
		void
			implInsertTrace(const datastore::DataSetSelection& dss);

		/// This is an event handler which will be attached to _pSkewTAdapter,
		/// to catch a right button mouse click. When a right button is detected,
		/// popupPlotContextMenu is called.
		bool eventFilter(QObject *object, QEvent *e);

		skewt::SkewTAdapterQt*  _pSkewTAdapter;	///< The adapter for translating between SkewT and Qt
		skewt::SkewT*           _pSkewT;		    ///< The generic skewt plotting object

		/// The prototype DataSetSelection for a skew-t trace
		datastore::DataSetSelection* _prototypeSelection;

		QLayout*         _plotLayout;       ///<  Layout that holds the skewt.
        QLabel*          _pointCount;       ///< Realtime values of point count will be displayed here.
		QLabel*          _pressure;         ///< Realtime values of pressure will be displayed here.
		QLabel*          _tdry;             ///< Realtime values of temperature will be displayed here.
		QLabel*          _dp;               ///< Realtime values of dp will be displayed here.
		QLabel*          _wspd;             ///< Realtime values of wind speed will be displayed here.
		QLabel*          _wdir;             ///< Realtime values of wind direction will be displayed here.
		QLabel*          _title;            ///< Title will be displayed here.

		datastore::DateTime _lastDataTime;  ///< The time of the most recent data on the plot
		
		static const uint DefaultSymbolSize;
		static const std::string P_AXIS, T_AXIS, DP_AXIS, WS_AXIS, WD_AXIS;
	};

}	// END NAMESPACE


#endif // _plotlib_skewt_SkewtQtPlot_h_
