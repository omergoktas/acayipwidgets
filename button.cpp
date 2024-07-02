// Copyright (C) 2024 Ömer Göktaş. All Rights Reserved.

#include "button.h"
#include "button_p.h"

#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>

/*!
    \class Button
    \brief The Button class provides a customizable button widget.
    \inmodule QtWidgets

    The Button class is a customizable button widget that combines features
    from both Fluent UI and Material Design. It supports different button types,
    styles, and visual effects such as ripple animations.

    Example usage:
    \code
    Button *button = new Button("Click me");
    button->setButtonType(Button::Primary);
    button->setButtonStyle(Button::Filled);
    connect(button, &Button::clicked, this, &MyWidget::handleButtonClick);
    \endcode
*/

/*!
    \enum Button::ButtonType

    This enum describes the type of the button.

    \value Standard   A standard button.
    \value Primary    A primary action button.
    \value Secondary  A secondary action button.
    \value Accent     An accent button for highlighting actions.
    \value FAB        A floating action button.
*/

/*!
    \enum Button::ButtonStyle

    This enum describes the visual style of the button.

    \value Filled     A button with a filled background.
    \value Outlined   A button with an outlined border.
    \value Text       A text-only button without background or border.
*/

#define AUTO_REPEAT_DELAY 300
#define AUTO_REPEAT_INTERVAL 100

QAbstractButtonPrivate::QAbstractButtonPrivate(QSizePolicy::ControlType type)
    :
#ifndef QT_NO_SHORTCUT
    shortcutId(0)
    ,
#endif
    checkable(false)
    , checked(false)
    , autoRepeat(false)
    , autoExclusive(false)
    , down(false)
    , blockRefresh(false)
    , pressed(false)
    ,
#if QT_CONFIG(buttongroup)
    group(nullptr)
    ,
#endif
    autoRepeatDelay(AUTO_REPEAT_DELAY)
    , autoRepeatInterval(AUTO_REPEAT_INTERVAL)
    , controlType(type)
{}

void QAbstractButtonPrivate::init()
{
    Q_Q(QAbstractButton);

    q->setFocusPolicy(
        Qt::FocusPolicy(q->style()->styleHint(QStyle::SH_Button_FocusPolicy)));
    q->setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed, controlType));
    q->setAttribute(Qt::WA_WState_OwnSizePolicy, false);
    q->setForegroundRole(QPalette::ButtonText);
    q->setBackgroundRole(QPalette::Button);
}

/*! \internal
 */
QAbstractButton::QAbstractButton(QAbstractButtonPrivate& dd, QWidget* parent)
    : QWidget(dd, parent, {})
{
    Q_D(QAbstractButton);
    d->init();
}

RippleEffect::RippleEffect(const QPoint& center, QObject* parent)
    : QObject(parent)
    , m_center(center)
    , m_radius(0)
{}

void RippleEffect::setRadius(qreal radius)
{
    if (m_radius != radius) {
        m_radius = radius;
    }
}

ButtonPrivate::ButtonPrivate()
    : QAbstractButtonPrivate(QSizePolicy::PushButton)
    , buttonType(Button::Secondary)
    , buttonStyle(Button::Outlined)
    , backgroundColor(0, 120, 212)
    , textColor(Qt::white)
    , borderColor(0, 120, 212)
    , borderWidth(1)
    , borderRadius(4)
    , outlined(false)
    , elevated(false)
    , elevation(2)
    , isHovered(false)
    , isPressed(false)
{}

void ButtonPrivate::init()
{
    Q_Q(Button);
    q->setMouseTracking(true);
    q->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    q->setCursor(Qt::PointingHandCursor);
    updateColors();
}

void ButtonPrivate::updateColors()
{
    Q_Q(Button);
    switch (buttonType) {
    case Button::Primary:
        backgroundColor = QColor(0, 120, 212);
        textColor = Qt::white;
        borderColor = QColor(0, 120, 212);
        break;
    case Button::Secondary:
        backgroundColor = QColor(255, 255, 255);
        textColor = QColor(0, 120, 212);
        borderColor = QColor(0, 120, 212);
        break;
    case Button::Accent:
        backgroundColor = QColor(255, 140, 0);
        textColor = Qt::white;
        borderColor = QColor(255, 140, 0);
        break;
    case Button::FAB:
        backgroundColor = QColor(0, 120, 212);
        textColor = Qt::white;
        borderColor = QColor(0, 120, 212);
        borderRadius = q->height() / 2;
        break;
    default:
        backgroundColor = QColor(240, 240, 240);
        textColor = Qt::black;
        borderColor = QColor(200, 200, 200);
        break;
    }

    if (buttonStyle == Button::Outlined) {
        std::swap(backgroundColor, textColor);
        outlined = true;
    } else if (buttonStyle == Button::Text) {
        backgroundColor = Qt::transparent;
        textColor = QColor(0, 120, 212);
        borderColor = Qt::transparent;
        outlined = false;
    }
}

