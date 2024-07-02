// Copyright (C) 2024 Ömer Göktaş. All Rights Reserved.

#pragma once

#include <QMap>
#include <QParallelAnimationGroup>
#include <QPropertyAnimation>

class BoxLayout;

class BoxLayoutPrivate
{
public:
    BoxLayoutPrivate(BoxLayout* q_ptr);

    void updateAnimations();
    void updateGeometries(bool oldGeometry);

    BoxLayout* q;
    QScopedPointer<QParallelAnimationGroup> animationGroup;
    QMap<QWidget*, QPropertyAnimation*> animations;
    QList<QWidget*> widgets;
};
