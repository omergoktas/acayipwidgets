// Copyright (C) 2024 Ömer Göktaş. All Rights Reserved.
// SPDX-License-Identifier: LicenseRef-AcayipWidgets-Commercial OR GPL-3.0-only

#pragma once

#include <acayipglobal.h>

#include <qpa/qwindowsysteminterface_p.h>

#include <QMap>
#include <QObject>

class QGuiApplication;

ACAYIPWIDGETS_BEGIN_NAMESPACE

class PixelPerfectScaling final : public QObject, public QWindowSystemEventHandler

{
    Q_OBJECT
    Q_DISABLE_COPY(PixelPerfectScaling)

    struct WindowEntry
    {
        bool initialized{false};
        qreal oldDpr;
        QSize oldSize;
        QWindow* window{nullptr};
        QScreen* screen{nullptr};
    };

public:
    PixelPerfectScaling();
    ~PixelPerfectScaling() override;
    bool sendEvent(QWindowSystemInterfacePrivate::WindowSystemEvent* event) override;
    static void init();

protected:
    void removeWindow(QObject* windowObj);
    void resizeWindow(WindowEntry* windowEntry);
    QWidget* windowWidget(const QWindow* window) const;

private:
    QList<WindowEntry*> m_windows;
};

ACAYIPWIDGETS_END_NAMESPACE