void ButtonPrivate::drawBackground(QPainter& painter)
{
    Q_Q(Button);
    QPainterPath path;
    path.addRoundedRect(q->rect(), borderRadius, borderRadius);

    if (elevated && !isPressed) {
        painter.save();
        painter.translate(0, elevation);
        painter.fillPath(path, QColor(0, 0, 0, 30));
        painter.restore();
    }

    QColor bgColor = backgroundColor;
    if (isHovered) {
        bgColor = bgColor.lighter(110);
    }
    if (isPressed) {
        bgColor = bgColor.darker(110);
    }

    painter.fillPath(path, bgColor);
}

void ButtonPrivate::drawBorder(QPainter& painter)
{
    Q_Q(Button);
    if (outlined || buttonStyle == Button::Outlined) {
        QPainterPath path;
        path.addRoundedRect(q->rect().adjusted(borderWidth / 2,
                                               borderWidth / 2,
                                               -borderWidth / 2,
                                               -borderWidth / 2),
                            borderRadius,
                            borderRadius);
        QPen pen(borderColor, borderWidth);
        painter.strokePath(path, pen);
    }
}

void ButtonPrivate::drawText(QPainter& painter)
{
    Q_Q(Button);
    painter.setPen(textColor);
    painter.drawText(q->rect(), Qt::AlignCenter, q->text());
}

void ButtonPrivate::drawIcon(QPainter& painter)
{
    Q_Q(Button);
    if (!q->icon().isNull()) {
        QIcon::Mode mode = q->isEnabled() ? QIcon::Normal : QIcon::Disabled;
        QIcon::State state = q->isChecked() ? QIcon::On : QIcon::Off;
        QSize iconSize = q->iconSize();
        QRect iconRect = q->rect();
        iconRect.setWidth(iconSize.width());
        iconRect.moveCenter(q->rect().center());
        q->icon().paint(&painter, iconRect, Qt::AlignCenter, mode, state);
    }
}

void ButtonPrivate::drawRippleEffect(QPainter& painter)
{
    painter.save();
    painter.setOpacity(0.3);
    painter.setBrush(textColor);
    painter.setPen(Qt::NoPen);

    for (const auto& ripple : rippleEffects) {
        painter.drawEllipse(QPointF{ripple->center()},
                            ripple->radius(),
                            ripple->radius());
    }

    painter.restore();
}

void ButtonPrivate::updateCursor()
{
    Q_Q(Button);
    q->setCursor(q->isEnabled() ? Qt::PointingHandCursor : Qt::ArrowCursor);
}

void ButtonPrivate::startRippleAnimation(const QPoint& pos)
{
    Q_Q(Button);
    RippleEffect* ripple = new RippleEffect(pos, q);
    rippleEffects.append(ripple);

    QPropertyAnimation* anim = new QPropertyAnimation(ripple, "radius", q);
    anim->setDuration(300);
    anim->setStartValue(0);
    anim->setEndValue(q->width());
    anim->setEasingCurve(QEasingCurve::OutQuad);

    QObject::connect(anim, &QPropertyAnimation::valueChanged, q, [q]() { q->update(); });
    QObject::connect(anim, &QPropertyAnimation::finished, q, [this, ripple, anim]() {
        rippleEffects.removeOne(ripple);
        rippleAnimations.removeOne(anim);
        ripple->deleteLater();
        anim->deleteLater();
    });

    rippleAnimations.append(anim);
    anim->start(QAbstractAnimation::DeleteWhenStopped);
}

/*!
    Constructs a Button with the given \a parent.
*/
Button::Button(QWidget* parent)
    : QAbstractButton(*new ButtonPrivate, parent)
{
    Q_D(Button);
    d->init();
}

/*!
    Constructs a Button with the given \a text and \a parent.
*/
Button::Button(const QString& text, QWidget* parent)
    : Button(parent)
{
    setText(text);
}

/*!
    Destroys the Button.
*/
Button::~Button() = default;

/*!
    \property Button::buttonType
    \brief the type of the button

    This property holds the type of the button, which affects its appearance and behavior.
    The default value is Button::Standard.

    \sa ButtonType
*/
Button::ButtonType Button::buttonType() const
{
    Q_D(const Button);
    return d->buttonType;
}

void Button::setButtonType(ButtonType type)
{
    Q_D(Button);
    if (d->buttonType != type) {
        d->buttonType = type;
        d->updateColors();
        update();
        emit buttonTypeChanged(type);
    }
}

/*!
    \property Button::buttonStyle
    \brief the visual style of the button

    This property holds the visual style of the button.
    The default value is Button::Filled.

    \sa ButtonStyle
*/
Button::ButtonStyle Button::buttonStyle() const
{
    Q_D(const Button);
    return d->buttonStyle;
}

