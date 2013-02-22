#ifndef PLOTLINE_H
#define PLOTLINE_H

#include <QDeclarativeItem>
#include <QPainter>
#include <QMap>
#include <functional>

#include "math.h"

using namespace std;

struct Channel : public QSharedData {
    enum Type {
        InvalidChannelType,
        ScopeChannelType,
        LogicChannelType,
        MathChannelType
    };
    typedef QPair<int, float> Point;

    Channel(const Channel& other )
     : QSharedData(other)
     , channelType(other.channelType)
     , data(other.data) { };

    Channel()
     : channelType(InvalidChannelType) { };

    ~Channel() { };

    // should return a "unit'd" value of the index,
    // i.e. transform "ADC sample #32" to "67.5ns"
    float indexToFloat(const int index) const {
        return index;
    };

    Type channelType;
    QMap<int, float> data;
    QRectF dataRange;
};

class Plotline : public QDeclarativeItem
{
    Q_OBJECT
    Q_PROPERTY(QRectF dataRange READ getDataRange WRITE setDataRange NOTIFY dataRangeChanged);
    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged);
    Q_PROPERTY(int penWidth READ penWidth WRITE setPenWidth NOTIFY penWidthChanged);

private:
    QSharedDataPointer<Channel> data;

public:
    Plotline(QDeclarativeItem *parent = 0) :
        QDeclarativeItem(parent),
        m_color(Qt::black), m_penWidth(1)
    {
        // Important, otherwise the paint method is never cdelayalled
        setFlag(QGraphicsItem::ItemHasNoContents, false);

        data = new Channel();
        for ( int i = 0; i < 3000; i++ ) {
            data->data[i] = sin(i / 200.0) * 100;
        }
        dataRange = QRectF(0, -150, 1000, 300);

        connect(this, SIGNAL(dataChanged()), SLOT(slotDataChanged()));
    }

    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
    {
        QColor paintColor = m_color;
        QPen pen(paintColor, m_penWidth);
        QBrush brush(m_color);
        painter->setPen(pen);
        painter->setBrush(brush);

        painter->setRenderHint(QPainter::Antialiasing, true);

        m_color.setAlpha(5);
        painter->drawRect(x(), y(), width(), height());

        m_color.setAlpha(127);

        const QMap<int, float>& points = data->data;
        std::function<float(float)> transformYValue = [&](float yValue) -> float {
            return -(yValue - dataRange.bottom()) * height() / dataRange.height();
        };
        std::function<float(float)> transformXValue = [&](float xValue) -> float {
            return (xValue - dataRange.left()) * width() / dataRange.width();
        };
        foreach ( const int key, points.keys() ) {
            // TODO slow, could do interval-halving for monotonous x coords
            const float x = data->indexToFloat(key);
            if ( dataRange.left() < x && dataRange.right() > x ) {
                QPointF start(transformXValue(x), transformYValue(points[x]));
                QPointF end(transformXValue(data->indexToFloat(key+1))-dataRange.left(), transformYValue(points[x+1]));
                painter->drawLine(start, end);
            }
        }
    }

    QRectF getDataRange() const { return dataRange; }
    QColor color() const { return m_color; }
    int penWidth() const { return m_penWidth; }

    void setColor(const QColor &color) {
        if(m_color == color) return;
        m_color = color;
        emit colorChanged();
        update();
    }

    void setDataRange(const QRectF& newDataRange) {
        dataRange = newDataRange;
        emit dataRangeChanged();
        update();
    }

    void setPenWidth(int newWidth) {
        if(m_penWidth == newWidth) return;
        m_penWidth = newWidth;
        emit penWidthChanged();
        update();
    }

signals:
    void colorChanged();
    void penWidthChanged();
    void dataRangeChanged();
    void dataChanged();

public slots:
    void slotDataChanged() {
        update();
    };

protected:

    QColor m_color;
    int m_penWidth;
    QRectF dataRange;
};

QML_DECLARE_TYPE(Plotline)

#endif // LINE_H
