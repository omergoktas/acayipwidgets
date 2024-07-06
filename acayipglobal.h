// Copyright (C) 2024 Ömer Göktaş. All Rights Reserved.
// SPDX-License-Identifier: LicenseRef-AcayipWidgets-Commercial OR GPL-3.0-only

#pragma once

#include <acayipconfig.h>
#include <acayipwidgets_export.h>

#include <QBrush>
#include <QPen>

ACAYIPWIDGETS_BEGIN_NAMESPACE

struct ButtonStyle
{
    qreal borderRadius{-1};
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
    ButtonStyle raised;
    ButtonStyle disabled;
    ButtonStyle defaultButton;
};

struct StyleDefaults
{
    static constexpr qreal margins = 4.0;
    static constexpr qreal paddings = 8.0;
    static constexpr qreal spacing = 2.0;
    static constexpr qreal borderRadiusIsPercentage = 1000.0;
    static inline const ButtonStyles buttonStyles = {
        .rest = {
            .borderRadius = borderRadiusIsPercentage + 50.0,
            .borderPen = QColor(0, 0, 0),
            .borderPenDark = QColor(255, 255, 255),
            .textColor = QColor(0, 0, 0),
            .textColorDark = QColor(255, 255, 255),
            .iconColor = QColor(0, 0, 0),
            .iconColorDark = QColor(255, 255, 255),
            .backgroundBrush = QBrush(Qt::white),
            .backgroundBrushDark = QBrush(Qt::black)
        },
        .hovered = {
            .borderRadius = 5.0,
            // ... other members ...
        },
        .pressed = {
            // ... members for pressed state ...
        },
        .checked = {
            // ... members for checked state ...
        },
        .raised = {
            // ... members for raised state ...
        },
        .disabled = {
            // ... members for disabled state ...
        },
        .defaultButton = {
            // ... members for default button state ...
        }
    };
};

ACAYIPWIDGETS_END_NAMESPACE
