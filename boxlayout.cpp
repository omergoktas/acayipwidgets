// Copyright (C) 2024 Ömer Göktaş. All Rights Reserved.
// SPDX-License-Identifier: LicenseRef-AcayipWidgets-Commercial OR GPL-3.0-only

#include "boxlayout.h"
#include "boxlayout_p.h"

#include <QCoreApplication>
#include <QWidget>

ACAYIPWIDGETS_BEGIN_NAMESPACE

BoxLayoutPrivate::BoxLayoutPrivate(BoxLayout* q_ptr)
    : q(q_ptr)
    , animationGroup(new QParallelAnimationGroup())
{}

void BoxLayoutPrivate::updateAnimations()
{
    widgets.clear();

    int n = q->count();
    for (int i = 0; i < n; ++i) {
        QLayoutItem* item = q->itemAt(i);
        if (item->widget())
            widgets.append(item->widget());
    }

    const QList<QWidget*>& oldWidgets = animations.keys();
    for (QWidget* widget : oldWidgets) {
        if (!widgets.contains(widget))
            delete animations.take(widget);
    }

    for (QWidget* widget : std::as_const(widgets)) {
        if (!animations.contains(widget)) {
            auto animation = new QPropertyAnimation(widget,
                                                    "geometry",
                                                    animationGroup.data());
            animation->setDuration(500);
            animation->setEasingCurve(QEasingCurve::OutExpo);
            animations[widget] = animation;
            animationGroup->addAnimation(animation);
        }
    }
}

void BoxLayoutPrivate::updateGeometries(bool oldGeometry)
{
    for (QWidget* widget : std::as_const(widgets)) {
        QPropertyAnimation* animation = animations[widget];
        if (oldGeometry)
            animation->setStartValue(widget->geometry());
        else
            animation->setEndValue(widget->geometry());
    }
}

BoxLayout::BoxLayout(Direction direction, QWidget* parent)
    : QBoxLayout(direction, parent)
    , d(new BoxLayoutPrivate(this))
{}

BoxLayout::~BoxLayout()
{
    delete d;
}

void BoxLayout::setGeometry(const QRect& rect)
{
    d->updateAnimations();
    d->updateGeometries(true);
    QBoxLayout::setGeometry(rect);
    d->updateGeometries(false);
}

void BoxLayout::animate()
{
    d->animationGroup->start();
}

HBoxLayout::HBoxLayout(QWidget* parent)
    : BoxLayout(LeftToRight, parent)
{}

VBoxLayout::VBoxLayout(QWidget* parent)
    : BoxLayout(TopToBottom, parent)
{}

ACAYIPWIDGETS_END_NAMESPACE
