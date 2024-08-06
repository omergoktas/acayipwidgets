// Copyright (C) 2024 Ömer Göktaş. All Rights Reserved.
// SPDX-License-Identifier: LicenseRef-AcayipWidgets-Commercial OR GPL-3.0-only

#pragma once

#include <acayipglobal.h>

#include <QScrollArea>

ACAYIPWIDGETS_BEGIN_NAMESPACE

class BoxLayout;
class TabsPrivate;

class ACAYIPWIDGETS_EXPORT Tabs : public QScrollArea
{
    Q_OBJECT
    Q_DISABLE_COPY(Tabs)
    Q_DECLARE_PRIVATE(Tabs)

    Q_PROPERTY(qreal opacity READ opacity WRITE setOpacity)
    Q_PROPERTY(QMargins margins READ margins WRITE setMargins)
    Q_PROPERTY(QMargins paddings READ paddings WRITE setPaddings)
    Q_PROPERTY(Style style READ style WRITE setStyle)
    Q_PROPERTY(Qt::Orientation orientation READ orientation WRITE setOrientation)

public:
    struct Style
    {
        qreal borderRadius{-1.0};
        QPen borderPen{Qt::NoPen};
        QPen borderPenDark{Qt::NoPen};
        QBrush backgroundBrush;
        QBrush backgroundBrushDark;
        QBrush selectionBrush;
        QBrush selectionBrushDark;
    };

public:
    explicit Tabs(QWidget* parent = nullptr);

    BoxLayout* contentLayout();

    qreal opacity() const;
    void setOpacity(qreal opacity);

    QMargins margins() const;
    void setMargins(const QMargins& margins);

    QMargins paddings() const;
    void setPaddings(const QMargins& paddings);

    const Style& style() const;
    void setStyle(const Style& style);

    Qt::Orientation orientation() const;
    void setOrientation(Qt::Orientation orientation);

    void addButton(const QString& text = QString(), const QIcon& icon = QIcon());

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    using QScrollArea::setWidget;
    using QScrollArea::setWidgetResizable;
    using QScrollArea::takeWidget;
    using QScrollArea::widget;
    using QScrollArea::widgetResizable;

protected:
    Tabs(TabsPrivate& dd, QWidget* parent);
};

ACAYIPWIDGETS_END_NAMESPACE
