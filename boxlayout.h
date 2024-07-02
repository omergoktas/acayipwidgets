// Copyright (C) 2024 Ömer Göktaş. All Rights Reserved.

#pragma once

#include <acayipwidgets_export.h>

#include <QBoxLayout>

class BoxLayoutPrivate;

class ACAYIPWIDGETS_EXPORT BoxLayout : public QBoxLayout
{
    Q_OBJECT
    Q_DISABLE_COPY(BoxLayout)

public:
    explicit BoxLayout(Direction direction, QWidget* parent = nullptr);
    ~BoxLayout() override;

    void setGeometry(const QRect& rect) override;

public slots:
    void animate();

private:
    BoxLayoutPrivate* d;
};

class ACAYIPWIDGETS_EXPORT HBoxLayout : public BoxLayout
{
    Q_OBJECT
    Q_DISABLE_COPY(HBoxLayout)
public:
    explicit HBoxLayout(QWidget* parent = nullptr);
};

class ACAYIPWIDGETS_EXPORT VBoxLayout : public BoxLayout
{
    Q_OBJECT
    Q_DISABLE_COPY(VBoxLayout)
public:
    explicit VBoxLayout(QWidget* parent = nullptr);
};
