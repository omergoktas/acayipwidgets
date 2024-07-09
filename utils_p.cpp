// Copyright (C) 2024 Ömer Göktaş. All Rights Reserved.
// SPDX-License-Identifier: LicenseRef-AcayipWidgets-Commercial OR GPL-3.0-only

#include "utils_p.h"

#include <QColor>

ACAYIPWIDGETS_BEGIN_NAMESPACE

QColor strongerColor(QColor c, int strength)
{
    if (c.alpha() <= 255 - strength)
        c.setAlpha(c.alpha() + strength);
    else if (c.value() <= 255 - strength)
        c.setHsv(c.hue(), c.saturation(), c.value() + strength, c.alpha());
    else
        c.setHsv(c.hue(), c.saturation(), c.value() - strength, c.alpha());
    return c;
}

ACAYIPWIDGETS_END_NAMESPACE
