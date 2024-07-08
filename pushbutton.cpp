// Copyright (C) 2024 Ömer Göktaş. All Rights Reserved.
// SPDX-License-Identifier: LicenseRef-AcayipWidgets-Commercial OR GPL-3.0-only

#include "pushbutton_p.h"

#include <QPainter>
#include <QStyleHints>
#include <QTextBlock>
#include <QTextCursor>

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
    , afloat(false)
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
    top.setAlignment(Qt::AlignTop | Qt::AlignLeft);
    top.setWrapMode(QTextOption::NoWrap);
    textDocument.setDefaultTextOption(top);
    textDocument.setIndentWidth(0);
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

    textDocument.clear();
    textDocument.setDefaultFont(q->font());

    QTextCursor cursor(&textDocument);
    cursor.select(QTextCursor::Document);
    cursor.setPosition(0);

    if (textFormat == Qt::RichText)
        cursor.insertHtml(q->text());
    else if (textFormat == Qt::PlainText)
        cursor.insertText(q->text());
    else if (Qt::mightBeRichText(q->text()))
        cursor.insertHtml(q->text());
    else
        cursor.insertText(q->text());

    redoTextLayout();
}

void PushButtonPrivate::redoTextLayout()
{
    Q_Q(const PushButton);

    textLines.clear();
    naturalTextRect = QRectF();

    if (q->text().isNull())
        return;

    qreal top = 0;
    QPainterPath path;
    for (QTextBlock block = textDocument.begin(); block != textDocument.end();
         block = block.next()) {
        QTextLayout* layout = block.layout();
        layout->beginLayout();
        while (true) {
            QTextLine line = layout->createLine();
            if (!line.isValid())
                break;
            line.setLineWidth(std::numeric_limits<int>::max());
            path.addRect(QRectF(QPointF(0, top), line.naturalTextRect().size()));
            textLines.append(line);
            top += line.height();
        }
        layout->endLayout();
    }

    naturalTextRect = path.boundingRect();

    Q_ASSERT(!textLines.isEmpty() && !naturalTextRect.isNull());
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
    setIcon(QIcon("/home/omergoktas/Projeler/AppTemplateQt/deploy/icon.svg"));
    setText("Naber\nlooop");
    setIconEdge(Qt::TopEdge);
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
    emit opacityChanged(opacity);
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
    emit spacingChanged(spacing);
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
    emit marginsChanged(margins);
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
    emit paddingsChanged(paddings);
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
    emit iconEdgeChanged(iconEdge);
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

    emit textFormatChanged(textFormat);
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
    d->mergeStyleWithRest(d->styles.afloat, styles.afloat);
    d->mergeStyleWithRest(d->styles.disabled, styles.disabled);
    d->mergeStyleWithRest(d->styles.defaultButton, styles.defaultButton);

    update();

    emit stylesChanged(d->styles);
}

void PushButton::setAfloat(bool afloat)
{
    Q_D(PushButton);
    d->afloat = afloat;
    update();
}

bool PushButton::isAfloat() const
{
    Q_D(const PushButton);
    return d->afloat;
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
                  + d->paddings.right() + d->naturalTextRect.width();
    qreal height = d->margins.top() + d->margins.bottom() + d->paddings.top()
                   + d->paddings.bottom() + d->naturalTextRect.height();

    if (!icon().isNull()) {
        if (d->iconEdge == Qt::RightEdge || d->iconEdge == Qt::LeftEdge)
            width += d->spacing + iconSize().width();
        else
            height += d->spacing + iconSize().height();
    }

    return QSize(qCeil(width), qCeil(height));
}

bool PushButton::event(QEvent* event)
{
    Q_D(PushButton);

    bool result = QPushButton::event(event);

    if (event->type() == QEvent::HoverEnter) {
        d->cursor = cursor();
        setCursor(Qt::ArrowCursor);
    } else if (event->type() == QEvent::HoverLeave) {
        setCursor(d->cursor);
    } else if (event->type() == QEvent::HoverMove) {
        auto hover = static_cast<QHoverEvent*>(event);
        if (hitButton(hover->position().toPoint()))
            setCursor(d->cursor);
        else
            setCursor(Qt::ArrowCursor);
    }

    return result;
}

void PushButton::changeEvent(QEvent* event)
{
    Q_D(PushButton);

    QPushButton::changeEvent(event);

    if (event->type() == QEvent::FontChange
        || event->type() == QEvent::ApplicationFontChange) {
        d->updateTextDocumentContent();
        updateGeometry();
        update();
    }

    if (event->type() == QEvent::CursorChange)
        qDebug() << "adasda";
}

