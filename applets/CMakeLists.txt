if(NOT WIN32)
    # uses the command line tool lliurex-onedrive, does not make sense on Windows
    add_subdirectory(lliurexonedrive)
endif(NOT WIN32)
