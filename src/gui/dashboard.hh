#pragma once

#include <QTableView>
#include <QMainWindow>

#include "models.hh"

#include "Hotel.hh"

class Dashboard : public QMainWindow {
  Q_OBJECT

public:
  explicit Dashboard(QWidget* parent=nullptr);

private:
  void load_reservation_model();
  void open_file();

  // properties
  Hotel hotel;
  ReservationModel *reservation_m;
};
