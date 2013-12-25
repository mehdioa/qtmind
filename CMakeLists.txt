# ------------------------------------------------------------------------------
#  QTMIND CMAKE FILE
# ------------------------------------------------------------------------------
cmake_minimum_required(VERSION 2.8.6)

project(qtmind)
set(QTMIND_MAJOR_VERSION 0)
set(QTMIND_MINOR_VERSION 6)
set(QTMIND_PATCH_VERSION 1)
set(QTMIND_VERSION ${QTMIND_MAJOR_VERSION}.${QTMIND_MINOR_VERSION}.${QTMIND_PATCH_VERSION})

# ------------------------------------------------------------------------------
#  build def
# ------------------------------------------------------------------------------
#ADD_DEFINITIONS( -DTEST_FLAG )
#ADD_DEFINITIONS(-Wall -Wextra )


# ------------------------------------------------------------------------------
#  EXTERNAL PACKAGE
# ------------------------------------------------------------------------------
find_package( Qt4 4.7.0 REQUIRED QtCore QtGui)
#FIND_PACKAGE(Qt5Widgets)
# ------------------------------------------------------------------------------
#  includeS
# ------------------------------------------------------------------------------
include(${QT_USE_FILE})
include(CheckCXXCompilerFlag)
include(FindPkgConfig)
include(cmake/Deb.cmake)
include(cmake/Rpm.cmake)
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

if (CMAKE_CXX_COMPILER MATCHES ".*clang")
  set(CMAKE_COMPILER_IS_CLANGXX 1)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-uninitialized")
endif ()

set(CMAKE_REQUIRED_FLAGS "-std=c++0x")

include_directories(
	${CMAKE_CURRENT_BINARY_DIR}
	${QT_INCLUDES}
)

file( GLOB TRANS_FILES ${CMAKE_SOURCE_DIR}/translation/qtmind_*.ts)
set(TS_FILES ${TRANS_FILES})

set(UPDATE_TRANSLATIONS OFF) # no ts update
#set(UPDATE_TRANSLATIONS ON) # ts update

# ------------------------------------------------------------------------------
#  SOURCES
# ------------------------------------------------------------------------------
add_subdirectory(src)

# ------------------------------------------------------------------------------
#  INSTALLATION
# ------------------------------------------------------------------------------
# -- install binary
#install( TARGETS qtmind DESTINATION /usr/bin/ )

# -- install destop file
install(FILES qtmind.desktop  DESTINATION /usr/share/applications/)

# -- install icons files
install(FILES icons/hicolor/16x16/qtmind.png DESTINATION share/icons/hicolor/16x16/apps/ RENAME application-x-qtmind.png)
install(FILES icons/hicolor/32x32/qtmind.png DESTINATION share/icons/hicolor/32x32/apps/ RENAME application-x-qtmind.png)
install(FILES icons/hicolor/48x48/qtmind.png DESTINATION share/icons/hicolor/48x48/apps/ RENAME application-x-qtmind.png)
install(FILES icons/hicolor/64x64/qtmind.png DESTINATION share/icons/hicolor/64x64/apps/ RENAME application-x-qtmind.png)
install(FILES icons/hicolor/128x128/qtmind.png DESTINATION share/icons/hicolor/128x128/apps/ RENAME application-x-qtmind.png)
install(FILES icons/hicolor/256x256/qtmind.png DESTINATION share/icons/hicolor/256x256/apps/ RENAME application-x-qtmind.png)
install(FILES icons/hicolor/512x512/qtmind.png DESTINATION share/icons/hicolor/512x512/apps/ RENAME application-x-qtmind.png)


# ------------------------------------------------------------------------------
#  UN-INSTALL SUPPORT
# ------------------------------------------------------------------------------

