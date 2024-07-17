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

qt_add_library(AcayipWidgets
    acayipglobal.h
    boxlayout.h
    boxlayout.cpp
    button.h
    button.cpp
)

target_sources(AcayipWidgets PRIVATE
    utils_p.h
    utils_p.cpp
    boxlayout_p.h
    button_p.h
)

add_subdirectory(plugins)

file(GLOB_RECURSE resources
    CONFIGURE_DEPENDS RELATIVE
    ${CMAKE_CURRENT_SOURCE_DIR} assets/*
)

qt_add_resources(AcayipWidgets acayipwidgets_assets
    PREFIX /acayipwidgets
    FILES ${resources}
)

target_include_directories(AcayipWidgets
    PUBLIC
        ${CMAKE_CURRENT_BINARY_DIR} # acayipwidgets_export.h, acayipconfig.h
    INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR} # acayipglobal.h
)

target_link_libraries(AcayipWidgets
    PUBLIC
        Qt::Widgets
    PRIVATE
        Qt::Svg
        Qt::WidgetsPrivate
        AcayipStylePlugin
        LottieIOPlugin
        PixelPerfectIconEnginePlugin
)

set(ACAYIPWIDGETS_NAMESPACE "Acayip" CACHE STRING "Acayip Widgets namespace")
if(ACAYIPWIDGETS_NAMESPACE STREQUAL "")
    set(ACAYIPWIDGETS_NAMESPACE_ENABLED 0)
else()
    set(ACAYIPWIDGETS_NAMESPACE_ENABLED 1)
endif()
configure_file(acayipconfig.h.in ${CMAKE_CURRENT_BINARY_DIR}/acayipconfig.h @ONLY)

generate_export_header(AcayipWidgets)
