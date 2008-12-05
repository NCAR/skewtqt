// SkewTAdapterQt.cpp: implementation of the SkewTAdapterQt class.
//
//////////////////////////////////////////////////////////////////////

#include "SkewTAdapterQt.h"
#include "SkewT/SkewTdefs.h"

#include <QtGui/QMouseEvent>
#include <QtGui/QPolygon>
#include <QtGui/QPrinter>
#include <QtGui/QRubberBand>

using namespace skewt;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SkewTAdapterQt::SkewTAdapterQt(QWidget* parent, int symbolSize, int resizeHoldOffMs):
  QWidget(parent),
  _firstLineCall(true),
  _symbolSize(symbolSize),
  _dontPaint(false),
  _resizeHoldOffMs(resizeHoldOffMs),
  _pSkewT(0),
  _ready(false)
{
  // set our background color
  QPalette palette;
  palette.setColor(backgroundRole(), QColor("white"));
  setPalette(palette);

  // configure  pens and brushes.
  _bluePen.setColor(getQColor(SKEWT_BLUE));
  _blueBrush.setColor(getQColor(SKEWT_BLUE));
  _blueBrush.setStyle(Qt::SolidPattern);

  _redPen.setColor(getQColor(SKEWT_RED));
  _redBrush.setColor(getQColor(SKEWT_RED));
  _redBrush.setStyle(Qt::SolidPattern);

  _blackPen.setColor(getQColor(SKEWT_BLACK));
  _blackBrush.setColor(getQColor(SKEWT_BLACK));
  _blackBrush.setStyle(Qt::SolidPattern);

  // connect the resize timer to its timeout slot.
  connect( &_resizeTimer, SIGNAL(timeout()), this, SLOT(resizeTimeout()) );

  // enable the display of QWidget.
  this->show();

}

//////////////////////////////////////////////////////////////////////
SkewTAdapterQt::~SkewTAdapterQt()
{ 
  for (unsigned int i = 0; i < _pLines.size(); i++)
    delete _pLines[i];
}

//////////////////////////////////////////////////////////////////////
void
SkewTAdapterQt::line(double x1, double y1, double x2, double y2, unsigned int colorCode, SkewTAdapter::LineType lineType)
{

  // create polyline elements from the specified line segments. Save the polyline, and draw it.

  // The incoming segments are examined. If the color and line type don't change, and the end of
  // one segment matches the beginning of the next segment, the segments are consolidated into
  // a single polyline.

  double x[2];
  double y[2];
  x[0] = x1;
  x[1] = x2;
  y[0] = y1;
  y[1] = y2;

  if (_firstLineCall) {
    _firstLineCall = false;
    _xvals.push_back(x1);
    _yvals.push_back(y1);
    _lastColorCode = colorCode;
  } else {
    if ((_lastX2 != x1) || (_lastY2 != y1) || colorCode != _lastColorCode) {

      _xvals.push_back(_lastX2);
      _yvals.push_back(_lastY2);

      QPen qpen(getQColor(colorCode), 1);
      SkewTQtPolyline* pl = new SkewTQtPolyline(_xvals, _yvals, qpen);
      _pLines.push_back(pl);

      int h = height();
      int w = width();

      _xvals.clear();
      _yvals.clear();
      _xvals.push_back(x1);
      _yvals.push_back(y1);
      _lastColorCode = colorCode;
    } else {
      _xvals.push_back(x1);
      _yvals.push_back(y1);
    }
  }
  _lastX2 = x2;
  _lastY2 = y2;


}

//////////////////////////////////////////////////////////////////////
void
SkewTAdapterQt::Text(const std::string &s, const double x, const double y, unsigned int colorCode)
{

  if (s.size() == 0)
    return;

  const char* label = s.c_str();

  // set the alignment, if the alignment codes are given as the
  // first two characters of the text. They can be : |r, |c or |l
  int alignFlag = Qt::AlignHCenter | Qt::AlignVCenter;;
  if (s[0] == '|') {
    // alignment flags can be any combination of
    // AlignLeft, AlignRight, AlignTop, AlignBottom, AlignHCenter, AlignVCenter, AlignCenter
    // we have a formatting code at the beginning
    if (s.size() == 1)
      return;
    // we have the alignment codes, so skip past them for the labelling.
    label += 2;
    switch (s[1]) {
    case 'r':
      alignFlag = Qt::AlignRight | Qt::AlignVCenter;
      break;
    case 'c':
      alignFlag = Qt::AlignHCenter | Qt::AlignVCenter;
      break;
    case 'l':
      alignFlag = Qt::AlignLeft | Qt::AlignVCenter;
      break;
    default:
      break;
    }
  }

  // set the label color
  QPen qpen(getQColor(colorCode), 1);

  // create the text graphic element
  std::string slabel(label);
  SkewTQtText t(slabel, x, y, qpen, alignFlag);

  // save it
  _texts.push_back(t);

  int h = height();
  int w = width();

}

