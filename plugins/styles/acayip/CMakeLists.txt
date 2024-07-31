# Copyright (C) 2024 Ömer Göktaş. All Rights Reserved.
# SPDX-License-Identifier: LicenseRef-AcayipWidgets-Commercial OR GPL-3.0-only

qt_add_library(AcayipStylePlugin
    OBJECT
        acayipstyle.h
        acayipstyle.cpp
        acayip.json
        main.cpp
)

target_link_libraries(AcayipStylePlugin
    PRIVATE
        Qt::WidgetsPrivate
)

target_compile_definitions(AcayipStylePlugin PRIVATE QT_STATICPLUGIN)
