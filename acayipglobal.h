// Copyright (C) 2024 Ömer Göktaş. All Rights Reserved.
// SPDX-License-Identifier: LicenseRef-AcayipWidgets-Commercial OR GPL-3.0-only

#pragma once

#include <acayiputils.h>

#include <QBrush>
#include <QEasingCurve>
#include <QPainter>
#include <QPen>

ACAYIPWIDGETS_BEGIN_NAMESPACE

struct ButtonStyle
{
    int borderRadius{-1};
    QPen borderPen{Qt::NoPen};
    QPen borderPenDark{Qt::NoPen};
    QColor textColor;
    QColor textColorDark;
    QColor iconColor;
    QColor iconColorDark;
    QBrush backgroundBrush;
    QBrush backgroundBrushDark;
};

struct ButtonStyles
{
    ButtonStyle rest;
    ButtonStyle hovered;
    ButtonStyle pressed;
    ButtonStyle checked;
    ButtonStyle disabled;
};

class ACAYIPWIDGETS_EXPORT Defaults
{
    friend class ButtonPrivate;

    static const int borderRadiusPercentagePoint;

public:
    static qreal scaleFactor;
    static int animationDuration;
    static int margins;
    static int paddings;
    static int spacing;
    static QEasingCurve::Type outEasingType;
    static QEasingCurve::Type inEasingType;
    static QPainter::RenderHints renderHints;
    static ButtonStyles buttonStyles;
};

ACAYIPWIDGETS_END_NAMESPACE
