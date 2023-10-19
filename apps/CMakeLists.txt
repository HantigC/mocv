set(SUBDIRS
    track_color
    mosaic
    stereo
)
foreach(SUBDIR ${SUBDIRS})
    add_subdirectory(${SUBDIR})
endforeach()
