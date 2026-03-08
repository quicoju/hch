# NOTE
# ====
# This is file expects to be included by the top-level Makefile.
# It extends the top-level Makefile by providing the targets:
# - hch_gui: which builds a Qt based Hch application
# - test_gui: tests the GUI

VPATH += src/gui

QT_CXXFLAGS != pkg-config --cflags Qt6Widgets Qt6Core Qt6Test
QT_LDFLAGS != pkg-config --libs Qt6Widgets Qt6Core Qt6Test
QT_LIBEXEC_PATH != pkg-config --variable=libexecdir Qt6Core

CXXFLAGS += ${QT_CXXFLAGS} -Isrc/gui
LDFLAGS += ${QT_LDFLAGS}

MOC = ${QT_LIBEXEC_PATH}/moc

HDRS = dashboard.hh models.hh
SRCS = ${HDRS:.hh=.cc}
MOCS = ${HDRS:.hh=.moc.cc}
OBJS = ${SRCS:.cc=.o} ${MOCS:.cc=.o}

hch-gui: hch-gui.o libhch.a  ${OBJS}
	${CXX} $^ -o $@ ${LDFLAGS}

%.o : %.cc
	${CXX} ${CXXFLAGS} -c $^ -o $@

%.moc.o : %.moc.cc
	${CXX} ${CXXFLAGS} -c $^ -o $@

%.moc.cc: %.hh
	${MOC} $^ -o $@

# build the tests
# ===============
t_hch-gui.moc: t/t_hch-gui.cc
	${MOC} t/t_hch-gui.cc -o $@

t_hch-gui.o: t_hch-gui.moc

GUITests: t_hch-gui.o ${OBJS}
	${CXX} $^ -o $@ ${LDFLAGS}

test_gui: GUITests
	@echo
	@echo "GUI TESTS"
	@echo "---------"
	@QT_QPA_PLATFORM=offscreen ./GUITests 2>/dev/null


.PHONY: test_gui
