// Copyright (C) 2024 Ömer Göktaş. All Rights Reserved.
// SPDX-License-Identifier: LicenseRef-AcayipWidgets-Commercial OR GPL-3.0-only

#pragma once

#include <acayipglobal.h>

ACAYIPWIDGETS_BEGIN_NAMESPACE

inline namespace Utils {

    void disableExistingIconEngines();
    QSize explicitWidgetMinMaxSize(const QWidget* widget, bool minimum);

} // namespace Utils

ACAYIPWIDGETS_END_NAMESPACE
