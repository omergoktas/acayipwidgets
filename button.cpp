// Copyright (C) 2024 Ömer Göktaş. All Rights Reserved.
// SPDX-License-Identifier: LicenseRef-AcayipWidgets-Commercial OR GPL-3.0-only

#include "button_p.h"
#include "utils_p.h"

#include <QAbstractTextDocumentLayout>
#include <QLayout>
#include <QStyleHints>
#include <QTimer>

using namespace Qt::Literals;

/*
 * FIXME: QGraphicsDropShadowEffect causes vibrations when hovered, especially
 * visible when the button has an icon
*/

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
    \property Button::flat
    \brief this property is private and not used anymore, use setStyles() instead.
*/

/*!
    \property Button::autoDefault
    \brief this property is private and not used anymore, use setDefault() instead.
*/

/*!
 *  \internal
*/
ButtonPrivate::ButtonPrivate()
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
    , menuArrow(":/acayipwidgets/assets/images/arrow-down.svg")
{
    autoDefault = false;
    QTextOption top;
    top.setAlignment(Qt::AlignLeft | Qt::AlignTop);
    top.setWrapMode(QTextOption::WordWrap);
    textDocument.setDefaultTextOption(top);
    textDocument.setDocumentMargin(0);
}

void ButtonPrivate::init()
{
    Q_Q(Button);

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
        QObject::connect(anim, &QVariantAnimation::valueChanged, q, [q, this] {
            updateHoverShadow();
            q->repaint();
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

    QObject::connect(q, &Button::clicked, q, [q] {
        QTimer::singleShot(StyleDefaults::animationDuration,
                           q,
                           &Button::animatedClicked);
    });
}

/*!
 *  \internal
*/
void ButtonPrivate::mergeStyleWithRest(ButtonStyle& target,
                                       const ButtonStyle& source) const
{
    target.borderRadius = source.borderRadius != -1 ? source.borderRadius
                                                    : stylesPainted.rest.borderRadius;
    target.borderPen = source.borderPen.style() != Qt::NoPen
                           ? source.borderPen
                           : stylesPainted.rest.borderPen;
    target.borderPenDark = source.borderPenDark.style() != Qt::NoPen
                               ? source.borderPenDark
                               : stylesPainted.rest.borderPenDark;
    target.textColor = source.textColor.isValid() ? source.textColor
                                                  : stylesPainted.rest.textColor;
    target.textColorDark = source.textColorDark.isValid()
                               ? source.textColorDark
                               : stylesPainted.rest.textColorDark;
    target.iconColor = source.iconColor.isValid() ? source.iconColor
                                                  : stylesPainted.rest.iconColor;
    target.iconColorDark = source.iconColorDark.isValid()
                               ? source.iconColorDark
                               : stylesPainted.rest.iconColorDark;
    target.backgroundBrush = source.backgroundBrush.style() != Qt::NoBrush
                                 ? source.backgroundBrush
                                 : stylesPainted.rest.backgroundBrush;
    target.backgroundBrushDark = source.backgroundBrushDark.style() != Qt::NoBrush
                                     ? source.backgroundBrushDark
                                     : stylesPainted.rest.backgroundBrushDark;
}

void ButtonPrivate::updateTextDocumentContent()
{
    Q_Q(const Button);

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

QRectF ButtonPrivate::itemRect(Item item) const
{
    Q_Q(const Button);

    QRectF rect = QRectF(q->rect()) - margins;

    if (item == Background)
        return rect;

    rect -= paddings;

    QRectF menuRect(0, 0, 16, 16);
    if (item == Menu) {
        if (!q->menu())
            return QRectF();
        menuRect.moveCenter(rect.center());
        menuRect.moveRight(rect.right());
        return menuRect;
    }

    if (item == Icon) {
        if (q->icon().isNull())
            return QRectF();

        switch (iconEdge) {
        case Qt::RightEdge:
            return QRectF({rect.right() - q->iconSize().width() - menuRect.width()
                               - spacing,
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

    if (q->icon().isNull() && !q->menu())
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

qreal ButtonPrivate::calculateRadius(int value) const
{
    const QRectF& b = itemRect(ButtonPrivate::Background);
    return value > StyleDefaults::borderRadiusIsPercentage
               ? (qMin(b.width(), b.height()) / 2.0)
                     * (value % StyleDefaults::borderRadiusIsPercentage) / 100.0
               : value;
}

void ButtonPrivate::startRippleAnimation(const QPoint& pos)
{
    for (QVariantAnimation* anim : std::as_const(rippleAnimations)) {
        if (anim->state() == QVariantAnimation::Running)
            continue;
        QRectF bgRect = itemRect(ButtonPrivate::Background);
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

QPainterPath ButtonPrivate::backgroundPath(const QMarginsF& m) const
{
    qreal radius = calculateRadius(activeStyle().borderRadius);
    QPainterPath path;
    path.addRoundedRect(itemRect(ButtonPrivate::Background)
                            .adjusted(m.left(), m.top(), -m.right(), -m.bottom()),
                        radius,
                        radius);
    return path;
}

const ButtonStyle& ButtonPrivate::activeStyle() const
{
    Q_Q(const Button);
    if (!q->isEnabled())
        return stylesPainted.disabled;
    if (pressed)
        return stylesPainted.pressed;
    if (hovering)
        return stylesPainted.hovered;
    if (checkable && checked)
        return stylesPainted.checked;
    return stylesPainted.rest;
}

bool ButtonPrivate::isRippling() const
{
    for (QVariantAnimation* anim : std::as_const(rippleAnimations)) {
        if (anim->state() == QVariantAnimation::Running)
            return true;
    }
    return false;
}

qreal ButtonPrivate::shortestActiveRippleAnimationTime() const
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

void ButtonPrivate::updateHoverShadow()
{
    if (hoverShadowEnabled && shadowEffect && mouseAttached) {
        static constexpr qreal offset = 3;
        const qreal t = shortestActiveRippleAnimationTime();
        if (hovering) {
            if (isRippling()) {
                shadowEffect->setYOffset(offset * t);
            } else if (shadowEffect->yOffset() != offset) {
                if (shadowAnimation.state() != QAbstractAnimation::Stopped) {
                    if (shadowAnimation.endValue() == offset)
                        return;
                    shadowAnimation.stop();
                }
                shadowAnimation.setStartValue(shadowEffect->yOffset());
                shadowAnimation.setEndValue(offset);
                shadowAnimation.start();
            }
        } else if (shadowEffect->yOffset() != 0) {
            if (shadowAnimation.state() != QAbstractAnimation::Stopped) {
                if (shadowAnimation.endValue() == 0)
                    return;
                shadowAnimation.stop();
            }
            shadowAnimation.setStartValue(shadowEffect->yOffset());
            shadowAnimation.setEndValue(0);
            shadowAnimation.start();
        }
    }
}

/*!
    Constructs a push button with no text and a \a parent.
*/
Button::Button(QWidget* parent)
    : Button(QIcon(), QString(), parent)
{}

/*!
    Constructs a push button with the parent \a parent and the text \a
    text.
*/
Button::Button(const QString& text, QWidget* parent)
    : Button(QIcon(), text, parent)
{}

/*!
    Constructs a push button with an \a icon, a \a text, and a \a parent.

    Note that you can also pass a QPixmap object as an icon (thanks to
    the implicit type conversion provided by C++).
*/
Button::Button(const QIcon& icon, const QString& text, QWidget* parent)
    : Button(*new ButtonPrivate, parent)
{
    setText(text);
    setIcon(icon);
}

/*!
 *  \internal
*/
Button::Button(ButtonPrivate& dd, QWidget* parent)
    : QPushButton(dd, parent)
{
    Q_D(Button);
    d->init();
}

qreal Button::opacity() const
{
    Q_D(const Button);
    return d->opacity;
}

void Button::setOpacity(qreal opacity)
{
    Q_D(Button);
    d->opacity = opacity;
    update();
}

qreal Button::spacing() const
{
    Q_D(const Button);
    return d->spacing;
}

void Button::setSpacing(qreal spacing)
{
    Q_D(Button);
    d->spacing = spacing;
    updateGeometry();
    update();
}

const QMarginsF& Button::margins() const
{
    Q_D(const Button);
    return d->margins;
}

void Button::setMargins(const QMarginsF& margins)
{
    Q_D(Button);
    d->margins = margins;
    updateGeometry();
    update();
}

const QMarginsF& Button::paddings() const
{
    Q_D(const Button);
    return d->paddings;
}

void Button::setPaddings(const QMarginsF& paddings)
{
    Q_D(Button);
    d->paddings = paddings;
    updateGeometry();
    update();
}

Qt::Edge Button::iconEdge() const
{
    Q_D(const Button);
    return d->iconEdge;
}

void Button::setIconEdge(Qt::Edge iconEdge)
{
    Q_D(Button);
    d->iconEdge = iconEdge;
    updateGeometry();
    update();
}

Qt::TextFormat Button::textFormat() const
{
    Q_D(const Button);
    return d->textFormat;
}

void Button::setTextFormat(Qt::TextFormat textFormat)
{
    Q_D(Button);

    if (d->textFormat == textFormat)
        return;

    d->textFormat = textFormat;

    QString copy = text();
    if (!copy.isNull()) {
        QPushButton::setText({});
        setText(copy);
    }
}

const QBrush& Button::rippleBrush(bool dark) const
{
    Q_D(const Button);
    return dark ? d->rippleBrushDark : d->rippleBrush;
}

void Button::setRippleBrush(const QBrush& brush, const QBrush& brushDark)
{
    Q_D(Button);
    d->rippleBrush = brush;
    d->rippleBrushDark = brushDark;
    update();
}

const ButtonStyles& Button::styles() const
{
    Q_D(const Button);
    return d->styles;
}

void Button::setStyles(const ButtonStyles& styles)
{
    Q_D(Button);

    d->styles = styles;

    d->stylesPainted.rest = styles.rest;
    d->mergeStyleWithRest(d->stylesPainted.hovered, styles.hovered);
    d->mergeStyleWithRest(d->stylesPainted.pressed, styles.pressed);
    d->mergeStyleWithRest(d->stylesPainted.checked, styles.checked);
    d->mergeStyleWithRest(d->stylesPainted.disabled, styles.disabled);

    update();
}

bool Button::isElevated() const
{
    Q_D(const Button);
    return d->elevated;
}

void Button::setElevated(bool elevated)
{
    Q_D(Button);
    d->elevated = elevated;
    if (d->shadowEffect) {
        d->shadowEffect->setBlurRadius(elevated ? 40 : 8);
        d->shadowEffect->setColor(QColor(0, 0, 0, elevated ? 160 : 80));
    }
}

bool Button::isHoverShadowEnabled() const
{
    Q_D(const Button);
    return d->elevated;
}

void Button::setHoverShadowEnabled(bool hoverShadowEnabled)
{
    Q_D(Button);
    d->hoverShadowEnabled = hoverShadowEnabled;
    d->updateHoverShadow();
}

void Button::setText(const QString& text)
{
    Q_D(Button);
    if (text == this->text())
        return;

    QPushButton::setText(text);
    d->updateTextDocumentContent();
    updateGeometry();
    update();
}

QSize Button::sizeHint() const
{
    return minimumSizeHint();
}

QSize Button::minimumSizeHint() const
{
    Q_D(const Button);

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

    // FIXME: if (menu())
    // width += d->spacing + d->menuArrow.width();

    return QSize(qCeil(width), qCeil(height));
}

void Button::hideAnimated()
{
    Q_D(Button);
    if (!isVisible())
        return;
    if (d->showHideAnimation.state() != QAbstractAnimation::Stopped) {
        if (d->showHideAnimation.endValue() == 0.0)
            return;
        d->showHideAnimation.stop();
    }
    d->showHideAnimation.disconnect();
    d->showHideAnimation.setStartValue(1.0);
    d->showHideAnimation.setEndValue(0.0);
    connect(&d->showHideAnimation,
            &QVariantAnimation::valueChanged,
            this,
            qOverload<>(&QWidget::repaint));
    connect(&d->showHideAnimation, &QVariantAnimation::finished, this, &Button::hide);
    d->showHideAnimation.start();
    if (parentWidget() && parentWidget()->layout()) {
        connect(&d->showHideAnimation,
                SIGNAL(finished()),
                parentWidget()->layout(),
                SLOT(animate()));
    }
}

void Button::showAnimated()
{
    Q_D(Button);
    if (isVisible())
        return;
    if (d->showHideAnimation.state() != QAbstractAnimation::Stopped) {
        if (d->showHideAnimation.endValue() == 1.0)
            return;
        d->showHideAnimation.stop();
    }
    d->showHideAnimation.disconnect();
    connect(&d->showHideAnimation,
            &QVariantAnimation::valueChanged,
            this,
            qOverload<>(&QWidget::repaint));
    d->showHideAnimation.setStartValue(0.0);
    d->showHideAnimation.setEndValue(1.0);
    d->showHideAnimation.start();
    show();
    if (parentWidget() && parentWidget()->layout())
        QMetaObject::invokeMethod(parentWidget()->layout(), "animate");
}

bool Button::hitButton(const QPoint& pos) const
{
    Q_D(const Button);
    return d->backgroundPath().contains(pos);
}

bool Button::event(QEvent* event)
{
    Q_D(Button);

    if (event->type() == QEvent::HoverEnter) {
        d->hovering = false;
        d->cursor = cursor();
        setCursor(Qt::ArrowCursor);
        d->updateHoverShadow();
        repaint();
    } else if (event->type() == QEvent::HoverLeave) {
        d->hovering = false;
        setCursor(d->cursor);
        d->updateHoverShadow();
        repaint();
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
        d->updateHoverShadow();
        repaint();
    }

    return QPushButton::event(event);
}

void Button::changeEvent(QEvent* event)
{
    Q_D(Button);

    if (event->type() == QEvent::FontChange
        || event->type() == QEvent::ApplicationFontChange) {
        d->updateTextDocumentContent();
        updateGeometry();
        update();
    }

    QPushButton::changeEvent(event);
}

void Button::mousePressEvent(QMouseEvent* event)
{
    Q_D(Button);
    if (hitButton(event->pos()))
        d->startRippleAnimation(event->pos());
    QPushButton::mousePressEvent(event);
}

void Button::mouseMoveEvent(QMouseEvent* event)
{
    Q_D(Button);
    d->mouseAttached = event->source() == Qt::MouseEventNotSynthesized;
    QAbstractButton::mouseMoveEvent(event); // QPushButton breaks the hover logic
}

void Button::resizeEvent(QResizeEvent* event)
{
    Q_D(Button);
    if (event->size().width() != event->oldSize().width())
        d->textDocument.setTextWidth(d->itemRect(ButtonPrivate::Text).width());
    QPushButton::resizeEvent(event);
}

void Button::paintEvent(QPaintEvent*)
{
    Q_D(Button);

    const bool darkStyle = QGuiApplication::styleHints()->colorScheme()
                           == Qt::ColorScheme::Dark;
    const ButtonStyle& style = d->activeStyle();
    const QRectF& bgRect = d->itemRect(ButtonPrivate::Background);
    const QRectF& muRect = d->itemRect(ButtonPrivate::Menu);
    const QRectF& txtRect = d->itemRect(ButtonPrivate::Text);
    const QRectF& icoRect = d->itemRect(ButtonPrivate::Icon);
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
                               ? (darkStyle ? d->stylesPainted.hovered.borderPenDark
                                            : d->stylesPainted.hovered.borderPen)
                               : (darkStyle ? d->stylesPainted.rest.borderPenDark
                                            : d->stylesPainted.rest.borderPen);
        const QBrush& rBrush = d->mouseAttached
                                   ? (darkStyle
                                          ? d->stylesPainted.hovered.backgroundBrushDark
                                          : d->stylesPainted.hovered.backgroundBrush)
                                   : (darkStyle
                                          ? d->stylesPainted.rest.backgroundBrushDark
                                          : d->stylesPainted.rest.backgroundBrush);

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
                            darkStyle
                                ? d->stylesPainted.pressed.backgroundBrushDark.color()
                                : d->stylesPainted.pressed.backgroundBrush.color());
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

    // Paint the icon
    if (!icon().isNull()) {
        QPixmap icoPixmap(d->icon.pixmap(iconSize(), devicePixelRatioF()));
        const QColor& icoColor = darkStyle ? style.iconColorDark : style.iconColor;
        if (icoColor.isValid()) {
            QPainter p(&icoPixmap);
            p.setRenderHints(painter.renderHints());
            p.setCompositionMode(QPainter::CompositionMode_SourceIn);
            p.fillRect(icoPixmap.rect(), icoColor);
        }
        painter.setPen(Qt::NoPen);
        painter.setBrush(Qt::NoBrush);
        painter.setOpacity(o);
        painter.setClipRect(icoRect);
        painter.drawPixmap(icoRect,
                           icoPixmap,
                           QRectF({}, iconSize() * devicePixelRatioF()));
    }

    // Paint the menu arrow
    if (menu()) {
        QPixmap muPixmap(
            d->menuArrow.pixmap(muRect.size().toSize(), devicePixelRatioF()));
        const QColor& muColor = darkStyle ? style.iconColorDark : style.iconColor;
        if (muColor.isValid()) {
            QPainter p(&muPixmap);
            p.setRenderHints(painter.renderHints());
            p.setCompositionMode(QPainter::CompositionMode_SourceIn);
            p.fillRect(muPixmap.rect(), muColor);
        }
        painter.setPen(Qt::NoPen);
        painter.setBrush(Qt::NoBrush);
        painter.setOpacity(o);
        painter.setClipRect(muRect);
        painter.drawPixmap(muRect,
                           muPixmap,
                           QRectF({}, muRect.size() * devicePixelRatioF()));
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
