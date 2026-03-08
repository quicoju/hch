#pragma once

#include <QAbstractTableModel>
#include <QMainWindow>

#include "Hotel.hh"

class Dashboard : public QMainWindow {
  Q_OBJECT

public:
  explicit Dashboard(QWidget* parent=nullptr);
};

class ReservationModel : public QAbstractTableModel {
  Q_OBJECT

public:
  explicit ReservationModel(QObject *parent = nullptr);
  explicit ReservationModel(Reservations &&, QObject *parent = nullptr);

  int rowCount(const QModelIndex &parent = QModelIndex{}) const override;
  int columnCount(const QModelIndex &parent = QModelIndex{}) const override;

  QVariant data(const QModelIndex &, int role = Qt::DisplayRole) const override;
  QVariant headerData(int, Qt::Orientation, int role = Qt::DisplayRole) const override;

private:
  Reservations reservations;
};
