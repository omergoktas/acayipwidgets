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
    return value * platformFactor * Defaults::scaleFactor * multiply;
}

int Utils::scaled(const QScreen* screen, int value, qreal multiply, bool roundUp)
{
    return roundUp ? qCeil(scaled(screen, qreal(value), multiply))
                   : qFloor(scaled(screen, qreal(value), multiply));
}

int Utils::scaled(
    const QScreen* screen, int value, int initialValue, qreal multiply, bool roundUp)
{
    int sps = scaled(screen, value, multiply, roundUp);
    int ips = scaled(screen, initialValue, 1.0, roundUp);
    if (qAbs(sps - ips) <= 1)
        return ips;
    else
        return sps;
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
    QPoint sps(scaled(screen, point.x(), multiply), scaled(screen, point.y(), multiply));
    QPoint ips(scaled(screen, initialPoint.x()), scaled(screen, initialPoint.y()));
    if (qAbs(sps.x() - ips.x()) <= 1 && qAbs(sps.y() - ips.y()) <= 1)
        return ips;
    else
        return sps;
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
    QSize sps(scaled(screen, size.width(), multiply),
              scaled(screen, size.height(), multiply));
    QSize ips(scaled(screen, initialSize.width()), scaled(screen, initialSize.height()));
    if (qAbs(sps.width() - ips.width()) <= 1 && qAbs(sps.height() - ips.height()) <= 1)
        return ips;
    else
        return sps;
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
    QRect sps(scaled(screen, rect.topLeft(), multiply),
              scaled(screen, rect.size(), multiply));
    QRect ips(scaled(screen, initialRect.topLeft()), scaled(screen, initialRect.size()));
    if (qAbs(sps.x() - ips.x()) <= 1 && qAbs(sps.y() - ips.y()) <= 1
        && qAbs(sps.width() - ips.width()) <= 1
        && qAbs(sps.height() - ips.height()) <= 1) {
        return ips;
    } else {
        return sps;
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
    QMargins sps(scaled(screen, margins.left(), multiply),
                 scaled(screen, margins.top(), multiply),
                 scaled(screen, margins.right(), multiply),
                 scaled(screen, margins.bottom(), multiply));
    QMargins ips(scaled(screen, initialPaddings.left()),
                 scaled(screen, initialPaddings.top()),
                 scaled(screen, initialPaddings.right()),
                 scaled(screen, initialPaddings.bottom()));
    if (qAbs(sps.left() - ips.left()) <= 1 && qAbs(sps.top() - ips.top()) <= 1
        && qAbs(sps.right() - ips.right()) <= 1
        && qAbs(sps.bottom() - ips.bottom()) <= 1) {
        return ips;
    } else {
        return sps;
    }
}

QFont Utils::scaled(const QScreen* screen, const QFont& font, qreal multiply)
{
    QFont copy(font);
    if (copy.pixelSize() == -1)
        copy.setPointSizeF(scaled(screen, copy.pointSizeF(), multiply));
    else
        copy.setPixelSize(scaled(screen, copy.pixelSize(), multiply));
    return copy;
}

QFont Utils::scaled(const QScreen* screen,
                    const QFont& font,
                    const QFont& initialFont,
                    qreal multiply)
{
    QFont copy(font);
    if (copy.pixelSize() == -1) {
        copy.setPointSizeF(scaled(screen, copy.pointSizeF(), multiply));
    } else {
        int sps = scaled(screen, copy.pixelSize(), multiply);
        int ips = scaled(screen, initialFont.pixelSize());
        if (qAbs(sps - ips) <= 1)
            copy.setPixelSize(qCeil(ips));
        else
            copy.setPixelSize(qCeil(sps));
    }
    return copy;
}

ACAYIPWIDGETS_END_NAMESPACE
