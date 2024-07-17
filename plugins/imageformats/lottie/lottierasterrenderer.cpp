// Copyright (C) 2018 The Qt Company Ltd.
// Copyright (C) 2024 Ömer Göktaş.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#include "lottierasterrenderer.h"

#include <QBrush>
#include <QGradient>
#include <QPainter>
#include <QPointer>
#include <QRectF>
#include <QTransform>

#include <QtBodymovin/private/bmbasictransform_p.h>
#include <QtBodymovin/private/bmellipse_p.h>
#include <QtBodymovin/private/bmfill_p.h>
#include <QtBodymovin/private/bmfilleffect_p.h>
#include <QtBodymovin/private/bmfreeformshape_p.h>
#include <QtBodymovin/private/bmgfill_p.h>
#include <QtBodymovin/private/bmimage_p.h>
#include <QtBodymovin/private/bmrect_p.h>
#include <QtBodymovin/private/bmrepeater_p.h>
#include <QtBodymovin/private/bmround_p.h>
#include <QtBodymovin/private/bmshape_p.h>
#include <QtBodymovin/private/bmshapetransform_p.h>
#include <QtBodymovin/private/bmtrimpath_p.h>

LottieRasterRenderer::LottieRasterRenderer(QPainter* painter)
    : m_painter(painter)
{
    m_painter->setPen(QPen(Qt::NoPen));
}

void LottieRasterRenderer::saveState()
{
    m_painter->save();
    saveTrimmingState();
    m_pathStack.push_back(m_unitedPath);
    m_fillEffectStack.push_back(m_fillEffect);
    m_unitedPath = QPainterPath();
}

void LottieRasterRenderer::restoreState()
{
    m_painter->restore();
    restoreTrimmingState();
    m_unitedPath = m_pathStack.pop();
    m_fillEffect = m_fillEffectStack.pop();
}

void LottieRasterRenderer::render(const BMLayer& layer)
{
    if (layer.isMaskLayer())
        m_buildingClipRegion = true;
    else if (!m_clipPath.isEmpty()) {
        if (layer.clipMode() == BMLayer::Alpha)
            m_painter->setClipPath(m_clipPath);
        else if (layer.clipMode() == BMLayer::InvertedAlpha) {
            QPainterPath screen;
            screen.addRect(0,
                           0,
                           m_painter->device()->width(),
                           m_painter->device()->height());
            m_painter->setClipPath(screen - m_clipPath);
        } else {
            // Clipping is not applied to paths that have
            // not setting clipping parameters
            m_painter->setClipPath(QPainterPath());
        }
        m_buildingClipRegion = false;
        m_clipPath = QPainterPath();
    }
}

void LottieRasterRenderer::render(const BMRect& rect)
{
    m_painter->save();

    for (int i = 0; i < m_repeatCount; i++) {
        applyRepeaterTransform(i);
        if (trimmingState() == LottieRenderer::Individual) {
            QTransform t = m_painter->transform();
            QPainterPath tp = t.map(rect.path());
            tp.addPath(m_unitedPath);
            m_unitedPath = tp;
        } else if (m_buildingClipRegion) {
            QTransform t = m_painter->transform();
            QPainterPath tp = t.map(rect.path());
            tp.addPath(m_clipPath);
            m_clipPath = tp;
        } else
            m_painter->drawPath(rect.path());
    }

    m_painter->restore();
}

void LottieRasterRenderer::render(const BMEllipse& ellipse)
{
    m_painter->save();

    for (int i = 0; i < m_repeatCount; i++) {
        applyRepeaterTransform(i);
        if (trimmingState() == LottieRenderer::Individual) {
            QTransform t = m_painter->transform();
            QPainterPath tp = t.map(ellipse.path());
            tp.addPath(m_unitedPath);
            m_unitedPath = tp;
        } else if (m_buildingClipRegion) {
            QTransform t = m_painter->transform();
            QPainterPath tp = t.map(ellipse.path());
            tp.addPath(m_clipPath);
            m_clipPath = tp;
        } else
            m_painter->drawPath(ellipse.path());
    }

    m_painter->restore();
}

void LottieRasterRenderer::render(const BMImage& image)
{
    m_painter->save();

    for (int i = 0; i < m_repeatCount; i++) {
        applyRepeaterTransform(i);
        QPointF center = image.getCenter();
        m_painter->drawImage(center.x(), center.y(), image.getImage());
    }

    m_painter->restore();
}

void LottieRasterRenderer::render(const BMRound& round)
{
    m_painter->save();

    for (int i = 0; i < m_repeatCount; i++) {
        if (trimmingState() == LottieRenderer::Individual) {
            QTransform t = m_painter->transform();
            QPainterPath tp = t.map(round.path());
            tp.addPath(m_unitedPath);
            m_unitedPath = tp;
        } else if (m_buildingClipRegion) {
            QTransform t = m_painter->transform();
            QPainterPath tp = t.map(round.path());
            tp.addPath(m_clipPath);
            m_clipPath = tp;
        } else
            m_painter->drawPath(round.path());
    }

    m_painter->restore();
}

void LottieRasterRenderer::render(const BMFill& fill)
{
    if (m_fillEffect)
        return;

    float alpha = fill.color().alphaF() * fill.opacity() / 100.0f;
    QColor color = fill.color();
    color.setAlphaF(alpha);
    m_painter->setBrush(color);
}

