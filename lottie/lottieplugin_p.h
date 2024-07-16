// Copyright (C) 2024 Ömer Göktaş. All Rights Reserved.
// SPDX-License-Identifier: LicenseRef-AcayipWidgets-Commercial OR GPL-3.0-only

#pragma once

#include <acayipglobal.h>

#include <QImageIOPlugin>

ACAYIPWIDGETS_BEGIN_NAMESPACE

class LottiePlugin : public QImageIOPlugin
{
    Q_OBJECT
    Q_DISABLE_COPY(LottiePlugin)
    Q_PLUGIN_METADATA(IID QImageIOHandlerFactoryInterface_iid FILE "lottie.json")

public:
    LottiePlugin() = default;

    Capabilities capabilities(QIODevice* device,
                              const QByteArray& format) const override;
    QImageIOHandler* create(QIODevice* device,
                            const QByteArray& format = QByteArray()) const override;
};

ACAYIPWIDGETS_END_NAMESPACE