//////////////////////////////////////////////////////////////////////
void
SkewTAdapterQt::init()
{
  for (unsigned int i = 0; i < _pLines.size(); i++)
    delete _pLines[i];

  _pLines.clear();

  _tdryPoints.clear();

  _dpPoints.clear();

  _texts.clear();

  _symbols.clear();

  // This can only happen in a paintEvent() as of Qt4.
  // this->erase();

  _ready = false;

}

//////////////////////////////////////////////////////////////////////
void
SkewTAdapterQt::setSkewT(SkewT* pSkewT)
{
  _pSkewT = pSkewT;
}

//////////////////////////////////////////////////////////////////////
void SkewTAdapterQt::addTdry(double x, double y)
{

  // create the datum
  SkewTQtDatum t(x, y, _symbolSize, _redPen, _redBrush);

  // save it
  _tdryPoints.push_back(t);

  int h = height();
  int w = width();

}

//////////////////////////////////////////////////////////////////////
void SkewTAdapterQt::addDp(double x, double y)
{
  // create the datum
  SkewTQtDatum t(x, y, _symbolSize, _bluePen, _blueBrush);

  // save it
  _dpPoints.push_back(t);

}

//////////////////////////////////////////////////////////////////////
void
SkewTAdapterQt::drawElements()
{

  // redraw all of the graphic elements.

  int h = height();
  int w = width();

  QPainter painter(this);

  _title.draw(painter, w, w);

  _subTitle.draw(painter, w, h);

  for (unsigned int i = _pLines._int1; i < _pLines.size(); i++) {
    _pLines[i]->draw(painter,w, h);
    _pLines._int1 = _pLines.size();
  }

  for (unsigned int j = _tdryPoints._int1; j < _tdryPoints.size(); j++) {
    _tdryPoints[j].draw(painter, w, h);
    _tdryPoints._int1 = _tdryPoints.size();
  }

  for (unsigned int d = _dpPoints._int1; d < _dpPoints.size(); d++) {
    _dpPoints[d].draw(painter, w, h);
    _dpPoints._int1 = _dpPoints.size();
  }

  for (unsigned int t = _texts._int1; t < _texts.size(); t++) {
    _texts[t].draw(painter, w, h);
    _texts._int1 = _texts.size();
  }

  for (unsigned int s = _symbols._int1; s < _symbols.size(); s++) {
    _symbols[s].draw(painter, w, h);
    _symbols._int1 = _symbols.size();
  }

  painter.end();

}

//////////////////////////////////////////////////////////////////////
void
SkewTAdapterQt::draw_finished()
{

  _ready = true;

}

//////////////////////////////////////////////////////////////////////
void
SkewTAdapterQt::maximize()
{
}

//////////////////////////////////////////////////////////////////////
void
SkewTAdapterQt::unzoom()
{
  init();
}

//////////////////////////////////////////////////////////////////////
void
SkewTAdapterQt::extents(double &xmin, double &xmax, double &ymin, double &ymax)
{
  xmin = ((double) _zoomStart.x()/(double)width());
  ymin = ((double) (height()-_zoomStart.y())/(double)height());
  xmax = ((double) _zoomStop.x()/(double)width());
  ymax = ((double) (height()-_zoomStop.y())/(double)height());
  if (ymax < ymin) {
    double temp = ymax;
    ymax = ymin;
    ymin = temp;
  }
  if (xmax < xmin) {
    double temp = xmax;
    xmax = xmin;
    xmin = temp;
  }

}


//////////////////////////////////////////////////////////////////////
void
SkewTAdapterQt::symbol(double x, double y, unsigned int colorCode, SymType st)
{
  // create the datum
  SkewTQtDatum t(x, y, _symbolSize, _bluePen, _blueBrush);

  // save it
  _symbols.push_back(t);

}

//////////////////////////////////////////////////////////////////////
void
SkewTAdapterQt::title(std::string s)
{
  _title = SkewTQtText(s, 0.50, 0.98, _bluePen, Qt::AlignHCenter);
}

//////////////////////////////////////////////////////////////////////
void
SkewTAdapterQt::subTitle(std::string s)
{
  _subTitle = SkewTQtText(s, 0.50, 0.94, _bluePen, Qt::AlignHCenter);
}

