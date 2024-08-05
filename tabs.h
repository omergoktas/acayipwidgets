// Copyright (C) 2024 Ömer Göktaş. All Rights Reserved.
// SPDX-License-Identifier: LicenseRef-AcayipWidgets-Commercial OR GPL-3.0-only

#pragma once

#include <boxlayout.h>

#include <QScrollArea>

ACAYIPWIDGETS_BEGIN_NAMESPACE

class TabsPrivate;

class ACAYIPWIDGETS_EXPORT Tabs : public QScrollArea
{
    Q_OBJECT
    Q_DISABLE_COPY(Tabs)
    Q_DECLARE_PRIVATE(Tabs)

public:
    explicit Tabs(QWidget* parent = nullptr);

    BoxLayout* contentLayout();

private:
    using QScrollArea::setWidget;
    using QScrollArea::setWidgetResizable;
    using QScrollArea::takeWidget;
    using QScrollArea::widget;
    using QScrollArea::widgetResizable;

protected:
    Tabs(TabsPrivate& dd, QWidget* parent);
};

ACAYIPWIDGETS_END_NAMESPACE
