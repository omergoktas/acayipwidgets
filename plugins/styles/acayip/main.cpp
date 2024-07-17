// Copyright (C) 2024 Ömer Göktaş. All Rights Reserved.
// SPDX-License-Identifier: LicenseRef-AcayipWidgets-Commercial OR GPL-3.0-only

#include "acayipstyle.h"

#include <QStylePlugin>

using namespace Qt::Literals;

class AcayipStylePlugin final : public QStylePlugin
{
    Q_OBJECT
    Q_DISABLE_COPY(AcayipStylePlugin)
    Q_PLUGIN_METADATA(IID QStyleFactoryInterface_iid FILE "acayip.json")

public:
    AcayipStylePlugin() = default;
    QStyle* create(const QString& key) override;
};

QStyle* AcayipStylePlugin::create(const QString& key)
{
    if (key.toLower() == "acayip"_L1)
        return new AcayipStyle();
    return nullptr;
}

Q_IMPORT_PLUGIN(AcayipStylePlugin)

#include "main.moc"
