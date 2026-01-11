#pragma once

#include <QtWidgets/QWidget>

struct Dashboard : public QWidget {
  explicit Dashboard(QWidget* parent=nullptr) : QWidget(parent)
  {
    setWindowTitle("Hotel Check-in Helper Dashboard");
  }
};
