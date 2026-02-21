#pragma once

#include <QMainWindow>
#include <QCloseEvent>

class Dashboard : public QMainWindow {
  Q_OBJECT

public:
  explicit Dashboard(QWidget* parent=nullptr);
};
