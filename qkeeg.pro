#-------------------------------------------------
#
# Project created by QtCreator 2017-11-05T17:52:52
#
#-------------------------------------------------

QT       -= gui

TARGET = qkeeg
TEMPLATE = lib
CONFIG += staticlib c++14

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS CRC32_SLICING_BY_8

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INTERMEDIATE = $${OUT_PWD}/GeneratedFiles/$${BUILD}
MOC_DIR = $${INTERMEDIATE}/.moc
OBJECTS_DIR = $${INTERMEDIATE}/.obj

###############################
## PROJECT SCOPES
###############################

INCLUDEPATH += src

SOURCES += \
    src/io/binaryreader.cpp \
    src/io/binarywriter.cpp \
    src/hashing/hashalgorithm.cpp \
    src/hashing/crc/crc32.cpp \
    src/hashing/crc/crc64.cpp \
    src/hashing/checksum/adler32.cpp \
    src/hashing/checksum/fletcher32.cpp \
    src/hashing/noncryptographic/aphash32.cpp \
    src/hashing/noncryptographic/bkdrhash32.cpp \
    src/hashing/noncryptographic/djb2hash32.cpp \
    src/hashing/noncryptographic/elfhash32.cpp \
    src/hashing/noncryptographic/fnv1hash32.cpp \
    src/hashing/noncryptographic/fnv1hash64.cpp \
    src/hashing/noncryptographic/fnv1ahash32.cpp \
    src/hashing/noncryptographic/fnv1ahash64.cpp \
    src/hashing/noncryptographic/joaathash32.cpp \
    src/hashing/noncryptographic/jshash32.cpp \
    src/hashing/noncryptographic/pjwhash32.cpp \
    src/hashing/noncryptographic/saxhash32.cpp

HEADERS += \
    src/common/stringutils.hpp \
    src/common/macrohelpers.hpp \
    src/common/enums.hpp \
    src/common/endian.hpp \
    src/common/intrinsic.hpp \
    src/io/binaryreader.hpp \
    src/io/binarywriter.hpp \
    src/hashing/hashalgorithm.hpp \
    src/common/cryptotransform.hpp \
    src/hashing/crc/crc32.hpp \
    src/hashing/crc/crc64.hpp \
    src/hashing/checksum/adler32.hpp \
    src/hashing/checksum/fletcher32.hpp \
    src/hashing/noncryptographic/aphash32.hpp \
    src/hashing/noncryptographic/bkdrhash32.hpp \
    src/hashing/noncryptographic/djb2hash32.hpp \
    src/hashing/noncryptographic/elfhash32.hpp \
    src/hashing/noncryptographic/fnv1hash32.hpp \
    src/hashing/noncryptographic/fnv1hash64.hpp \
    src/hashing/noncryptographic/fnv1ahash32.hpp \
    src/hashing/noncryptographic/fnv1ahash64.hpp \
    src/hashing/noncryptographic/joaathash32.hpp \
    src/hashing/noncryptographic/jshash32.hpp \
    src/hashing/noncryptographic/pjwhash32.hpp \
    src/hashing/noncryptographic/saxhash32.hpp

unix {
    target.path = /usr/lib
    INSTALLS += target
}

###############################
## COMPILER SCOPES
###############################

*msvc* {
        # So VCProj Filters do not flatten headers/source
        CONFIG -= flat

        # COMPILER FLAGS

        #  Optimization flags
        #QMAKE_CXXFLAGS_RELEASE -= /O2
        QMAKE_CXXFLAGS_RELEASE *= /O2 /Ot /Ox #/GL
        #  Multithreaded compiling for Visual Studio
        QMAKE_CXXFLAGS += -MP
        # Linker flags
        QMAKE_LFLAGS_RELEASE += /LTCG
}

*-g++ {

        # COMPILER FLAGS

        #  Optimization flags
        QMAKE_CXXFLAGS_DEBUG -= -O0 -g
        QMAKE_CXXFLAGS_DEBUG *= -Og -g3
        QMAKE_CXXFLAGS_RELEASE *= -O3 -mfpmath=sse

        #  Extension flags
        QMAKE_CXXFLAGS_RELEASE += -msse2 -msse
}
