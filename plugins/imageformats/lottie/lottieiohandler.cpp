// Copyright (C) 2024 Ömer Göktaş. All Rights Reserved.
// SPDX-License-Identifier: LicenseRef-AcayipWidgets-Commercial OR GPL-3.0-only

#include "lottieiohandler.h"
#include "lottierasterrenderer.h"

#include <QtBodymovin/private/bmlayer_p.h>

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

using namespace Qt::Literals;

LottieIOHandler::LottieIOHandler()
    : QImageIOHandler()
    , m_startFrame(0)
    , m_endFrame(0)
    , m_currentFrame(0)
    , m_frameRate(30)
{}

bool LottieIOHandler::canRead() const
{
    if (!device())
        return false;
    return load();
}

bool LottieIOHandler::read(QImage* image)
{
    if (!canRead())
        return false;

    if (m_currentFrame > m_endFrame)
        return false;

    // Create a temporary image to render the frame
    QImage tempImage(m_scaledSize.isValid() ? m_scaledSize : m_size,
                     QImage::Format_ARGB32_Premultiplied);
    tempImage.fill(Qt::transparent);

    qreal sx = m_scaledSize.isValid() ? m_scaledSize.width() / qreal(m_size.width())
                                      : 1.0;
    qreal sy = m_scaledSize.isValid() ? m_scaledSize.height() / qreal(m_size.height())
                                      : 1.0;

    // Create a painter for the temporary image
    QPainter painter(&tempImage);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing
                           | QPainter::SmoothPixmapTransform
                           | QPainter::LosslessImageRendering);
    painter.scale(sx, sy);

    // Create a LottieRasterRenderer
    LottieRasterRenderer renderer(&painter);

    // Render the frame
    BMBase frame(m_rootElement);
    for (BMBase* elem : frame.children()) {
        if (elem->active(m_currentFrame)) {
            elem->updateProperties(m_currentFrame);
            elem->render(renderer);
        }
    }

    painter.end();

    // Copy the rendered frame to the output image
    *image = tempImage;

    m_currentFrame++;

    return true;
}

QVariant LottieIOHandler::option(ImageOption option) const
{
    switch (option) {
    case Size:
        canRead();
        return m_size;
    case ScaledSize:
        return m_scaledSize;
    case Animation:
        return true;
    case ImageFormat:
        return QImage::Format_ARGB32;
    default:
        return QVariant();
    }
}

void LottieIOHandler::setOption(ImageOption option, const QVariant& value)
{
    if (option == ScaledSize)
        m_scaledSize = value.toSize();
}

bool LottieIOHandler::supportsOption(ImageOption option) const
{
    return option == Size || option == ScaledSize || option == Animation
           || option == ImageFormat;
}

int LottieIOHandler::imageCount() const
{
    if (canRead())
        return m_endFrame - m_startFrame + 1;
    return 0;
}

int LottieIOHandler::loopCount() const
{
    if (canRead())
        return 1;
    return 0;
}

int LottieIOHandler::nextImageDelay() const
{
    if (canRead())
        return 1000.0 / m_frameRate;
    return 0;
}

int LottieIOHandler::currentImageNumber() const
{
    if (canRead())
        return m_currentFrame - m_startFrame;
    return -1;
}

bool LottieIOHandler::jumpToNextImage()
{
    if (canRead() && m_currentFrame < m_endFrame) {
        m_currentFrame++;
        return true;
    }
    return false;
}

bool LottieIOHandler::jumpToImage(int imageNumber)
{
    if (canRead()) {
        int frame = m_startFrame + imageNumber;
        if (frame >= m_startFrame && frame <= m_endFrame) {
            m_currentFrame = frame;
            return true;
        }
    }
    return false;
}

bool LottieIOHandler::load() const
{
    if (m_rootElement.children().size() > 0)
        return true;

    if (!device()->isOpen())
        device()->open(QIODevice::ReadOnly);

    if (!parse(device()->readAll()))
        return false;

    return true;
}

bool LottieIOHandler::parse(const QByteArray& jsonSource) const
{
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(jsonSource, &error);

    if (error.error != QJsonParseError::NoError) {
        qWarning() << "JSON parse error:" << error.errorString();
        return false;
    }

    QJsonObject rootObj = doc.object();

    if (rootObj.isEmpty())
        return false;

    m_version = QVersionNumber::fromString(rootObj.value("v"_L1).toString());
    m_startFrame = rootObj.value("ip"_L1).toInt();
    m_endFrame = rootObj.value("op"_L1).toInt();
    m_frameRate = rootObj.value("fr"_L1).toInt();
    m_size = QSize(rootObj.value("w"_L1).toInt(), rootObj.value("h"_L1).toInt());
    m_currentFrame = m_startFrame;

    QMap<QString, QJsonObject> assets;
    QJsonArray jsonLayers = rootObj.value("layers"_L1).toArray();
    QJsonArray jsonAssets = rootObj.value("assets"_L1).toArray();
    QJsonArray::const_iterator jsonAssetsIt = jsonAssets.constBegin();
    while (jsonAssetsIt != jsonAssets.constEnd()) {
        QJsonObject jsonAsset = (*jsonAssetsIt).toObject();
        QString id = jsonAsset.value("id"_L1).toString();
        assets.insert(id, jsonAsset);
        jsonAssetsIt++;
    }

    QJsonArray::const_iterator jsonLayerIt = jsonLayers.constEnd();
    while (jsonLayerIt != jsonLayers.constBegin()) {
        jsonLayerIt--;
        QJsonObject jsonLayer = (*jsonLayerIt).toObject();
        if (jsonLayer.value("ty"_L1).toInt() == 2) {
            QString refId = jsonLayer.value("refId"_L1).toString();
            jsonLayer.insert(u"asset"_s, assets.value(refId));
        }
        BMLayer* layer = BMLayer::construct(jsonLayer, m_version);
        if (layer) {
            layer->setParent(&m_rootElement);
            if (layer->isMaskLayer())
                m_rootElement.prependChild(layer);
            else
                m_rootElement.appendChild(layer);
        }
    }

    return true;
}
