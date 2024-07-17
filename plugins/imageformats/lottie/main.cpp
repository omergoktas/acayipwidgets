// Copyright (C) 2024 Ömer Göktaş. All Rights Reserved.
// SPDX-License-Identifier: LicenseRef-AcayipWidgets-Commercial OR GPL-3.0-only

#include "lottieiohandler.h"

#include <QImageIOPlugin>

using namespace Qt::Literals;

class LottieIOPlugin final : public QImageIOPlugin
{
    Q_OBJECT
    Q_DISABLE_COPY(LottieIOPlugin)
    Q_PLUGIN_METADATA(IID QImageIOHandlerFactoryInterface_iid FILE "lottie.json")

public:
    LottieIOPlugin() = default;

    Capabilities capabilities(QIODevice* device,
                              const QByteArray& format) const override;
    QImageIOHandler* create(QIODevice* device,
                            const QByteArray& format = QByteArray()) const override;
};

QImageIOPlugin::Capabilities LottieIOPlugin::capabilities(QIODevice* device,
                                                          const QByteArray& format) const
{
    if (format == "lottie"_ba || format == "json"_ba || (device && device->isReadable()))
        return Capabilities(CanRead);
    return {};
}

QImageIOHandler* LottieIOPlugin::create(QIODevice* device,
                                        const QByteArray& format) const
{
    auto handler = new LottieIOHandler;
    handler->setDevice(device);
    handler->setFormat(format);
    return handler;
}

Q_IMPORT_PLUGIN(LottieIOPlugin)

#include "main.moc"
