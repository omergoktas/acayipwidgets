# Copyright (C) 2024 Ömer Göktaş. All Rights Reserved.
# SPDX-License-Identifier: LicenseRef-AcayipWidgets-Commercial OR GPL-3.0-only

qt_add_library(acayipstyle
    OBJECT
        acayipstyle.h
        acayipstyle.cpp
        acayip.json
        main.cpp
)

target_link_libraries(acayipstyle
    PRIVATE
        Qt::Widgets
)

target_compile_definitions(acayipstyle PRIVATE QT_STATICPLUGIN)
