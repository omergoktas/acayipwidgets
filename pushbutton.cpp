// Copyright (C) 2024 Ömer Göktaş. All Rights Reserved.
// SPDX-License-Identifier: LicenseRef-AcayipWidgets-Commercial OR GPL-3.0-only

#include "pushbutton_p.h"
#include "utils_p.h"

#include <QAbstractTextDocumentLayout>
#include <QGraphicsDropShadowEffect>
#include <QLayout>
#include <QStyleHints>
#include <QTimer>

using namespace Qt::Literals;

/*
 * TODO: Remove QAbstractButtonPrivate implementation down below once
 * QPushButtonPrivate moves its constructor function over to .cpp file
*/
QAbstractButtonPrivate::QAbstractButtonPrivate(QSizePolicy::ControlType type)
    : shortcutId(0)
    , checkable(false)
    , checked(false)
    , autoRepeat(false)
    , autoExclusive(false)
    , down(false)
    , blockRefresh(false)
    , pressed(false)
    , group(nullptr)
    , autoRepeatDelay(300)
    , autoRepeatInterval(100)
    , controlType(type)
{}

ACAYIPWIDGETS_BEGIN_NAMESPACE

/*!
    \property QPushButton::flat
    \brief this property is private and not used anymore, use setStyles() instead.
*/

/*!
    \property QPushButton::autoDefault
    \brief this property is private and not used anymore, use setDefault() instead.
*/

/*!
 *  \internal
*/
PushButtonPrivate::PushButtonPrivate()
    : QPushButtonPrivate()
    , mouseAttached(false)
    , hoverShadowEnabled(true)
    , elevated(false)
    , opacity(1.0)
    , spacing(StyleDefaults::spacing)
    , margins(StyleDefaults::margins,
              StyleDefaults::margins,
              StyleDefaults::margins,
              StyleDefaults::margins)
    , paddings(StyleDefaults::paddings,
               StyleDefaults::paddings,
               StyleDefaults::paddings,
               StyleDefaults::paddings)
    , iconEdge(Qt::LeftEdge)
    , textFormat(Qt::AutoText)
{
    autoDefault = false;
    QTextOption top;
    top.setAlignment(Qt::AlignLeft | Qt::AlignTop);
    top.setWrapMode(QTextOption::WordWrap);
    textDocument.setDefaultTextOption(top);
    textDocument.setDocumentMargin(0);
}

void PushButtonPrivate::init()
{
    Q_Q(PushButton);

    q->setMouseTracking(true);
    q->setAttribute(Qt::WA_Hover);
    q->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    q->setCursor(Qt::PointingHandCursor);
    q->setStyles(StyleDefaults::buttonStyles);

    rippleAnimations.append(new QVariantAnimation(q));
    rippleAnimations.append(new QVariantAnimation(q));
    rippleAnimations.append(new QVariantAnimation(q));
    for (QVariantAnimation* anim : std::as_const(rippleAnimations)) {
        // We ripple half the time and fade half the time
        anim->setDuration(StyleDefaults::animationDuration * 2);
        anim->setEasingCurve(StyleDefaults::outEasingType);
        QObject::connect(anim, &QVariantAnimation::valueChanged, q, [q] {
            q->update();
        });
    }

    shadowEffect = new QGraphicsDropShadowEffect(q);
    shadowEffect->setColor(QColor(0, 0, 0, 80));
    shadowEffect->setBlurRadius(8);
    shadowEffect->setOffset(0, 0);
    q->setGraphicsEffect(shadowEffect);

    shadowAnimation.setTargetObject(shadowEffect);
    shadowAnimation.setDuration(StyleDefaults::animationDuration);
    shadowAnimation.setPropertyName("yOffset"_ba);
    shadowAnimation.setEasingCurve(StyleDefaults::outEasingType);

    showHideAnimation.setDuration(StyleDefaults::animationDuration);
    showHideAnimation.setEasingCurve(StyleDefaults::outEasingType);

    QObject::connect(q, &PushButton::clicked, q, [q] {
        QTimer::singleShot(StyleDefaults::animationDuration,
                           q,
                           &PushButton::animatedClicked);
    });
}

