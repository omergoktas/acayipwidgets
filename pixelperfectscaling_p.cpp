// Copyright (C) 2024 Ömer Göktaş. All Rights Reserved.
// SPDX-License-Identifier: LicenseRef-AcayipWidgets-Commercial OR GPL-3.0-only

#include "pixelperfectscaling_p.h"
#include "utils_p.h"

#include <QApplication>
#include <QLayout>
#include <QMovie>
#include <QScreen>
#include <QTimer>
#include <QWindow>

#include <private/qguiapplication_p.h>
#include <private/qhighdpiscaling_p.h>
#include <qpa/qplatformwindow.h>
#include <qpa/qwindowsysteminterface.h>

#if defined(Q_OS_WINDOWS)
#include <shellscalingapi.h>
#include <windows.h>
#endif

ACAYIPWIDGETS_BEGIN_NAMESPACE

PixelPerfectScaling::PixelPerfectScaling()
    : QObject()
    , QWindowSystemEventHandler()
{}

PixelPerfectScaling::~PixelPerfectScaling()
{
    qDeleteAll(m_windows);
}

void PixelPerfectScaling::init()
{
    qputenv("QT_ENABLE_HIGHDPI_SCALING", "0");
    QGuiApplication::setAttribute(Qt::AA_Use96Dpi);

#if defined(Q_OS_WINDOWS)
    SetProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE);
#endif

    static PixelPerfectScaling self;
    QWindowSystemInterfacePrivate::installWindowSystemEventHandler(&self);
}

void PixelPerfectScaling::removeWindow(QObject* windowObj)
{
    for (const WindowEntry* we : std::as_const(m_windows)) {
        if (we->window == windowObj) {
            m_windows.removeOne(we);
            break;
        }
    }
}

