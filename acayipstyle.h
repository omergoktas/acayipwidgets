// Copyright (C) 2024 Ömer Göktaş. All Rights Reserved.

#pragma once

#include <acayipwidgets_export.h>

#include <QCommonStyle>

class AcayipStylePrivate;

class ACAYIPWIDGETS_EXPORT AcayipStyle : public QCommonStyle
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