/*!
 *  \internal
*/
void PushButtonPrivate::mergeStyleWithRest(ButtonStyle& target,
                                           const ButtonStyle& source) const
{
    target.borderRadius = source.borderRadius != -1 ? source.borderRadius
                                                    : styles.rest.borderRadius;
    target.borderPen = source.borderPen.style() != Qt::NoPen ? source.borderPen
                                                             : styles.rest.borderPen;
    target.borderPenDark = source.borderPenDark.style() != Qt::NoPen
                               ? source.borderPenDark
                               : styles.rest.borderPenDark;
    target.textColor = source.textColor.isValid() ? source.textColor
                                                  : styles.rest.textColor;
    target.textColorDark = source.textColorDark.isValid() ? source.textColorDark
                                                          : styles.rest.textColorDark;
    target.iconColor = source.iconColor.isValid() ? source.iconColor
                                                  : styles.rest.iconColor;
    target.iconColorDark = source.iconColorDark.isValid() ? source.iconColorDark
                                                          : styles.rest.iconColorDark;
    target.backgroundBrush = source.backgroundBrush.style() != Qt::NoBrush
                                 ? source.backgroundBrush
                                 : styles.rest.backgroundBrush;
    target.backgroundBrushDark = source.backgroundBrushDark.style() != Qt::NoBrush
                                     ? source.backgroundBrushDark
                                     : styles.rest.backgroundBrushDark;
}

void PushButtonPrivate::updateTextDocumentContent()
{
    Q_Q(const PushButton);

    auto wrapInHtml = [](const QString& str) {
        return u"<p style='text-align:center'>"_s + str + u"</p>"_s;
    };

    textDocument.setDefaultFont(q->font());

    if (textFormat == Qt::RichText)
        textDocument.setHtml(q->text());
    else if (textFormat == Qt::PlainText)
        textDocument.setHtml(wrapInHtml(q->text()));
    else if (textFormat == Qt::MarkdownText)
        textDocument.setMarkdown(q->text());
    else if (Qt::mightBeRichText(q->text()))
        textDocument.setHtml(q->text());
    else
        textDocument.setHtml(wrapInHtml(q->text()));

    textDocument.setTextWidth(-1);

    textDocumentSizeHint = textDocument.size();

    textDocument.setTextWidth(itemRect(Text).width());
}

QRectF PushButtonPrivate::itemRect(Item item) const
{
    Q_Q(const PushButton);

    QRectF rect = QRectF(q->rect()) - margins;

    if (item == Background)
        return rect;

    rect -= paddings;

    if (item == Icon) {
        if (q->icon().isNull())
            return QRectF();

        switch (iconEdge) {
        case Qt::RightEdge:
            return QRectF({rect.right() - q->iconSize().width(),
                           rect.top() + rect.height() / 2.0
                               - q->iconSize().height() / 2.0},
                          q->iconSize());
        case Qt::LeftEdge:
            return QRectF({rect.left(),
                           rect.top() + rect.height() / 2.0
                               - q->iconSize().height() / 2.0},
                          q->iconSize());
        case Qt::TopEdge:
            return QRectF({rect.left() + rect.width() / 2.0
                               - q->iconSize().width() / 2.0,
                           rect.top()},
                          q->iconSize());
        case Qt::BottomEdge:
            return QRectF(
                {
                    rect.top() + rect.width() / 2.0 - q->iconSize().width() / 2.0,
                    rect.bottom() - q->iconSize().height(),
                },
                q->iconSize());
        default:
            return QRectF();
        }
    }

    if (q->icon().isNull())
        return rect;

    switch (iconEdge) {
    case Qt::RightEdge:
        return rect.adjusted(0, 0, -spacing - q->iconSize().width(), 0);
    case Qt::LeftEdge:
        return rect.adjusted(spacing + q->iconSize().width(), 0, 0, 0);
    case Qt::TopEdge:
        return rect.adjusted(0, spacing + q->iconSize().height(), 0, 0);
    case Qt::BottomEdge:
        return rect.adjusted(0, 0, 0, -spacing - q->iconSize().height());
    default:
        return QRectF();
    }
}

qreal PushButtonPrivate::calculateRadius(int value) const
{
    const QRectF& b = itemRect(PushButtonPrivate::Background);
    return value > StyleDefaults::borderRadiusIsPercentage
               ? (qMin(b.width(), b.height()) / 2.0)
                     * (value % StyleDefaults::borderRadiusIsPercentage) / 100.0
               : value;
}

