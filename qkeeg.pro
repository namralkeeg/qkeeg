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
DEFINES += QT_DEPRECATED_WARNINGS

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

SOURCES += #\

HEADERS += \
    src/common/stringutils.hpp \
    src/common/macrohelpers.hpp \
    src/common/enums.hpp \
    src/common/endian.hpp

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
        QMAKE_CXXFLAGS_RELEASE -= /O2
        QMAKE_CXXFLAGS_RELEASE *= /O2 /Ot /Ox /GL
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
