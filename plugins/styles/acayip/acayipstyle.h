// Copyright (C) 2024 Ömer Göktaş. All Rights Reserved.
// SPDX-License-Identifier: LicenseRef-AcayipWidgets-Commercial OR GPL-3.0-only

#pragma once

#include <private/qfusionstyle_p.h>

class AcayipStyle final : public QFusionStyle
{
    Q_OBJECT
    Q_DISABLE_COPY(AcayipStyle)

public:
    AcayipStyle() = default;
    QPalette standardPalette() const override;
};
