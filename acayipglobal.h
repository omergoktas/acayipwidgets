// Copyright (C) 2024 Ömer Göktaş. All Rights Reserved.
// SPDX-License-Identifier: LicenseRef-AcayipWidgets-Commercial OR GPL-3.0-only

#pragma once

#include <acayipconfig.h>
#include <acayipwidgets_export.h>

#include <QBrush>
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

struct StyleDefaults
{
    static constexpr qreal margins = 2.0;
    static constexpr qreal paddings = 8.0;
    static constexpr qreal spacing = 2.0;
    static constexpr int animationDuration = 500;
    static constexpr int borderRadiusIsPercentage = 1000.0;
    static inline const ButtonStyles buttonStyles = {
        .rest = {
            .borderRadius = borderRadiusIsPercentage + 100,
            .borderPen = QColor(Qt::transparent),
            .borderPenDark = QColor(Qt::transparent),
            .textColor = QColor(0xffffff),
            .textColorDark = QColor(0xffffff),
            .iconColor = QColor(0xffffff),
            .iconColorDark = QColor(0xffffff),
            .backgroundBrush = QBrush(0x0f6cbd),
            .backgroundBrushDark = QBrush(0x115ea3)
        },
        .hovered = {
            .backgroundBrush = QBrush(0x115ea3),
            .backgroundBrushDark = QBrush(0x0f6cbd)
        },
        .pressed = {
            .backgroundBrush = QBrush(0x0c3b5e),
            .backgroundBrushDark = QBrush(0x0c3b5e)
        },
        .checked = {
            .backgroundBrush = QBrush(0x0f548c),
            .backgroundBrushDark = QBrush(0x0f548c)
        },
        .disabled = {
            .textColor = QColor(0xbdbdbd),
            .textColorDark = QColor(0x5c5c5c),
            .iconColor = QColor(0xbdbdbd),
            .iconColorDark = QColor(0x5c5c5c),
            .backgroundBrush = QBrush(0xf0f0f0),
            .backgroundBrushDark = QBrush(0x141414)
        }
    };
    static constexpr QPainter::RenderHints renderHints{
        QPainter::Antialiasing | QPainter::TextAntialiasing
        | QPainter::SmoothPixmapTransform | QPainter::LosslessImageRendering};
};

ACAYIPWIDGETS_END_NAMESPACE
