#include "graph.h"

#include <QPainter>

GraphLine::GraphLine()
: m_hidden(false)
{}

const QPen &GraphLine::pen() const { return m_pen; }
void GraphLine::setPen(const QPen &pen){ m_pen = pen; emit modified(); }
const QBrush &GraphLine::brush() const { return m_brush; }
void GraphLine::setBrush(const QBrush &brush) { m_brush = brush; emit modified(); }

void GraphLine::setHidden(bool hidden) { m_hidden = hidden; emit modified(); }
bool GraphLine::isHidden() const { return m_hidden; }

void GraphLine::addPoint(qreal x, qreal y)
{
    m_points.append( QPointF(x,y) );
    emit modified();
}

void GraphLine::clear()
{
    m_points.clear();
    emit modified();
}


Graph::Graph(QWidget *parent) :
    QWidget(parent)
{
    QPalette p = palette();
    p.setColor(QPalette::Window, Qt::white );
    setPalette( p );
    setAutoFillBackground(true);

    QFont f( font() );
    f.setPointSize( 10 );
    setFont( f );
    setRange();
}

void Graph::setRange(qreal bottom, qreal left, qreal top, qreal right)
{
    // we invert some of teh values, because the Qt coord system is top-left to bottom-right
//    m_range.setCoords(bottom, left, top, right );
    m_range.setCoords(left, top, right, bottom );
    update();
}


void Graph::clear()
{
    foreach( GraphLine *line, m_lines )
        delete line;

    setRange();
    m_vLabels.clear();
    m_hLabels.clear();
    m_lines.clear();
    update();
}

GraphLine *Graph::addLine()
{
    // TODO should I set this to GraphLine's parrent?
    GraphLine *line = new GraphLine();
    m_lines.push_front( line );
//    connect(line, SIGNAL(modified()), this, SLOT(update()));
    return line;
}


void Graph::setVLabels(QStringList &labels)
{
    m_vLabels = labels;
    update();
}

void Graph::setVLabels(int start, int end, int step )
{
    QStringList labels;
    for(int i = start ; i <= end ; i += step)
    {
        labels.push_back( QString().setNum(i) );
    }
    setVLabels(labels);
}

void Graph::setHLabels(QStringList &labels)
{
    m_hLabels = labels;
    update();
}

void Graph::setHLabels(int start, int end, int step )
{
    QStringList labels;
    for(int i = start ; i <= end ; i += step)
    {
        labels.push_back( QString().setNum(i) );
    }
    setHLabels(labels);
}

int Graph::vLabelMaxWidth()
{
    int max = 0;
    QFontMetrics fontMetrics( font() );
    foreach(QString string, m_vLabels)
    {
        max = qMax( max, fontMetrics.width( string ) );
    }
    return max;
}

int Graph::hLabelWidth(int index)
{
    if ( 0 > index )
        index = m_hLabels.count() + index;

    if ( m_hLabels.count() <= index )
        return 0;

    QFontMetrics fontMetrics( font() );
    return fontMetrics.width( m_hLabels.at(index) );
}

int Graph::vLabelWidth(int index)
{
    if ( 0 > index )
        index = m_vLabels.count() + index;

    if ( m_vLabels.count() <= index )
        return 0;

    QFontMetrics fontMetrics( font() );
    return fontMetrics.width( m_vLabels.at(index) );
}

const QPointF Graph::scaleToRange(const QPointF &p)
{
//    return p;
    return QPointF( p.x() - m_range.left(), p.y() - m_range.bottom() );
}

void Graph::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    QFontMetrics fontMetrics( font() );

    // Why would you NOT want to Antialias? (Not rhetorical, I really want to know)
    painter.setRenderHint(QPainter::Antialiasing, true);

    /*
        There are really two views here
        First is the Outer Label View
        Second is the inner graph view

        The inner views left edge is calculated as such:
        outerViewLeftEdge + vLabelMaxWidth + ( 1stHLabelWidth / 2 )

        The inner views right edge is calculated as such:
        outerViewRightEdge - ( LastHLabelWidth / 2 )

        The inner views top edge is calculated as such:
        outerViewTopEdge + ( fontHeight / 2 )

        The inner views bottom edge is calculated as such:
        outerViewBottomEdge - ( fontHeight )

    */
    // debug Lines
