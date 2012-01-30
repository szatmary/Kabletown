/*
    (C) 2012 Matthew Szatmary <szatmary@gmail.com>
    This work is licensed under the Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License.
    To view a copy of this license, visit http://creativecommons.org/licenses/by-nc-sa/3.0/ or send a letter to
    Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.
*/

#ifndef GRAPH_H
#define GRAPH_H

#include <QPen>
#include <QRect>
#include <QBrush>
#include <QWidget>
#include <QPalette>

class GraphLine : public QWidget
{
    Q_OBJECT
public:
    GraphLine();

    void setHidden(bool hidden);
    bool isHidden() const;

    const QPen &pen() const;
    void setPen(const QPen &pen);

    const QBrush &brush() const;
    void setBrush(const QBrush &brush);

    void addPoint(qreal x, qreal y);
    void clear();
signals:
    void modified();
private:
    friend class Graph;
    bool           m_hidden;
    QPen           m_pen;
    QBrush         m_brush;
    QList<QPointF> m_points;
};


// TODO change this to a QImage or something that is eaiser to save to a PNG
class Graph : public QWidget
{
    Q_OBJECT
public:
    explicit Graph(QWidget *parent = 0);
    void setRange(qreal bottom = 0, qreal left = 0, qreal top = 100, qreal right = 100);
    void clear();
    GraphLine *addLine();

    void setVLabels(QStringList &labels);
    void setVLabels(int start, int end, int step = 1);
    void setHLabels(QStringList &labels);
    void setHLabels(int start, int end, int step = 1);
protected:
    void paintEvent(QPaintEvent *);
private:
    int vLabelMaxWidth();
    int hLabelWidth(int index);
    int vLabelWidth(int index);
    const QPointF scaleToRange(const QPointF &p);

    QStringList         m_vLabels;
    QStringList         m_hLabels;
    QRect               m_graphView;
    QRectF              m_range;
    QVector<GraphLine*> m_lines;
};

#endif // GRAPH_H
