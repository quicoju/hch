#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/chrono.h>

#include "Hotel.hh"

namespace py = pybind11;

PYBIND11_MODULE(hch, m) {
  m.doc() = "hch - Python Bindings";

  // bindings the relevant "concepts"
  // ================================
  // TODO: it may be a good idea to include the
  // concepts.hh instead of relying on "Hotel.hh"
  py::class_<Date>(m, "Date")
    .def(py::init<int, int, int>())
    .def("__str__", [](const Date& d) { return _dstr(d); });
}
