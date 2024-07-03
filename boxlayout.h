// Copyright (C) 2024 Ömer Göktaş. All Rights Reserved.
// SPDX-License-Identifier: LicenseRef-AcayipWidgets-Commercial OR GPL-3.0-only

#pragma once

#include <acayipglobal.h>

#include <QBoxLayout>

ACAYIPWIDGETS_BEGIN_NAMESPACE

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

ACAYIPWIDGETS_END_NAMESPACE
