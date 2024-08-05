// Copyright (C) 2024 Ömer Göktaş. All Rights Reserved.
// SPDX-License-Identifier: LicenseRef-AcayipWidgets-Commercial OR GPL-3.0-only

#include "acayipglobal.h"
#include "pixelperfectscaling_p.h"
#include "utils_p.h"

#include <private/qabstractanimation_p.h>

#include <QCoreApplication>

using namespace Qt::Literals;

ACAYIPWIDGETS_BEGIN_NAMESPACE

const int Defaults::borderRadiusPercentagePoint = 1000;

int Defaults::animationDuration{300};

#if defined(ACAYIP_PLATFORM_MOBILE)
qreal Defaults::extraScaleFactor{1.2};
#else
qreal Defaults::extraScaleFactor{1.0};
#endif

int Defaults::spacing{6};

QMargins Defaults::paddings{6, 6, 6, 6};

QMargins Defaults::margins{0, 0, 0, 0};

QEasingCurve::Type Defaults::outEasingType{QEasingCurve::OutCubic};

QEasingCurve::Type Defaults::inEasingType{QEasingCurve::InCubic};

QPainter::RenderHints Defaults::renderHints {
    QPainter::Antialiasing |
    QPainter::TextAntialiasing |
    QPainter::SmoothPixmapTransform |
    QPainter::VerticalSubpixelPositioning |
    QPainter::LosslessImageRendering
};

static void prepare()
{
    PixelPerfectScaling::init();
}

static void init()
{
    // FIXME: Find a way to knock down static icon engine plugins too
    // Knock down existing icon engine plugins for the sake of PixelPerfectIconEngine
    Utils::disableExistingIconEngines();

    QUnifiedTimer::instance()->setTimingInterval(
        1000.0 / qBound(30.0, QGuiApplication::primaryScreen()->refreshRate(), 120.0));
}

Q_CONSTRUCTOR_FUNCTION(prepare);
Q_COREAPP_STARTUP_FUNCTION(init);

ACAYIPWIDGETS_END_NAMESPACE
