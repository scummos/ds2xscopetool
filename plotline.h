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
        MiscChannelType,
        ScopeChannelType,
        LogicChannelType,
        MathChannelType
    };
    enum Axis {
        HorizontalAxis,
        VerticalAxis
    };
    enum TransformationKind {
        Scale,
        Movement,
        Rotation // ;)
    };
    typedef QPair<int, float> Point;

    Channel(const Channel& other )
     : QSharedData(other)
     , channelType(other.channelType)
     , data(other.data) { };

    Channel()
     : channelType(MiscChannelType) { };

    ~Channel() { };

    // should return a "unit'd" value of the index,
    // i.e. transform "ADC sample #32" to "67.5ns"
    float indexToFloat(const int index) const {
        return index;
    };

    Type channelType;
    QMap<int, float> data;
};

class PlotLine : public QDeclarativeItem
{
    Q_OBJECT
    Q_PROPERTY(QRectF dataRange READ getDataRange WRITE setDataRange NOTIFY dataRangeChanged);
    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged);
    Q_PROPERTY(int penWidth READ penWidth WRITE setPenWidth NOTIFY penWidthChanged);

public:
    QSharedDataPointer<Channel> data;
    bool enabled;

public:
    PlotLine(QDeclarativeItem *parent = 0)
        : QDeclarativeItem(parent)
        , m_color(Qt::black)
        , m_penWidth(1)
        , dataRange(QRectF(-0.5, 0, 1000, 1.0))
        , channelNumber(0)
    {
        // Important, otherwise the paint method is never called
        setFlag(QGraphicsItem::ItemHasNoContents, false);

        data = new Channel();

        connect(this, SIGNAL(dataChanged()), SLOT(slotDataChanged()));
    }

    void paint(QPainter* painter, const QStyleOptionGraphicsItem* /*option*/, QWidget* /*widget*/)
    {
        if ( ! enabled ) {
            return;
        }
        QColor paintColor = m_color;
        QPen pen(paintColor, m_penWidth);
        painter->setPen(pen);

        painter->setRenderHint(QPainter::Antialiasing, true);

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
                QPointF end(transformXValue(data->indexToFloat(key+1)), transformYValue(points[x+1]));

                std::function<float(float)> clipTop = [&](float yvalue) -> float {
                    return qMin(static_cast<float>(yvalue), transformYValue(dataRange.top())-1);
                };
                std::function<float(float)> clipBottom = [&](float yvalue) -> float {
                    return qMax(static_cast<float>(yvalue), transformYValue(dataRange.bottom()));
                };

                // don't clip, but instead draw lines at the corners
                start.setY(clipTop(start.y()));
                end.setY(clipTop(end.y()));
                start.setY(clipBottom(start.y()));
                end.setY(clipBottom(end.y()));
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

    void notifyDataChanged() {
        emit dataChanged();
    }

    int getChannelNumber() const {
        return channelNumber;
    }

    void setChannelNumber(int channelNumber_) {
        channelNumber = channelNumber_;
        setProperty("id", "Channel" + QString::number(channelNumber));
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

private:
    int channelNumber;
};

QML_DECLARE_TYPE(PlotLine)

#endif // LINE_H
