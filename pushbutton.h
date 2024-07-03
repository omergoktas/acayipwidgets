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
    ~PushButton() override;

private:
    using QPushButton::isFlat;
    using QPushButton::setFlat;

protected:
    PushButton(PushButtonPrivate& dd, QWidget* parent);
};

ACAYIPWIDGETS_END_NAMESPACE
