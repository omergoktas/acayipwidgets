// Copyright (C) 2024 Ömer Göktaş. All Rights Reserved.
// SPDX-License-Identifier: LicenseRef-AcayipWidgets-Commercial OR GPL-3.0-only

#pragma once

#include <acayipglobal.h>

#include <QPushButton>

ACAYIPWIDGETS_BEGIN_NAMESPACE

class PushButtonPrivate;

class ACAYIPWIDGETS_EXPORT PushButton : public QPushButton
{
    Q_OBJECT
    Q_DISABLE_COPY(PushButton)
    Q_DECLARE_PRIVATE(PushButton)

public:
    explicit PushButton(QWidget* parent = nullptr);
    explicit PushButton(const QString& text, QWidget* parent = nullptr);
    explicit PushButton(const QIcon& icon,
                        const QString& text,
                        QWidget* parent = nullptr);

    qreal opacity() const;
    void setOpacity(qreal opacity);

    qreal spacing() const;
    void setSpacing(qreal spacing);

    const QMarginsF& margins() const;
    void setMargins(const QMarginsF& margins);

    const QMarginsF& paddings() const;
    void setPaddings(const QMarginsF& paddings);

    Qt::Edge iconEdge() const;
    void setIconEdge(Qt::Edge iconEdge);

    Qt::TextFormat textFormat() const;
    void setTextFormat(Qt::TextFormat textFormat);

    const QBrush& rippleBrush(bool dark) const;
    void setRippleBrush(const QBrush& brush, const QBrush& brushDark);

    const ButtonStyles& styles() const;
    void setStyles(const ButtonStyles& styles);

    void setElevated(bool elevated);
    bool isElevated() const;

    void setHoverShadowEnabled(bool hoverShadowEnabled);
    bool isHoverShadowEnabled() const;

    void setText(const QString& text);

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

public slots:
    void hideAnimated();
    void showAnimated();

protected:
    bool hitButton(const QPoint& pos) const override;
    bool event(QEvent* event) override;
    void changeEvent(QEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void paintEvent(QPaintEvent* event) override;

private:
    using QPushButton::autoDefault;
    using QPushButton::isFlat;
    using QPushButton::setAutoDefault;
    using QPushButton::setFlat;

protected:
    PushButton(PushButtonPrivate& dd, QWidget* parent);
};

ACAYIPWIDGETS_END_NAMESPACE