//////////////////////////////////////////////////////////////////////
double
SkewTAdapterQt::aspectRatio()
{
  int h = height();
  int w  = width();

  if (w == 0)
    return 0.0;

  return ((double)h)/((double) w);
}

//////////////////////////////////////////////////////////////////////
void
SkewTAdapterQt::markPoints(bool flag)
{
}

//////////////////////////////////////////////////////////////////////
void
SkewTAdapterQt::savePlot(std::string path, int xPixels, int yPixels, PlotFileType fileType)
{
}

//////////////////////////////////////////////////////////////////////
QColor
SkewTAdapterQt::getQColor(unsigned int colorCode) {

  QColor qcolor;
  switch (colorCode) {
  case SKEWT_GREY:
    qcolor.setNamedColor("grey75");
    break;

  case SKEWT_RED:
    qcolor.setNamedColor("red");
    break;

  case SKEWT_GREEN:
    qcolor.setNamedColor("green");
    break;

  case SKEWT_BLUE:
    qcolor.setNamedColor("blue");
    break;

  default:
  case SKEWT_BLACK:
    qcolor.setNamedColor("black");
    break;

  }

  return qcolor;
}

//////////////////////////////////////////////////////////////////////
void
SkewTAdapterQt::resizeEvent( QResizeEvent *e )
{
  // set the dontPaint flag, to prohibit repaints until the resizing is finished.
  _dontPaint = true;

  // start the timer, so that we can figure out when resizing has finished.
  _resizeTimer.setSingleShot(true);
  _resizeTimer.start(_resizeHoldOffMs);
}

//////////////////////////////////////////////////////////////////////
void
SkewTAdapterQt::resizeTimeout()
{
  // The resize timer finally timed out, so the resizing must be done.
  // Enable painting, and trigger a paint event via update().
  _dontPaint = false;

  update();
}


//////////////////////////////////////////////////////////////////////
void
SkewTAdapterQt::paintEvent( QPaintEvent *e )
{
  // if painting is allowed, redraw the whole thing.
  if (!_dontPaint)
    if (_ready)
      drawElements();
}

//////////////////////////////////////////////////////////////////////
void
SkewTAdapterQt::mousePressEvent( QMouseEvent* e)
{
  _zoomStart = _zoomStop = e->pos();
}

//////////////////////////////////////////////////////////////////////
void
SkewTAdapterQt::mouseReleaseEvent( QMouseEvent* e )
{
  _zoomStop = e->pos();

  if( _zoomStart.x() != _zoomStop.x() && _zoomStart.y() != _zoomStop.y() ) {
    QRubberBand rb(QRubberBand::Rectangle, this);

    drawBoundingRect( &rb, _zoomStart, _zoomStop );

    if (_pSkewT)
      _pSkewT->zoomin();

  } else {

    if (_pSkewT)
      _pSkewT->unzoom();

  }
}
//////////////////////////////////////////////////////////////////////
void
SkewTAdapterQt::mouseMoveEvent( QMouseEvent* e )
{
  QRubberBand rb(QRubberBand::Rectangle, this);

  if( _zoomStart.x() != _zoomStop.x() && _zoomStart.y() != _zoomStop.y() )
    drawBoundingRect( &rb, _zoomStart, _zoomStop );

  _zoomStop = e->pos();

  drawBoundingRect( &rb, _zoomStart, _zoomStop );
}

//////////////////////////////////////////////////////////////////////
void
SkewTAdapterQt::drawBoundingRect( QRubberBand* rb, const QPoint& p1, const QPoint& p2 )
{
  rb->setGeometry(
    QRect( QPoint( qMin( p1.x(), p2.x() ), qMin( p1.y(), p2.y() ) ),
	   QPoint( qMax( p1.x(), p2.x() ), qMax( p1.y(), p2.y() ) ) ) );

}

//////////////////////////////////////////////////////////////////////
void
SkewTAdapterQt::removeElements()
{

  for (unsigned int i = 0; i < _pLines.size(); i++)
    delete _pLines[i];
  _pLines.clear();

  _tdryPoints.clear();

  _dpPoints.clear();

  _symbols.clear();

  _texts.clear();

}

//////////////////////////////////////////////////////////////////////
void
SkewTAdapterQt::print() {

	QPrinter printer;
	print(&printer);
}

