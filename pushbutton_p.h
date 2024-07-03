// Copyright (C) 2024 Ömer Göktaş. All Rights Reserved.
// SPDX-License-Identifier: LicenseRef-AcayipWidgets-Commercial OR GPL-3.0-only

/*
 * WARNING: This file exists purely as a private implementation
 * detail. This header file may change from version to version
 * without notice or even be removed.
*/

#pragma once

#include <acayipglobal.h>

#include "pushbutton.h"

#include <private/qpushbutton_p.h>

#include <QColor>
#include <QPropertyAnimation>

ACAYIPWIDGETS_BEGIN_NAMESPACE

class RippleEffect : public QObject
{
    Q_OBJECT
    Q_PROPERTY(qreal radius READ radius WRITE setRadius)

public:
    RippleEffect(const QPoint& center, QObject* parent = nullptr);
    qreal radius() const { return m_radius; }
    void setRadius(qreal radius);
    QPoint center() const { return m_center; }

private:
    QPoint m_center;
    qreal m_radius;
};

class PushButtonPrivate : public QPushButtonPrivate
{
    Q_DECLARE_PUBLIC(PushButton)

public:
    PushButtonPrivate();

    void init();

    QColor backgroundColor;
    QColor textColor;
    QColor borderColor;
    int borderWidth;
    int borderRadius;

    bool isHovered;
    bool isPressed;
    QVector<RippleEffect*> rippleEffects;
    QVector<QPropertyAnimation*> rippleAnimations;
};

ACAYIPWIDGETS_END_NAMESPACE
