#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/chrono.h>

// TODO: it may be a good idea to include the
// concepts.hh instead of relying on "Hotel.hh"
#include "Hotel.hh"

// Conversion between "chrono::year_month_day" and "datetime.date"
namespace pybind11::detail {
  template <>
  struct type_caster<std::chrono::year_month_day> {
  public:
    PYBIND11_TYPE_CASTER(std::chrono::year_month_day, _("datetime.date"));

    bool load(handle src, bool)
    {
      if (!src) return false;

      PyDateTime_IMPORT;
      if (!PyDate_Check(src.ptr())) return false;

      int y = PyDateTime_GET_YEAR(src.ptr());
      unsigned int m = PyDateTime_GET_MONTH(src.ptr());
      unsigned int d = PyDateTime_GET_DAY(src.ptr());

      value = std::chrono::year{y} / std::chrono::month{m} / std::chrono::day{d};
      return true;
    }

    static handle
    cast(const std::chrono::year_month_day& src, return_value_policy, handle)
    {
      PyDateTime_IMPORT;
      return PyDate_FromDate(int(src.year()), unsigned(src.month()), unsigned(src.day()));
    }
  };
}

namespace py = pybind11;

PYBIND11_MODULE(hch, m) {
  m.doc() = "hch - Python Bindings";

  // bindings the relevant "concepts"
  // ================================
py::class_<Period>(m, "Period")
    .def(py::init<Date, Days>())
    .def_property_readonly("start", &Period::start)
    .def_property_readonly("duration", &Period::duration)
    .def_property_readonly("end", &Period::end)
    .def("intersects", &Period::intersects)
    .def("__eq__", &Period::operator==)
    .def("__str__", [](const Period& p){ return _pstr(p); })
    .def("__repr__", [](const Period& p) {
        return std::format("<Period start={} duration={} days>",
                          _dstr(p.start()), p.duration().count());
    });

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

  // RateReport
  // ==========
  py::class_<RateReport>(m, "RateReport")
    .def_readonly("date", &RateReport::date)
    .def_readonly("duration", &RateReport::duration)
    .def_readonly("total", &RateReport::total)
    .def_readonly("details", &RateReport::details);

  // Hotel
  // =====
  py::class_<Hotel>(m, "Hotel")
    .def(py::init<std::string>(), py::arg("conn_str") = "")
    .def("record_guest", &Hotel::record_guest)
    .def("reserve", &Hotel::reserve,
         py::arg("guest"),
         py::arg("room"),
         py::arg("date") = Today,
         py::arg("duration") = Days{1})
    .def("cancel", &Hotel::cancel)
    .def("checkin", &Hotel::checkin)
    .def("checkout", &Hotel::checkout)
    .def("reservation", &Hotel::reservation)
    .def("room_reservations", &Hotel::room_reservations)
    .def("guest_reservations", &Hotel::guest_reservations)
    .def("reservations_starting_on", &Hotel::reservations_starting_on)
    .def("reservations_ending_on", &Hotel::reservations_ending_on)
    .def("reservation_notes", &Hotel::reservation_notes)
    .def("patch_reservation_notes", &Hotel::patch_reservation_notes)
    .def("room", &Hotel::room)
    .def("room_amenities", &Hotel::room_amenities)
    .def("rate_report_for", &Hotel::rate_report_for,
         py::arg("room"),
         py::arg("date") = Today,
         py::arg("duration") = Days{1});
}
