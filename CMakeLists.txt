CMAKE_MINIMUM_REQUIRED(VERSION 3.10)

# For Apple Silicon:
# https://stackoverflow.com/questions/64923977/mac-os-with-m1-encounters-with-an-error-when-using-glfw-glew
# cmake -DCMAKE_APPLE_SILICON_PROCESSOR=arm64 ..

# Name of the project
PROJECT(Barnes-Hut-N-Body)

# Use glob to get the list of all source files.
FILE(GLOB_RECURSE SOURCES "src/*.cpp" "src/*.hpp")

# We don't really need to include header and resource files to build, but it's
# nice to have them show up in IDEs.
FILE(GLOB_RECURSE HEADERS "src/*.h")
FILE(GLOB_RECURSE GLSL "resources/*.glsl")

# Set the executable.
ADD_EXECUTABLE(${CMAKE_PROJECT_NAME} ${SOURCES} ${HEADERS} ${GLSL})

# Get the GLM environment variable. Since GLM is a header-only library, we
# just need to add it to the include directory.
SET(GLM_INCLUDE_DIR "$ENV{GLM_INCLUDE_DIR}")
IF(NOT GLM_INCLUDE_DIR)
	# The environment variable was not set
	SET(ERR_MSG "Please point the environment variable GLM_INCLUDE_DIR to the root directory of your GLM installation.")
	IF(WIN32)
		# On Windows, try the default location
		MESSAGE(STATUS "Looking for GLM in ${DEF_DIR_GLM}")
		IF(IS_DIRECTORY ${DEF_DIR_GLM})
			MESSAGE(STATUS "Found!")
			SET(GLM_INCLUDE_DIR ${DEF_DIR_GLM})
		ELSE()
			MESSAGE(FATAL_ERROR ${ERR_MSG})
		ENDIF()
	ELSE()
		MESSAGE(FATAL_ERROR ${ERR_MSG})
	ENDIF()
ENDIF()
INCLUDE_DIRECTORIES(${GLM_INCLUDE_DIR})

SET(EIGEN_DIR $ENV{THIRDPARTY_DIR}/eigen-3.4.0)
include_directories(${EIGEN_DIR})

SET(RAPIDXML_DIR $ENV{THIRDPARTY_DIR}/rapidxml-1.13)
include_directories(${RAPIDXML_DIR})


SET(RAPIDXML_DIR $ENV{THIRDPARTY_DIR}/rapidxml-1.13)
include_directories(${RAPIDXML_DIR})


# TODO I added this 
SET(CUDART_LIBRARY $ENV{THIRDPARTY_DIR}/cuda-12.1)
INCLUDE_DIRECTORIES(${CUDART_LIBRARY}/include)


SET(FREETYPE_DIR $ENV{THIRDPARTY_DIR}/freetype-2.12.1)
IF(NOT FREETYPE_DIR)
	# The environment variable was not set
	SET(ERR_MSG "Please point the environment variable THIRDPARTY_DIR to the root directory of your freetype-2.12.1 installation.")
	MESSAGE(FATAL_ERROR ${ERR_MSG})
ENDIF()
IF(CMAKE_BUILD_TYPE MATCHES Release)
	ADD_SUBDIRECTORY(${FREETYPE_DIR} ${FREETYPE_DIR}/release)
    TARGET_LINK_LIBRARIES(${CMAKE_PROJECT_NAME} freetype ${FREETYPE_DIR}/release/Release/freetype.lib)
ELSE()
	ADD_SUBDIRECTORY(${FREETYPE_DIR} ${FREETYPE_DIR}/debug)
TARGET_LINK_LIBRARIES(${CMAKE_PROJECT_NAME} freetype ${FREETYPE_DIR}/debug/Debug/freetyped.lib)

ENDIF()
INCLUDE_DIRECTORIES(${FREETYPE_DIR}/include)



