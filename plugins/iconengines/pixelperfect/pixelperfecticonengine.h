// Copyright (C) 2024 Ömer Göktaş. All Rights Reserved.
// SPDX-License-Identifier: LicenseRef-AcayipWidgets-Commercial OR GPL-3.0-only

#pragma once

#include <QIconEngine>
#include <QMap>

struct PixelPerfectIconEngineEntry
{
    QSize size;
    QString filePath;
};
Q_DECLARE_TYPEINFO(PixelPerfectIconEngineEntry, Q_RELOCATABLE_TYPE);

using PixelPerfectIconEngineEntryMap = QMap<int, PixelPerfectIconEngineEntry>;
using PixelPerfectIconEngineEntryMapIterator
    = QMapIterator<int, PixelPerfectIconEngineEntry>;

class PixelPerfectIconEngine final : public QIconEngine
{
public:
    PixelPerfectIconEngine(const QString& filePath);
    PixelPerfectIconEngine(const PixelPerfectIconEngine& other);

    QString iconName() override;
    QString iconName() const;
    QString key() const override;
    QIconEngine* clone() const override;
    bool isNull() const;
    bool isNull() override;
    QPixmap pixmap(const QSize& size, QIcon::Mode mode, QIcon::State state) override;
    QPixmap scaledPixmap(const QSize& size,
                         QIcon::Mode mode,
                         QIcon::State state,
                         qreal scale) override;
    void paint(QPainter* painter,
               const QRect& rect,
               QIcon::Mode mode,
               QIcon::State state) override;

private:
    void init(const QString& filePath);
    const PixelPerfectIconEngineEntryMap& activeEntries() const;
    QString cacheKeyFor(const QString& filePath,
                        const QSize& size,
                        qreal devicePixelRatio,
                        QIcon::Mode mode,
                        QIcon::State state) const;
    QPixmap bestMatch(const QSize& size,
                      qreal devicePixelRatio,
                      QIcon::Mode mode,
                      QIcon::State state) const;
    QRect greaterRect(const QSize& size, qreal devicePixelRatio) const;

private:
    PixelPerfectIconEngineEntryMap m_entries;
    PixelPerfectIconEngineEntryMap m_entriesDark;
};
