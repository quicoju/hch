#include <QApplication>

#include "dashboard.hh"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    Dashboard win_dash{};
    win_dash.show();

    return app.exec();
}