void PushButtonPrivate::startRippleAnimation(const QPoint& pos)
{
    for (QVariantAnimation* anim : std::as_const(rippleAnimations)) {
        if (anim->state() == QVariantAnimation::Running)
            continue;
        QRectF bgRect = itemRect(PushButtonPrivate::Background);
        qreal hypotenuse = std::sqrt(bgRect.width() * bgRect.width()
                                     + bgRect.height() * bgRect.height());
        qreal scale = 2 * StyleDefaults::animationDuration / 100.0;
        QRectF rect;
        rect.moveCenter(pos);
        anim->setStartValue(rect);
        rect.setSize({scale * hypotenuse, scale * hypotenuse});
        rect.moveCenter(pos);
        anim->setEndValue(rect);
        anim->start();
        break;
    }
}

QPainterPath PushButtonPrivate::backgroundPath(const QMarginsF& m) const
{
    qreal radius = calculateRadius(activeStyle().borderRadius);
    QPainterPath path;
    path.addRoundedRect(itemRect(PushButtonPrivate::Background)
                            .adjusted(m.left(), m.top(), -m.right(), -m.bottom()),
                        radius,
                        radius);
    return path;
}

const ButtonStyle& PushButtonPrivate::activeStyle() const
{
    Q_Q(const PushButton);
    if (!q->isEnabled())
        return styles.disabled;
    if (pressed)
        return styles.pressed;
    if (hovering)
        return styles.hovered;
    if (checkable && checked)
        return styles.checked;
    return styles.rest;
}

bool PushButtonPrivate::isRippling() const
{
    for (QVariantAnimation* anim : std::as_const(rippleAnimations)) {
        if (anim->state() == QVariantAnimation::Running)
            return true;
    }
    return false;
}

qreal PushButtonPrivate::shortestActiveRippleAnimationTime() const
{
    qreal t = 1.0;
    for (QVariantAnimation* anim : std::as_const(rippleAnimations)) {
        if (anim->state() == QVariantAnimation::Running) {
            qreal mT = anim->currentLoopTime() / qreal(anim->duration());
            if (mT < t)
                t = mT;
        }
    }
    return t;
}

/*!
    Constructs a push button with no text and a \a parent.
*/
PushButton::PushButton(QWidget* parent)
    : PushButton(QIcon(), QString(), parent)
{}

/*!
    Constructs a push button with the parent \a parent and the text \a
    text.
*/
PushButton::PushButton(const QString& text, QWidget* parent)
    : PushButton(QIcon(), text, parent)
{}

/*!
    Constructs a push button with an \a icon and a \a text, and a \a parent.

    Note that you can also pass a QPixmap object as an icon (thanks to
    the implicit type conversion provided by C++).
*/
PushButton::PushButton(const QIcon& icon, const QString& text, QWidget* parent)
    : PushButton(*new PushButtonPrivate, parent)
{
    setText(text);
    setIcon(icon);
    setText("hello <b>world");
}

/*!
 *  \internal
*/
PushButton::PushButton(PushButtonPrivate& dd, QWidget* parent)
    : QPushButton(dd, parent)
{
    Q_D(PushButton);
    d->init();
}

qreal PushButton::opacity() const
{
    Q_D(const PushButton);
    return d->opacity;
}

void PushButton::setOpacity(qreal opacity)
{
    Q_D(PushButton);
    d->opacity = opacity;
    update();
}

qreal PushButton::spacing() const
{
    Q_D(const PushButton);
    return d->spacing;
}

void PushButton::setSpacing(qreal spacing)
{
    Q_D(PushButton);
    d->spacing = spacing;
    updateGeometry();
    update();
}

const QMarginsF& PushButton::margins() const
{
    Q_D(const PushButton);
    return d->margins;
}

void PushButton::setMargins(const QMarginsF& margins)
{
    Q_D(PushButton);
    d->margins = margins;
    updateGeometry();
    update();
}

const QMarginsF& PushButton::paddings() const
{
    Q_D(const PushButton);
    return d->paddings;
}

void PushButton::setPaddings(const QMarginsF& paddings)
{
    Q_D(PushButton);
    d->paddings = paddings;
    updateGeometry();
    update();
}

Qt::Edge PushButton::iconEdge() const
{
    Q_D(const PushButton);
    return d->iconEdge;
}

void PushButton::setIconEdge(Qt::Edge iconEdge)
{
    Q_D(PushButton);
    d->iconEdge = iconEdge;
    update();
}

Qt::TextFormat PushButton::textFormat() const
{
    Q_D(const PushButton);
    return d->textFormat;
}

