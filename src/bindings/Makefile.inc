# NOTE: This file isn't aimed to be executed directly.
# This is file is assumed to be included by the main
# Makefile when at wither the "bindings" or "test_py"
# targets are requested by the user

PY_SUFFIX != python3-config --extension-suffix
PY_INCLUDE != python3 -m pybind11 --includes
PY_BINDINGS = src/bindings/py_bindings.cc
PY_TARGET = Hch$(PY_SUFFIX)

$(PY_TARGET): libhch.a $(PY_BINDINGS)
	$(CXX) $(CXXFLAGS) -shared $(PY_INCLUDE) $(PY_BINDINGS) $(LDFLAGS) -L. -lhch -o $@

bindings: $(PY_TARGET)

test_py: $(PY_TARGET)
	@echo
	@echo "PYTHON BINDING TESTS"
	@echo "---------------------"
	python3 t/t.py

.PHONY: bindings
