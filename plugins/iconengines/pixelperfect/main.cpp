// Copyright (C) 2024 Ömer Göktaş. All Rights Reserved.
// SPDX-License-Identifier: LicenseRef-AcayipWidgets-Commercial OR GPL-3.0-only

#include "pixelperfecticonengine.h"

#include <private/qlibrary_p.h>

#include <QCoreApplication>
#include <QDirIterator>
#include <QIconEnginePlugin>
#include <QLibrary>

using namespace Qt::Literals;

class PixelPerfectIconEnginePlugin final : public QIconEnginePlugin
{
    Q_OBJECT
    Q_DISABLE_COPY(PixelPerfectIconEnginePlugin)
    Q_PLUGIN_METADATA(IID QIconEngineFactoryInterface_iid FILE "pixelperfect.json")

public:
    PixelPerfectIconEnginePlugin() = default;
    QIconEngine* create(const QString& filePath) override;
};

QIconEngine* PixelPerfectIconEnginePlugin::create(const QString& filePath)
{
    return new PixelPerfectIconEngine(filePath);
}

Q_IMPORT_PLUGIN(PixelPerfectIconEnginePlugin)

class QLibraryStore : public QObject
{
public:
    enum LoadStatusTag { NotLoaded, Loaded, IsNotAPlugin = 1 };
    void invalidate() { reinterpret_cast<int&>(d->pluginState) = int(IsNotAPlugin); }
    QTaggedPointer<QLibraryPrivate, LoadStatusTag> d;
};

static void disableExistingIconEngines()
{
    const QStringList& libPaths = QCoreApplication::libraryPaths();
    for (const QString& path : libPaths) {
        QDirIterator plugins(path,
#if defined(Q_OS_WIN)
                             QStringList(QStringLiteral("*.dll")),
#elif defined(Q_OS_ANDROID)
                             QStringList("libplugins_%1_*.so"_L1.arg(suffix)),
#endif
                             QDir::Files,
                             QDirIterator::Subdirectories);
        while (plugins.hasNext()) {
            const QFileInfo& fileInfo = plugins.nextFileInfo();
            const QString& fileName = fileInfo.fileName();

#if defined(Q_PROCESSOR_X86)
            if (fileName.endsWith(".avx2"_L1) || fileName.endsWith(".avx512"_L1))
                continue;
#endif
            if (!fileInfo.filePath().contains("/iconengines/"))
                continue;

            QLibrary lib(fileInfo.canonicalFilePath());
            lib.load();
            reinterpret_cast<QLibraryStore*>(&lib)->invalidate();
        }
    }
}

Q_CONSTRUCTOR_FUNCTION(disableExistingIconEngines);

#include "main.moc"
