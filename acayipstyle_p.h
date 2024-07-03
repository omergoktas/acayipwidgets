// Copyright (C) 2024 Ömer Göktaş. All Rights Reserved.
// SPDX-License-Identifier: LicenseRef-AcayipWidgets-Commercial OR GPL-3.0-only

/*
 * WARNING: This file exists purely as a private implementation
 * detail. This header file may change from version to version
 * without notice or even be removed.
*/

#pragma once

#include <acayipglobal.h>

#include <private/qcommonstyle_p.h>

ACAYIPWIDGETS_BEGIN_NAMESPACE

class AcayipStylePrivate : public QCommonStylePrivate
{
    Q_DECLARE_PUBLIC(AcayipStyle)

public:
    AcayipStylePrivate();
};

ACAYIPWIDGETS_END_NAMESPACE
