#add data
macro(add_files_macro FPATH FGROUP)
    file(GLOB PROJECT_AHHEADERS ${FPATH}/*.h  ${FPATH}/*.hpp)
    set(PROJECT_HHEADERS ${PROJECT_HHEADERS} ${PROJECT_AHHEADERS})
    source_group("Header Files\\${FGROUP}" FILES ${PROJECT_AHHEADERS})
    file(GLOB PROJECT_ACSOURCES ${FPATH}/*.cpp ${FPATH}/*.cc ${FPATH}/*.c)
    set(PROJECT_CSOURCES ${PROJECT_CSOURCES} ${PROJECT_ACSOURCES})
    source_group("Source Files\\${FGROUP}" FILES ${PROJECT_ACSOURCES})
endmacro()


