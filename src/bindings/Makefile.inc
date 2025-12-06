PY_SUFFIX != python3-config --extension-suffix
PY_INCLUDE != python3 -m pybind11 --includes
PY_BINDINGS = src/bindings/py_bindings.cc
PY_TARGET = hch$(PY_SUFFIX)

$(PY_TARGET): libhch.a $(PY_BINDINGS)
	$(CXX) $(CXXFLAGS) -shared $(PY_INCLUDE) $(PY_BINDINGS) $(LDFLAGS) -L. -lhch -o $@

bindings: $(PY_TARGET)

test_py: $(PY_TARGET)
	@echo
	@echo "PYTHON BINDING TESTS"
	@echo "---------------------"
	python3 t/t.py

.PHONY: bindings
