cmake_minimum_required(VERSION 2.8)
project(ltest)

set (CMAKE_C_FLAGS "-std=c99 -Wall -Wextra -D_POSIX_C_SOURCE=2")

set_source_files_properties(tags PROPERTIES GENERATED true)

add_custom_target(tags
        COMMAND ctags -R .
	        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

include_directories (${CMAKE_SOURCE_DIR}/inc)

add_executable (ltest
		src/armor.c
		src/cpairs.c
		src/entity.c
		src/entity_name.c
		src/entity_types.c
		src/fight.c
		src/globals.c
		src/item.c
		src/location.c
		src/main.c
		src/map_actions.c
		src/map_ai.c
		src/map_fov.c
		src/map_path.c
		src/map_ui.c
		src/mapgen.c
		src/mapmode.c
		src/pqueue.c
		src/random.c
		src/ui.c
		src/ui_windows.c
		src/weapon.c
		src/utf8.c
	       )

add_library(libutf8proc STATIC IMPORTED)
SET_TARGET_PROPERTIES(libutf8proc PROPERTIES
	IMPORTED_LOCATION utf8proc/libutf8proc.a)

include_directories (${CMAKE_SOURCE_DIR}/utf8proc)

find_package(PkgConfig)
pkg_check_modules(NCURSESW "ncursesw" REQUIRED)

include_directories(${NCURSESW_INCLUDE_DIRS})

target_link_libraries(ltest ncursesw menu curl pthread libutf8proc m)

install(TARGETS ltest
	RUNTIME DESTINATION bin
	)
