# NOTE
# ====
# This is file expects to be included by the top-level Makefile.
# It extends the top-level Makefile by providing the targets:
# - hch_gui: which builds a Qt based Hch application
# - test_gui: tests the GUI

QT_CXXFLAGS != pkg-config --cflags Qt6Widgets Qt6Core Qt6Test
QT_LDFLAGS != pkg-config --libs Qt6Widgets Qt6Core Qt6Test
QT_LIBEXEC_PATH != pkg-config --variable=libexecdir Qt6Core
MOC = $(QT_LIBEXEC_PATH)/moc

CXXFLAGS += $(QT_CXXFLAGS)
LDFLAGS += $(QT_LDFLAGS)

hch-gui: hch-gui.o libhch.a
	$(CXX) $(CXXFLAGS) $(QT_CXXFLAGS) -o $@ hch-gui.o $(LDFLAGS) $(QT_LDFLAGS)

t_hch-gui.moc: t/t_hch-gui.cc
	$(MOC) $(QT_CXXFLAGS) t/t_hch-gui.cc -o $@

t_hch-gui.o: t_hch-gui.moc

GUITests: t_hch-gui.o
	$(CXX) $(CXXFLAGS) -o $@ t_hch-gui.o $(QT_LDFLAGS)

test_gui: GUITests
	@echo
	@echo "GUI TESTS"
	@echo "---------"
	@./GUITests

.PHONY: test_gui