//    painter.drawLine(0,0,width(),height());

    // Calculate the graph view (gutters)
    m_graphView.setCoords(0, 0, width(), height() );
    m_graphView.setLeft  ( m_graphView.left  () + vLabelMaxWidth() + ( hLabelWidth(0) / 2.0 ) );
    m_graphView.setRight ( m_graphView.right () - ( hLabelWidth(-1) / 2.0 ) ); // TODO do I need a -1 here?
    m_graphView.setTop   ( m_graphView.top   () + fontMetrics.height() / 2.0 );
    m_graphView.setBottom( m_graphView.bottom() - fontMetrics.height() ); // TODO do I need a -1 here?

    // The first label is centered on the left edge of the m_graphView
    // The last label is centered on teh right edge of the m_graphView
    if ( m_hLabels.count() > 1 )
    {
        double lineWidths = m_graphView.width() / double ( m_hLabels.count() - 1 );
        for(int i = 0 ; i < m_hLabels.count() ; ++i )
        {
            int textWidth = hLabelWidth(i);
            int centerLine = m_graphView.left() + (i * lineWidths);
            painter.drawText( centerLine - (textWidth / 2.0), height(), m_hLabels.at(i) );

            painter.save();
            painter.setPen( QPen( QColor(0xcc,0xcc,0xcc) ));
            painter.drawLine( QPointF(centerLine, m_graphView.bottom()), QPointF(centerLine, m_graphView.top()) );
            painter.restore();
        }
    }

    if ( m_vLabels.count() > 1 )
    {
        int maxWidth = vLabelMaxWidth();
        double lineWidths = m_graphView.height() / double ( m_vLabels.count() - 1 );
        for(int i = 0 ; i < m_vLabels.count() ; ++i )
        {
            int textWidth = vLabelWidth(i);
            int centerLine = m_graphView.bottom() - (i * lineWidths );
            painter.drawText( maxWidth - textWidth, centerLine + ( fontMetrics.height() / 2.0), m_vLabels.at(i) );

            painter.save();
            painter.setPen( QPen( QColor(0xcc,0xcc,0xcc) ));
            painter.drawLine( QPointF(m_graphView.left(), centerLine ), QPointF(m_graphView.right(), centerLine) );
            painter.restore();
        }
    }

    painter.save();
    painter.setClipRect( m_graphView );
    painter.setClipping(true);
    painter.setViewport( m_graphView.left(), m_graphView.bottom(), width(), height() );
    painter.scale( double( m_graphView.width() ) / m_range.width(), double( m_graphView.height() ) / m_range.height() );

    // Draw GraphLines objects
    foreach( GraphLine *line, m_lines )
    {

        if ( line->isHidden() || 2 > line->m_points.count() )
            continue;

        painter.setPen( QPen( Qt::NoPen ) );
        painter.setBrush( line->brush() );

        QPolygonF polygon;
        polygon.append( scaleToRange( QPointF( line->m_points.last ().x(), 0 ) ) );
        polygon.append( scaleToRange( QPointF( line->m_points.first().x(), 0 ) ) );
        for( int i = 0 ; i < line->m_points.count() ; ++i )
            polygon.append( scaleToRange( line->m_points.at(i) ) );
        painter.drawPolygon( polygon );

        painter.setPen( line->pen() );
        QPointF p1 = line->m_points.first();
        for( int i = 1 ; i < line->m_points.count() ; ++i )
        {
            QPointF p2 = line->m_points.at( i );
            painter.setPen  ( line->m_pen   );
            painter.drawLine( scaleToRange( p1 ), scaleToRange( p2 ));
            p1 = p2;
        }
    }

    painter.restore();
}
