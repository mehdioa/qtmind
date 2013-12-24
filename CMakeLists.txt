# ------------------------------------------------------------------------------
#  QTMIND CMAKE FILE
# ------------------------------------------------------------------------------
cmake_minimum_required(VERSION 2.8.6)
#SET( CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake )

PROJECT( qtmind )
SET(QTMIND_MAJOR_VERSION 0)
SET(QTMIND_MINOR_VERSION 6)
SET(QTMIND_PATCH_VERSION 1)
SET(QTMIND_VERSION ${QTMIND_MAJOR_VERSION}.${QTMIND_MINOR_VERSION}.${QTMIND_PATCH_VERSION})

# ------------------------------------------------------------------------------
#  build def
# ------------------------------------------------------------------------------
#ADD_DEFINITIONS( -DTEST_FLAG )
ADD_DEFINITIONS(-Wall -Wextra )


# ------------------------------------------------------------------------------
#  EXTERNAL PACKAGE
# ------------------------------------------------------------------------------
FIND_PACKAGE( Qt4 4.7.0 REQUIRED QtCore QtGui)

# ------------------------------------------------------------------------------
#  INCLUDES
# ------------------------------------------------------------------------------
INCLUDE(${QT_USE_FILE})

INCLUDE_DIRECTORIES(
	${CMAKE_CURRENT_BINARY_DIR}
	${QT_INCLUDES}
)

# ------------------------------------------------------------------------------
#  SUBDIRECTORIES
# ------------------------------------------------------------------------------
# ------------------------------------------------------------------------------
#  TRANSLATIONS
# ------------------------------------------------------------------------------
# Handle Translations, take all * files from trans directory.
FILE( GLOB TRANS_FILES ${CMAKE_SOURCE_DIR}/translation/qtmind_*.ts)
SET(TS_FILES ${TRANS_FILES})

SET(UPDATE_TRANSLATIONS OFF) # no ts update
#SET(UPDATE_TRANSLATIONS ON) # ts update

# ------------------------------------------------------------------------------
#  SOURCES
# ------------------------------------------------------------------------------
ADD_SUBDIRECTORY(src)

# ------------------------------------------------------------------------------
#  INSTALLATION
# ------------------------------------------------------------------------------
# -- install binary
# install( TARGETS qtmind DESTINATION /usr/local/bin/ )

# -- install destop file
INSTALL(FILES qtmind.desktop  DESTINATION /usr/share/applications/)

# -- install icons files
INSTALL(FILES icons/hicolor/16x16/qtmind.png DESTINATION share/icons/hicolor/16x16/apps/ RENAME application-x-qtmind.png)
INSTALL(FILES icons/hicolor/32x32/qtmind.png DESTINATION share/icons/hicolor/32x32/apps/ RENAME application-x-qtmind.png)
INSTALL(FILES icons/hicolor/48x48/qtmind.png DESTINATION share/icons/hicolor/48x48/apps/ RENAME application-x-qtmind.png)
INSTALL(FILES icons/hicolor/64x64/qtmind.png DESTINATION share/icons/hicolor/64x64/apps/ RENAME application-x-qtmind.png)
INSTALL(FILES icons/hicolor/128x128/qtmind.png DESTINATION share/icons/hicolor/128x128/apps/ RENAME application-x-qtmind.png)
INSTALL(FILES icons/hicolor/256x256/qtmind.png DESTINATION share/icons/hicolor/256x256/apps/ RENAME application-x-qtmind.png)
INSTALL(FILES icons/hicolor/512x512/qtmind.png DESTINATION share/icons/hicolor/512x512/apps/ RENAME application-x-qtmind.png)


# ------------------------------------------------------------------------------
#  UN-INSTALL SUPPORT
# ------------------------------------------------------------------------------

 