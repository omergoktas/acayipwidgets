// Copyright (C) 2024 Ömer Göktaş. All Rights Reserved.
// SPDX-License-Identifier: LicenseRef-AcayipWidgets-Commercial OR GPL-3.0-only

#include "pixelperfecticonengine.h"

#include <QIconEnginePlugin>
#include <QPixmapCache>

class PixelPerfectIconEnginePlugin final : public QIconEnginePlugin
{
    Q_OBJECT
    Q_DISABLE_COPY(PixelPerfectIconEnginePlugin)
    Q_PLUGIN_METADATA(IID QIconEngineFactoryInterface_iid FILE "pixelperfect.json")

public:
    PixelPerfectIconEnginePlugin();
    QIconEngine* create(const QString& filePath) override;
};

PixelPerfectIconEnginePlugin::PixelPerfectIconEnginePlugin()
{
    QPixmapCache::setCacheLimit(102400); // Set pixmap cache to 100 Mb
}

QIconEngine* PixelPerfectIconEnginePlugin::create(const QString& filePath)
{
    return new PixelPerfectIconEngine(filePath);
}

Q_IMPORT_PLUGIN(PixelPerfectIconEnginePlugin)

#include "main.moc"
