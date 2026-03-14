#include <QCloseEvent>
#include <QHBoxLayout>
#include <QLabel>
#include <QMenuBar>
#include <QVBoxLayout>
#include <QWidget>

#include "dashboard.hh"
#include "models.hh"

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

  Hotel hotel = Hotel{"db/unit_test.db"};
  auto *main_label = new QLabel{"Reservation overview"};
  auto *main_table = reservations_table(hotel);

  central_layout->addWidget(main_label);
  central_layout->addWidget(main_table);

  setCentralWidget(central_widget);
}

QTableView*
Dashboard::reservations_table(Hotel &h) const
{
  auto *table = new QTableView{};

  auto rsv = h.reservations_starting_on(Date::today());
  auto leaving = h.reservations_ending_on(Date::today());
  rsv.insert(rsv.end(), leaving.begin(), leaving.end());

  table->setModel(new ReservationModel{std::move(rsv)});
  table->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);

  return table;
}
