// Copyright (C) 2024 Ömer Göktaş. All Rights Reserved.
// SPDX-License-Identifier: LicenseRef-AcayipWidgets-Commercial OR GPL-3.0-only

#pragma once

#include <acayipconfig.h>
#include <acayipwidgets_export.h>

#include <QBrush>

ACAYIPWIDGETS_BEGIN_NAMESPACE

struct ButtonStyle
{
    qreal borderWidth = -1;
    qreal borderRadius = -1;
    QColor borderColor;
    QColor textColor;
    QColor iconColor;
    QBrush backgroundBrush;
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

ACAYIPWIDGETS_END_NAMESPACE
