// Copyright (C) 2024 Ömer Göktaş. All Rights Reserved.
// SPDX-License-Identifier: LicenseRef-AcayipWidgets-Commercial OR GPL-3.0-only

/*
 * WARNING: This file exists purely as a private implementation
 * detail. This header file may change from version to version
 * without notice or even be removed.
*/

#pragma once

#include "button.h"

#include <private/qpushbutton_p.h>

#include <QColor>
#include <QPointer>
#include <QPropertyAnimation>
#include <QTextDocument>
#include <QTextLine>
#include <QVariantAnimation>

class QGraphicsDropShadowEffect;

ACAYIPWIDGETS_BEGIN_NAMESPACE

class ButtonPrivate : public QPushButtonPrivate
{
    Q_DECLARE_PUBLIC(Button)

public:
    ButtonPrivate();
    enum Item { Background, Icon, Menu, Text };
    void init();
    void mergeStyleWithRest(Button::Style& target,
                            const Button::Style& source,
                            const Button::Style& fallback) const;
    void updateTextDocumentContent();
    QRect itemRect(Item item) const;
    qreal calculateRadius(qreal value) const;
    void startRippleAnimation(const QPoint& pos);
    QPainterPath backgroundPath(const QMarginsF& margins = QMargins()) const;
    const Button::Style& activeStyle() const;
    bool isRippling() const;
    qreal shortestActiveRippleAnimationTime() const;
    void updateHoverShadow();
    void updateFont();
    QIcon::Mode iconMode() const;
    QIcon::State iconState() const;

    bool mouseAttached;
    bool hoverShadowEnabled;
    bool elevated;
    qreal opacity;
    int spacing;
    QMargins margins;
    QMargins paddings;
    Qt::Edge iconEdge;
    Qt::TextFormat textFormat;
    Button::Styles styles;
    Button::Styles stylesPainted;
    Button::Style& rippleStyle;
    QTextDocument textDocument;
    QSize textDocumentSizeHint;
    QCursor cursor;
    QBrush rippleBrush;
    QBrush rippleBrushDark;
    QIcon menuArrow;
    QPointer<QGraphicsDropShadowEffect> shadowEffect;
    QPropertyAnimation shadowAnimation;
    QVariantAnimation showHideAnimation;
    QList<QVariantAnimation*> rippleAnimations;
};

ACAYIPWIDGETS_END_NAMESPACE
