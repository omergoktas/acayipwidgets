// Copyright (C) 2024 Ömer Göktaş. All Rights Reserved.
// SPDX-License-Identifier: LicenseRef-AcayipWidgets-Commercial OR GPL-3.0-only

#include "acayiputils.h"
#include "button_p.h"

#include <private/qhexstring_p.h>

#include <QAbstractTextDocumentLayout>
#include <QLayout>
#include <QStyleHints>
#include <QTimer>

using namespace Qt::Literals;

/*
 * TODO: QGraphicsDropShadowEffect causes vibrations when hovered, especially
 * visible when the button has an icon
*/

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
    , spacing(Defaults::spacing / 2.0)
    , margins(Defaults::margins)
    , paddings(Defaults::paddings * 1.5)
    , iconEdge(Qt::LeftEdge)
    , textFormat(Qt::AutoText)
    , rippleStyle(styles.rest)
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
    q->setStyles({
        .rest = {
            .borderRadius = Defaults::borderRadiusPercentagePoint + 25.0,
            .textColor = QColor(0xffffff),
            .textColorDark = QColor(0xffffff),
            .iconColor = QColor(0xffffff),
            .iconColorDark = QColor(0xffffff),
            .backgroundBrush = QColor(0x0f6cbd),
            .backgroundBrushDark = QColor(0x115ea3),
        },
        .restHovered = {
            .backgroundBrush = QColor(0x115ea3),
            .backgroundBrushDark = QColor(0x0f6cbd)
        },
        .restPressed = {
            .backgroundBrush = QColor(0x0c3b5e),
            .backgroundBrushDark = QColor(0x0c3b5e)
        },
        .restDisabled = {
            .textColor = QColor(0xbdbdbd),
            .textColorDark = QColor(0x5c5c5c),
            .iconColor = QColor(0xbdbdbd),
            .iconColorDark = QColor(0x5c5c5c),
            .backgroundBrush = QColor(0xf0f0f0),
            .backgroundBrushDark = QColor(0x141414),
        },
        .checked = {
            .backgroundBrush = QColor(0x0f548c),
            .backgroundBrushDark = QColor(0x0f548c)
        }
    });

    rippleAnimations.append(new QVariantAnimation(q));
    rippleAnimations.append(new QVariantAnimation(q));
    rippleAnimations.append(new QVariantAnimation(q));
    for (QVariantAnimation* anim : std::as_const(rippleAnimations)) {
        // We ripple half the time and fade half the time
        anim->setDuration(Defaults::animationDuration * 2);
        anim->setEasingCurve(Defaults::outEasingType);
        QObject::connect(anim, &QVariantAnimation::valueChanged, q, [q, this] {
            updateHoverShadow();
            q->repaint();
        });
        QObject::connect(anim, &QVariantAnimation::finished, q, [q, this] {
            updateHoverShadow();
            q->update();
        });
    }

    shadowEffect = new QGraphicsDropShadowEffect(q);
    shadowEffect->setColor(QColor(0, 0, 0, 80));
    shadowEffect->setBlurRadius(8);
    shadowEffect->setOffset(0, 0);
    shadowEffect->setEnabled(hoverShadowEnabled || elevated);
    q->setGraphicsEffect(shadowEffect);

    shadowAnimation.setTargetObject(shadowEffect);
    shadowAnimation.setDuration(Defaults::animationDuration);
    shadowAnimation.setPropertyName("yOffset"_ba);
    shadowAnimation.setEasingCurve(Defaults::outEasingType);

    showHideAnimation.setDuration(Defaults::animationDuration);
    showHideAnimation.setEasingCurve(Defaults::outEasingType);

    auto uFont = [this] { updateFont(); };
    QObject::connect(q, &Button::pressed, q, uFont);
    QObject::connect(q, &Button::released, q, uFont);
    QObject::connect(q, &Button::toggled, q, uFont);
    QObject::connect(q, &Button::clicked, q, [q] {
        QTimer::singleShot(Defaults::animationDuration, q, &Button::animatedClicked);
    });
}

