set(SUBDIRS
    track_color
    mosaic
)
foreach(SUBDIR ${SUBDIRS})
    add_subdirectory(${SUBDIR})
endforeach()
