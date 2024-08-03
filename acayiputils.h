// Copyright (C) 2024 Ömer Göktaş. All Rights Reserved.
// SPDX-License-Identifier: LicenseRef-AcayipWidgets-Commercial OR GPL-3.0-only

#pragma once

#include <acayipconfig.h>
#include <acayipwidgets_export.h>

#include <QWidget>

ACAYIPWIDGETS_BEGIN_NAMESPACE

inline namespace Utils {

    ACAYIPWIDGETS_EXPORT QColor strongerColor(QColor color, int strength = 70);

    ACAYIPWIDGETS_EXPORT qreal scaled(const QScreen* screen,
                                      qreal value,
                                      qreal multiply = 1.0);
    ACAYIPWIDGETS_EXPORT int scaled(const QScreen* screen,
                                    int value,
                                    qreal multiply = 1.0,
                                    bool roundUp = true);
    ACAYIPWIDGETS_EXPORT int scaled(const QScreen* screen,
                                    int value,
                                    int initialValue,
                                    qreal multiply = 1.0,
                                    bool roundUp = true);
    ACAYIPWIDGETS_EXPORT QPointF scaled(const QScreen* screen,
                                        const QPointF& point,
                                        qreal multiply = 1.0);
    ACAYIPWIDGETS_EXPORT QPoint scaled(const QScreen* screen,
                                       const QPoint& point,
                                       qreal multiply = 1.0);
    ACAYIPWIDGETS_EXPORT QPoint scaled(const QScreen* screen,
                                       const QPoint& point,
                                       const QPoint& initialPoint,
                                       qreal multiply = 1.0);
    ACAYIPWIDGETS_EXPORT QSizeF scaled(const QScreen* screen,
                                       const QSizeF& size,
                                       qreal multiply = 1.0);
    ACAYIPWIDGETS_EXPORT QSize scaled(const QScreen* screen,
                                      const QSize& size,
                                      qreal multiply = 1.0);
    ACAYIPWIDGETS_EXPORT QSize scaled(const QScreen* screen,
                                      const QSize& size,
                                      const QSize& initialFont,
                                      qreal multiply = 1.0);
    ACAYIPWIDGETS_EXPORT QRectF scaled(const QScreen* screen,
                                       const QRectF& rect,
                                       qreal multiply = 1.0);
    ACAYIPWIDGETS_EXPORT QRect scaled(const QScreen* screen,
                                      const QRect& rect,
                                      qreal multiply = 1.0);
    ACAYIPWIDGETS_EXPORT QRect scaled(const QScreen* screen,
                                      const QRect& rect,
                                      const QRect& initialRect,
                                      qreal multiply = 1.0);
    ACAYIPWIDGETS_EXPORT QMarginsF scaled(const QScreen* screen,
                                          const QMarginsF& margins,
                                          qreal multiply = 1.0);
    ACAYIPWIDGETS_EXPORT QMargins scaled(const QScreen* screen,
                                         const QMargins& margins,
                                         qreal multiply = 1.0);
    ACAYIPWIDGETS_EXPORT QMargins scaled(const QScreen* screen,
                                         const QMargins& margins,
                                         const QMargins& initialMargins,
                                         qreal multiply = 1.0);
    ACAYIPWIDGETS_EXPORT QFont scaled(const QScreen* screen,
                                      const QFont& font,
                                      qreal multiply = 1.0);
    ACAYIPWIDGETS_EXPORT QFont scaled(const QScreen* screen,
                                      const QFont& font,
                                      const QFont& initialFont,
                                      qreal multiply = 1.0);
    ACAYIPWIDGETS_EXPORT QPen scaled(const QScreen* screen,
                                     const QPen& pen,
                                     qreal multiply = 1.0);

    template <typename... Args>
    auto scaled(const QWidget* widget, Args&&... args)
        -> decltype(scaled(widget->screen(), std::forward<Args>(args)...))
    {
        Q_ASSERT_X(widget, "AcayipUtils", "null pointer pased");
        if (widget)
            return scaled(widget->screen(), std::forward<Args>(args)...);
        return {};
    }

} // namespace Utils

ACAYIPWIDGETS_END_NAMESPACE
