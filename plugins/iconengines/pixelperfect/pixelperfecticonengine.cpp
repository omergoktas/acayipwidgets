// Copyright (C) 2024 Ömer Göktaş. All Rights Reserved.
// SPDX-License-Identifier: LicenseRef-AcayipWidgets-Commercial OR GPL-3.0-only

#include "pixelperfecticonengine.h"

#include <private/qhexstring_p.h>

#include <QDir>
#include <QFileInfo>
#include <QGuiApplication>
#include <QImageReader>
#include <QPainter>
#include <QPixmapCache>
#include <QStyleHints>

// TODO: Implement data stream functions
// TODO: Implement addPixmap function and all the rest functionality to
// simulate the original QPixmapIconEngine behavior as a fallback mechanism

using namespace Qt::Literals;

PixelPerfectIconEngine::PixelPerfectIconEngine(const QString& filePath)
    : QIconEngine()
{
    init(filePath);
}

PixelPerfectIconEngine::PixelPerfectIconEngine(const PixelPerfectIconEngine& other)
    : QIconEngine(other)
    , m_entries(other.m_entries)
    , m_entriesDark(other.m_entriesDark)
{}

QIconEngine* PixelPerfectIconEngine::clone() const
{
    return new PixelPerfectIconEngine(*this);
}

void PixelPerfectIconEngine::paint(QPainter* painter,
                                   const QRect& rect,
                                   QIcon::Mode mode,
                                   QIcon::State state)
{
    Q_UNUSED(mode)
    Q_UNUSED(state)
    const QPixmap& pm = bestMatch(rect.size(), painter->device()->devicePixelRatioF());
    QRectF pRect({0, 0}, QSizeF(pm.size()) / pm.devicePixelRatio());
    pRect.moveCenter(rect.center());
    painter->fillRect(rect, Qt::red);
    painter->drawPixmap(pRect.topLeft().toPoint(), pm);
}

void PixelPerfectIconEngine::init(const QString& filePath)
{
    if (filePath.isEmpty())
        return;

    QFileInfo info(filePath);
    if (!info.exists() || info.isDir()) {
        qWarning("Invalid icon path provided: %s", qUtf8Printable(filePath));
        return;
    }

    const QDir& dir = info.dir();
    const QStringList& fileNames
        = dir.entryList({u"%1*.%2"_s.arg(info.baseName(), info.completeSuffix())},
                        QDir::Files);

    // Collect light variants
    QRegularExpression rel(
        u"%1(\\.(\\d{3}))?\\.%2"_s.arg(info.baseName(), info.completeSuffix()));
    const QStringList& fileNamesLight = fileNames.filter(rel);
    for (const QString& fileName : fileNamesLight) {
        QImageReader reader(dir.absoluteFilePath(fileName));
        QRegularExpressionMatch match = rel.match(fileName);
        bool ok = false;
        int dpi = match.captured(2).toInt(&ok);
        m_entries.insert(ok ? dpi : 100, {reader.size(), reader.fileName()});
    }

    // Collect dark variants
    QRegularExpression red(
        u"%1\\.dark(\\.(\\d{3}))?\\.%2"_s.arg(info.baseName(), info.completeSuffix()));
    const QStringList& fileNamesDark = fileNames.filter(red);
    for (const QString& fileName : fileNamesDark) {
        QImageReader reader(dir.absoluteFilePath(fileName));
        QRegularExpressionMatch match = red.match(fileName);
        bool ok = false;
        int dpi = match.captured(2).toInt(&ok);
        m_entriesDark.insert(ok ? dpi : 100, {reader.size(), reader.fileName()});
    }
}

const PixelPerfectIconEngineEntryMap& PixelPerfectIconEngine::activeEntries() const
{
    return QGuiApplication::styleHints()->colorScheme() == Qt::ColorScheme::Dark
               ? (m_entriesDark.isEmpty() ? m_entries : m_entriesDark)
               : (m_entries.isEmpty() ? m_entriesDark : m_entries);
}

QString PixelPerfectIconEngine::cacheKeyFor(const QString& filePath,
                                            const QSize& size,
                                            qreal devicePixelRatio) const
{
    QFileInfo info(filePath);
    return key() % info.canonicalFilePath()
           % HexString<uint>(info.lastModified(QTimeZone::UTC).toSecsSinceEpoch())
           % HexString<quint64>(info.size()) % HexString<uint>(size.width())
           % HexString<uint>(size.height())
           % HexString<quint16>(qRound(devicePixelRatio * 1000));
    ;
}

