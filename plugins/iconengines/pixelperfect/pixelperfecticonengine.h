// Copyright (C) 2024 Ömer Göktaş. All Rights Reserved.
// SPDX-License-Identifier: LicenseRef-AcayipWidgets-Commercial OR GPL-3.0-only

#pragma once

#include <QIconEngine>

class PixelPerfectIconEngine final : public QIconEngine
{
    Q_DISABLE_COPY(PixelPerfectIconEngine)

public:
    PixelPerfectIconEngine(const QString& filePath);

    void paint(QPainter* painter,
               const QRect& rect,
               QIcon::Mode mode,
               QIcon::State state) override;
    QPixmap pixmap(const QSize& size, QIcon::Mode mode, QIcon::State state) override;
    QIconEngine* clone() const override;

private:
    QString m_filePath;
    QPixmap loadPixmap(const QSize& size) const;
};
