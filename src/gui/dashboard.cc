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
  central_layout->addWidget(main_label);
  central_layout->addWidget(reservation_table);

  setCentralWidget(central_widget);
}
