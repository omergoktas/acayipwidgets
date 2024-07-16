// Copyright (C) 2024 Ömer Göktaş. All Rights Reserved.
// SPDX-License-Identifier: LicenseRef-AcayipWidgets-Commercial OR GPL-3.0-only

#pragma once

#include <acayipglobal.h>

#include <QStylePlugin>

ACAYIPWIDGETS_BEGIN_NAMESPACE

class AcayipStylePlugin : public QStylePlugin
{
    Q_OBJECT
    Q_DISABLE_COPY(AcayipStylePlugin)
    Q_PLUGIN_METADATA(IID QStyleFactoryInterface_iid FILE "acayipstyle.json")

public:
    AcayipStylePlugin() = default;
    QStyle* create(const QString& key) override;
};

ACAYIPWIDGETS_END_NAMESPACE
