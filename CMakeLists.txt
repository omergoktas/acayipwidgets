# Copyright (C) 2024 Ömer Göktaş. All Rights Reserved.

find_package(Qt${QT_VERSION_MAJOR}
    REQUIRED
        Widgets
        Svg
)

qt_add_library(acayipwidgets
    button.h
    button.cpp
    boxlayout.h
    boxlayout.cpp
    acayipstyle.h
    acayipstyle.cpp
)

target_sources(core PRIVATE
    button_p.h
    boxlayout_p.h
    acayipstyle_p.h
)

target_include_directories(acayipwidgets
    PUBLIC
        ${CMAKE_CURRENT_BINARY_DIR} # acayipwidgets_export.h
    INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(acayipwidgets
    PUBLIC
        Qt::WidgetsPrivate
    PRIVATE
        Qt::Svg
)

generate_export_header(acayipwidgets)
