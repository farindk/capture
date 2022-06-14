
#include <QApplication>
#include <QPushButton>
#include <QAbstractVideoSurface>

#include "MainWindow.h"
#include <time.h>
#include <QErrorMessage>


int main(int argc, char* argv[])
{
  QApplication a(argc, argv);

  if (argc >= 2 && strcmp(argv[1],"--help")==0) {
    fprintf(stderr, "Video recorder,  2021 Dirk Farin (dirk.farin@gmail.com)\n");
    return 5;
  }

  MainWindow mainWindow;
  mainWindow.show();

  return QApplication::exec();
}
