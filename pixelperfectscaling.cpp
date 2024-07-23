// Copyright (C) 2024 Ömer Göktaş. All Rights Reserved.
// SPDX-License-Identifier: LicenseRef-AcayipWidgets-Commercial OR GPL-3.0-only

#include "pixelperfectscaling.h"

#include <QApplication>
#include <QLayout>
#include <QScreen>
#include <QTimer>
#include <QWindow>

#include <private/qguiapplication_p.h>
#include <private/qhighdpiscaling_p.h>
#include <qpa/qplatformwindow.h>
#include <qpa/qwindowsysteminterface.h>
#include <qpa/qwindowsysteminterface_p.h>

#if defined(Q_OS_WINDOWS)
#include <shellscalingapi.h>
#include <windows.h>
#endif

ACAYIPWIDGETS_BEGIN_NAMESPACE

class WindowSystemEventFilter final : public QWindowSystemEventHandler
{
    PixelPerfectScaling* m_scaler;

public:
    WindowSystemEventFilter(PixelPerfectScaling* scaler)
        : m_scaler(scaler)
    {}
    bool sendEvent(QWindowSystemInterfacePrivate::WindowSystemEvent* event) override
    {
        if (event->type == QWindowSystemInterfacePrivate::WindowScreenChanged) {
            auto e
                = static_cast<QWindowSystemInterfacePrivate::WindowScreenChangedEvent*>(
                    event);
            for (PixelPerfectScaling::WindowEntry* we :
                 std::as_const(m_scaler->m_windows)) {
                if (we->window == e->window) {
                    we->oldSize = e->window->size();
                    we->screen = e->screen;
#if defined(Q_OS_WINDOWS)
                    we->window->setFlag(Qt::FramelessWindowHint, true);
#endif
                    QMetaObject::invokeMethod(m_scaler,
                                              "resizeWindow",
                                              Qt::QueuedConnection,
                                              Q_ARG(PixelPerfectScaling::WindowEntry*,
                                                    we));
                    break;
                }
            }
        } else if (event->type
                   == QWindowSystemInterfacePrivate::WindowDevicePixelRatioChanged) {
            auto e = static_cast<
                QWindowSystemInterfacePrivate::WindowDevicePixelRatioChangedEvent*>(
                event);
            for (PixelPerfectScaling::WindowEntry* we :
                 std::as_const(m_scaler->m_windows)) {
                if (we->window == e->window) {
                    we->oldSize = e->window->size();
                    QMetaObject::invokeMethod(m_scaler,
                                              "resizeWindow",
                                              Qt::QueuedConnection,
                                              Q_ARG(PixelPerfectScaling::WindowEntry*,
                                                    we));
                    break;
                }
            }
        } else if (event->type
                   == QWindowSystemInterfacePrivate::ScreenLogicalDotsPerInch) {
            auto e = static_cast<
                QWindowSystemInterfacePrivate::ScreenLogicalDotsPerInchEvent*>(event);
            for (PixelPerfectScaling::WindowEntry* we :
                 std::as_const(m_scaler->m_windows)) {
                if (we->screen == e->screen) {
                    we->oldSize = we->window->size();
                    QMetaObject::invokeMethod(m_scaler,
                                              "resizeWindow",
                                              Qt::QueuedConnection,
                                              Q_ARG(PixelPerfectScaling::WindowEntry*,
                                                    we));
                }
            }
        }
        return QWindowSystemEventHandler::sendEvent(event);
    }
};

PixelPerfectScaling::PixelPerfectScaling(QObject* parent)
    : QObject(parent)
{}

PixelPerfectScaling::~PixelPerfectScaling()
{
    qDeleteAll(m_windows);
}

void PixelPerfectScaling::prepare()
{
    QGuiApplication::setHighDpiScaleFactorRoundingPolicy(
        Qt::HighDpiScaleFactorRoundingPolicy::Floor);
    QGuiApplication::setAttribute(Qt::AA_Use96Dpi);

#if defined(Q_OS_WINDOWS)
    SetProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE);
    // SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
#endif
}

void PixelPerfectScaling::install(QGuiApplication* guiApp)
{
    static PixelPerfectScaling self;
    static WindowSystemEventFilter filter(&self);
    guiApp->installEventFilter(&self);
    QWindowSystemInterfacePrivate::installWindowSystemEventHandler(&filter);
}

