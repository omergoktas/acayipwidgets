// Copyright (C) 2024 Ömer Göktaş. All Rights Reserved.
// SPDX-License-Identifier: LicenseRef-AcayipWidgets-Commercial OR GPL-3.0-only

#pragma once

#include <acayipglobal.h>

#include <QPushButton>

ACAYIPWIDGETS_BEGIN_NAMESPACE

class ButtonPrivate;

class ACAYIPWIDGETS_EXPORT Button : public QPushButton
{
    Q_OBJECT
    Q_DISABLE_COPY(Button)
    Q_DECLARE_PRIVATE(Button)

    Q_PROPERTY(qreal opacity READ opacity WRITE setOpacity)
    Q_PROPERTY(int spacing READ spacing WRITE setSpacing)
    Q_PROPERTY(QMargins margins READ margins WRITE setMargins)
    Q_PROPERTY(QMargins paddings READ paddings WRITE setPaddings)
    Q_PROPERTY(Qt::Edge iconEdge READ iconEdge WRITE setIconEdge)
    Q_PROPERTY(Qt::TextFormat textFormat READ textFormat WRITE setTextFormat)
    Q_PROPERTY(Styles styles READ styles WRITE setStyles)
    Q_PROPERTY(bool elevated READ isElevated WRITE setElevated)
    Q_PROPERTY(
        bool hoverShadowEnabled READ isHoverShadowEnabled WRITE setHoverShadowEnabled)

public:
    struct Style
    {
        qreal borderRadius{-1.0};
        QPen borderPen{Qt::NoPen};
        QPen borderPenDark{Qt::NoPen};
        QColor textColor;
        QColor textColorDark;
        QColor iconColor;
        QColor iconColorDark;
        QBrush backgroundBrush;
        QBrush backgroundBrushDark;
        QFont font;
    };

    struct Styles
    {
        Style rest;
        Style restHovered;
        Style restPressed;
        Style restDisabled;
        Style checked;
        Style checkedHovered;
        Style checkedPressed;
        Style checkedDisabled;
    };

public:
    explicit Button(const QString& text = QString(),
                    const QIcon& icon = QIcon(),
                    QWidget* parent = nullptr);

    qreal opacity() const;
    void setOpacity(qreal opacity);

    int spacing() const;
    void setSpacing(int spacing);

    QMargins margins() const;
    void setMargins(const QMargins& margins);

    QMargins paddings() const;
    void setPaddings(const QMargins& paddings);

    Qt::Edge iconEdge() const;
    void setIconEdge(Qt::Edge iconEdge);

    Qt::TextFormat textFormat() const;
    void setTextFormat(Qt::TextFormat textFormat);

    const QBrush& rippleBrush(bool dark) const;
    void setRippleBrush(const QBrush& brush, const QBrush& brushDark);

    const Styles& styles() const;
    void setStyles(const Styles& styles);

    bool isElevated() const;
    void setElevated(bool elevated);

    bool isHoverShadowEnabled() const;
    void setHoverShadowEnabled(bool hoverShadowEnabled);

    void setText(const QString& text);

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

signals:
    void animatedClicked();

public slots:
    void hideAnimated();
    void showAnimated();

protected:
    bool hitButton(const QPoint& pos) const override;
    bool event(QEvent* event) override;
    void changeEvent(QEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void paintEvent(QPaintEvent* event) override;

private:
    using QPushButton::autoDefault;
    using QPushButton::font;
    using QPushButton::isFlat;
    using QPushButton::setAutoDefault;
    using QPushButton::setFlat;
    using QPushButton::setFont;

protected:
    Button(ButtonPrivate& dd, QWidget* parent);
};

ACAYIPWIDGETS_END_NAMESPACE

Q_DECLARE_METATYPE(ACAYIPWIDGETS_PREPEND_NAMESPACE(Button::Style))
Q_DECLARE_METATYPE(ACAYIPWIDGETS_PREPEND_NAMESPACE(Button::Styles))
