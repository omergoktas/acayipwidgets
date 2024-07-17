// Copyright (C) 2024 Ömer Göktaş. All Rights Reserved.
// SPDX-License-Identifier: LicenseRef-AcayipWidgets-Commercial OR GPL-3.0-only

#include "pixelperfecticonengine.h"

#include <QFileInfo>
#include <QImageReader>
#include <QPainter>

PixelPerfectIconEngine::PixelPerfectIconEngine(const QString& filePath)
    : m_filePath(filePath)
{}

void PixelPerfectIconEngine::paint(QPainter* painter,
                                   const QRect& rect,
                                   QIcon::Mode mode,
                                   QIcon::State state)
{
    Q_UNUSED(mode)
    Q_UNUSED(state)
    painter->drawPixmap(rect, loadPixmap(rect.size()));
}

QPixmap PixelPerfectIconEngine::pixmap(const QSize& size,
                                       QIcon::Mode mode,
                                       QIcon::State state)
{
    Q_UNUSED(mode)
    Q_UNUSED(state)
    return loadPixmap(size);
}

QIconEngine* PixelPerfectIconEngine::clone() const
{
    return new PixelPerfectIconEngine(m_filePath);
}

QPixmap PixelPerfectIconEngine::loadPixmap(const QSize& size) const
{
    QFileInfo fileInfo(m_filePath);
    QString baseName = fileInfo.baseName();
    QString suffix = fileInfo.suffix();

    // Parse size from filename (e.g., "icon_16x16.png")
    QRegularExpression re("_(\\d+)x(\\d+)");
    QRegularExpressionMatch match = re.match(baseName);

    QSize pixmapSize = size;
    if (match.hasMatch()) {
        int width = match.captured(1).toInt();
        int height = match.captured(2).toInt();
        pixmapSize = QSize(width, height);
    }

    QImageReader reader(m_filePath);
    reader.setScaledSize(pixmapSize);
    return QPixmap::fromImage(reader.read());
}
