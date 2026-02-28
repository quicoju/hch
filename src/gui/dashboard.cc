#include <QCloseEvent>
#include <QHBoxLayout>
#include <QLabel>
#include <QListView>
#include <QMenuBar>
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

  // build the base layout
  auto *central = new QWidget{this};
  auto *mainLayout = new QVBoxLayout{central};

  auto *sectionLabel = new QLabel{"Reservation overview"};
  mainLayout->addWidget(sectionLabel);

  auto *contentLayout = new QHBoxLayout{};
  auto *reservationView = new QListView{};

  contentLayout->addWidget(reservationView);
  mainLayout->addLayout(contentLayout);
  setCentralWidget(central);
}