/*!
 *  \internal
*/
void ButtonPrivate::mergeStyleWithRest(Button::Style& target,
                                       const Button::Style& source,
                                       const Button::Style& fallback) const
{
    target.borderRadius = source.borderRadius != -1.0 ? source.borderRadius
                                                      : fallback.borderRadius;
    target.borderPen = source.borderPen.style() != Qt::NoPen ? source.borderPen
                                                             : fallback.borderPen;
    target.borderPenDark = source.borderPenDark.style() != Qt::NoPen
                               ? source.borderPenDark
                               : fallback.borderPenDark;
    target.textColor = source.textColor.isValid() ? source.textColor
                                                  : fallback.textColor;
    target.textColorDark = source.textColorDark.isValid() ? source.textColorDark
                                                          : fallback.textColorDark;
    target.iconColor = source.iconColor.isValid() ? source.iconColor
                                                  : fallback.iconColor;
    target.iconColorDark = source.iconColorDark.isValid() ? source.iconColorDark
                                                          : fallback.iconColorDark;
    target.backgroundBrush = source.backgroundBrush.style() != Qt::NoBrush
                                 ? source.backgroundBrush
                                 : fallback.backgroundBrush;
    target.backgroundBrushDark = source.backgroundBrushDark.style() != Qt::NoBrush
                                     ? source.backgroundBrushDark
                                     : fallback.backgroundBrushDark;
    target.font = !source.font.isCopyOf(QFont()) ? source.font : fallback.font;
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

    textDocumentSizeHint = QSize(qCeil(textDocument.size().width()),
                                 qCeil(textDocument.size().height()));

    textDocument.setTextWidth(itemRect(Text).width());
}

QRect ButtonPrivate::itemRect(Item item) const
{
    Q_Q(const Button);

    QRect rect = q->rect() - margins;

    if (item == Background)
        return rect;

    rect -= paddings;

    QRect menuRect;
    if (q->menu())
        menuRect = QRect(QPoint(), scaled(q, QSize(11, 11)));
    if (item == Menu) {
        menuRect.moveCenter(rect.center());
        menuRect.moveRight(rect.right());
        return menuRect;
    }

    QRect iconRect;
    if (!q->icon().isNull())
        iconRect = QRect(QPoint(), iconSize);
    if (item == Icon) {
        switch (iconEdge) {
        case Qt::RightEdge:
            iconRect.moveTo(
                {rect.right() - iconRect.width() - menuRect.width()
                     - (menuRect.isEmpty() ? 0 : spacing),
                 rect.top() + qRound(rect.height() / 2.0 - iconRect.height() / 2.0)});
            break;
        case Qt::LeftEdge:
            iconRect.moveTo(
                {rect.left(),
                 rect.top() + qRound(rect.height() / 2.0 - iconRect.height() / 2.0)});
            break;
        case Qt::TopEdge:
            iconRect.moveTo(
                {rect.left() + qRound(rect.width() / 2.0 - iconRect.width() / 2.0),
                 rect.top()});
            break;
        case Qt::BottomEdge:
            iconRect.moveTo({
                rect.top() + qRound(rect.width() / 2.0 - iconRect.width() / 2.0),
                rect.bottom() - iconRect.height(),
            });
            break;
        default:
            break;
        }
        return iconRect;
    }

    switch (iconEdge) {
    case Qt::RightEdge:
        return rect.adjusted(0,
                             0,
                             -iconRect.width() - (iconRect.isEmpty() ? 0 : spacing)
                                 - menuRect.width() - (menuRect.isEmpty() ? 0 : spacing),
                             0);
    case Qt::LeftEdge:
        return rect.adjusted((iconRect.isEmpty() ? 0 : spacing) + iconRect.width(),
                             0,
                             -menuRect.width() - (menuRect.isEmpty() ? 0 : spacing),
                             0);
    case Qt::TopEdge:
        return rect.adjusted(0,
                             (iconRect.isEmpty() ? 0 : spacing) + iconRect.height(),
                             -menuRect.width() - (menuRect.isEmpty() ? 0 : spacing),
                             0);
    case Qt::BottomEdge:
        return rect.adjusted(0,
                             0,
                             -menuRect.width() - (menuRect.isEmpty() ? 0 : spacing),
                             -(iconRect.isEmpty() ? 0 : spacing) - iconRect.height());
    default:
        return QRect();
    }
}