//////////////////////////////////////////////////////////////////////
void
SkewTAdapterQt::print(QPrinter* printer) {
	QPixmap pixmap(width(), height());
	pixmap = QPixmap::grabWidget(this);

	if (pixmap.isNull()) {
		return;
	}

    double ScreenToPrinterX = (double)printer->logicalDpiX() /
                                (double)logicalDpiX();
    double ScreenToPrinterY = (double)printer->logicalDpiY() /
                                (double)logicalDpiY();

    // Printer image dimensions
    double pvw = ScreenToPrinterX * pixmap.width();
    double pvh = ScreenToPrinterY * pixmap.height();

    // Printer Page Size
    double wPage = (double)printer->width();
    double hPage = (double)printer->height();

    // Scale to fit the page
    if ( pvw > wPage )
    {
      qWarning( "Scaling width to fit" );
      double scale = wPage / pvw;
      pvw = wPage;
      pvh *= scale;
    }
    if ( pvh > hPage )
    {
      qWarning( "Scaling height to fit" );
      double scale = hPage / pvh;
      pvh = hPage;
      pvw *= scale;
    }

    // Center on the page
    double pvx = ( wPage - pvw ) / 2.0;
    double pvy = ( hPage - pvh ) / 2.0;

    QPainter p( printer );
    p.setWindow( 0, 0, pixmap.width(), pixmap.height() );
    p.setViewport( (int)floor(pvx), (int)floor(pvy),
		   (int)floor(pvw), (int)floor(pvh) );
    p.drawPixmap( 0, 0, pixmap );

}

//////////////////////////////////////////////////////////////////////

SkewTAdapterQt::
SkewTQtPolyline::SkewTQtPolyline(std::vector<double> x, std::vector<double> y, QPen pen):
  _x(x),
  _y(y),
  _pen(pen)
{

}


//////////////////////////////////////////////////////////////////////

SkewTAdapterQt::
SkewTQtPolyline::~SkewTQtPolyline()
{
}


//////////////////////////////////////////////////////////////////////
void
SkewTAdapterQt::
SkewTQtPolyline::draw(QPainter& painter, int width, int height)
{

  // draw a polyline, using our collected points.
  int nPoints = _x.size();
  QPolygon points = QPolygon(nPoints);

  for (int i = 0; i < nPoints; i++) {
    int x = (int)(width * _x[i]);
    int y = (int)(height - height*_y[i]);
    points.setPoint(i, x, y);
  }

  // set the pen
  painter.setPen(_pen);

  // draw it
  painter.drawPolyline(points);
}

//////////////////////////////////////////////////////////////////////

SkewTAdapterQt::
SkewTQtText::SkewTQtText(std::string text, double x, double y, QPen pen, int alignFlag):
  _text(text),
  _x(x),
  _y(y),
  _pen(pen),
  _alignFlag(alignFlag)
{
}


//////////////////////////////////////////////////////////////////////

SkewTAdapterQt::
SkewTQtText::SkewTQtText()
{
}


//////////////////////////////////////////////////////////////////////

SkewTAdapterQt::
SkewTQtText::~SkewTQtText()
{
}

//////////////////////////////////////////////////////////////////////
void
SkewTAdapterQt::
SkewTQtText::draw(QPainter& painter, int width, int height)
{
  QString text(_text.c_str());

  QFontMetrics m(painter.font());
  QSize s = m.size(0, text);

  int x = (int)(width * _x);
  int y = (int)(height - height*_y);


  // adjust according to the alignment flags
  //  if (_alignFlag & Qt::AlignLeft) This is the default
  //  if (_alignFlag & Qt::AlignBottom) This is the default
  if (_alignFlag & Qt::AlignRight)
    x -= s.width();
  if (_alignFlag & Qt::AlignHCenter || _alignFlag & Qt::AlignCenter)
    x -= s.width()/2;
  if (_alignFlag & Qt::AlignTop)
    y -= s.height();
  if (_alignFlag & Qt::AlignVCenter || _alignFlag & Qt::AlignCenter)
    y -= s.height()/2;

  QRect rect(QPoint(x, y), s);

  painter.setPen(_pen);
  painter.drawText(x, y, text);
}

//////////////////////////////////////////////////////////////////////

SkewTAdapterQt::
SkewTQtDatum::SkewTQtDatum(double x, double y, int size, QPen pen, QBrush brush):
  _x(x),
  _y(y),
  _size(size),
  _pen(pen),
  _brush(brush)
{
}


//////////////////////////////////////////////////////////////////////

SkewTAdapterQt::
SkewTQtDatum::~SkewTQtDatum()
{
}


//////////////////////////////////////////////////////////////////////
void
SkewTAdapterQt::
SkewTQtDatum::draw(QPainter& painter, int w, int h)
{

  int x = (int)(w * _x - _size/2);
  int y = (int)(h - h*_y - _size/2);

  painter.setPen(_pen);
  painter.setBrush(_brush);
  painter.drawEllipse(x, y, _size, _size);
}

