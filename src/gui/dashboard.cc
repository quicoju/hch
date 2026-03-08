#include <QCloseEvent>
#include <QHBoxLayout>
#include <QLabel>
#include <QMenuBar>
#include <QTableView>
#include <QVBoxLayout>
#include <QWidget>

#include "dashboard.hh"

#include "Hotel.hh"

Dashboard::Dashboard(QWidget* parent)
  : QMainWindow{parent}
{
  auto logger = Log::logger();
  Log::debug(logger, "Setting up the hotel dashboard...");

  setWindowTitle("Hotel Check-in Helper Dashboard");

  // start building the menu bar
  QAction *exitAction = new QAction{"&Exit", this};
  exitAction->setShortcut(QKeySequence::Quit);
  connect(exitAction, &QAction::triggered, this, &QWidget::close);

  QMenu *fileMenu = menuBar()->addMenu("&File");
  fileMenu->addAction(exitAction);

  // setup the central layout design
  auto *central_widget = new QWidget{this};
  auto *central_layout = new QVBoxLayout{central_widget};

  auto *main_label = new QLabel{"Reservation overview"};
  auto *reservation_table = new QTableView{};

  // prepare the model for the reservation
  Hotel hotel = Hotel{"db/hotel.db"};
  auto reservations = hotel.reservations_starting_on(Date::today());
  reservations.append_range(hotel.reservations_ending_on(Date::today()));

  reservation_table->setModel(new ReservationModel{std::move(reservations)});
  reservation_table->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);

  central_layout->addWidget(main_label);
  central_layout->addWidget(reservation_table);

  setCentralWidget(central_widget);
}

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