void LottieRasterRenderer::render(const BMGFill& gradient)
{
    if (m_fillEffect)
        return;

    if (gradient.value())
        m_painter->setBrush(*gradient.value());
    else
        qWarning() << "Gradient:" << gradient.name() << "Cannot draw gradient fill";
}

void LottieRasterRenderer::render(const BMStroke& stroke)
{
    if (m_fillEffect)
        return;

    m_painter->setPen(stroke.pen());
}

void applyBMTransform(QTransform* xf,
                      const BMBasicTransform& bmxf,
                      bool isBMShapeTransform = false)
{
    QPointF pos = bmxf.position();
    qreal rot = bmxf.rotation();
    QPointF sca = bmxf.scale();
    QPointF anc = bmxf.anchorPoint();

    xf->translate(pos.x(), pos.y());

    if (!qFuzzyIsNull(rot))
        xf->rotate(rot);

    if (isBMShapeTransform) {
        const BMShapeTransform& shxf = static_cast<const BMShapeTransform&>(bmxf);
        if (!qFuzzyIsNull(shxf.skew())) {
            QTransform t(shxf.shearX(),
                         shxf.shearY(),
                         0,
                         -shxf.shearY(),
                         shxf.shearX(),
                         0,
                         0,
                         0,
                         1);
            t *= QTransform(1, 0, 0, shxf.shearAngle(), 1, 0, 0, 0, 1);
            t *= QTransform(shxf.shearX(),
                            -shxf.shearY(),
                            0,
                            shxf.shearY(),
                            shxf.shearX(),
                            0,
                            0,
                            0,
                            1);
            *xf = t * (*xf);
        }
    }

    xf->scale(sca.x(), sca.y());
    xf->translate(-anc.x(), -anc.y());
}

void LottieRasterRenderer::render(const BMBasicTransform& transform)
{
    QTransform t = m_painter->transform();
    applyBMTransform(&t, transform);
    m_painter->setTransform(t);
    m_painter->setOpacity(m_painter->opacity() * transform.opacity());
}

void LottieRasterRenderer::render(const BMShapeTransform& transform)
{
    QTransform t = m_painter->transform();
    applyBMTransform(&t, transform, true);
    m_painter->setTransform(t);
    m_painter->setOpacity(m_painter->opacity() * transform.opacity());
}

void LottieRasterRenderer::render(const BMFreeFormShape& shape)
{
    m_painter->save();

    for (int i = 0; i < m_repeatCount; i++) {
        applyRepeaterTransform(i);
        if (trimmingState() == LottieRenderer::Individual) {
            QTransform t = m_painter->transform();
            QPainterPath tp = t.map(shape.path());
            tp.addPath(m_unitedPath);
            m_unitedPath = tp;
        } else if (m_buildingClipRegion) {
            QTransform t = m_painter->transform();
            QPainterPath tp = t.map(shape.path());
            tp.addPath(m_clipPath);
            m_clipPath = tp;
        } else
            m_painter->drawPath(shape.path());
    }

    m_painter->restore();
}

void LottieRasterRenderer::render(const BMTrimPath& trimPath)
{
    // TODO: Remove "Individual" trimming to the prerendering thread
    // Now it is done in the GUI thread

    m_painter->save();

    for (int i = 0; i < m_repeatCount; i++) {
        applyRepeaterTransform(i);
        if (!trimPath.simultaneous() && !qFuzzyCompare(0.0, m_unitedPath.length())) {
            QPainterPath tr = trimPath.trim(m_unitedPath);
            // Do not use the applied transform, as the transform
            // is already included in m_unitedPath
            m_painter->setTransform(QTransform());
            m_painter->drawPath(tr);
        }
    }

    m_painter->restore();
}

void LottieRasterRenderer::render(const BMFillEffect& effect)
{
    m_fillEffect = &effect;
    m_painter->setBrush(m_fillEffect->color());
    m_painter->setOpacity(m_painter->opacity() * m_fillEffect->opacity());
}

void LottieRasterRenderer::render(const BMRepeater& repeater)
{
    if (m_repeaterTransform) {
        qWarning() << "Only one Repeater can be active at a time!";
        return;
    }

    m_repeatCount = repeater.copies();
    m_repeatOffset = repeater.offset();

    // Can store pointer to transform, although the transform
    // is managed by another thread. The object will be available
    // until the frame has been rendered
    m_repeaterTransform = &repeater.transform();

    m_painter->translate(m_repeatOffset * m_repeaterTransform->position().x(),
                         m_repeatOffset * m_repeaterTransform->position().y());
}

void LottieRasterRenderer::applyRepeaterTransform(int instance)
{
    if (!m_repeaterTransform || instance == 0)
        return;

    QTransform t = m_painter->transform();

    QPointF anchors = -m_repeaterTransform->anchorPoint();
    QPointF position = m_repeaterTransform->position();
    QPointF anchoredCenter = anchors + position;

    t.translate(anchoredCenter.x(), anchoredCenter.y());
    t.rotate(m_repeaterTransform->rotation());
    t.scale(m_repeaterTransform->scale().x(), m_repeaterTransform->scale().y());
    m_painter->setTransform(t);

    qreal o = m_repeaterTransform->opacityAtInstance(instance);

    m_painter->setOpacity(m_painter->opacity() * o);
}
