// Copyright (C) 2024 Ömer Göktaş. All Rights Reserved.
// SPDX-License-Identifier: LicenseRef-AcayipWidgets-Commercial OR GPL-3.0-only

#include "acayipstyle.h"
#include "acayipstyle_p.h"

#include <QPixmapCache>
#include <QStyleHints>

ACAYIPWIDGETS_BEGIN_NAMESPACE

using namespace Qt::Literals;

/*!
 *  \internal
 */
AcayipStylePrivate::AcayipStylePrivate() {}

/*!
    \class AcayipStyle
    \brief The AcayipStyle is a custom widget style from the Acayip Widgets

    \inmodule AcayipWidgets
    \internal

    The AcayipStyle style provides a custom look and feel that is not
    tied to a particular platform.
    \sa QStyle, QWindowsStyle, QFusionStyle, QCommonStyle
*/

/*!
    Constructs an AcayipStyle object.
*/
AcayipStyle::AcayipStyle()
    : QCommonStyle(*new AcayipStylePrivate)
{
    setObjectName("Acayip"_L1);
    connect(QGuiApplication::styleHints(),
            &QStyleHints::colorSchemeChanged,
            this,
            &AcayipStyle::onColorSchemeChanged);
    onColorSchemeChanged();
}

/*!
    \internal

    Constructs an AcayipStyle object.
*/
AcayipStyle::AcayipStyle(AcayipStylePrivate& dd)
    : QCommonStyle(dd)
{}

QPalette AcayipStyle::standardPalette() const
{
    static const QPalette lightPalette = [this]() {
        QPalette palette(QCommonStyle::standardPalette());
        palette.setColor(QPalette::Active, QPalette::Text, 0x181818);
        palette.setColor(QPalette::Inactive, QPalette::Text, 0x181818);
        palette.setColor(QPalette::Disabled, QPalette::Text, 0x656565);
        palette.setColor(QPalette::Active, QPalette::WindowText, 0x181818);
        palette.setColor(QPalette::Inactive, QPalette::WindowText, 0x181818);
        palette.setColor(QPalette::Disabled, QPalette::WindowText, 0x656565);
        palette.setColor(QPalette::Active, QPalette::ButtonText, 0x181818);
        palette.setColor(QPalette::Inactive, QPalette::ButtonText, 0x181818);
        palette.setColor(QPalette::Disabled, QPalette::ButtonText, 0x656565);
        palette.setColor(QPalette::Active, QPalette::PlaceholderText, 0xbfbfbf);
        palette.setColor(QPalette::Inactive, QPalette::PlaceholderText, 0xbfbfbf);
        palette.setColor(QPalette::Disabled, QPalette::PlaceholderText, 0xbfbfbf);
        palette.setColor(QPalette::Active, QPalette::Highlight, 0x2483ec);
        palette.setColor(QPalette::Inactive, QPalette::Highlight, 0x2483ec);
        palette.setColor(QPalette::Disabled, QPalette::Highlight, 0x727272);
        palette.setColor(QPalette::Base, 0xffffff);
        palette.setColor(QPalette::AlternateBase, 0xf5f5f5);
        palette.setColor(QPalette::Button, 0xf0f0f0);
        palette.setColor(QPalette::Window, 0xf0f0f0);
        palette.setColor(QPalette::BrightText, 0xffffff);
        palette.setColor(QPalette::HighlightedText, 0xffffff);
        palette.setColor(QPalette::ToolTipText, 0x181818);
        palette.setColor(QPalette::ToolTipBase, 0xf0f0f0);
        palette.setColor(QPalette::Link, 0x025dbf);
        palette.setColor(QPalette::LinkVisited, 0xb44b46);
        return palette;
    }();

    static const QPalette darkPalette = [this]() {
        QPalette palette(QCommonStyle::standardPalette());
        palette.setColor(QPalette::Active, QPalette::Text, 0xf0f0f0);
        palette.setColor(QPalette::Inactive, QPalette::Text, 0xf0f0f0);
        palette.setColor(QPalette::Disabled, QPalette::Text, 0x707070);
        palette.setColor(QPalette::Active, QPalette::WindowText, 0xf0f0f0);
        palette.setColor(QPalette::Inactive, QPalette::WindowText, 0xf0f0f0);
        palette.setColor(QPalette::Disabled, QPalette::WindowText, 0x707070);
        palette.setColor(QPalette::Active, QPalette::ButtonText, 0xf0f0f0);
        palette.setColor(QPalette::Inactive, QPalette::ButtonText, 0xf0f0f0);
        palette.setColor(QPalette::Disabled, QPalette::ButtonText, 0x707070);
        palette.setColor(QPalette::Active, QPalette::PlaceholderText, 0x646464);
        palette.setColor(QPalette::Inactive, QPalette::PlaceholderText, 0x646464);
        palette.setColor(QPalette::Disabled, QPalette::PlaceholderText, 0x646464);
        palette.setColor(QPalette::Active, QPalette::Highlight, 0x0078d4);
        palette.setColor(QPalette::Inactive, QPalette::Highlight, 0x0078d4);
        palette.setColor(QPalette::Disabled, QPalette::Highlight, 0x727272);
        palette.setColor(QPalette::Base, 0x282828);
        palette.setColor(QPalette::AlternateBase, 0x2f2f2f);
        palette.setColor(QPalette::Button, 0x3a3a3a);
        palette.setColor(QPalette::Window, 0x3a3a3a);
        palette.setColor(QPalette::BrightText, 0xf2f2f2);
        palette.setColor(QPalette::HighlightedText, 0xf2f2f2);
        palette.setColor(QPalette::ToolTipText, 0xf0f0f0);
        palette.setColor(QPalette::ToolTipBase, 0x3a3a3a);
        palette.setColor(QPalette::Link, 0x025dbf);
        palette.setColor(QPalette::LinkVisited, 0xb44b46);
        return palette;
    }();

    return QGuiApplication::styleHints()->colorScheme() == Qt::ColorScheme::Dark
               ? darkPalette
               : lightPalette;
}

void AcayipStyle::onColorSchemeChanged() const
{
    QGuiApplication::setPalette(standardPalette());
    QPixmapCache::clear(); // Cached QIcon pixmaps make use of the old palette
}

ACAYIPWIDGETS_END_NAMESPACE
