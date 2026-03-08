#include "models.hh"

ReservationModel::ReservationModel(QObject *parent)
  : QAbstractTableModel{parent}
{}

ReservationModel::ReservationModel(Reservations &&r, QObject *parent)
  : QAbstractTableModel{parent}
  , reservations{std::move(r)}
{}

int
ReservationModel::rowCount(const QModelIndex &_) const
{
  return static_cast<int>(reservations.size());
}

int
ReservationModel::columnCount(const QModelIndex &_) const
{
  return 4;  // columns: id, guest, room, expected
}

QVariant
ReservationModel::data(const QModelIndex &ix, int role) const
{
  if (Qt::DisplayRole != role)
    return QVariant{};

  auto row = ix.row();
  auto col = ix.column();
  auto r = reservations[row];

  switch (col) {
  case 0:
    return QString{r.id.c_str()};
  case 1:
    return QString{r.guest_id.c_str()};
  case 2:
    return QString{r.room_id.c_str()};
  case 3:
    return QString{r.checkin_at ? "check-out" : "check-in"};
  }

  return QVariant{};
}

QVariant
ReservationModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if (role != Qt::DisplayRole || orientation != Qt::Horizontal)
    return QVariant{};

  switch(section) {
  case 0:
    return QString{"Reservation"};
  case 1:
    return QString{"Guest"};
  case 2:
    return QString{"Room"};
  case 3:
    return QString{"Expected to"};
  }

  return QVariant();
}
