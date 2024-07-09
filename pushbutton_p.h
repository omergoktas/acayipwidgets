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
#include <QTextDocument>
#include <QTextLine>
#include <QVariantAnimation>

ACAYIPWIDGETS_BEGIN_NAMESPACE

class PushButtonPrivate : public QPushButtonPrivate
{
    Q_DECLARE_PUBLIC(PushButton)

public:
    PushButtonPrivate();
    ~PushButtonPrivate();
    enum Item { Background, Icon, Text };
    void init();
    void mergeStyleWithRest(ButtonStyle& target, const ButtonStyle& source) const;
    void updateTextDocumentContent();
    void redoTextLayout();
    QRectF itemRect(Item item) const;
    qreal calculateRadius(int value) const;
    void startRippleAnimation(const QPoint& pos);
    QPainterPath backgroundPath() const;
    const ButtonStyle& activeStyle() const;
    bool isRippling() const;
    qreal shortestActiveRippleAnimationTime() const;

    bool elevated;
    qreal opacity;
    qreal spacing;
    QMarginsF margins;
    QMarginsF paddings;
    Qt::Edge iconEdge;
    Qt::TextFormat textFormat;
    ButtonStyles styles;
    QTextDocument textDocument;
    QList<QTextLine> textLines;
    QRectF naturalTextRect;
    QCursor cursor;
    QList<QVariantAnimation*> rippleAnimations;
};

ACAYIPWIDGETS_END_NAMESPACE
