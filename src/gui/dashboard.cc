#include <QMenuBar>

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

}
