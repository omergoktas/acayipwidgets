// Copyright (C) 2024 Ömer Göktaş. All Rights Reserved.
// SPDX-License-Identifier: LicenseRef-AcayipWidgets-Commercial OR GPL-3.0-only

/*
 * WARNING: This file exists purely as a private implementation
 * detail. This header file may change from version to version
 * without notice or even be removed.
*/

#pragma once

#include "tabs.h"

#include <private/qscrollarea_p.h>

ACAYIPWIDGETS_BEGIN_NAMESPACE

class TabsPrivate : public QScrollAreaPrivate
{
    Q_DECLARE_PUBLIC(Tabs)

public:
    TabsPrivate();
    void init();

    BoxLayout* contentLayout;
};

ACAYIPWIDGETS_END_NAMESPACE
