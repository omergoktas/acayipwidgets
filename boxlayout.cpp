// Copyright (C) 2024 Ömer Göktaş. All Rights Reserved.
// SPDX-License-Identifier: LicenseRef-AcayipWidgets-Commercial OR GPL-3.0-only

#include "boxlayout.h"
#include "boxlayout_p.h"

#include <QWidget>

using namespace Qt::Literals;

ACAYIPWIDGETS_BEGIN_NAMESPACE

BoxLayoutPrivate::BoxLayoutPrivate(BoxLayout* q_ptr)
    : q(q_ptr)
{}

void BoxLayoutPrivate::init()
{
    QObject::connect(&animationGroup, &QPropertyAnimation::finished, q, [this] {
        q->setEnabled(true);
        q->activate();
    });
    q->setSpacing(Defaults::spacing);
    q->setContentsMargins(Defaults::paddings);
}

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
                                                    "geometry"_ba,
                                                    &animationGroup);
            animation->setDuration(Defaults::animationDuration);
            animation->setEasingCurve(Defaults::outEasingType);
            animations[widget] = animation;
            animationGroup.addAnimation(animation);
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
{
    d->init();
}

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
    activate();
    d->animationGroup.start();
    foreach (auto layout, findChildren<QLayout*>(Qt::FindDirectChildrenOnly)) {
        if (layout->metaObject()->indexOfMethod("animate()") != -1)
            QMetaObject::invokeMethod(layout, "animate");
    }
}

void BoxLayout::prepareAnimate()
{
    setEnabled(false);
    foreach (auto layout, findChildren<QLayout*>(Qt::FindDirectChildrenOnly)) {
        if (layout->metaObject()->indexOfMethod("animate()") != -1)
            layout->setEnabled(false);
    }
}

HBoxLayout::HBoxLayout(QWidget* parent)
    : BoxLayout(LeftToRight, parent)
{}

VBoxLayout::VBoxLayout(QWidget* parent)
    : BoxLayout(TopToBottom, parent)
{}

ACAYIPWIDGETS_END_NAMESPACE
