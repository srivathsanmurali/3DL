project(stlplus)

FILE(GLOB LIBSLTPLUS_HPP *.hpp )
FILE(GLOB LIBSLTPLUS_CPP *.cpp )

ADD_LIBRARY(libStlplus STATIC ${LIBSLTPLUS_HPP} ${LIBSLTPLUS_CPP})
install (TARGETS "libStlplus"
        DESTINATION "lib/3DL"
)
