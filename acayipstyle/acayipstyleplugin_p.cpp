// Copyright (C) 2024 Ömer Göktaş. All Rights Reserved.
// SPDX-License-Identifier: LicenseRef-AcayipWidgets-Commercial OR GPL-3.0-only

#include "acayipstyleplugin_p.h"
#include "acayipstyle_p.h"

using namespace Qt::Literals;

Q_IMPORT_PLUGIN(AcayipStylePlugin)

ACAYIPWIDGETS_BEGIN_NAMESPACE

QStyle* AcayipStylePlugin::create(const QString& key)
{
    if (key.toLower() == "acayip"_L1)
        return new AcayipStyle();
    return nullptr;
}

ACAYIPWIDGETS_END_NAMESPACE
