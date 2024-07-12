// Copyright (C) 2024 Ömer Göktaş. All Rights Reserved.
// SPDX-License-Identifier: LicenseRef-AcayipWidgets-Commercial OR GPL-3.0-only

/*
 * WARNING: This file exists purely as a private implementation
 * detail. This header file may change from version to version
 * without notice or even be removed.
*/

#pragma once

#include <acayipglobal.h>

#include <QMap>
#include <QParallelAnimationGroup>
#include <QPropertyAnimation>

ACAYIPWIDGETS_BEGIN_NAMESPACE

class BoxLayout;

/*
 * We don't implement the private class like we usually do
 * because Qt made the symbols of QBoxLayoutPrivate hidden
*/

class BoxLayoutPrivate
{
public:
    BoxLayoutPrivate(BoxLayout* q_ptr);

    void init();
    void updateAnimations();
    void updateGeometries(bool oldGeometry);

    BoxLayout* q;
    QParallelAnimationGroup animationGroup;
    QMap<QWidget*, QPropertyAnimation*> animations;
    QList<QWidget*> widgets;
};

ACAYIPWIDGETS_END_NAMESPACE