void Button::setButtonStyle(ButtonStyle style)
{
    Q_D(Button);
    if (d->buttonStyle != style) {
        d->buttonStyle = style;
        d->updateColors();
        update();
        emit buttonStyleChanged(style);
    }
}

/*!
    \property Button::backgroundColor
    \brief the background color of the button
*/
QColor Button::backgroundColor() const
{
    Q_D(const Button);
    return d->backgroundColor;
}

void Button::setBackgroundColor(const QColor& color)
{
    Q_D(Button);
    if (d->backgroundColor != color) {
        d->backgroundColor = color;
        update();
        emit backgroundColorChanged(color);
    }
}

QColor Button::textColor() const
{
    Q_D(const Button);
    return d->textColor;
}

void Button::setTextColor(const QColor& color)
{
    Q_D(Button);
    if (d->textColor != color) {
        d->textColor = color;
        update();
        emit textColorChanged(color);
    }
}

/*!
    \property Button::borderColor
    \brief the border color of the button
*/
QColor Button::borderColor() const
{
    Q_D(const Button);
    return d->borderColor;
}

void Button::setBorderColor(const QColor& color)
{
    Q_D(Button);
    if (d->borderColor != color) {
        d->borderColor = color;
        update();
        emit borderColorChanged(color);
    }
}

/*!
    \property Button::borderWidth
    \brief the border width of the button
*/
int Button::borderWidth() const
{
    Q_D(const Button);
    return d->borderWidth;
}

void Button::setBorderWidth(int width)
{
    Q_D(Button);
    if (d->borderWidth != width) {
        d->borderWidth = width;
        update();
        emit borderWidthChanged(width);
    }
}

/*!
    \property Button::borderRadius
    \brief the border radius of the button
*/
int Button::borderRadius() const
{
    Q_D(const Button);
    return d->borderRadius;
}

void Button::setBorderRadius(int radius)
{
    Q_D(Button);
    if (d->borderRadius != radius) {
        d->borderRadius = radius;
        update();
        emit borderRadiusChanged(radius);
    }
}

/*!
    \property Button::isOutlined
    \brief whether the button has an outlined appearance
*/
bool Button::isOutlined() const
{
    Q_D(const Button);
    return d->outlined;
}

void Button::setOutlined(bool outlined)
{
    Q_D(Button);
    if (d->outlined != outlined) {
        d->outlined = outlined;
        d->updateColors();
        update();
        emit outlinedChanged(outlined);
    }
}

/*!
    \property Button::isElevated
    \brief whether the button has an elevated appearance
*/
bool Button::isElevated() const
{
    Q_D(const Button);
    return d->elevated;
}

void Button::setElevated(bool elevated)
{
    Q_D(Button);
    if (d->elevated != elevated) {
        d->elevated = elevated;
        update();
        emit elevatedChanged(elevated);
    }
}

/*!
    \property Button::elevation
    \brief the elevation of the button when it has an elevated appearance
*/
int Button::elevation() const
{
    Q_D(const Button);
    return d->elevation;
}

void Button::setElevation(int elevation)
{
    Q_D(Button);
    if (d->elevation != elevation) {
        d->elevation = elevation;
        update();
        emit elevationChanged(elevation);
    }
}

/*!
    \reimp
*/
QSize Button::sizeHint() const
{
    QFontMetrics fm(font());
    int width = fm.horizontalAdvance(text()) + 20;
    int height = fm.height() + 12;
    return QSize(width, height);
}

/*!
    \reimp
*/
void Button::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);
    Q_D(Button);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    d->drawBackground(painter);
    d->drawBorder(painter);
    d->drawRippleEffect(painter);
    d->drawText(painter);
    d->drawIcon(painter);
}

/*!
    \reimp
*/
void Button::enterEvent(QEnterEvent* event)
{
    Q_D(Button);
    QAbstractButton::enterEvent(event);
    d->isHovered = true;
    d->updateCursor();
    update();
}

/*!
    \reimp
*/
void Button::leaveEvent(QEvent* event)
{
    Q_D(Button);
    QAbstractButton::leaveEvent(event);
    d->isHovered = false;
    d->updateCursor();
    update();
}

/*!
    \reimp
*/
void Button::mousePressEvent(QMouseEvent* event)
{
    Q_D(Button);
    QAbstractButton::mousePressEvent(event);
    d->isPressed = true;
    d->startRippleAnimation(event->pos());
    update();
}

/*!
    \reimp
*/
void Button::mouseReleaseEvent(QMouseEvent* event)
{
    Q_D(Button);
    QAbstractButton::mouseReleaseEvent(event);
    d->isPressed = false;
    update();
}

/*!
    \internal
*/
Button::Button(ButtonPrivate& dd, QWidget* parent)
    : QAbstractButton(dd, parent)
{
    Q_D(Button);
    d->init();
}

#include "moc_button.cpp"
