// Copyright (C) 2024 Ömer Göktaş. All Rights Reserved.
// SPDX-License-Identifier: LicenseRef-AcayipWidgets-Commercial OR GPL-3.0-only

#pragma once

#include <acayipconfig.h>
#include <acayipwidgets_export.h>

#include <QWidget>

class QScreen;
class QColor;
class QSize;
class QSizeF;
class QMargins;
class QMarginsF;

ACAYIPWIDGETS_BEGIN_NAMESPACE

inline namespace Utils {

    ACAYIPWIDGETS_EXPORT QColor strongerColor(QColor color, int strength = 70);
    ACAYIPWIDGETS_EXPORT qreal scaled(const QScreen* screen, qreal value);
    ACAYIPWIDGETS_EXPORT int scaled(const QScreen* screen, int value);
    ACAYIPWIDGETS_EXPORT QSizeF scaled(const QScreen* screen, const QSizeF& size);
    ACAYIPWIDGETS_EXPORT QSize scaled(const QScreen* screen, const QSize& size);
    ACAYIPWIDGETS_EXPORT QMarginsF scaled(const QScreen* screen,
                                          const QMarginsF& margins);
    ACAYIPWIDGETS_EXPORT QMargins scaled(const QScreen* screen, const QMargins& margins);

    template <typename... Args>
    auto scaled(const QWidget* widget, Args&&... args)
        -> decltype(scaled(widget->screen(), std::forward<Args>(args)...))
    {
        Q_ASSERT_X(widget, "AcayipWidgets", "null pointer pased");
        if (widget)
            return scaled(widget->screen(), std::forward<Args>(args)...);
        return {};
    }

} // namespace Utils

ACAYIPWIDGETS_END_NAMESPACE