bool PixelPerfectScaling::eventFilter(QObject* watched, QEvent* event)
{
    bool result = false;
    if (event->type() == QEvent::Expose) {
        if (auto window = qobject_cast<QWindow*>(watched)) {
            for (const WindowEntry* we : std::as_const(m_windows)) {
                if (we->window == watched)
                    return result;
            }
            connect(window,
                    &QWindow::destroyed,
                    this,
                    &PixelPerfectScaling::removeWindow);
            auto we = new WindowEntry{.oldDpr = 1.0,
                                      .oldSize = window->size(),
                                      .window = window,
                                      .screen = window->screen()};
            m_windows.append(we);
            resizeWindow(we);
        }
    }
    return result;
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
    QWidget* windowWidget = nullptr;
    foreach (QWidget* widget, QApplication::topLevelWidgets()) {
        if (widget->windowHandle() == windowEntry->window) {
            windowWidget = widget;
            break;
        }
    }

    Q_ASSERT_X(windowWidget, "PixelPerfectScaling", "couldn't find the window widget");
    if (!windowWidget)
        return;

    qreal old = windowEntry->oldDpr;
    const QScreen* screen = windowEntry->screen;
    const QSize& size = windowEntry->oldSize;
    const QFont& font = windowWidget->font();
    const QPoint& pos = windowWidget->pos();
    const QSize& scaledSize = scaled(screen, size, old);

    if (!windowEntry->initialized) {
        const QSize& d = (scaledSize - size) / 2.0;
        windowWidget->move(pos - QPoint(d.width(), d.height()));
        windowEntry->initialized = true;
    }

#if defined(Q_OS_WINDOWS)
    windowWidget->resize(scaledSize);
    windowEntry->window->setFlag(Qt::FramelessWindowHint, false);
#else
    const QSize& minSize = windowWidget->minimumSize();
    const QSize& maxSize = windowWidget->maximumSize();
    // if (!minSize.isNull())
    //     windowWidget->setMinimumSize(scaled(screen, minSize, old));
    // if (maxSize != QSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX))
    //     windowWidget->setMaximumSize(scaled(screen, maxSize, old));
    windowWidget->resize(scaledSize);
#endif

    windowWidget->setFont(scaled(screen, font, old));
    foreach (QWidget* widget, windowWidget->findChildren<QWidget*>()) {
        if (!widget->parentWidget() || !widget->parentWidget()->layout())
            widget->setGeometry(scaled(widget, widget->geometry(), old));

        if (!widget->font().isCopyOf(font)
            && !(widget->parentWidget()
                 && widget->font().isCopyOf(widget->parentWidget()->font()))) {
            widget->setFont(scaled(widget, widget->font(), old));
        }

        if (!widget->minimumSize().isNull())
            widget->setMinimumSize(scaled(screen, widget->minimumSize(), old));

        if (widget->maximumSize() != QSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX))
            widget->setMaximumSize(scaled(screen, widget->maximumSize(), old));

        const QSize& iconSize = widget->property("iconSize").toSize();
        if (!iconSize.isNull())
            widget->setProperty("iconSize", scaled(screen, iconSize, old));

        const QMargins& paddings = widget->property("paddings").value<QMargins>();
        if (!paddings.isNull())
            widget->setProperty("paddings",
                                QVariant::fromValue(scaled(screen, paddings, old)));

        const QMargins& margins = widget->property("margins").value<QMargins>();
        if (!margins.isNull())
            widget->setProperty("margins",
                                QVariant::fromValue(scaled(screen, margins, old)));

        bool ok = false;
        int value = widget->property("spacing").toInt(&ok);
        if (ok && value > 0)
            widget->setProperty("spacing", scaled(screen, value, old));

        ok = false;
        value = widget->property("lineWidth").toInt(&ok);
        if (ok && value > 0)
            widget->setProperty("lineWidth", scaled(screen, value, old, false));

        ok = false;
        value = widget->property("midLineWidth").toInt(&ok);
        if (ok && value > 0)
            widget->setProperty("midLineWidth", scaled(screen, value, old, false));
    }

    foreach (QLayout* layout, windowWidget->findChildren<QLayout*>()) {
        if (!layout->contentsMargins().isNull())
            layout->setContentsMargins(scaled(screen, layout->contentsMargins(), old));
        if (layout->spacing() != 0)
            layout->setSpacing(scaled(screen, layout->spacing(), old));
        for (int i = 0; i < layout->count(); i++) {
            if (auto spacer = layout->itemAt(i)->spacerItem()) {
                const QSize& sh = scaled(screen, spacer->sizeHint(), old);
                spacer->changeSize(sh.width(),
                                   sh.height(),
                                   spacer->sizePolicy().horizontalPolicy(),
                                   spacer->sizePolicy().verticalPolicy());
            }
        }
    }

    windowEntry->oldSize = scaledSize;
    windowEntry->oldDpr = scaled(screen, 1.0);
}

ACAYIPWIDGETS_END_NAMESPACE
