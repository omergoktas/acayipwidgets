// Copyright (C) 2024 Ömer Göktaş. All Rights Reserved.

#pragma once

#include "button.h"
#include <private/qabstractbutton_p.h>
#include <QColor>
#include <QPropertyAnimation>

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

class ButtonPrivate : public QAbstractButtonPrivate
{
    Q_DECLARE_PUBLIC(Button)

public:
    ButtonPrivate();

    void init();
    void updateColors();
    void drawBackground(QPainter& painter);
    void drawBorder(QPainter& painter);
    void drawText(QPainter& painter);
    void drawIcon(QPainter& painter);
    void drawRippleEffect(QPainter& painter);
    void updateCursor();
    void startRippleAnimation(const QPoint& pos);

    Button::ButtonType buttonType;
    Button::ButtonStyle buttonStyle;
    QColor backgroundColor;
    QColor textColor;
    QColor borderColor;
    int borderWidth;
    int borderRadius;
    bool outlined;
    bool elevated;
    int elevation;

    bool isHovered;
    bool isPressed;
    QVector<RippleEffect*> rippleEffects;
    QVector<QPropertyAnimation*> rippleAnimations;
};
