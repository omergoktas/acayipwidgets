// Copyright (C) 2024 Ömer Göktaş. All Rights Reserved.

#pragma once

#include <acayipwidgets_export.h>

#include <QAbstractButton>

class ButtonPrivate;

class ACAYIPWIDGETS_EXPORT Button : public QAbstractButton
{
    Q_OBJECT
    Q_DISABLE_COPY(Button)
    Q_DECLARE_PRIVATE(Button)

    Q_PROPERTY(ButtonType buttonType READ buttonType WRITE setButtonType NOTIFY
                   buttonTypeChanged)
    Q_PROPERTY(ButtonStyle buttonStyle READ buttonStyle WRITE setButtonStyle NOTIFY
                   buttonStyleChanged)
    Q_PROPERTY(QColor backgroundColor READ backgroundColor WRITE setBackgroundColor
                   NOTIFY backgroundColorChanged)
    Q_PROPERTY(QColor textColor READ textColor WRITE setTextColor NOTIFY textColorChanged)
    Q_PROPERTY(QColor borderColor READ borderColor WRITE setBorderColor NOTIFY
                   borderColorChanged)
    Q_PROPERTY(
        int borderWidth READ borderWidth WRITE setBorderWidth NOTIFY borderWidthChanged)
    Q_PROPERTY(int borderRadius READ borderRadius WRITE setBorderRadius NOTIFY
                   borderRadiusChanged)
    Q_PROPERTY(bool outlined READ isOutlined WRITE setOutlined NOTIFY outlinedChanged)
    Q_PROPERTY(bool elevated READ isElevated WRITE setElevated NOTIFY elevatedChanged)
    Q_PROPERTY(int elevation READ elevation WRITE setElevation NOTIFY elevationChanged)

public:
    enum ButtonType { Standard, Primary, Secondary, Accent, FAB };
    Q_ENUM(ButtonType)

    enum ButtonStyle { Filled, Outlined, Text };
    Q_ENUM(ButtonStyle)

    explicit Button(QWidget* parent = nullptr);
    explicit Button(const QString& text, QWidget* parent = nullptr);
    ~Button();

    ButtonType buttonType() const;
    void setButtonType(ButtonType type);

    ButtonStyle buttonStyle() const;
    void setButtonStyle(ButtonStyle style);

    QColor backgroundColor() const;
    void setBackgroundColor(const QColor& color);

    QColor textColor() const;
    void setTextColor(const QColor& color);

    QColor borderColor() const;
    void setBorderColor(const QColor& color);

    int borderWidth() const;
    void setBorderWidth(int width);

    int borderRadius() const;
    void setBorderRadius(int radius);

    bool isOutlined() const;
    void setOutlined(bool outlined);

    bool isElevated() const;
    void setElevated(bool elevated);

    int elevation() const;
    void setElevation(int elevation);

    QSize sizeHint() const override;

signals:
    void buttonTypeChanged(ButtonType type);
    void buttonStyleChanged(ButtonStyle style);
    void backgroundColorChanged(const QColor& color);
    void textColorChanged(const QColor& color);
    void borderColorChanged(const QColor& color);
    void borderWidthChanged(int width);
    void borderRadiusChanged(int radius);
    void outlinedChanged(bool outlined);
    void elevatedChanged(bool elevated);
    void elevationChanged(int elevation);

protected:
    Button(ButtonPrivate& dd, QWidget* parent = nullptr);

    void paintEvent(QPaintEvent* event) override;
    void enterEvent(QEnterEvent* event) override;
    void leaveEvent(QEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
};
