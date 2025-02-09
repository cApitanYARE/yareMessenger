cmake_minimum_required(VERSION 3.16)

project(YareMessengerServer LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Вибір бібліотек Qt
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Core Network Sql Gui)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Core Network Sql Gui)

add_executable(YareMessengerServer
  main.cpp
  server.cpp
  server.h

)

# Лінкуємо бібліотеки Qt до проєкту
target_link_libraries(YareMessengerServer
    PRIVATE Qt${QT_VERSION_MAJOR}::Core
    PRIVATE Qt${QT_VERSION_MAJOR}::Network
    PRIVATE Qt${QT_VERSION_MAJOR}::Sql
    PRIVATE Qt${QT_VERSION_MAJOR}::Gui
)
include(GNUInstallDirs)
install(TARGETS YareMessengerServer
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
