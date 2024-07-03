# Copyright (C) 2024 Ömer Göktaş. All Rights Reserved.
# SPDX-License-Identifier: LicenseRef-AcayipWidgets-Commercial OR GPL-3.0-only

cmake_minimum_required(VERSION 3.27)

project(AcayipWidgets
    VERSION 0.1.0
    HOMEPAGE_URL omergoktas.com
    DESCRIPTION "The Acayip Widgets Project."
)

find_package(Qt${QT_VERSION_MAJOR}
    REQUIRED
        Widgets
        Svg
)

qt_add_library(acayipwidgets
    acayipglobal.h
    boxlayout.h
    boxlayout.cpp
    pushbutton.h
    pushbutton.cpp
)

target_sources(acayipwidgets PRIVATE
    acayipstyle.h
    acayipstyle.cpp
    acayipstyleplugin.h
    acayipstyleplugin.cpp
    acayipstyle.json
    boxlayout_p.h
    acayipstyle_p.h
    pushbutton_p.h
)

target_include_directories(acayipwidgets
    PUBLIC
        ${CMAKE_CURRENT_BINARY_DIR} # acayipwidgets_export.h, acayipconfig.h
    INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(acayipwidgets
    PUBLIC
        Qt::WidgetsPrivate
    PRIVATE
        Qt::Svg
)

target_compile_definitions(acayipwidgets PRIVATE QT_STATICPLUGIN)

set(ACAYIPWIDGETS_NAMESPACE "Acayip" CACHE STRING "Acayip Widgets namespace")
if(ACAYIPWIDGETS_NAMESPACE STREQUAL "")
    set(ACAYIPWIDGETS_NAMESPACE_ENABLED 0)
else()
    set(ACAYIPWIDGETS_NAMESPACE_ENABLED 1)
endif()
configure_file(acayipconfig.h.in ${CMAKE_CURRENT_BINARY_DIR}/acayipconfig.h @ONLY)

generate_export_header(acayipwidgets)
