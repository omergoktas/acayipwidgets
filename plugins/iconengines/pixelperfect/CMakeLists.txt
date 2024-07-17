# Copyright (C) 2024 Ömer Göktaş. All Rights Reserved.
# SPDX-License-Identifier: LicenseRef-AcayipWidgets-Commercial OR GPL-3.0-only

qt_add_library(PixelPerfectIconEnginePlugin
    OBJECT
        pixelperfecticonengine.h
        pixelperfecticonengine.cpp
        pixelperfect.json
        main.cpp
)

target_link_libraries(PixelPerfectIconEnginePlugin
    PRIVATE
        Qt::GuiPrivate
)

target_compile_definitions(PixelPerfectIconEnginePlugin PRIVATE QT_STATICPLUGIN)