QPixmap PixelPerfectIconEngine::bestMatch(const QSize& size,
                                          qreal devicePixelRatio) const
{
    QPixmap pm;
    const QSizeF& requestedSize = greaterRect(size, devicePixelRatio).size();
    const qreal rw = requestedSize.width();
    const qreal rh = requestedSize.height();

    // Try integer upscaling for a crisper look
    PixelPerfectIconEngineEntryMapIterator i(activeEntries());
    i.toBack();
    while (i.hasPrevious()) {
        i.previous();
        int iw = i.value().size.width();
        int ih = i.value().size.height();
        int scale = qMin(qFloor(rw / iw), qFloor(rh / ih));

        if (scale >= 1 && (rw - iw * scale < rw * 0.2 || rh - ih * scale < rh * 0.2)) {
            const QString& cacheKey = cacheKeyFor(i.value().filePath,
                                                  i.value().size,
                                                  scale);
            if (!QPixmapCache::find(cacheKey, &pm)) {
                QImageReader reader(i.value().filePath);
                if (scale > 1) {
                    if (reader.supportsOption(QImageIOHandler::ScaledSize)) {
                        reader.setScaledSize(scale * i.value().size);
                        pm = QPixmap::fromImage(reader.read());
                    } else {
                        QPixmap px(scale * i.value().size);
                        px.fill(Qt::transparent);
                        QPainter painter(&px);
                        painter.setRenderHints(QPainter::Antialiasing
                                               | QPainter::TextAntialiasing
                                               | QPainter::SmoothPixmapTransform
                                               | QPainter::LosslessImageRendering);
                        painter.scale(scale, scale);
                        painter.drawImage(QPoint{0, 0}, reader.read());
                        painter.end();
                        pm = px;
                    }
                } else {
                    pm = QPixmap::fromImage(reader.read());
                }
                pm.setDevicePixelRatio(devicePixelRatio);
                QPixmapCache::insert(cacheKey, pm);
            }
            return pm;
        }
    }

    // Oops, integer upscaling didn't work. Let's try the direct approach
    i.toBack();
    while (i.hasPrevious()) {
        i.previous();
        int iw = i.value().size.width();
        int ih = i.value().size.height();
        qreal scale = qMin(rw / iw, rh / ih);

        if (scale >= 1) {
            const QString& cacheKey = cacheKeyFor(i.value().filePath,
                                                  i.value().size,
                                                  scale);
            if (!QPixmapCache::find(cacheKey, &pm)) {
                QImageReader reader(i.value().filePath);
                if (scale > 1) {
                    if (reader.supportsOption(QImageIOHandler::ScaledSize)) {
                        reader.setScaledSize(greaterRect(i.value().size, scale).size());
                        pm = QPixmap::fromImage(reader.read());
                    } else {
                        QPixmap px(greaterRect(i.value().size, scale).size());
                        px.fill(Qt::transparent);
                        QPainter painter(&px);
                        painter.setRenderHints(QPainter::Antialiasing
                                               | QPainter::TextAntialiasing
                                               | QPainter::SmoothPixmapTransform
                                               | QPainter::LosslessImageRendering);
                        painter.scale(scale, scale);
                        painter.drawImage(QPoint{0, 0}, reader.read());
                        painter.end();
                        pm = px;
                    }
                } else {
                    pm = QPixmap::fromImage(reader.read());
                }
                pm.setDevicePixelRatio(devicePixelRatio);
                QPixmapCache::insert(cacheKey, pm);
            }
            return pm;
        }
    }

    return QPixmap();
}

QRect PixelPerfectIconEngine::greaterRect(const QSize& size,
                                          qreal devicePixelRatio) const
{
    return QRect(QPoint{0, 0},
                 QSize{qCeil(size.width() * devicePixelRatio),
                       qCeil(size.height() * devicePixelRatio)});
}

QPixmap PixelPerfectIconEngine::pixmap(const QSize& size,
                                       QIcon::Mode mode,
                                       QIcon::State state)
{
    return scaledPixmap(size, mode, state, 1.0);
}

QPixmap PixelPerfectIconEngine::scaledPixmap(const QSize& size,
                                             QIcon::Mode mode,
                                             QIcon::State state,
                                             qreal scale)
{
    if (isNull())
        return QPixmap();

    QPixmap pm(greaterRect(size, scale).size());
    pm.setDevicePixelRatio(scale);
    const QString& cacheKey = cacheKeyFor(activeEntries().first().filePath, size, scale);
    if (!QPixmapCache::find(cacheKey, &pm)) {
        pm.fill(Qt::transparent);
        QPainter painter(&pm);
        painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing
                               | QPainter::SmoothPixmapTransform
                               | QPainter::LosslessImageRendering);
        paint(&painter, QRect({0, 0}, size), mode, state);
        painter.end();
        QPixmapCache::insert(cacheKey, pm);
    }
    return pm;
}

QString PixelPerfectIconEngine::iconName()
{
    return const_cast<const PixelPerfectIconEngine*>(this)->iconName();
}

QString PixelPerfectIconEngine::iconName() const
{
    if (isNull())
        return QString();
    return activeEntries().first().filePath;
}

bool PixelPerfectIconEngine::isNull()
{
    return const_cast<const PixelPerfectIconEngine*>(this)->isNull();
}

bool PixelPerfectIconEngine::isNull() const
{
    return activeEntries().isEmpty();
}

QString PixelPerfectIconEngine::key() const
{
    return u"PixelPerfectIconEngine"_s;
}
