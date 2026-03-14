#pragma once

#include <QTableView>
#include <QMainWindow>

#include "Hotel.hh"

class Dashboard : public QMainWindow {
  Q_OBJECT

public:
  explicit Dashboard(QWidget* parent=nullptr);

private:
  QTableView* reservations_table(Hotel&) const;
};
