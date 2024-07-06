// Copyright (C) 2024 Ömer Göktaş. All Rights Reserved.
// SPDX-License-Identifier: LicenseRef-AcayipWidgets-Commercial OR GPL-3.0-only

#include "pushbutton_p.h"

#include <QMouseEvent>
#include <QPainter>
#include <QTextBlock>
#include <QTextCursor>

#include <QStyleOptionButton>

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
 *  \internal
*/
PushButtonPrivate::PushButtonPrivate()
    : QPushButtonPrivate()
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
    , styles(StyleDefaults::buttonStyles)
{
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
    q->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    q->setCursor(Qt::PointingHandCursor);
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
    d->mergeStyleWithRest(d->styles.raised, styles.raised);
    d->mergeStyleWithRest(d->styles.disabled, styles.disabled);
    d->mergeStyleWithRest(d->styles.defaultButton, styles.defaultButton);

    update();

    emit stylesChanged(d->styles);
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
}

void PushButton::paintEvent(QPaintEvent* e)
{
    Q_D(PushButton);

    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing
                           | QPainter::SmoothPixmapTransform
                           | QPainter::LosslessImageRendering);

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

    painter.drawPixmap(d->itemRect(PushButtonPrivate::Icon),
                       d->icon.pixmap(iconSize(), devicePixelRatioF()),
                       QRectF({}, iconSize() * devicePixelRatioF()));

    // Draw text
    QString dots("...");
    QRectF textRect = d->itemRect(PushButtonPrivate::Text);
    qreal top = 0;
    for (QTextLine line : d->textLines) {
        line.draw(&painter, textRect.topLeft() + QPointF{0.0, top});
        top += line.height();
    }
}

void PushButton::hideAnimated() {}

void PushButton::showAnimated() {}

ACAYIPWIDGETS_END_NAMESPACE
