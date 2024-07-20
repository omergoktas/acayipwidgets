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

qreal Utils::scaled(const QScreen* screen, qreal value)
{
    Q_ASSERT_X(screen && screen->handle(), "AcayipWidgets", "null pointer pased");
    if (!screen || !screen->handle())
        return value;
    QDpi dpi1(QHighDpiScaling::logicalDpi(screen));
    QDpi dpi2(screen->handle()->logicalBaseDpi());
    qreal platformFactor = (dpi1.first / dpi2.first + dpi1.second / dpi2.second) / 2.0;
    return value * platformFactor * Defaults::scaleFactor;
}

int Utils::scaled(const QScreen* screen, int value)
{
    return qCeil(scaled(screen, qreal(value)));
}

QSizeF Utils::scaled(const QScreen* screen, const QSizeF& size)
{
    return size * scaled(screen, 1.0);
}

QSize Utils::scaled(const QScreen* screen, const QSize& size)
{
    return QSize(scaled(screen, size.width()), scaled(screen, size.height()));
}

QMarginsF Utils::scaled(const QScreen* screen, const QMarginsF& margins)
{
    return margins * scaled(screen, 1.0);
}

QMargins Utils::scaled(const QScreen* screen, const QMargins& margins)
{
    return QMargins(scaled(screen, margins.left()),
                    scaled(screen, margins.top()),
                    scaled(screen, margins.right()),
                    scaled(screen, margins.bottom()));
}

ACAYIPWIDGETS_END_NAMESPACE