void PixelPerfectScaling::resizeWindow(WindowEntry* windowEntry)
{
    QWidget* winWidget = windowWidget(windowEntry->window);
    QSize scaledSize;
    qreal factor = 1.0 / windowEntry->oldDpr;
    const QScreen* screen = windowEntry->screen;

    const QFont font = winWidget->font();
    if (!windowEntry->initialized) {
        winWidget->setProperty("pps_initialFont", font);
        winWidget->setFont(scaled(screen, font, factor));
    } else {
        const QFont& initialFont = winWidget->property("pps_initialFont").value<QFont>();
        winWidget->setFont(scaled(screen, font, initialFont, factor));
    }

    foreach (QWidget* widget, winWidget->findChildren<QWidget*>()) {
        if (!widget->parentWidget() || !widget->parentWidget()->layout()) {
            const QRect& geometry = widget->geometry();
            if (!windowEntry->initialized) {
                widget->setProperty("pps_initialGeometry", geometry);
                widget->setGeometry(scaled(screen, geometry, factor));
            } else {
                const QRect& initialGeometry
                    = widget->property("pps_initialGeometry").toRect();
                widget->setGeometry(scaled(widget, geometry, initialGeometry, factor));
            }
        }

        if (!widget->font().isCopyOf(font)
            && !(widget->parentWidget()
                 && widget->font().isCopyOf(widget->parentWidget()->font()))) {
            if (!windowEntry->initialized) {
                widget->setProperty("pps_initialFont", widget->font());
                widget->setFont(scaled(screen, widget->font(), factor));
            } else {
                const QFont& initialFont
                    = widget->property("pps_initialFont").value<QFont>();
                widget->setFont(scaled(screen, widget->font(), initialFont, factor));
            }
        }

        const QSize& minimumSize = Utils::explicitWidgetMinMaxSize(widget, true);
        if (!minimumSize.isEmpty()) {
            if (!windowEntry->initialized) {
                widget->setProperty("pps_initialMinimumSize", minimumSize);
                scaledSize = scaled(screen, minimumSize, factor);
            } else {
                const QSize& initialMinimumSize
                    = widget->property("pps_initialMinimumSize").toSize();
                scaledSize = scaled(screen, minimumSize, initialMinimumSize, factor);
            }
            widget->setMinimumSize(scaledSize);
        }

        const QSize& maximumSize = Utils::explicitWidgetMinMaxSize(widget, false);
        if (!maximumSize.isEmpty()
            && maximumSize != QSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX)) {
            if (!windowEntry->initialized) {
                widget->setProperty("pps_initialMaximumSize", maximumSize);
                scaledSize = scaled(screen, maximumSize, factor);
            } else {
                const QSize& initialMaximumSize
                    = widget->property("pps_initialMaximumSize").toSize();
                scaledSize = scaled(screen, maximumSize, initialMaximumSize, factor);
            }
            widget->setMaximumSize(scaledSize);
        }

        const QSize& iconSize = widget->property("iconSize").toSize();
        if (!iconSize.isNull()) {
            if (!windowEntry->initialized) {
                widget->setProperty("pps_initialIconSize", iconSize);
                widget->setProperty("iconSize", scaled(screen, iconSize, factor));
            } else {
                const QSize& initialIconSize
                    = widget->property("pps_initialIconSize").toSize();
                widget->setProperty("iconSize",
                                    scaled(screen, iconSize, initialIconSize, factor));
            }
        }

        const QMargins& paddings = widget->property("paddings").value<QMargins>();
        if (!paddings.isNull()) {
            if (!windowEntry->initialized) {
                widget->setProperty("pps_initialPaddings",
                                    QVariant::fromValue(paddings));
                widget->setProperty("paddings",
                                    QVariant::fromValue(
                                        scaled(screen, paddings, factor)));
            } else {
                const QMargins& initialPaddings
                    = widget->property("pps_initialPaddings").value<QMargins>();
                widget->setProperty("paddings",
                                    QVariant::fromValue(scaled(screen,
                                                               paddings,
                                                               initialPaddings,
                                                               factor)));
            }
        }

        const QMargins& margins = widget->property("margins").value<QMargins>();
        if (!margins.isNull()) {
            if (!windowEntry->initialized) {
                widget->setProperty("pps_initialMargins", QVariant::fromValue(margins));
                widget->setProperty("margins",
                                    QVariant::fromValue(
                                        scaled(screen, margins, factor)));
            } else {
                const QMargins& initialMargins
                    = widget->property("pps_initialMargins").value<QMargins>();
                widget->setProperty("margins",
                                    QVariant::fromValue(
                                        scaled(screen, margins, initialMargins, factor)));
            }
        }

        bool ok = false;
        int value = widget->property("spacing").toInt(&ok);
        if (ok && value > 0) {
            if (!windowEntry->initialized) {
                widget->setProperty("pps_initialSpacing", value);
                widget->setProperty("spacing", scaled(screen, value, factor));
            } else {
                int initialSpacing = widget->property("pps_initialSpacing").toInt();
                widget->setProperty("spacing",
                                    scaled(screen, value, initialSpacing, factor));
            }
        }

        ok = false;
        value = widget->property("lineWidth").toInt(&ok);
        if (ok && value > 0) {
            if (!windowEntry->initialized) {
                widget->setProperty("pps_initialLineWidth", value);
                widget->setProperty("lineWidth", scaled(screen, value, factor));
            } else {
                int initialLineWidth = widget->property("pps_initialLineWidth").toInt();
                widget->setProperty("lineWidth",
                                    scaled(screen, value, initialLineWidth, factor));
            }
        }

        ok = false;
        value = widget->property("midLineWidth").toInt(&ok);
        if (ok && value > 0) {
            if (!windowEntry->initialized) {
                widget->setProperty("pps_initialMidLineWidth", value);
                widget->setProperty("midLineWidth", scaled(screen, value, factor));
            } else {
                int initialMidLineWidth
                    = widget->property("pps_initialMidLineWidth").toInt();
                widget->setProperty("midLineWidth",
                                    scaled(screen, value, initialMidLineWidth, factor));
            }
        }
    }

    foreach (QLayout* layout, winWidget->findChildren<QLayout*>()) {
        if (!layout->contentsMargins().isNull()) {
            const QMargins& margins = layout->contentsMargins();
            if (!windowEntry->initialized) {
                layout->setProperty("pps_initialContentsMargins",
                                    QVariant::fromValue(margins));
                layout->setContentsMargins(scaled(screen, margins, factor));
            } else {
                const QMargins& initialMargins
                    = layout->property("pps_initialContentsMargins").value<QMargins>();
                layout->setContentsMargins(
                    scaled(screen, margins, initialMargins, factor));
            }
        }
        int spacing = layout->spacing();
        if (spacing != 0) {
            if (!windowEntry->initialized) {
                layout->setProperty("pps_initialSpacing", spacing);
                layout->setSpacing(scaled(screen, spacing, factor));
            } else {
                int initialSpacing = layout->property("pps_initialSpacing").toInt();
                layout->setSpacing(scaled(screen, spacing, initialSpacing, factor));
            }
        }
        for (int i = 0; i < layout->count(); i++) {
            if (auto spacer = layout->itemAt(i)->spacerItem()) {
                QSize sh = spacer->sizeHint();
                if (!windowEntry->initialized) {
                    layout->setProperty("pps_initialSizeHint", sh);
                    sh = scaled(screen, sh, factor);
                } else {
                    const QSize& initialSizeHint
                        = layout->property("pps_initialSizeHint").toSize();
                    sh = scaled(screen, sh, initialSizeHint, factor);
                }
                spacer->changeSize(sh.width(),
                                   sh.height(),
                                   spacer->sizePolicy().horizontalPolicy(),
                                   spacer->sizePolicy().verticalPolicy());
            }
        }
    }

    foreach (QMovie* movie, winWidget->findChildren<QMovie*>()) {
        const QSize& size = movie->scaledSize();
        if (!windowEntry->initialized) {
            movie->setProperty("pps_initialScaledSize", size);
            scaledSize = scaled(screen, size, factor);
        } else {
            const QSize& initialScaledSize
                = movie->property("pps_initialScaledSize").toSize();
            scaledSize = scaled(screen, size, initialScaledSize, factor);
        }
        movie->setScaledSize(scaledSize);
    }

