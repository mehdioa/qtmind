
SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -Woverloaded-virtual -Wall -Wno-sign-compare")

OPTION(BUILD_WERROR "Build with -Werror" ON)

IF(BUILD_WERROR)
  IF (LINUX)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror")
  ENDIF (LINUX)
ENDIF(BUILD_WERROR)

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR})

INCLUDE_DIRECTORIES(${CMAKE_BINARY_DIR})

SET (QTMIND_SOURCES 
	button.cpp
	emptybox.cpp
	game.cpp
	main.cpp
	mainwindow.cpp
	message.cpp
	peg.cpp
	pegbox.cpp
	pin.cpp
	pinbox.cpp
	preferences.cpp
	solver.cpp
)

SET(QTMIND_HEADERS 
	button.h
	constants.h
	emptybox.h
	game.h
	mainwindow.h
	message.h
	peg.h
	pegbox.h
	pin.h
	pinbox.h
	preferences.h
	solver.h
)

SET(QTMIND_FORMS
	mainwindow.ui
	preferences.ui
)

SET(QTMIND_RESOURCES
	../resource.qrc
)

QT4_WRAP_CPP(QTMIND_HEADERS_MOC ${QTMIND_HEADERS})
QT4_WRAP_UI(QTMIND_FORMS_HEADERS ${QTMIND_FORMS})
QT4_ADD_RESOURCES(QTMIND_RESOURCES_RCC ${QTMIND_RESOURCES})


# ------------------------------------------------------------------------------
#  TRANSLATION
# ------------------------------------------------------------------------------
OPTION (UPDATE_TRANSLATIONS "Update source translation ../translation/*.ts files")
SET (FILES_TO_TRANSLATE ${FILES_TO_TRANSLATE} ${QTMIND_SOURCES})

IF (UPDATE_TRANSLATIONS)
  QT4_CREATE_TRANSLATION(QTMIND_QM_FILES ${FILES_TO_TRANSLATE} ${TS_FILES})
ELSE (UPDATE_TRANSLATIONS)
  QT4_ADD_TRANSLATION(QTMIND_QM_FILES ${TS_FILES})
ENDIF (UPDATE_TRANSLATIONS)

# # ------------------------------------------------------------------------------
# #  BUILDING
# # ------------------------------------------------------------------------------
ADD_EXECUTABLE(qtmind 
	${QT_DEFINITIONS}
	${QTMIND_SOURCES}
	${QTMIND_FORMS_HEADERS}
	${QTMIND_RESOURCES_RCC}
	${QTMIND_QM_FILES}
)

SET_target_properties(qtmind PROPERTIES AUTOMOC TRUE)

#to include translation file in binary
SET(EXECUTABLE_OUTPUT_PATH ..)

TARGET_LINK_LIBRARIES(qtmind
  X11
  ${QT_LIBRARIES}
)

# ------------------------------------------------------------------------------
#  INSTALL
# ------------------------------------------------------------------------------
# -- install binary file
INSTALL( TARGETS qtmind DESTINATION /usr/local/bin/ )

# -- install translations files
INSTALL(FILES ${QTMIND_QM_FILES} DESTINATION /usr/share/qtmind/)
  