void PushButton::setTextFormat(Qt::TextFormat textFormat)
{
    Q_D(PushButton);

    if (d->textFormat == textFormat)
        return;

    d->textFormat = textFormat;

    QString copy = text();
    if (!copy.isNull()) {
        QPushButton::setText({});
        setText(copy);
    }
}

const QBrush& PushButton::rippleBrush(bool dark) const
{
    Q_D(const PushButton);
    return dark ? d->rippleBrushDark : d->rippleBrush;
}

void PushButton::setRippleBrush(const QBrush& brush, const QBrush& brushDark)
{
    Q_D(PushButton);
    d->rippleBrush = brush;
    d->rippleBrushDark = brushDark;
    update();
}

const ButtonStyles& PushButton::styles() const
{
    Q_D(const PushButton);
    return d->styles;
}

void PushButton::setStyles(const ButtonStyles& styles)
{
    Q_D(PushButton);

    d->styles.rest = styles.rest;

    d->mergeStyleWithRest(d->styles.hovered, styles.hovered);
    d->mergeStyleWithRest(d->styles.pressed, styles.pressed);
    d->mergeStyleWithRest(d->styles.checked, styles.checked);
    d->mergeStyleWithRest(d->styles.disabled, styles.disabled);

    update();
}

void PushButton::setElevated(bool elevated)
{
    Q_D(PushButton);
    d->elevated = elevated;
    if (d->shadowEffect) {
        d->shadowEffect->setBlurRadius(elevated ? 40 : 8);
        d->shadowEffect->setColor(QColor(0, 0, 0, elevated ? 160 : 80));
    }
}

bool PushButton::isElevated() const
{
    Q_D(const PushButton);
    return d->elevated;
}

void PushButton::setHoverShadowEnabled(bool hoverShadowEnabled)
{
    Q_D(PushButton);
    d->hoverShadowEnabled = hoverShadowEnabled;
    update();
}

bool PushButton::isHoverShadowEnabled() const
{
    Q_D(const PushButton);
    return d->elevated;
}

void PushButton::setText(const QString& text)
{
    Q_D(PushButton);
    if (text == this->text())
        return;

    QPushButton::setText(text);
    d->updateTextDocumentContent();
    updateGeometry();
    update();
}

QSize PushButton::sizeHint() const
{
    return minimumSizeHint();
}

QSize PushButton::minimumSizeHint() const
{
    Q_D(const PushButton);

    qreal width = d->margins.left() + d->margins.right() + d->paddings.left()
                  + d->paddings.right() + d->textDocumentSizeHint.width();
    qreal height = d->margins.top() + d->margins.bottom() + d->paddings.top()
                   + d->paddings.bottom() + d->textDocumentSizeHint.height();

    if (!icon().isNull()) {
        if (d->iconEdge == Qt::RightEdge || d->iconEdge == Qt::LeftEdge)
            width += d->spacing + iconSize().width();
        else
            height += d->spacing + iconSize().height();
    }

    return QSize(qCeil(width), qCeil(height));
}

void PushButton::hideAnimated()
{
    Q_D(PushButton);
    if (d->showHideAnimation.state() != QAbstractAnimation::Stopped)
        d->showHideAnimation.stop();
    d->showHideAnimation.setStartValue(1.0);
    d->showHideAnimation.setEndValue(0.0);
    d->showHideAnimation.start();
    d->showHideAnimation.disconnect(this);
    connect(&d->showHideAnimation,
            &QVariantAnimation::finished,
            this,
            &PushButton::hide);
    if (parentWidget()) {
        connect(&d->showHideAnimation,
                SIGNAL(finished()),
                parentWidget()->layout(),
                SLOT(animate()));
    }
}

void PushButton::showAnimated()
{
    Q_D(PushButton);
    if (d->showHideAnimation.state() != QAbstractAnimation::Stopped)
        d->showHideAnimation.stop();
    d->showHideAnimation.setStartValue(0.0);
    d->showHideAnimation.setEndValue(1.0);
    d->showHideAnimation.start();
    d->showHideAnimation.disconnect(this);
    connect(&d->showHideAnimation,
            &QVariantAnimation::finished,
            this,
            &PushButton::show);
    if (parentWidget()) {
        connect(&d->showHideAnimation,
                SIGNAL(finished()),
                parentWidget()->layout(),
                SLOT(animate()));
    }
}

bool PushButton::hitButton(const QPoint& pos) const
{
    Q_D(const PushButton);
    return d->backgroundPath().contains(pos);
}

