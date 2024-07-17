// Copyright (C) 2024 Ömer Göktaş. All Rights Reserved.
// SPDX-License-Identifier: LicenseRef-AcayipWidgets-Commercial OR GPL-3.0-only

#pragma once

#include <QtBodymovin/private/bmbase_p.h>

#include <QImageIOHandler>
#include <QVersionNumber>

class LottieIOHandler final : public QImageIOHandler
{
    Q_DISABLE_COPY(LottieIOHandler)

public:
    LottieIOHandler();

    bool canRead() const override;
    bool read(QImage* image) override;

    QVariant option(ImageOption option) const override;
    void setOption(ImageOption option, const QVariant& value) override;
    bool supportsOption(ImageOption option) const override;

    int imageCount() const override;
    int loopCount() const override;
    int nextImageDelay() const override;
    int currentImageNumber() const override;

    bool jumpToNextImage() override;
    bool jumpToImage(int imageNumber) override;

private:
    bool load() const;
    bool parse(const QByteArray& jsonSource) const;

    mutable int m_startFrame;
    mutable int m_endFrame;
    mutable int m_currentFrame;
    mutable int m_frameRate;
    mutable QSize m_size;
    mutable QVersionNumber m_version;
    mutable BMBase m_rootElement;
    QSize m_scaledSize;
};
