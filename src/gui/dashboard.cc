#include <QCloseEvent>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QLabel>
#include <QMenuBar>
#include <QVBoxLayout>
#include <QWidget>

#include "dashboard.hh"

Dashboard::Dashboard(QWidget* parent)
  : QMainWindow{parent}
  , hotel{Hotel{"db/unit_test.db"}}
  , reservation_m{new ReservationModel{}}
{
  auto logger = Log::logger();
  Log::debug(logger, "Setting up the hotel dashboard...");

  setWindowTitle("Hotel Check-in Helper Dashboard");

  // start building the menu bar
  QAction *open_action = new QAction{"&Open Hotel File", this};
  open_action->setShortcut(QKeySequence::Open);
  connect(open_action, &QAction::triggered, this, &Dashboard::open_file);

  QAction *exit_action = new QAction{"&Exit", this};
  exit_action->setShortcut(QKeySequence::Quit);
  connect(exit_action, &QAction::triggered, this, &QWidget::close);

  QMenu *fileMenu = menuBar()->addMenu("&File");
  fileMenu->addAction(open_action);
  fileMenu->addAction(exit_action);

  // setup the central layout design
  auto *central_widget = new QWidget{this};
  auto *central_layout = new QVBoxLayout{central_widget};

  auto *main_label = new QLabel{"Reservation overview"};
  auto *main_table = new QTableView{};

  main_table->setModel(reservation_m);
  main_table->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);

  central_layout->addWidget(main_label);
  central_layout->addWidget(main_table);

  setCentralWidget(central_widget);

  load_reservation_model();
}

void
Dashboard::load_reservation_model()
{
  auto rsv = hotel.reservations_starting_on(Date::today());
  auto leaving = hotel.reservations_ending_on(Date::today());
  rsv.insert(rsv.end(), leaving.begin(), leaving.end());

  reservation_m->load(std::move(rsv));
  return;
}

void
Dashboard::open_file()
{
  QString file_name = QFileDialog::getOpenFileName(
    this, "Open Hotel File", "", "SQLite Files(*.db)");

  if (file_name.isEmpty()) return;

  hotel = Hotel{file_name.toStdString()};
  load_reservation_model();
}