bool PushButton::event(QEvent* event)
{
    Q_D(PushButton);

    if (event->type() == QEvent::HoverEnter) {
        d->hovering = false;
        d->cursor = cursor();
        setCursor(Qt::ArrowCursor);
        update();
    } else if (event->type() == QEvent::HoverLeave) {
        d->hovering = false;
        setCursor(d->cursor);
        update();
    } else if (event->type() == QEvent::HoverMove) {
        auto hover = static_cast<QHoverEvent*>(event);
        if (hitButton(hover->position().toPoint())) {
            if (d->mouseAttached)
                d->hovering = true;
            setCursor(d->cursor);
        } else {
            d->hovering = false;
            setCursor(Qt::ArrowCursor);
        }
        update();
    }

    return QPushButton::event(event);
}

void PushButton::changeEvent(QEvent* event)
{
    Q_D(PushButton);

    if (event->type() == QEvent::FontChange
        || event->type() == QEvent::ApplicationFontChange) {
        d->updateTextDocumentContent();
        updateGeometry();
        update();
    }

    QPushButton::changeEvent(event);
}

void PushButton::mousePressEvent(QMouseEvent* event)
{
    Q_D(PushButton);
    if (hitButton(event->pos()))
        d->startRippleAnimation(event->pos());
    QPushButton::mousePressEvent(event);
}

void PushButton::mouseMoveEvent(QMouseEvent* event)
{
    Q_D(PushButton);
    d->mouseAttached = event->source() == Qt::MouseEventNotSynthesized;
    QAbstractButton::mouseMoveEvent(event); // QPushButton breaks the hover logic
}

void PushButton::resizeEvent(QResizeEvent* event)
{
    Q_D(PushButton);
    if (event->size().width() != event->oldSize().width())
        d->textDocument.setTextWidth(d->itemRect(PushButtonPrivate::Text).width());
    QPushButton::resizeEvent(event);
}