void PushButton::paintEvent(QPaintEvent*)
{
    Q_D(PushButton);

    const bool darkStyle = QGuiApplication::styleHints()->colorScheme()
                           == Qt::ColorScheme::Dark;

    QPainter painter(this);
    painter.setRenderHints(StyleDefaults::renderHints);

    // Paint the background
    qreal radius;
    qreal h; // half pen with
    QRectF bgRect = d->itemRect(PushButtonPrivate::Background);
    qDebug() << bgRect << rect();
    if (isEnabled()) {
        if (d->down) {
            radius = d->calculateRadius(d->styles.pressed.borderRadius);
            painter.setPen(darkStyle ? d->styles.pressed.borderPen
                                     : d->styles.pressed.borderPenDark);
            painter.setBrush(darkStyle ? d->styles.pressed.backgroundBrush
                                       : d->styles.pressed.backgroundBrushDark);
            h = painter.pen().widthF() / 2.0;
            if (painter.pen() != Qt::NoPen || painter.brush() != Qt::NoBrush)
                painter.drawRoundedRect(bgRect.adjusted(h, h, -h, -h), radius, radius);
        } else if (d->hovering) {
            radius = d->calculateRadius(d->styles.hovered.borderRadius);
            painter.setPen(darkStyle ? d->styles.hovered.borderPen
                                     : d->styles.hovered.borderPenDark);
            painter.setBrush(darkStyle ? d->styles.hovered.backgroundBrush
                                       : d->styles.hovered.backgroundBrushDark);
            qDebug() << painter.pen() << painter.brush();
            h = painter.pen().widthF() / 2.0;
            if (painter.pen() != Qt::NoPen || painter.brush() != Qt::NoBrush)
                painter.drawRoundedRect(bgRect.adjusted(h, h, -h, -h), radius, radius);
        } else if (d->checkable && d->checked) {
            radius = d->calculateRadius(d->styles.checked.borderRadius);
            painter.setPen(darkStyle ? d->styles.checked.borderPen
                                     : d->styles.checked.borderPenDark);
            painter.setBrush(darkStyle ? d->styles.checked.backgroundBrush
                                       : d->styles.checked.backgroundBrushDark);
            h = painter.pen().widthF() / 2.0;
            if (painter.pen() != Qt::NoPen || painter.brush() != Qt::NoBrush)
                painter.drawRoundedRect(bgRect.adjusted(h, h, -h, -h), radius, radius);
        } else {
            radius = d->calculateRadius(d->styles.rest.borderRadius);
            painter.setPen(darkStyle ? d->styles.rest.borderPen
                                     : d->styles.rest.borderPenDark);
            painter.setBrush(darkStyle ? d->styles.rest.backgroundBrush
                                       : d->styles.rest.backgroundBrushDark);
            h = painter.pen().widthF() / 2.0;
            if (painter.pen() != Qt::NoPen || painter.brush() != Qt::NoBrush)
                painter.drawRoundedRect(bgRect.adjusted(h, h, -h, -h),
                                        radius,
                                        radius);
            if (isAfloat()) {
                radius = d->calculateRadius(d->styles.afloat.borderRadius);
                painter.setPen(darkStyle ? d->styles.afloat.borderPen
                                         : d->styles.afloat.borderPenDark);
                painter.setBrush(darkStyle ? d->styles.afloat.backgroundBrush
                                           : d->styles.afloat.backgroundBrushDark);
                h = painter.pen().widthF() / 2.0;
                if (painter.pen() != Qt::NoPen || painter.brush() != Qt::NoBrush)
                    painter.drawRoundedRect(bgRect.adjusted(h, h, -h, -h),
                                            radius,
                                            radius);
            }
            if (d->defaultButton) {
                radius = d->calculateRadius(d->styles.defaultButton.borderRadius);
                painter.setPen(darkStyle ? d->styles.defaultButton.borderPen
                                         : d->styles.defaultButton.borderPenDark);
                painter.setBrush(darkStyle
                                     ? d->styles.defaultButton.backgroundBrush
                                     : d->styles.defaultButton.backgroundBrushDark);
                h = painter.pen().widthF() / 2.0;
                if (painter.pen() != Qt::NoPen || painter.brush() != Qt::NoBrush)
                    painter.drawRoundedRect(bgRect.adjusted(h, h, -h, -h),
                                            radius,
                                            radius);
            }
        }
    } else {
        radius = d->calculateRadius(d->styles.disabled.borderRadius);
        painter.setPen(darkStyle ? d->styles.disabled.borderPen
                                 : d->styles.disabled.borderPenDark);
        painter.setBrush(darkStyle ? d->styles.disabled.backgroundBrush
                                   : d->styles.disabled.backgroundBrushDark);
        h = painter.pen().widthF() / 2.0;
        if (painter.pen() != Qt::NoPen || painter.brush() != Qt::NoBrush)
            painter.drawRoundedRect(bgRect.adjusted(h, h, -h, -h), radius, radius);
    }

    // d->textLine.draw(&painter, QPointF(left, top));
    // if (d->naturalTextRect.width() > d->availableTextSpace()) {
    //     painter.drawText(QRectF(left + d->textLine.horizontalAdvance(),
    //                             top,
    //                             fontMetrics().horizontalAdvance(dots),
    //                             d->textLine.height()),
    //                      dots,
    //                      Qt::AlignVCenter | Qt::AlignLeft);
    // }

    // painter.drawRect(d->itemRect(PushButtonPrivate::Background));
    // painter.drawRect(
    //     QRectF(rect()).marginsRemoved(d->margins).marginsRemoved(d->paddings));

    if (!icon().isNull()) {
        painter.drawPixmap(d->itemRect(PushButtonPrivate::Icon),
                           d->icon.pixmap(iconSize(), devicePixelRatioF()),
                           QRectF({}, iconSize() * devicePixelRatioF()));
    }

    // Draw text
    QString dots("...");
    QRectF textRect = d->itemRect(PushButtonPrivate::Text);
    qreal top = 0;
    for (QTextLine line : d->textLines) {
        line.draw(&painter, textRect.topLeft() + QPointF{0.0, top});
        top += line.height();
    }
}

bool PushButton::hitButton(const QPoint& pos) const
{
    Q_D(const PushButton);
    qreal radius = d->calculateRadius(d->styles.rest.borderRadius);
    QPainterPath path;
    path.addRoundedRect(d->itemRect(PushButtonPrivate::Background), radius, radius);
    return path.contains(pos);
}

void PushButton::hideAnimated() {}

void PushButton::showAnimated() {}

ACAYIPWIDGETS_END_NAMESPACE
