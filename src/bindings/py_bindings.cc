#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/chrono.h>

// TODO: it may be a good idea to include the
// concepts.hh instead of relying on "Hotel.hh"
#include "Hotel.hh"

namespace py = pybind11;

PYBIND11_MODULE(Hch, m) {
  m.doc() = "hch - Python Bindings";

  // bindings the relevant "concepts"
  // ================================
  py::class_<Date>(m, "Date")
    .def(py::init<int, int, int>())
    .def("__str__", [](const Date& d){ return _dstr(d); });

  py::class_<Days>(m, "Days") // ...duration
    .def(py::init<long>())
    .def("__int__", [](const Days& d){ return d.days(); });

  py::class_<Period>(m, "Period")
    .def(py::init<Date, Days>())
    .def_property_readonly("begin", &Period::begin)
    .def_property_readonly("end", &Period::end)
    .def("__str__", [](const Period& p){ return _pstr(p); });

  // Room
  // ====
  py::class_<Room>(m, "Room")
    .def_readonly("name", &Room::id)
    .def_readonly("capacity", &Room::capacity)
    .def("__str__", [](const Room& r){ return r.id; });

  // Reservation
  // ===========
  py::class_<Reservation>(m, "Reservation")
    .def_readonly("id", &Reservation::id)
    .def_readonly("period", &Reservation::period)
    .def_readonly("checkin_at", &Reservation::checkin_at)
    .def_readonly("checkout_at", &Reservation::checkout_at)
    .def("__str__", [](const Reservation& r){ return r.id; });

  // Hotel
  // =====
  py::class_<Hotel>(m, "Hotel")
    .def(py::init<std::string>(), py::arg("conn_str") = "")
    .def("record_guest", &Hotel::record_guest)
    .def("reserve", &Hotel::reserve,
         py::arg("guest"), py::arg("room"),
         py::arg("date") = Today, py::arg("duration") = Days{1})
    .def("reservation", &Hotel::reservation);

}
