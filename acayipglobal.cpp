// Copyright (C) 2024 Ömer Göktaş. All Rights Reserved.
// SPDX-License-Identifier: LicenseRef-AcayipWidgets-Commercial OR GPL-3.0-only

#include "acayipglobal.h"
#include "utils_p.h"
#include "pixelperfectscaling_p.h"

#include <QCoreApplication>

using namespace Qt::Literals;

ACAYIPWIDGETS_BEGIN_NAMESPACE

#if defined(Q_OS_IOS) || defined(Q_OS_ANDROID)
    qreal Defaults::scaleFactor{1.5};
#else
    qreal Defaults::scaleFactor{1.0};
#endif

const int Defaults::borderRadiusPercentagePoint = 1000;

int Defaults::animationDuration{300};

int Defaults::spacing{2};

QMargins Defaults::margins{0, 0, 0, 0};

QMargins Defaults::paddings{8, 8, 8, 8};

QEasingCurve::Type Defaults::outEasingType{QEasingCurve::OutCubic};

QEasingCurve::Type Defaults::inEasingType{QEasingCurve::InCubic};

QPainter::RenderHints Defaults::renderHints {
    QPainter::Antialiasing |
    QPainter::TextAntialiasing |
    QPainter::SmoothPixmapTransform |
    QPainter::VerticalSubpixelPositioning |
    QPainter::LosslessImageRendering
};

ButtonStyles Defaults::buttonStyles {
    .rest = {
        .borderRadius = borderRadiusPercentagePoint + 100,
        .textColor = QColor(0xffffff),
        .textColorDark = QColor(0xffffff),
        .iconColor = QColor(0xffffff),
        .iconColorDark = QColor(0xffffff),
        .backgroundBrush = QColor(0x0f6cbd),
        .backgroundBrushDark = QColor(0x115ea3)
    },
    .hovered = {
        .backgroundBrush = QColor(0x115ea3),
        .backgroundBrushDark = QColor(0x0f6cbd)
    },
    .pressed = {
        .backgroundBrush = QColor(0x0c3b5e),
        .backgroundBrushDark = QColor(0x0f6cbd)
    },
    .checked = {
        .backgroundBrush = QColor(0x0f548c),
        .backgroundBrushDark = QColor(0x0f548c)
    },
    .disabled = {
        .textColor = QColor(0xbdbdbd),
        .textColorDark = QColor(0x5c5c5c),
        .iconColor = QColor(0xbdbdbd),
        .iconColorDark = QColor(0x5c5c5c),
        .backgroundBrush = QColor(0xf0f0f0),
        .backgroundBrushDark = QColor(0x141414)
    }
};

static void prepare()
{
    // TODO: Find a way to knock down static icon engine plugins too
    // Knock down existing icon engine plugins for the sake of PixelPerfectIconEngine
    Utils::disableExistingIconEngines();
    PixelPerfectScaling::init();
}

Q_CONSTRUCTOR_FUNCTION(prepare);

ACAYIPWIDGETS_END_NAMESPACE
