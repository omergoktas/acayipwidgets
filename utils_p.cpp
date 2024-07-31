// Copyright (C) 2024 Ömer Göktaş. All Rights Reserved.
// SPDX-License-Identifier: LicenseRef-AcayipWidgets-Commercial OR GPL-3.0-only

#include "utils_p.h"

#include <private/qlibrary_p.h>

#include <QDirIterator>
#include <QGuiApplication>

using namespace Qt::Literals;

class QLibraryStore final : public QObject
{
public:
    enum LoadStatusTag { NotLoaded, Loaded, IsNotAPlugin = 1 };
    void invalidate() { reinterpret_cast<int&>(d->pluginState) = int(IsNotAPlugin); }
    QTaggedPointer<QLibraryPrivate, LoadStatusTag> d;
};

ACAYIPWIDGETS_BEGIN_NAMESPACE

void Utils::disableExistingIconEngines()
{
    foreach (const QString& path, QCoreApplication::libraryPaths()) {
        QDirIterator plugins(path,
#if defined(Q_OS_WIN)
                             QStringList(u"*.dll"_s),
#elif defined(Q_OS_ANDROID)
                             QStringList(u"libplugins_iconengines_*.so"_s),
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
            if (!fileInfo.filePath().contains("iconengines"_L1))
                continue;

            QLibrary lib(fileInfo.canonicalFilePath());
            lib.load();
            reinterpret_cast<QLibraryStore*>(&lib)->invalidate();
        }
    }
}

ACAYIPWIDGETS_END_NAMESPACE
