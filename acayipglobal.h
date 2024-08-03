// Copyright (C) 2024 Ömer Göktaş. All Rights Reserved.
// SPDX-License-Identifier: LicenseRef-AcayipWidgets-Commercial OR GPL-3.0-only

#pragma once

#include <acayiputils.h>

#include <QEasingCurve>
#include <QMargins>
#include <QPainter>

ACAYIPWIDGETS_BEGIN_NAMESPACE

class ACAYIPWIDGETS_EXPORT Defaults
{
    friend class ButtonPrivate;
    friend class PixelPerfectScaling;

    static const int borderRadiusPercentagePoint;

public:
    static int animationDuration;
    static int spacing;
    static qreal extraScaleFactor;
    static QMargins paddings;
    static QMargins margins;
    static QEasingCurve::Type outEasingType;
    static QEasingCurve::Type inEasingType;
    static QPainter::RenderHints renderHints;
};

ACAYIPWIDGETS_END_NAMESPACE
