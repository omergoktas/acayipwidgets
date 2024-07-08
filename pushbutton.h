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

    Q_PROPERTY(qreal opacity READ opacity WRITE setOpacity NOTIFY opacityChanged)
    Q_PROPERTY(qreal spacing READ spacing WRITE setSpacing NOTIFY spacingChanged)
    Q_PROPERTY(QMarginsF margins READ margins WRITE setMargins NOTIFY marginsChanged)
    Q_PROPERTY(QMarginsF paddings READ paddings WRITE setPaddings NOTIFY paddingsChanged)
    Q_PROPERTY(Qt::Edge iconEdge READ iconEdge WRITE setIconEdge NOTIFY iconEdgeChanged)
    Q_PROPERTY(Qt::TextFormat textFormat READ textFormat WRITE setTextFormat NOTIFY
                   textFormatChanged)
    Q_PROPERTY(ButtonStyles styles READ styles WRITE setStyles NOTIFY stylesChanged)

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

    const ButtonStyles& styles() const;
    void setStyles(const ButtonStyles& styles);

    void setElevated(bool elevated);
    bool isElevated() const;

    void setText(const QString& text);

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

protected:
    bool event(QEvent* event) override;
    void changeEvent(QEvent* event) override;
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    bool hitButton(const QPoint& pos) const override;

public slots:
    void hideAnimated();
    void showAnimated();

signals:
    void opacityChanged(qreal opacity);
    void spacingChanged(qreal spacing);
    void marginsChanged(const QMarginsF& margins);
    void paddingsChanged(const QMarginsF& paddings);
    void iconEdgeChanged(Qt::Edge iconEdge);
    void textFormatChanged(Qt::TextFormat textFormat);
    void stylesChanged(const ButtonStyles& styles);

private:
    using QPushButton::autoDefault;
    using QPushButton::isFlat;
    using QPushButton::setAutoDefault;
    using QPushButton::setFlat;

protected:
    PushButton(PushButtonPrivate& dd, QWidget* parent);
};

ACAYIPWIDGETS_END_NAMESPACE
