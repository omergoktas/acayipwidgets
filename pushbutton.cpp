// Copyright (C) 2024 Ömer Göktaş. All Rights Reserved.
// SPDX-License-Identifier: LicenseRef-AcayipWidgets-Commercial OR GPL-3.0-only

#include "pushbutton.h"
#include "pushbutton_p.h"

#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>

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
    \class QPushButton
    \brief The QPushButton widget provides a command button.

    \ingroup basicwidgets
    \inmodule QtWidgets

    \image windows-pushbutton.png

    The push button, or command button, is perhaps the most commonly
    used widget in any graphical user interface. Push (click) a button
    to command the computer to perform some action, or to answer a
    question. Typical buttons are OK, Apply, Cancel, Close, Yes, No
    and Help.

    A command button is rectangular and typically displays a text
    label describing its action. A shortcut key can be specified by
    preceding the preferred character with an ampersand in the
    text. For example:

    \snippet code/src_gui_widgets_qpushbutton.cpp 0

    In this example the shortcut is \e{Alt+D}. See the \l
    {QShortcut#mnemonic}{QShortcut} documentation for details (to
    display an actual ampersand, use '&&').

    Push buttons display a textual label, and optionally a small
    icon. These can be set using the constructors and changed later
    using setText() and setIcon().  If the button is disabled, the
    appearance of the text and icon will be manipulated with respect
    to the GUI style to make the button look "disabled".

    A push button emits the signal clicked() when it is activated by
    the mouse, the Spacebar or by a keyboard shortcut. Connect to
    this signal to perform the button's action. Push buttons also
    provide less commonly used signals, for example pressed() and
    released().

    Command buttons in dialogs are by default auto-default buttons,
    i.e., they become the default push button automatically when they
    receive the keyboard input focus. A default button is a push
    button that is activated when the user presses the Enter or Return
    key in a dialog. You can change this with setAutoDefault(). Note
    that auto-default buttons reserve a little extra space which is
    necessary to draw a default-button indicator. If you do not want
    this space around your buttons, call setAutoDefault(false).

    Being so central, the button widget has grown to accommodate a
    great many variations in the past decade. The Microsoft style
    guide now shows about ten different states of Windows push buttons
    and the text implies that there are dozens more when all the
    combinations of features are taken into consideration.

    The most important modes or states are:
    \list
    \li Available or not (grayed out, disabled).
    \li Standard push button, toggling push button or menu button.
    \li On or off (only for toggling push buttons).
    \li Default or normal. The default button in a dialog can generally
       be "clicked" using the Enter or Return key.
    \li Auto-repeat or not.
    \li Pressed down or not.
    \endlist

    As a general rule, use a push button when the application or
    dialog window performs an action when the user clicks on it (such
    as Apply, Cancel, Close and Help) \e and when the widget is
    supposed to have a wide, rectangular shape with a text label.
    Small, typically square buttons that change the state of the
    window rather than performing an action (such as the buttons in
    the top-right corner of the QFileDialog) are not command buttons,
    but tool buttons. Qt provides a special class (QToolButton) for
    these buttons.

    If you need toggle behavior (see setCheckable()) or a button
    that auto-repeats the activation signal when being pushed down
    like the arrows in a scroll bar (see setAutoRepeat()), a command
    button is probably not what you want. When in doubt, use a tool
    button.

    \note On \macos when a push button's width becomes smaller than 50 or
    its height becomes smaller than 30, the button's corners are
    changed from round to square. Use the setMinimumSize()
    function to prevent this behavior.

    A variation of a command button is a menu button. These provide
    not just one command, but several, since when they are clicked
    they pop up a menu of options. Use the method setMenu() to
    associate a popup menu with a push button.

    Other classes of buttons are option buttons (see QRadioButton) and
    check boxes (see QCheckBox).


    In Qt, the QAbstractButton base class provides most of the modes
    and other API, and QPushButton provides GUI logic.
    See QAbstractButton for more information about the API.

    \sa QToolButton, QRadioButton, QCheckBox
*/

/*!
    \property QPushButton::autoDefault
    \brief whether the push button is an auto default button

    If this property is set to true then the push button is an auto
    default button.

    In some GUI styles a default button is drawn with an extra frame
    around it, up to 3 pixels or more. Qt automatically keeps this
    space free around auto-default buttons, i.e., auto-default buttons
    may have a slightly larger size hint.

    This property's default is true for buttons that have a QDialog
    parent; otherwise it defaults to false.

    See the \l default property for details of how \l default and
    auto-default interact.
*/

/*!
    \property QPushButton::default
    \brief whether the push button is the default button

    Default and autodefault buttons decide what happens when the user
    presses enter in a dialog.

    A button with this property set to true (i.e., the dialog's
    \e default button,) will automatically be pressed when the user presses enter,
    with one exception: if an \a autoDefault button currently has focus, the autoDefault
    button is pressed. When the dialog has \l autoDefault buttons but no default button,
    pressing enter will press either the \l autoDefault button that currently has focus, or if no
    button has focus, the next \l autoDefault button in the focus chain.

    In a dialog, only one push button at a time can be the default
    button. This button is then displayed with an additional frame
    (depending on the GUI style).

    The default button behavior is provided only in dialogs. Buttons
    can always be clicked from the keyboard by pressing Spacebar when
    the button has focus.

    If the default property is set to false on the current default button
    while the dialog is visible, a new default will automatically be
    assigned the next time a push button in the dialog receives focus.

    This property's default is false.
*/

/*!
    \property QPushButton::flat
    \brief this is private and not used anymore, use border properties instead.
*/

RippleEffect::RippleEffect(const QPoint& center, QObject* parent)
    : QObject(parent)
    , m_center(center)
    , m_radius(0)
{}

void RippleEffect::setRadius(qreal radius)
{
    m_radius = radius;
}

PushButtonPrivate::PushButtonPrivate()
    : QPushButtonPrivate()
    , backgroundColor(0, 120, 212)
    , textColor(Qt::white)
    , borderColor(0, 120, 212)
    , borderWidth(1)
    , borderRadius(4)
    , isHovered(false)
    , isPressed(false)
{}

void PushButtonPrivate::init()
{
    Q_Q(PushButton);
    q->setMouseTracking(true);
    q->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    q->setCursor(Qt::PointingHandCursor);
}

/*!
    Constructs a push button with no text and a \a parent.
*/
PushButton::PushButton(QWidget* parent)
    : QPushButton(QIcon(), QString(), parent)
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

/*!
    Destroys the push button.
*/
PushButton::~PushButton() {}

ACAYIPWIDGETS_END_NAMESPACE
