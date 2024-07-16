// Copyright (C) 2024 Ömer Göktaş. All Rights Reserved.
// SPDX-License-Identifier: LicenseRef-AcayipWidgets-Commercial OR GPL-3.0-only

#include "lottieplugin_p.h"
#include "lottiehandler_p.h"

using namespace Qt::Literals;

Q_IMPORT_PLUGIN(LottiePlugin)

ACAYIPWIDGETS_BEGIN_NAMESPACE

QImageIOPlugin::Capabilities LottiePlugin::capabilities(QIODevice* device,
                                                        const QByteArray& format) const
{
    if (format == "lottie"_ba || format == "json"_ba || (device && device->isReadable()))
        return Capabilities(CanRead);
    return {};
}

QImageIOHandler* LottiePlugin::create(QIODevice* device, const QByteArray& format) const
{
    auto handler = new LottieHandler;
    handler->setDevice(device);
    handler->setFormat(format);
    return handler;
}

ACAYIPWIDGETS_END_NAMESPACE
