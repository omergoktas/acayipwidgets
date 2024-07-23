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

qreal Utils::scaled(const QScreen* screen, qreal value, qreal old)
{
    Q_ASSERT_X(screen && screen->handle(), "AcayipWidgets", "null pointer pased");
    if (!screen || !screen->handle())
        return value;
    QDpi dpi1(QHighDpiScaling::logicalDpi(screen));
    QDpi dpi2(screen->handle()->logicalBaseDpi());
    qreal platformFactor = (dpi1.first / dpi2.first + dpi1.second / dpi2.second) / 2.0;
    return value * platformFactor * Defaults::scaleFactor * (1.0 / old);
}

int Utils::scaled(const QScreen* screen, int value, qreal old, bool roundUp)
{
    return roundUp ? qCeil(scaled(screen, qreal(value), old))
                   : qFloor(scaled(screen, qreal(value), old));
}

QPointF Utils::scaled(const QScreen* screen, const QPointF& point, qreal old)
{
    return point * scaled(screen, 1.0, old);
}

QPoint Utils::scaled(const QScreen* screen, const QPoint& point, qreal old)
{
    return QPoint(scaled(screen, point.x(), old), scaled(screen, point.y(), old));
}

QSizeF Utils::scaled(const QScreen* screen, const QSizeF& size, qreal old)
{
    return size * scaled(screen, 1.0, old);
}

QSize Utils::scaled(const QScreen* screen, const QSize& size, qreal old)
{
    return QSize(scaled(screen, size.width(), old), scaled(screen, size.height(), old));
}

QRectF Utils::scaled(const QScreen* screen, const QRectF& rect, qreal old)
{
    return QRectF(scaled(screen, rect.topLeft(), old), scaled(screen, rect.size(), old));
}

QRect Utils::scaled(const QScreen* screen, const QRect& rect, qreal old)
{
    return QRect(scaled(screen, rect.topLeft(), old), scaled(screen, rect.size(), old));
}

QMarginsF Utils::scaled(const QScreen* screen, const QMarginsF& margins, qreal old)
{
    return margins * scaled(screen, 1.0, old);
}

QMargins Utils::scaled(const QScreen* screen, const QMargins& margins, qreal old)
{
    return QMargins(scaled(screen, margins.left(), old),
                    scaled(screen, margins.top(), old),
                    scaled(screen, margins.right(), old),
                    scaled(screen, margins.bottom(), old));
}

QFont Utils::scaled(const QScreen* screen, const QFont& font, qreal old)
{
    QFont copy(font);
    if (copy.pixelSize() == -1)
        copy.setPointSizeF(scaled(screen, copy.pointSizeF(), old));
    else
        copy.setPixelSize(scaled(screen, copy.pixelSize(), old));
    return copy;
}

ACAYIPWIDGETS_END_NAMESPACE
