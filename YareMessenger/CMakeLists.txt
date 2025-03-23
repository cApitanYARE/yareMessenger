cmake_minimum_required(VERSION 3.16)

project(YareMessenger VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(ASIO_INCLUDE_DIR "D:/visualStudio/boost_1_87_0/boost_1_87_0")

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Core Network Sql Multimedia)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Core Network Sql Multimedia)


set(PROJECT_SOURCES
    main.cpp
    messengerwin.cpp
    messengerwin.h
    messengerwin.ui
    signin.h signin.cpp signin.ui
    login.h login.cpp login.ui

)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(YareMessenger
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}


        signin.h signin.cpp signin.ui
        login.h login.cpp login.ui



    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET YareMessenger APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(YareMessenger SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(YareMessenger
            ${PROJECT_SOURCES}
        )
    endif()
endif()


target_link_libraries(YareMessenger
    PRIVATE Qt${QT_VERSION_MAJOR}::Widgets
    PRIVATE Qt${QT_VERSION_MAJOR}::Core
    PRIVATE Qt${QT_VERSION_MAJOR}::Network
    PRIVATE Qt${QT_VERSION_MAJOR}::Sql
    PRIVATE Qt${QT_VERSION_MAJOR}::Multimedia
)

target_include_directories(YareMessenger PRIVATE ${ASIO_INCLUDE_DIR})


# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.YareMessenger)
endif()
set_target_properties(YareMessenger PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)



include(GNUInstallDirs)
install(TARGETS YareMessenger
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(YareMessenger)
endif()