#if defined(ACAYIP_PLATFORM_DESKTOP)
    const QSize& winMinimumSize = Utils::explicitWidgetMinMaxSize(winWidget, true);
    if (!winMinimumSize.isEmpty()) {
        if (!windowEntry->initialized) {
            winWidget->setProperty("pps_initialMinimumSize", winMinimumSize);
            scaledSize = scaled(screen, winMinimumSize, factor);
        } else {
            const QSize& initialMinimumSize
                = winWidget->property("pps_initialMinimumSize").toSize();
            scaledSize = scaled(screen, winMinimumSize, initialMinimumSize, factor);
        }
        winWidget->setMinimumSize(scaledSize);
    }

    const QSize& winMaximumSize = Utils::explicitWidgetMinMaxSize(winWidget, false);
    if (!winMaximumSize.isEmpty()
        && winMaximumSize != QSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX)) {
        if (!windowEntry->initialized) {
            winWidget->setProperty("pps_initialMaximumSize", winMaximumSize);
            scaledSize = scaled(screen, winMaximumSize, factor);
        } else {
            const QSize& initialMaximumSize
                = winWidget->property("pps_initialMaximumSize").toSize();
            scaledSize = scaled(screen, winMaximumSize, initialMaximumSize, factor);
        }
        winWidget->setMaximumSize(scaledSize);
    }

    const QSize& winSize = windowEntry->oldSize;
    const QPoint& winPos = winWidget->pos();
    if (!windowEntry->initialized) {
        winWidget->setProperty("pps_initialSize", winSize);
        scaledSize = scaled(screen, winSize, factor);
        const QSize& d = (scaledSize - winSize) / 2.0;
        winWidget->move(winPos - QPoint(d.width(), d.height()));
    } else {
        const QSize& initialSize = winWidget->property("pps_initialSize").toSize();
        scaledSize = scaled(screen, winSize, initialSize, factor);
    }
    winWidget->resize(scaledSize);
    QTimer::singleShot(1000, winWidget, [winWidget, scaledSize] {
        winWidget->resize(scaledSize);
    });
