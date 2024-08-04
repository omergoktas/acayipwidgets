#include <acayipglobal.h>

#include <private/qhighdpiscaling_p.h>
#include <qpa/qplatformscreen.h>

#include <QGuiApplication>
#include <QScreen>
#include <QWidget>

using namespace Qt::Literals;

ACAYIPWIDGETS_BEGIN_NAMESPACE

QColor Utils::strongerColor(QColor c, int strength)
{
    if (c.alpha() <= 255 - strength)
        c.setAlpha(c.alpha() + strength);
    else if (c.value() <= 255 - strength)
        c.setHsv(c.hue(), c.saturation(), c.value() + strength, c.alpha());
    else
        c.setHsv(c.hue(), c.saturation(), c.value() - strength, c.alpha());
    return c;
}

qreal Utils::scaled(const QScreen* screen, qreal value, qreal multiply)
{
    Q_ASSERT_X(screen && screen->handle(), "AcayipWidgets", "null pointer pased");
    if (!screen || !screen->handle())
        return value;
    QDpi dpi1(QHighDpiScaling::logicalDpi(screen));
    QDpi dpi2(screen->handle()->logicalBaseDpi());
    qreal platformFactor = (dpi1.first / dpi2.first + dpi1.second / dpi2.second) / 2.0;
    return value * platformFactor * Defaults::extraScaleFactor * multiply;
}

int Utils::scaled(const QScreen* screen, int value, qreal multiply, bool roundUp)
{
    return roundUp ? qCeil(scaled(screen, qreal(value), multiply))
                   : qFloor(scaled(screen, qreal(value), multiply));
}

int Utils::scaled(
    const QScreen* screen, int value, int initialValue, qreal multiply, bool roundUp)
{
    qreal initializer = multiply / scaled(screen, 1.0);
    qreal sps(scaled(screen, qreal(value), initializer));
    int ips = initialValue;
    if (qAbs(sps - ips) <= 1.0)
        return scaled(screen, ips, 1.0, roundUp);
    else
        return scaled(screen, value, multiply, roundUp);
}

QPointF Utils::scaled(const QScreen* screen, const QPointF& point, qreal multiply)
{
    return point * scaled(screen, 1.0, multiply);
}

QPoint Utils::scaled(const QScreen* screen, const QPoint& point, qreal multiply)
{
    return QPoint(scaled(screen, point.x(), multiply),
                  scaled(screen, point.y(), multiply));
}

QPoint Utils::scaled(const QScreen* screen,
                     const QPoint& point,
                     const QPoint& initialPoint,
                     qreal multiply)
{
    qreal initializer = multiply / scaled(screen, 1.0);
    QPointF sps(scaled(screen, QPointF(point), initializer));
    const QPoint& ips = initialPoint;
    if (qAbs(sps.x() - ips.x()) <= 1.0 && qAbs(sps.y() - ips.y()) <= 1.0)
        return scaled(screen, ips);
    else
        return scaled(screen, point, multiply);
}

QSizeF Utils::scaled(const QScreen* screen, const QSizeF& size, qreal multiply)
{
    return size * scaled(screen, 1.0, multiply);
}

QSize Utils::scaled(const QScreen* screen, const QSize& size, qreal multiply)
{
    return QSize(scaled(screen, size.width(), multiply),
                 scaled(screen, size.height(), multiply));
}

QSize Utils::scaled(const QScreen* screen,
                    const QSize& size,
                    const QSize& initialSize,
                    qreal multiply)
{
    qreal initializer = multiply / scaled(screen, 1.0);
    QSizeF sps(scaled(screen, QSizeF(size), initializer));
    const QSize& ips = initialSize;
    if (qAbs(sps.width() - ips.width()) <= 1.0
        && qAbs(sps.height() - ips.height()) <= 1.0) {
        return scaled(screen, ips);
    } else {
        return scaled(screen, size, multiply);
    }
}

QRectF Utils::scaled(const QScreen* screen, const QRectF& rect, qreal multiply)
{
    return QRectF(scaled(screen, rect.topLeft(), multiply),
                  scaled(screen, rect.size(), multiply));
}

QRect Utils::scaled(const QScreen* screen, const QRect& rect, qreal multiply)
{
    return QRect(scaled(screen, rect.topLeft(), multiply),
                 scaled(screen, rect.size(), multiply));
}

QRect Utils::scaled(const QScreen* screen,
                    const QRect& rect,
                    const QRect& initialRect,
                    qreal multiply)
{
    qreal initializer = multiply / scaled(screen, 1.0);
    QRectF sps(scaled(screen, QRectF(rect), initializer));
    const QRect& ips = initialRect;
    if (qAbs(sps.x() - ips.x()) <= 1.0
        && qAbs(sps.y() - ips.y()) <= 1.0
        && qAbs(sps.width() - ips.width()) <= 1.0
        && qAbs(sps.height() - ips.height()) <= 1.0) {
        return scaled(screen, ips);
    } else {
        return scaled(screen, rect, multiply);
    }
}

QMarginsF Utils::scaled(const QScreen* screen, const QMarginsF& margins, qreal multiply)
{
    return margins * scaled(screen, 1.0, multiply);
}

QMargins Utils::scaled(const QScreen* screen, const QMargins& margins, qreal multiply)
{
    return QMargins(scaled(screen, margins.left(), multiply),
                    scaled(screen, margins.top(), multiply),
                    scaled(screen, margins.right(), multiply),
                    scaled(screen, margins.bottom(), multiply));
}

QMargins Utils::scaled(const QScreen* screen,
                       const QMargins& margins,
                       const QMargins& initialPaddings,
                       qreal multiply)
{
    qreal initializer = multiply / scaled(screen, 1.0);
    QMarginsF sps(scaled(screen, QMarginsF(margins), initializer));
    const QMargins& ips = initialPaddings;
    if (qAbs(sps.left() - ips.left()) <= 1.0
        && qAbs(sps.top() - ips.top()) <= 1.0
        && qAbs(sps.right() - ips.right()) <= 1.0
        && qAbs(sps.bottom() - ips.bottom()) <= 1.0) {
        return scaled(screen, ips);
    } else {
        return scaled(screen, margins, multiply);
    }
}

QFont Utils::scaled(const QScreen* screen, const QFont& font, qreal multiply)
{
    QFont copy(font);
    if (copy.pixelSize() != -1)
        copy.setPixelSize(scaled(screen, copy.pixelSize(), multiply));
    return copy;
}

QFont Utils::scaled(const QScreen* screen,
                    const QFont& font,
                    const QFont& initialFont,
                    qreal multiply)
{
    QFont copy(font);
    if (copy.pixelSize() != -1) {
        qreal initializer = multiply / scaled(screen, 1.0);
        qreal sps = scaled(screen, qreal(font.pixelSize()), initializer);
        int ips = initialFont.pixelSize();
        if (qAbs(sps - ips) <= 1.0)
            copy.setPixelSize(scaled(screen, ips));
        else
            copy.setPixelSize(scaled(screen, copy.pixelSize(), multiply));
    }
    return copy;
}

QPen Utils::scaled(const QScreen* screen, const QPen& pen, qreal multiply)
{
    QPen copy(pen);
    copy.setWidthF(scaled(screen, copy.widthF(), multiply));
    return copy;
}

ACAYIPWIDGETS_END_NAMESPACE
