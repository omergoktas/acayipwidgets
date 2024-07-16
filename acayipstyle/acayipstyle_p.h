// Copyright (C) 2024 Ömer Göktaş. All Rights Reserved.
// SPDX-License-Identifier: LicenseRef-AcayipWidgets-Commercial OR GPL-3.0-only

#pragma once

#include <acayipglobal.h>

#include <QCommonStyle>

ACAYIPWIDGETS_BEGIN_NAMESPACE

class AcayipStylePrivate;

class AcayipStyle : public QCommonStyle
{
    Q_OBJECT
    Q_DISABLE_COPY(AcayipStyle)
    Q_DECLARE_PRIVATE(AcayipStyle)

public:
    AcayipStyle();
    QPalette standardPalette() const override;

protected slots:
    void onColorSchemeChanged() const;

protected:
    AcayipStyle(AcayipStylePrivate& dd);
};

ACAYIPWIDGETS_END_NAMESPACE
