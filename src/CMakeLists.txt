
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")

#set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}  -std=c++0x")
option(BUILD_WERROR "Build with -Werror" ON)

if(BUILD_WERROR)
  if (LINUX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror")
  endif (LINUX)
endif(BUILD_WERROR)


 include_directories(
# 	${CMAKE_SOURCE_DIR} 
 	${CMAKE_CURRENT_BINARY_DIR} 
# 	${CMAKE_CURRENT_SOURCE_DIR}
 )

set (QTMIND_SOURCES 
	boardaid.cpp
	button.cpp
	emptybox.cpp
	game.cpp
	gamerules.cpp
	gamesounds.cpp
	guesselement.cpp
	indicator.cpp
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

# set(QTMIND_HEADERS 
# 	button.h
# 	constants.h
# 	emptybox.h
# 	game.h
# 	mainwindow.h
# 	message.h
# 	peg.h
# 	pegbox.h
# 	pin.h
# 	pinbox.h
# 	preferences.h
# 	solver.h
# )

set(QTMIND_UI
	mainwindow.ui
	preferences.ui
)

set(QTMIND_RESOURCES
	../resource.qrc
)

#qt4_wrap_cpp(QTMIND_HEADERS_MOC ${QTMIND_HEADERS})
qt4_wrap_ui(QTMIND_UI_HEADERS ${QTMIND_UI})
qt4_add_resources(QTMIND_RESOURCES_RCC ${QTMIND_RESOURCES})


# ------------------------------------------------------------------------------
#  TRANSLATION
# ------------------------------------------------------------------------------
# option (UPDATE_TRANSLATIONS "Update source translation ../translations/*.ts files")
# set (FILES_TO_TRANSLATE ${FILES_TO_TRANSLATE} ${QTMIND_SOURCES})
# 
# if (UPDATE_TRANSLATIONS)
#   QT4_CREATE_TRANSLATION(QTMIND_QM_FILES ${FILES_TO_TRANSLATE} ${TS_FILES})
# else (UPDATE_TRANSLATIONS)
#   QT4_ADD_TRANSLATION(QTMIND_QM_FILES ${TS_FILES})
# endif (UPDATE_TRANSLATIONS)

# # ------------------------------------------------------------------------------
# #  BUILDING
# # ------------------------------------------------------------------------------
add_executable(qtmind 
#	${QT_DEFINITIONS}
	${QTMIND_SOURCES}
	${QTMIND_UI_HEADERS}
	${QTMIND_RESOURCES_RCC}
#	${QTMIND_QM}
)

#set_target_properties(qtmind PROPERTIES AUTOMOC TRUE)

#to include translation file in binary
set(EXECUTABLE_OUTPUT_PATH ..)

target_link_libraries(qtmind ${QT_LIBRARIES})

# ------------------------------------------------------------------------------
#  INSTALL
# ------------------------------------------------------------------------------
# -- install binary file
install( TARGETS qtmind DESTINATION /usr/bin/ )

# -- install translations files
#install(FILES ${QTMIND_QM} DESTINATION /usr/share/qtmind/)
  