qreal ButtonPrivate::calculateRadius(qreal value) const
{
    const QRect& b = itemRect(ButtonPrivate::Background);
    return value > Defaults::borderRadiusPercentagePoint
               ? (qMin(b.width(), b.height()) / 2.0)
                     * (value - Defaults::borderRadiusPercentagePoint) / 100.0
               : (value < 0 ? 0 : value);
}

void ButtonPrivate::startRippleAnimation(const QPoint& pos)
{
    for (QVariantAnimation* anim : std::as_const(rippleAnimations)) {
        if (anim->state() == QVariantAnimation::Running)
            continue;
        QRect bgRect = itemRect(ButtonPrivate::Background);
        qreal hypotenuse = std::sqrt(bgRect.width() * bgRect.width()
                                     + bgRect.height() * bgRect.height());
        qreal scale
            = 6.0; // x2 (for diameter) x3 (1x ripple, 1x wait to fill, 1x fade out)
        QRect rect;
        rect.moveCenter(pos);
        anim->setStartValue(rect);
        rect.setSize({qCeil(scale * hypotenuse), qCeil(scale * hypotenuse)});
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
    path.addRoundedRect(QRectF(itemRect(ButtonPrivate::Background))
                            .adjusted(m.left(), m.top(), -m.right(), -m.bottom()),
                        radius,
                        radius);
    return path;
}

const Button::Style& ButtonPrivate::activeStyle() const
{
    Q_Q(const Button);
    if (checkable && checked) {
        if (!q->isEnabled())
            return stylesPainted.checkedDisabled;
        if (pressed)
            return stylesPainted.checkedPressed;
        if (hovering)
            return stylesPainted.checkedHovered;
        return stylesPainted.checked;
    } else {
        if (!q->isEnabled())
            return stylesPainted.restDisabled;
        if (pressed)
            return stylesPainted.restPressed;
        if (hovering)
            return stylesPainted.restHovered;
        return stylesPainted.rest;
    }
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
    Q_Q(const Button);
    if ((elevated || hoverShadowEnabled) && shadowEffect && mouseAttached) {
        const qreal offset = scaled(q, 3.0);
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

void ButtonPrivate::updateFont()
{
    Q_Q(Button);

    const Button::Style& style = activeStyle();
    if (style.font.isCopyOf(QFont())) {
        if (q->parentWidget() && !q->parentWidget()->font().isCopyOf(q->font()))
            q->setFont(q->parentWidget()->font());
        else if (!style.font.isCopyOf(q->font()))
            q->setFont(style.font);
    } else if (!style.font.isCopyOf(q->font())) {
        q->setFont(style.font);
    }
}

QIcon::Mode ButtonPrivate::iconMode() const
{
    Q_Q(const Button);
    if (!q->isEnabled())
        return QIcon::Disabled;
    if (hovering)
        return QIcon::Active;
    if (q->hasFocus())
        return QIcon::Selected;
    return QIcon::Normal;
}

QIcon::State ButtonPrivate::iconState() const
{
    return (checkable && checked) ? QIcon::On : QIcon::Off;
}

Button::Button(const QString& text, const QIcon& icon, QWidget* parent)
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

int Button::spacing() const
{
    Q_D(const Button);
    return d->spacing;
}

void Button::setSpacing(int spacing)
{
    Q_D(Button);
    d->spacing = spacing;
    updateGeometry();
    update();
}

QMargins Button::margins() const
{
    Q_D(const Button);
    return d->margins;
}

void Button::setMargins(const QMargins& margins)
{
    Q_D(Button);
    d->margins = margins;
    updateGeometry();
    update();
}

QMargins Button::paddings() const
{
    Q_D(const Button);
    return d->paddings;
}

void Button::setPaddings(const QMargins& paddings)
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

const Button::Styles& Button::styles() const
{
    Q_D(const Button);
    return d->styles;
}

void Button::setStyles(const Button::Styles& styles)
{
    Q_D(Button);

    d->styles = styles;
    d->stylesPainted.rest = styles.rest;

    d->mergeStyleWithRest(d->stylesPainted.restHovered,
                          styles.restHovered,
                          d->stylesPainted.rest);
    d->mergeStyleWithRest(d->stylesPainted.restPressed,
                          styles.restPressed,
                          d->stylesPainted.rest);
    d->mergeStyleWithRest(d->stylesPainted.restDisabled,
                          styles.restDisabled,
                          d->stylesPainted.rest);

    d->mergeStyleWithRest(d->stylesPainted.checked,
                          styles.checked,
                          d->stylesPainted.rest);

    d->mergeStyleWithRest(d->stylesPainted.checkedHovered,
                          styles.checkedHovered,
                          d->stylesPainted.restHovered);
    d->mergeStyleWithRest(d->stylesPainted.checkedPressed,
                          styles.checkedPressed,
                          d->stylesPainted.restPressed);
    d->mergeStyleWithRest(d->stylesPainted.checkedDisabled,
                          styles.checkedDisabled,
                          d->stylesPainted.restPressed);

    d->updateFont();
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
        d->shadowEffect->setEnabled(d->hoverShadowEnabled || d->elevated);
        d->shadowEffect->setBlurRadius(scaled(this, elevated ? 40 : 8));
        d->shadowEffect->setColor(QColor(0, 0, 0, elevated ? 160 : 80));
    }
}

bool Button::isHoverShadowEnabled() const
{
    Q_D(const Button);
    return d->hoverShadowEnabled;
}

void Button::setHoverShadowEnabled(bool hoverShadowEnabled)
{
    Q_D(Button);
    d->hoverShadowEnabled = hoverShadowEnabled;
    if (d->shadowEffect)
        d->shadowEffect->setEnabled(d->hoverShadowEnabled || d->elevated);
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

    int width = d->margins.left() + d->margins.right() + d->paddings.left()
                + d->paddings.right() + d->textDocumentSizeHint.width();
    int height = d->margins.top() + d->margins.bottom() + d->paddings.top()
                 + d->paddings.bottom() + d->textDocumentSizeHint.height();

    QRect iconRect = d->itemRect(ButtonPrivate::Icon);
    QRect menuRect = d->itemRect(ButtonPrivate::Menu);

    if (!iconRect.isEmpty()) {
        if (d->iconEdge == Qt::RightEdge || d->iconEdge == Qt::LeftEdge)
            width += d->spacing + iconRect.width();
        else
            height += d->spacing + iconRect.height();
    }

    if (!menuRect.isEmpty())
        width += d->spacing + menuRect.width();

    if (d->iconEdge == Qt::RightEdge || d->iconEdge == Qt::LeftEdge) {
        int maxH = qMax(iconRect.height(), menuRect.height());
        if (maxH > d->textDocumentSizeHint.height())
            height += maxH - d->textDocumentSizeHint.height();
    } else {
        int maxW = qMax(iconRect.width(), menuRect.width());
        if (maxW > d->textDocumentSizeHint.width())
            width += maxW - d->textDocumentSizeHint.width();
    }

    return QSize(width, height);
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
    connect(&d->showHideAnimation,
            &QVariantAnimation::finished,
            this,
            qOverload<>(&QWidget::update));
    if (parentWidget() && parentWidget()->layout()) {
        connect(&d->showHideAnimation,
                SIGNAL(finished()),
                parentWidget()->layout(),
                SLOT(prepareAnimate()));
    }
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
    connect(&d->showHideAnimation,
            &QVariantAnimation::finished,
            this,
            qOverload<>(&QWidget::update));
    d->showHideAnimation.setStartValue(0.0);
    d->showHideAnimation.setEndValue(1.0);
    d->showHideAnimation.start();
    if (parentWidget() && parentWidget()->layout())
        QMetaObject::invokeMethod(parentWidget()->layout(), "prepareAnimate");
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
        d->updateFont();
        update();
    } else if (event->type() == QEvent::HoverLeave) {
        d->hovering = false;
        setCursor(d->cursor);
        d->updateHoverShadow();
        d->updateFont();
        update();
    } else if (event->type() == QEvent::HoverMove) {
        auto hover = static_cast<QHoverEvent*>(event);
        if (hitButton(hover->position().toPoint())) {
            if (d->mouseAttached && isEnabled())
                d->hovering = true;
            setCursor(d->cursor);
        } else {
            d->hovering = false;
            setCursor(Qt::ArrowCursor);
        }
        d->updateHoverShadow();
        d->updateFont();
        repaint();
    } else if (event->type() == QEvent::MouseButtonPress) {
        auto press = static_cast<QMouseEvent*>(event);
        if (!d->isRippling() && hitButton(press->position().toPoint())) {
            if (d->checkable && d->checked) {
                if (d->hovering)
                    d->rippleStyle = d->stylesPainted.checkedHovered;
                else
                    d->rippleStyle = d->stylesPainted.checked;
            } else {
                if (d->hovering)
                    d->rippleStyle = d->stylesPainted.restHovered;
                else
                    d->rippleStyle = d->stylesPainted.rest;
            }
            repaint();
        }
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
    } else if (event->type() == QEvent::EnabledChange) {
        d->updateFont();
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
    const Style& style = d->activeStyle();
    const QRect& bgRect = d->itemRect(ButtonPrivate::Background);
    const qreal t = d->shortestActiveRippleAnimationTime();
    const qreal o = d->opacity
                    * (d->showHideAnimation.state() == QAbstractAnimation::Running
                           ? d->showHideAnimation.currentValue().toReal()
                           : 1.0);

    QPainter painter(this);
    painter.setRenderHints(Defaults::renderHints);

    // Paint the background
    {
        const QPen& bPen = darkStyle ? style.borderPenDark : style.borderPen;
        const QBrush& bBrush = darkStyle ? style.backgroundBrushDark
                                         : style.backgroundBrush;
        const qreal bRadius = d->calculateRadius(style.borderRadius);
        const qreal a = bPen.style() != Qt::NoPen ? bPen.widthF() / 2.0 : 0;
        painter.setPen(bPen);
        painter.setBrush(bBrush);
        painter.setOpacity(o * t);
        painter.setClipPath(d->backgroundPath());
        painter.drawRoundedRect(bgRect.adjusted(a, a, -a, -a), bRadius, bRadius);
    }

    // Paint the ripple effect
    if (d->isRippling()) {
        // Paint the ripple background
        const QPen& rbPen = darkStyle ? d->rippleStyle.borderPenDark
                                      : d->rippleStyle.borderPen;
        const QBrush& rbBrush = darkStyle ? d->rippleStyle.backgroundBrushDark
                                          : d->rippleStyle.backgroundBrush;
        const qreal rbRadius = d->calculateRadius(style.borderRadius);
        const qreal b = rbPen.style() != Qt::NoPen ? rbPen.widthF() / 2.0 : 0;

        painter.setPen(rbPen);
        painter.setBrush(rbBrush);
        painter.setOpacity(o * (1.0 - t));
        painter.drawRoundedRect(bgRect.adjusted(b, b, -b, -b), rbRadius, rbRadius);

        // Paint the ripple wave
        for (QVariantAnimation* anim : std::as_const(d->rippleAnimations)) {
            if (anim->state() == QVariantAnimation::Running) {
                const qreal rT = anim->currentLoopTime() / qreal(anim->duration());
                const QRect& ripRect = anim->currentValue().toRect();
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
                    if (rbBrush.style() == Qt::SolidPattern) {
                        gradient.setColorAt(0.0, strongerColor(rbBrush.color()));
                    } else {
                        gradient
                            .setColorAt(0.0,
                                        darkStyle
                                            ? ((d->checkable && d->checked)
                                                   ? d->stylesPainted.checkedPressed
                                                         .backgroundBrushDark.color()
                                                   : d->stylesPainted.restPressed
                                                         .backgroundBrushDark.color())
                                            : ((d->checkable && d->checked)
                                                   ? d->stylesPainted.checkedPressed
                                                         .backgroundBrush.color()
                                                   : d->stylesPainted.restPressed
                                                         .backgroundBrush.color()));
                    }
                    gradient.setColorAt(0.5 + qMax(rT / 2.0 - 0.25, 0.0),
                                        Qt::transparent);
                    painter.setBrush(gradient);
                } else {
                    painter.setBrush(suppliedBrush);
                }
                painter.setPen(Qt::NoPen);
                painter.setOpacity(
                    o * (1.0 - 1.0 / (1.0 + std::exp(-12.0 * (rT - 0.85)))));
                painter.setClipPath(d->backgroundPath(
                    rbPen.style() != Qt::NoPen
                        ? QMarginsF{2.0 * b, 2.0 * b, 2.0 * b, 2.0 * b}
                        : QMarginsF{}));
                painter.drawEllipse(ripRect);
            }
        }
    }

    // Paint the icon
    const QRect& iconRect = d->itemRect(ButtonPrivate::Icon);
    const QColor& iconColor = darkStyle ? style.iconColorDark : style.iconColor;
    if (!iconRect.isEmpty()) {
        painter.setClipRect(iconRect);
        painter.setOpacity(o);
        if (iconColor.isValid()) {
            const QString& cacheKey
                = u"Acayip::Button::icon"_s
                  % HexString<uint>(iconColor.rgba())
                  % HexString<uint>(iconRect.width())
                  % HexString<uint>(iconRect.height())
                  % HexString<quint8>(d->iconMode())
                  % HexString<quint8>(d->iconState())
                  % HexString<quint16>(qRound(devicePixelRatio() * 1000));
            QPixmap px;
            if (!QPixmapCache::find(cacheKey, &px)) {
                px = d->icon.pixmap(iconRect.size(),
                                    devicePixelRatio(),
                                    d->iconMode(),
                                    d->iconState());
                QPainter p(&px);
                p.setRenderHints(painter.renderHints());
                p.setCompositionMode(QPainter::CompositionMode_SourceIn);
                p.fillRect(QRect(QPoint(), iconRect.size()), iconColor);
                p.end();
                QPixmapCache::insert(cacheKey, px);
            }
            painter.drawPixmap(iconRect, px);
        } else {
            d->icon.paint(&painter,
                          iconRect,
                          Qt::AlignCenter,
                          d->iconMode(),
                          d->iconState());
        }
    }

    // Paint the menu arrow
    const QRect& menuRect = d->itemRect(ButtonPrivate::Menu);
    if (!menuRect.isEmpty()) {
        painter.setClipRect(menuRect);
        painter.setOpacity(o);
        if (iconColor.isValid()) {
            const QString& cacheKey
                = u"Acayip::Button::menuArrow"_s
                  % HexString<uint>(iconColor.rgba())
                  % HexString<uint>(menuRect.width())
                  % HexString<uint>(menuRect.height())
                  % HexString<quint8>(d->iconMode())
                  % HexString<quint8>(d->iconState())
                  % HexString<quint16>(qRound(devicePixelRatio() * 1000));
            QPixmap px;
            if (!QPixmapCache::find(cacheKey, &px)) {
                px = d->menuArrow.pixmap(menuRect.size(),
                                         devicePixelRatio(),
                                         d->iconMode(),
                                         d->iconState());
                QPainter p(&px);
                p.setRenderHints(painter.renderHints());
                p.setCompositionMode(QPainter::CompositionMode_SourceIn);
                p.fillRect(QRect(QPoint(), menuRect.size()), iconColor);
                p.end();
                QPixmapCache::insert(cacheKey, px);
            }
            painter.drawPixmap(menuRect, px);
        } else {
            d->menuArrow.paint(&painter,
                               menuRect,
                               Qt::AlignCenter,
                               d->iconMode(),
                               d->iconState());
        }
    }

    // Paint the text
    const QRect& txtRect = d->itemRect(ButtonPrivate::Text);
    const QColor& txtColor = darkStyle ? style.textColorDark : style.textColor;
    if (!text().isEmpty()) {
        painter.setPen(txtColor);
        painter.setBrush(Qt::NoBrush);
        painter.setOpacity(o);
        painter.setClipRect(txtRect);

        QAbstractTextDocumentLayout::PaintContext context;
        context.palette.setColor(QPalette::Text, painter.pen().color());
        context.clip = QRect({}, txtRect.size());

        QRect documentRect({},
                           QSize(qCeil(d->textDocument.size().width()),
                                 qCeil(d->textDocument.size().height())));
        documentRect.moveCenter(txtRect.center());
        painter.translate(documentRect.topLeft());
        d->textDocument.documentLayout()->draw(&painter, context);
        painter.translate(-documentRect.topLeft());
    }
}

ACAYIPWIDGETS_END_NAMESPACE
