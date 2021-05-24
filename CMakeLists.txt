cmake_minimum_required(VERSION 3.16)

project(deepstream_video_player VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FindPkgConfig)
if(!${PKG_CONFIG_FOUND})
  MESSAGE(FATAL_ERROR "pkg-config required. Please install it")
  return ("Error - pkg-config not found")
endif()

pkg_check_modules(GSTREAMER gstreamer-1.0>=1.6 gstreamer-video-1.0>=1.6 REQUIRED)

# QtCreator supports the following variables for Android, which are identical to qmake Android variables.
# Check https://doc.qt.io/qt/deployment-android.html for more information.
# They need to be set before the find_package(...) calls below.

#if(ANDROID)
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
#    if (ANDROID_ABI STREQUAL "armeabi-v7a")
#        set(ANDROID_EXTRA_LIBS
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libcrypto.so
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libssl.so)
#    endif()
#endif()

find_package(QT NAMES Qt6 Qt5 COMPONENTS Core Quick QuickControls2 LinguistTools REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Quick QuickControls2 LinguistTools REQUIRED)

set(TS_FILES deepstream_video_player_zh_CN.ts)

set(PROJECT_SOURCES
        main.cpp
        player.cpp
        player.h
        qml.qrc
        ${TS_FILES}
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(deepstream_video_player
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )

    qt_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
else()
    if(ANDROID)
        add_library(deepstream_video_player SHARED
            ${PROJECT_SOURCES}
        )
    else()
        add_executable(deepstream_video_player
          ${PROJECT_SOURCES}
        )
    endif()

    qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
endif()


set(DEEPSTREAM_DIR "/opt/nvidia/deepstream/deepstream-5.1")
set(DEEPSTREAM_INCLUDE_DIRS "${DEEPSTREAM_DIR}/sources/includes")
set(DEEPSTREAM_LINK_DIRS "${DEEPSTREAM_DIR}/lib" "${DEEPSTREAM_DIR}/lib/gst-plugins")
set(DEEPSTREAM_LIBRARYS "nvdsgst_meta" "nvds_meta")

target_link_directories(deepstream_video_player PRIVATE ${DEEPSTREAM_LINK_DIRS})
target_include_directories(deepstream_video_player PRIVATE ${GSTREAMER_INCLUDE_DIRS} ${DEEPSTREAM_INCLUDE_DIRS})
target_link_directories(deepstream_video_player PRIVATE ${GSTREAMER_LIBRARY_DIRS} ${DEEPSTREAM_INCLUDE_DIRS})

target_compile_definitions(deepstream_video_player
  PRIVATE $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>)

target_link_libraries(deepstream_video_player
    PRIVATE
        Qt${QT_VERSION_MAJOR}::Core
        Qt${QT_VERSION_MAJOR}::Quick
        Qt${QT_VERSION_MAJOR}::QuickControls2
        ${GSTREAMER_LIBRARIES}
        ${DEEPSTREAM_LIBRARYS}
)

set_target_properties(deepstream_video_player PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_import_qml_plugins(deepstream_video_player)
    qt_finalize_executable(deepstream_video_player)
endif()
