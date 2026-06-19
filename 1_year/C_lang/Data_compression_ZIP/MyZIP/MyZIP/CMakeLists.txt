cmake_minimum_required(VERSION 3.16)

project(MyZIP LANGUAGES C)

add_executable(MyZIP
    #huffman.h
    build_tree.c
    generate_codes.c
    encode.c
    decode.c
    huffman.c
    main.c)

include(GNUInstallDirs)
install(TARGETS MyZIP
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
