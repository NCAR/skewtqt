# SkewTAdapterQt

An adapter for rendering SkewT as a QWidget.

This implementation of SkewTAdapter draws the SkewT using a Qt Qwidget.
SkewTAdapterQt is derived from both SkewTAdapter and QWidget.
	
The QWidget provides a blank area that the skew-t will be drawn upon. A QPainter performs the
actual drawing. Fortunately, QPainter provides an adequate collection of primitives
for drawing polylines, text, etc.
	
At the heart of this adapter, all drawing activities are broken down into a small
number of graphic elements. These are strictly graphic constructs; SkewTAdapterQt
really has very little (but some) specialized knowledge of what a skew-t graph is.
	
All coordinates are given as a value between 0 and 1. Whenever drawing takes place,
these are converted to screen coordinates through scaling by the QWidget height or
width.
	
One of the nicest features of the Qt model is the builtin support for resizing,
which is handled by resize and paint event handlers. Naturally, to exploit this
dynamic resizing capability, a certain amount of imformation about each graphic
element must be stored, so that the complete plot can be regenerated when the
geometry changes. The elements that are stored within SkewTAdapterQt are:

1. Texts: strings for labelling anything on the graph
1. Polylines: arbitrary lines with specified color and characteristics.
1. These are used for isopleths and any other fixed lines.
1. Tdry points: A collection of temperature values to be plotted as the temperature trace.
1. DP points: A collection of dew point values to be plotted as the dew point trace.

	
SkewTAdapterQt defines some helper classes (SkewTQtText, SkewTQtPolyline, and
SkewTQtDatum) which facilitate the storage and rendering of the graphic elements.
Lists of these elemnts are managed by standard STL containers.

## The Resizing Issue
I ran across a common resizing issue. The skew-t plot, which renders data
in a fairly complex fashion, must expend a significant amount of computation
to regenerate the diagram, when the window is resized. When a window is being
resized, Qt sends pairs of resize/paint events to the SkewTAdapterQt.

Now this would be fine, if Qt only sent the events after the final size of
the widget had been established.  However, during a resize, many events get
sent as the user drags the window border. This is seen to happen
when the SkewtAdapterQt is managed as by a layout, within a workspace widget.

A workaround is employed here, at least until we find a way to eliminate the sending of multiple
paint events. (Trolltech said that they might add this capability to QWorkspace for us, in a
post Qt 4.0 version!).

The technique uses a timer and a "dontPaint" flag. If the dontPaint flag is true, a paint event will
not redraw the display. Every time a resize event occurs, dontPaint is set true, and the timer is started.
The dontPaint flag will continue to be set true as long as resize events are received before the timer
run out. When the timer finally is able to time out, dontPaint is returned to its false state, and a redraw is
initiated.

SkewTAdapterQt provides zooming support. When it detects a mouse press it captures the starting position.
Then it draws a box as the mouse is dragged. When the mouse is released, the stop position is recorded,
and then calls the SkewT::zoomin() function. Thus, this adapter does know about the SkewT.
We could have built an opaque interface to hide SkewT from the adapter, but it doesn't seem necessary.

## Todo 
* Investigate whether QCanvas would be a better graphics component for this application.
* Find out if a QPainter can be interrogated for the device limits (width and height). It seems
foolish to have to send a QPainter, and width and height, into the draw functions.
* The paint event handler gets called no fewer than 4 times during the initial construction, and this is after
all of the graphic elements have been defined. Since draw_finished() is called from paintEvent().
the complete plot is drawn four times in a row. The drawing procedure should be modified, to draw into a
QPixmap, and then just have paintEvent() bitBlt this pixmap.
	
