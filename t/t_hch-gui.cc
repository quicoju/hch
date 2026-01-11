#include <QtTest/QtTest>
#include <QtWidgets/QApplication>

#include "../src/gui/QHch.hh"

struct TestDashboard : public QObject {
  Q_OBJECT

private slots:
  void testWindowTitle();
};

void TestDashboard::testWindowTitle()
{
  Dashboard dashboard;
  QCOMPARE(dashboard.windowTitle(), QString("Hotel Check-in Helper Dashboard"));
}

QTEST_MAIN(TestDashboard)
#include "../t_hch-gui.moc"
