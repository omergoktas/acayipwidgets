// Copyright (C) 2024 Ömer Göktaş. All Rights Reserved.
// SPDX-License-Identifier: LicenseRef-AcayipWidgets-Commercial OR GPL-3.0-only

#pragma once

#include <acayipglobal.h>

#include <QMap>
#include <QObject>

ACAYIPWIDGETS_BEGIN_NAMESPACE

class ACAYIPWIDGETS_EXPORT PixelPerfectScaling : public QObject
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

    friend class WindowSystemEventFilter;

public:
    explicit PixelPerfectScaling(QObject* parent = nullptr);
    ~PixelPerfectScaling() override;

    static void prepare();
    static void install(QGuiApplication* guiApp);

    bool eventFilter(QObject* watched, QEvent* event) override;

protected slots:
    void removeWindow(QObject* windowObj);
    void resizeWindow(WindowEntry* windowEntry);

private:
    QList<WindowEntry*> m_windows;
};

ACAYIPWIDGETS_END_NAMESPACE
