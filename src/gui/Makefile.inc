# NOTE
# ====
# This is file expects to be included by the top-level Makefile.
# It extends the top-level Makefile by providing the targets:
# - hch_gui: which builds a Qt based Hch application
# - test_gui: tests the GUI

.SUFFIXES: .hh .cc .o .moc.cc .moc.o .moc
.PATH.hh: src/gui

QT_CXXFLAGS != pkg-config --cflags Qt6Widgets Qt6Core Qt6Test
QT_LDFLAGS != pkg-config --libs Qt6Widgets Qt6Core Qt6Test
QT_LIBEXEC_PATH != pkg-config --variable=libexecdir Qt6Core

CXXFLAGS += ${QT_CXXFLAGS} -Isrc/gui
LDFLAGS += ${QT_LDFLAGS}

MOC = ${QT_LIBEXEC_PATH}/moc

HDRS = dashboard.hh
SRCS = dashboard.cc
MOCS = dashboard.moc.cc
OBJS = dashboard.o dashboard.moc.o

hch-gui: hch-gui.o libhch.a  ${OBJS}
	${CXX} $> -o $@ ${LDFLAGS}

dashboard.o : dashboard.hh dashboard.cc
	${CXX} ${CXXFLAGS} -c $< -o $@

dashboard.moc.o : dashboard.moc.cc
	${CXX} ${CXXFLAGS} -c $< -o $@

dashboard.moc.cc: dashboard.hh
	${MOC} $> -o $@

# build the tests
# ===============
t_hch-gui.moc: t/t_hch-gui.cc
	${MOC} t/t_hch-gui.cc -o $@

t_hch-gui.o: t_hch-gui.moc

GUITests: t_hch-gui.o ${OBJS}
	${CXX} $> -o $@ ${LDFLAGS}

test_gui: GUITests
	@echo
	@echo "GUI TESTS"
	@echo "---------"
	@QT_QPA_PLATFORM=offscreen ./GUITests 2>/dev/null

.PHONY: test_gui