void PushButton::paintEvent(QPaintEvent*)
{
    Q_D(PushButton);

    const bool darkStyle = QGuiApplication::styleHints()->colorScheme()
                           == Qt::ColorScheme::Dark;
    const ButtonStyle& style = d->activeStyle();
    const QRectF& bgRect = d->itemRect(PushButtonPrivate::Background);
    const QRectF& txtRect = d->itemRect(PushButtonPrivate::Text);
    const QRectF& icoRect = d->itemRect(PushButtonPrivate::Icon);
    const QPen& bPen = darkStyle ? style.borderPenDark : style.borderPen;
    const QBrush& bBrush = darkStyle ? style.backgroundBrushDark
                                     : style.backgroundBrush;
    const qreal bRadius = d->calculateRadius(style.borderRadius);
    const qreal a = bPen.style() != Qt::NoPen ? bPen.widthF() / 2.0 : 0;
    const qreal t = d->shortestActiveRippleAnimationTime();
    const qreal o = d->opacity
                    * (d->showHideAnimation.state() == QAbstractAnimation::Running
                           ? d->showHideAnimation.currentValue().toReal()
                           : 1.0);

    QPainter painter(this);
    painter.setRenderHints(StyleDefaults::renderHints);

    // Paint the background
    painter.setPen(bPen);
    painter.setBrush(bBrush);
    painter.setOpacity(o * t);
    painter.setClipPath(d->backgroundPath());
    painter.drawRoundedRect(bgRect.adjusted(a, a, -a, -a), bRadius, bRadius);

    // Paint the ripple effect
    if (d->isRippling()) {
        const QPen& rPen = d->mouseAttached
                               ? (darkStyle ? d->styles.hovered.borderPenDark
                                            : d->styles.hovered.borderPen)
                               : (darkStyle ? d->styles.rest.borderPenDark
                                            : d->styles.rest.borderPen);
        const QBrush& rBrush = d->mouseAttached
                                   ? (darkStyle ? d->styles.hovered.backgroundBrushDark
                                                : d->styles.hovered.backgroundBrush)
                                   : (darkStyle ? d->styles.rest.backgroundBrushDark
                                                : d->styles.rest.backgroundBrush);

        // Paint the ripple background
        painter.setPen(rPen);
        painter.setBrush(rBrush);
        painter.setOpacity(o * (1.0 - t));
        painter.drawRoundedRect(bgRect.adjusted(a, a, -a, -a), bRadius, bRadius);

        // Paint the ripple wave
        for (QVariantAnimation* anim : std::as_const(d->rippleAnimations)) {
            if (anim->state() == QVariantAnimation::Running) {
                const qreal rT = anim->currentLoopTime() / qreal(anim->duration());
                const QRectF& ripRect = anim->currentValue().toRectF();
                const QBrush& suppliedBrush = darkStyle ? d->rippleBrushDark
                                                        : d->rippleBrush;
                const auto suppliedGrad = suppliedBrush.style()
                                                  == Qt::RadialGradientPattern
                                              ? static_cast<const QRadialGradient*>(
                                                  suppliedBrush.gradient())
                                              : nullptr;
                QRadialGradient gradient(ripRect.center(),
                                         ripRect.width(),
                                         ripRect.center());
                if (suppliedGrad) {
                    gradient.setStops(suppliedGrad->stops());
                    painter.setBrush(gradient);
                } else if (suppliedBrush.style() == Qt::SolidPattern) {
                    gradient.setColorAt(0.0, suppliedBrush.color());
                    gradient.setColorAt(0.5 + qMax(rT / 2.0 - 0.25, 0.0),
                                        Qt::transparent);
                    painter.setBrush(gradient);
                } else if (suppliedBrush.style() == Qt::NoBrush) {
                    if (rBrush.style() == Qt::SolidPattern) {
                        gradient.setColorAt(0.0, strongerColor(rBrush.color()));
                    } else {
                        gradient.setColorAt(
                            0.0,
                            darkStyle ? d->styles.pressed.backgroundBrushDark.color()
                                      : d->styles.pressed.backgroundBrush.color());
                    }
                    gradient.setColorAt(0.5 + qMax(rT / 2.0 - 0.25, 0.0),
                                        Qt::transparent);
                    painter.setBrush(gradient);
                } else {
                    painter.setBrush(suppliedBrush);
                }
                painter.setPen(Qt::NoPen);
                painter.setOpacity(
                    o * (1.0 - 1.0 / (1.0 + std::exp(-12.0 * (rT - 0.65)))));
                painter.setClipPath(d->backgroundPath(
                    rPen.style() != Qt::NoPen ? QMarginsF{2 * a, 2 * a, 2 * a, 2 * a}
                                              : QMarginsF{}));
                painter.drawEllipse(ripRect);
            }
        }
    }

    if (d->hoverShadowEnabled && d->shadowEffect && d->mouseAttached) {
        if (d->hovering) {
            if (d->isRippling()) {
                d->shadowEffect->setYOffset(3 * t);
            } else if (d->shadowEffect->yOffset() != 3) {
                if (d->shadowAnimation.state() != QAbstractAnimation::Stopped)
                    d->shadowAnimation.stop();
                d->shadowAnimation.setStartValue(d->shadowEffect->yOffset());
                d->shadowAnimation.setEndValue(3);
                d->shadowAnimation.start();
            }
        } else if (d->shadowEffect->yOffset() != 0) {
            if (d->shadowAnimation.state() != QAbstractAnimation::Stopped)
                d->shadowAnimation.stop();
            d->shadowAnimation.setStartValue(d->shadowEffect->yOffset());
            d->shadowAnimation.setEndValue(0);
            d->shadowAnimation.start();
        }
    }

    // Paint the icon
    if (!icon().isNull()) {
        painter.setPen(darkStyle ? style.iconColorDark : style.iconColor);
        painter.setBrush(Qt::NoBrush);
        painter.setOpacity(o);
        painter.setClipRect(icoRect);
        painter.drawPixmap(d->itemRect(PushButtonPrivate::Icon),
                           d->icon.pixmap(iconSize(), devicePixelRatioF()),
                           QRectF({}, iconSize() * devicePixelRatioF()));
    }

    // Paint the text
    if (!text().isEmpty()) {
        painter.setPen(darkStyle ? style.textColorDark : style.textColor);
        painter.setBrush(Qt::NoBrush);
        painter.setOpacity(o);
        painter.setClipRect(txtRect);

        QAbstractTextDocumentLayout::PaintContext context;
        context.palette.setColor(QPalette::Text, painter.pen().color());
        context.clip = QRectF({}, txtRect.size());

        QRectF documentRect({}, d->textDocument.size());
        documentRect.moveCenter(txtRect.center());
        painter.translate(documentRect.topLeft());
        d->textDocument.documentLayout()->draw(&painter, context);
        painter.translate(-documentRect.topLeft());
    }
}

ACAYIPWIDGETS_END_NAMESPACE