# Get the GLFW environment variable. There should be a CMakeLists.txt in the 
# specified directory.
SET(GLFW_DIR $ENV{THIRDPARTY_DIR}/glfw-3.3.8)
IF(NOT GLFW_DIR)
	# The environment variable was not set
	SET(ERR_MSG "Please point the environment variable GLFW_DIR to the root directory of your GLFW installation.")
	IF(WIN32)
		# On Windows, try the default location
		MESSAGE(STATUS "Looking for GLFW in ${DEF_DIR_GLFW}")
		IF(IS_DIRECTORY ${DEF_DIR_GLFW})
			MESSAGE(STATUS "Found!")
			SET(GLFW_DIR ${DEF_DIR_GLFW})
		ELSE()
			MESSAGE(FATAL_ERROR ${ERR_MSG})
		ENDIF()
	ELSE()
		MESSAGE(FATAL_ERROR ${ERR_MSG})
	ENDIF()
ENDIF()
OPTION(GLFW_BUILD_EXAMPLES "GLFW_BUILD_EXAMPLES" OFF)
OPTION(GLFW_BUILD_TESTS "GLFW_BUILD_TESTS" OFF)
OPTION(GLFW_BUILD_DOCS "GLFW_BUILD_DOCS" OFF)
IF(CMAKE_BUILD_TYPE MATCHES Release)
	ADD_SUBDIRECTORY(${GLFW_DIR} ${GLFW_DIR}/release)
ELSE()
	ADD_SUBDIRECTORY(${GLFW_DIR} ${GLFW_DIR}/debug)
ENDIF()
INCLUDE_DIRECTORIES(${GLFW_DIR}/include)
TARGET_LINK_LIBRARIES(${CMAKE_PROJECT_NAME} glfw ${GLFW_LIBRARIES})

# Get the GLEW environment variable.
SET(GLEW_DIR $ENV{THIRDPARTY_DIR}/glew-2.1.0)
IF(NOT GLEW_DIR)
	# The environment variable was not set
	SET(ERR_MSG "Please point the environment variable GLEW_DIR to the root directory of your GLEW installation.")
	IF(WIN32)
		# On Windows, try the default location
		MESSAGE(STATUS "Looking for GLEW in ${DEF_DIR_GLEW}")
		IF(IS_DIRECTORY ${DEF_DIR_GLEW})
			MESSAGE(STATUS "Found!")
			SET(GLEW_DIR ${DEF_DIR_GLEW})
		ELSE()
			MESSAGE(FATAL_ERROR ${ERR_MSG})
		ENDIF()
	ELSE()
		MESSAGE(FATAL_ERROR ${ERR_MSG})
	ENDIF()
ENDIF()
INCLUDE_DIRECTORIES(${GLEW_DIR}/include)
IF(WIN32)
	# With prebuilt binaries
	# Check for 32 vs 64 bit generator
	IF(NOT CMAKE_CL_64)
		MESSAGE(STATUS "Using 32Bit")
		TARGET_LINK_LIBRARIES(${CMAKE_PROJECT_NAME} 
		${GLEW_DIR}/lib/Release/Win32/glew32s.lib
)
	ELSE()
		MESSAGE(STATUS "Using 64Bit")
		TARGET_LINK_LIBRARIES(${CMAKE_PROJECT_NAME} 
		${GLEW_DIR}/lib/Release/x64/glew32s.lib
)
	ENDIF()
ELSE()
	TARGET_LINK_LIBRARIES(${CMAKE_PROJECT_NAME} ${GLEW_DIR}/lib/libGLEW.a)
ENDIF()

# Use c++17
SET_TARGET_PROPERTIES(${CMAKE_PROJECT_NAME} PROPERTIES CXX_STANDARD 17)

# OS specific options and libraries
IF(WIN32)
	# -Wall produces way too many warnings.
	# -pedantic is not supported.
	# Disable warning 4996.
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4996")
	TARGET_LINK_LIBRARIES(${CMAKE_PROJECT_NAME} opengl32.lib)

	SET_PROPERTY(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${CMAKE_PROJECT_NAME})
	SET_TARGET_PROPERTIES(${CMAKE_PROJECT_NAME} PROPERTIES VS_DEBUGGER_COMMAND_ARGUMENTS "../resources")
ELSE()
	# Enable all pedantic warnings.
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic")
	IF(APPLE)
		# Add required frameworks for GLFW.
		TARGET_LINK_LIBRARIES(${CMAKE_PROJECT_NAME} "-framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo")
	ELSE()
		#Link the Linux OpenGL library
		TARGET_LINK_LIBRARIES(${CMAKE_PROJECT_NAME} "GL")
	ENDIF()
ENDIF()