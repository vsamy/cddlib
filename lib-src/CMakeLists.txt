set(SOURCES cddcore.c cddio.c cddlib.c cddlp.c cddmp.c cddproj.c setoper.c)
set(HEADERS cdd.h cddmp.h cddtypes.h setoper.h splitmix64.h)

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

add_library(${PROJECT_NAME} SHARED ${SOURCES} ${HEADERS})
target_include_directories(${PROJECT_NAME} PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}> $<INSTALL_INTERFACE:include>)
if (${USE_GMP})
    target_compile_definitions(${PROJECT_NAME} PUBLIC GMPRATIONAL)
endif()

install(
    TARGETS ${PROJECT_NAME}
    EXPORT "${TARGETS_EXPORT_NAME}"
    LIBRARY  DESTINATION lib
    ARCHIVE  DESTINATION lib
    RUNTIME  DESTINATION bin
)
install(FILES ${HEADERS} DESTINATION include/${PROJECT_NAME})