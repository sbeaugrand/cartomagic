#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <iostream>

#include <QtCore>
#include <QtGui>

#include "ProjectParams.hpp"
#include "DispatchControl.hpp"
#include "PatternGuesser.hpp"
#include "GttExchanger.hpp"
#include "MainWindow.hpp"


int main(int argc, char **argv) {
  

  QApplication app(argc,argv);

  app.setStyle("plastique");
  Q_INIT_RESOURCE(cartomagic);

  QTranslator qtTranslator;
  qtTranslator.load("qt_" + QLocale::system().name(),
                    QLibraryInfo::location(QLibraryInfo::TranslationsPath));
  app.installTranslator(&qtTranslator);

  QTranslator myappTranslator;
#ifdef CARTO_LIN
  myappTranslator.load("/usr/local/share/cartomagic/locales/cartomagic_" + QLocale::system().name());
#endif
#ifdef CARTO_WIN
  // just find locales where cartomagic is installed : a bit ugly
  myappTranslator.load("cartomagic_" + QLocale::system().name());
#endif

  app.installTranslator(&myappTranslator);

  ProjectParams *params = new ProjectParams();

  PatternGuesser *guesser = new PatternGuesser(params);

  GttExchanger *xchgGtt = new GttExchanger(params);

  DispatchControl *dispatcher = new DispatchControl(params,guesser);

  MainWindow *window = new MainWindow(params,guesser,dispatcher,xchgGtt);

  dispatcher->setMainWindow(window);

  window->show();

  return app.exec();
}

