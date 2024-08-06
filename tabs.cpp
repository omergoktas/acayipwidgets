// Copyright (C) 2024 Ömer Göktaş. All Rights Reserved.
// SPDX-License-Identifier: LicenseRef-AcayipWidgets-Commercial OR GPL-3.0-only

#include "boxlayout.h"
#include "button.h"
#include "tabs_p.h"

using namespace Qt::Literals;

ACAYIPWIDGETS_BEGIN_NAMESPACE

/*!
 *  \internal
*/
TabsPrivate::TabsPrivate()
    : QScrollAreaPrivate()
    , opacity(1.0)
    , contentLayout(new BoxLayout(BoxLayout::TopToBottom, new QWidget()))
{
    contentLayout->setContentsMargins(0, 0, 0, 0);
    contentLayout->setSizeConstraint(BoxLayout::SetMinAndMaxSize);
}

void TabsPrivate::init()
{
    Q_Q(Tabs);

    q->setWidgetResizable(true);
    q->setWidget(contentLayout->parentWidget());
    q->setContentsMargins(0, 0, 0, 0);
    q->setViewportMargins(0, 0, 0, 0);
    q->setOrientation(Qt::Vertical);
    q->setSizeAdjustPolicy(QScrollArea::AdjustToContents);
    //q->setFrameShape(QFrame::NoFrame);
    q->setStyle({
        .backgroundBrush = QColor(0x0f6cbd),
        .backgroundBrushDark = QColor(0x115ea3),
        .selectionBrush = QColor(0x0f6cbd),
        .selectionBrushDark = QColor(0x115ea3)
    });
}

qreal TabsPrivate::calculateRadius(qreal value) const
{
    Q_Q(const Tabs);
    const QRect& b = q->viewport()->geometry();
    return value > Defaults::borderRadiusPercentagePoint
               ? (qMin(b.width(), b.height()) / 2.0)
                     * (value - Defaults::borderRadiusPercentagePoint) / 100.0
               : (value < 0 ? 0 : value);
}

Tabs::Tabs(QWidget* parent)
    : Tabs(*new TabsPrivate, parent)
{}

/*!
 *  \internal
*/
Tabs::Tabs(TabsPrivate& dd, QWidget* parent)
    : QScrollArea(dd, parent)
{
    Q_D(Tabs);
    d->init();
}

BoxLayout* Tabs::contentLayout()
{
    Q_D(Tabs);
    return d->contentLayout;
}

qreal Tabs::opacity() const
{
    Q_D(const Tabs);
    return d->opacity;
}

void Tabs::setOpacity(qreal opacity)
{
    Q_D(Tabs);
    d->opacity = opacity;
    update();
}

QMargins Tabs::margins() const
{
    return viewportMargins();
}

void Tabs::setMargins(const QMargins& margins)
{
    setViewportMargins(margins);
    updateGeometry();
    update();
}

QMargins Tabs::paddings() const
{
    Q_D(const Tabs);
    return d->contentLayout->contentsMargins();
}

void Tabs::setPaddings(const QMargins& paddings)
{
    Q_D(Tabs);
    d->contentLayout->setContentsMargins(paddings);
    updateGeometry();
    update();
}

const Tabs::Style& Tabs::style() const
{
    Q_D(const Tabs);
    return d->style;
}

void Tabs::setStyle(const Style& style)
{
    Q_D(Tabs);
    d->style = style;
    update();
}

Qt::Orientation Tabs::orientation() const
{
    Q_D(const Tabs);
    return d->contentLayout->direction() == BoxLayout::LeftToRight ? Qt::Horizontal
                                                                   : Qt::Vertical;
}

void Tabs::setOrientation(Qt::Orientation orientation)
{
    Q_D(Tabs);
    if (orientation == Qt::Horizontal) {
        d->contentLayout->setDirection(BoxLayout::LeftToRight);
        setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    } else {
        d->contentLayout->setDirection(BoxLayout::TopToBottom);
        setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    }
}

void Tabs::addButton(const QString& text, const QIcon& icon)
{
    Q_D(Tabs);

    QFont f(font());
    f.setBold(true);

    auto button = new Button(text, icon);
    button->setCheckable(true);
    button->setAutoExclusive(true);
    button->setHoverShadowEnabled(false);
    button->setStyles({
        .rest = {
            .borderRadius = Defaults::borderRadiusPercentagePoint + 20.0,
            .textColor = QColor(0xffffff),
            .textColorDark = QColor(0xffffff),
            .iconColor = QColor(0xffffff),
            .iconColorDark = QColor(0xffffff),
        },
        .hovered = {
            .backgroundBrush = QColor::fromRgba(0x10000000),
            .backgroundBrushDark = QColor::fromRgba(0x10000000)
        },
        .pressed = {
            .backgroundBrush = QColor::fromRgba(0x30000000),
            .backgroundBrushDark = QColor::fromRgba(0x30000000)
        },
        .checked = {
            .backgroundBrush = QColor::fromRgba(0x20000000),
            .backgroundBrushDark = QColor::fromRgba(0x20000000),
            .font = f
        }
    });
    d->contentLayout->addWidget(button);
    // d->contentLayout->addStretch();
}

void Tabs::paintEvent(QPaintEvent*)
{

}

ACAYIPWIDGETS_END_NAMESPACE