#if defined(Q_OS_WINDOWS)
    windowEntry->window->setFlag(Qt::FramelessWindowHint, false);
#endif
#endif
    windowEntry->oldDpr = scaled(screen, 1.0);
    windowEntry->initialized = true;
}

QWidget* PixelPerfectScaling::windowWidget(const QWindow* window) const
{
    foreach (QWidget* widget, QApplication::topLevelWidgets()) {
        if (widget->windowHandle() == window)
            return widget;
    }
    Q_ASSERT_X(false, "PixelPerfectScaling", "couldn't find the window widget");
    return nullptr;
}

bool PixelPerfectScaling::sendEvent(
    QWindowSystemInterfacePrivate::WindowSystemEvent* event)
{
    if (event->type == QWindowSystemInterfacePrivate::Expose) {
        auto e = static_cast<QWindowSystemInterfacePrivate::WindowScreenChangedEvent*>(
            event);
        for (const WindowEntry* we : std::as_const(m_windows)) {
            if (we->window == e->window)
                return true;
        }
        connect(e->window,
                &QWindow::destroyed,
                this,
                &PixelPerfectScaling::removeWindow);
        auto we = new WindowEntry{.oldDpr = 1.0,
                                  .oldSize = windowWidget(e->window)->size(),
                                  .window = e->window,
                                  .screen = e->window->screen()};
        m_windows.append(we);
        QMetaObject::invokeMethod(this,
                                  &PixelPerfectScaling::resizeWindow,
                                  Qt::QueuedConnection,
                                  we);
    } else if (event->type == QWindowSystemInterfacePrivate::WindowScreenChanged) {
        auto e = static_cast<QWindowSystemInterfacePrivate::WindowScreenChangedEvent*>(
            event);
        for (WindowEntry* we : std::as_const(m_windows)) {
            if (we->window == e->window) {
                we->oldSize = windowWidget(e->window)->size();
                we->screen = e->screen;
#if defined(Q_OS_WINDOWS)
                we->window->setFlag(Qt::FramelessWindowHint, true);
#endif
                QMetaObject::invokeMethod(this,
                                          &PixelPerfectScaling::resizeWindow,
                                          Qt::QueuedConnection,
                                          we);
                break;
            }
        }
    } else if (event->type
               == QWindowSystemInterfacePrivate::WindowDevicePixelRatioChanged) {
        auto e = static_cast<
            QWindowSystemInterfacePrivate::WindowDevicePixelRatioChangedEvent*>(event);
        for (WindowEntry* we : std::as_const(m_windows)) {
            if (we->window == e->window) {
                we->oldSize = windowWidget(e->window)->size();
                QMetaObject::invokeMethod(this,
                                          &PixelPerfectScaling::resizeWindow,
                                          Qt::QueuedConnection,
                                          we);
                break;
            }
        }
    } else if (event->type == QWindowSystemInterfacePrivate::ScreenLogicalDotsPerInch) {
        auto e
            = static_cast<QWindowSystemInterfacePrivate::ScreenLogicalDotsPerInchEvent*>(
                event);
        for (WindowEntry* we : std::as_const(m_windows)) {
            if (we->screen == e->screen) {
                we->oldSize = windowWidget(we->window)->size();
                QMetaObject::invokeMethod(this,
                                          &PixelPerfectScaling::resizeWindow,
                                          Qt::QueuedConnection,
                                          we);
            }
        }
    }
    return QWindowSystemEventHandler::sendEvent(event);
}

ACAYIPWIDGETS_END_NAMESPACE
