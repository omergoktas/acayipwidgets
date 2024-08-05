// Copyright (C) 2024 Ömer Göktaş. All Rights Reserved.
// SPDX-License-Identifier: LicenseRef-AcayipWidgets-Commercial OR GPL-3.0-only

#include "tabs_p.h"

using namespace Qt::Literals;

ACAYIPWIDGETS_BEGIN_NAMESPACE

/*!
 *  \internal
*/
TabsPrivate::TabsPrivate()
    : QScrollAreaPrivate()
    , contentLayout(new BoxLayout(BoxLayout::LeftToRight, new QWidget()))
{}

void TabsPrivate::init()
{
    Q_Q(Tabs);

    q->setWidget(contentLayout->widget());
}

Tabs::Tabs(QWidget* parent)
    : Tabs(*new TabsPrivate, parent)
{}

/*!
 *  \internal
*/
Tabs::Tabs(TabsPrivate& dd, QWidget* parent)
    : QScrollArea(dd, parent)
{
    Q_D(Tabs);
    d->init();
}

BoxLayout* Tabs::contentLayout()
{
    Q_D(Tabs);
    return d->contentLayout;
}

ACAYIPWIDGETS_END_NAMESPACE
