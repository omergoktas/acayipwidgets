# Copyright (C) 2024 Ömer Göktaş. All Rights Reserved.
# SPDX-License-Identifier: LicenseRef-AcayipWidgets-Commercial OR GPL-3.0-only

find_package(Qt${QT_VERSION_MAJOR}
    REQUIRED
        BodymovinPrivate
)

qt_add_library(lottieio
    OBJECT
        lottieiohandler.h
        lottieiohandler.cpp
        lottierasterrenderer.h
        lottierasterrenderer.cpp
        lottie.json
        main.cpp
)

target_link_libraries(lottieio
    PRIVATE
        Qt::BodymovinPrivate
)

target_compile_definitions(lottieio PRIVATE QT_STATICPLUGIN)
