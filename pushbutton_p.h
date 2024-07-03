// Copyright (C) 2024 Ömer Göktaş. All Rights Reserved.
// SPDX-License-Identifier: LicenseRef-AcayipWidgets-Commercial OR GPL-3.0-only

/*
 * WARNING: This file exists purely as a private implementation
 * detail. This header file may change from version to version
 * without notice or even be removed.
*/

#pragma once

#include <acayipglobal.h>

#include "pushbutton.h"

#include <private/qpushbutton_p.h>

#include <QColor>
#include <QPropertyAnimation>

ACAYIPWIDGETS_BEGIN_NAMESPACE

class PushButtonPrivate : public QPushButtonPrivate
{
    Q_DECLARE_PUBLIC(PushButton)

public:
    PushButtonPrivate();
    void init();
    void mergeStyleWithRest(ButtonStyle& target, const ButtonStyle& source) const;

    ButtonStyles styles;
};

ACAYIPWIDGETS_END_NAMESPACE
