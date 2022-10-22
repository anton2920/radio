cmake_minimum_required(VERSION 3.18)
project(radio C)

set(CMAKE_C_STANDARD 99)

# Finding GLib
find_package(PkgConfig REQUIRED)
pkg_check_modules(GLIB REQUIRED IMPORTED_TARGET glib-2.0 gmodule-2.0 gio-2.0 libsoup-2.4)

set(RADIO_HTTP  radio_http.c radio_http.h)
set(RADIO_CLI   radio_cli.c radio_cli.h)
set(RADIO_AUDIO radio_audio.c radio_audio.h)

# TODO
# set(RADIO_GUI  radio_gui.c radio_gui.h)

# TODO: conditional build with either CLI or GUI (or maybe both)
add_executable(radio radio_main.c ${RADIO_HTTP} ${RADIO_CLI} ${RADIO_AUDIO})
target_include_directories(radio PUBLIC ${GLIB_INCLUDE_DIRS})
target_link_directories(radio PUBLIC ${GLIB_INCLUDE_DIRS})
target_link_libraries(radio PkgConfig::GLIB pulse pulse-simple)
