/*-==============================================================-

file : BandWidget.hpp

creation date : 06/07/2011

author : S. Domas (sdomas@iut-bm.univ-fcomte.fr)

description : 

supp. infos : saved in UTF-8 [éè]

-==============================================================-*/
#ifndef __BANDWIDGET_HPP__
#define __BANDWIDGET_HPP__

#include <iostream>
#include <fstream>

#include <QtCore>
#include <QtGui>

#include "ProjectParams.hpp"
#include "BandView.hpp"
#include "BandLockersView.hpp"
#include "BandSelectorsView.hpp"
#include "BandControl.hpp"
#include "BandModel.hpp"

using namespace std;
using namespace Qt;

class BandWidget : public QWidget {

  Q_OBJECT

public:

  BandWidget(ProjectParams *_params, QWidget *parent = NULL);
  ~BandWidget();

  /* NB : the control is the only part accessible
     from "upper" widgets, i.e. widget that contains
     BandWidget. In this case, it is the Design widget
    */
  BandControl *control;
  BandView *view;
  BandLockersView *lockers;
  BandSelectorsView *selectors;
  BandModel *model;
  QLabel *warnLettering;
  QLabel *labLettering;

private:

  ProjectParams *params;  

  QScrollArea *scrollBand;
  QScrollArea *scrollLockers;
  QScrollArea *scrollSelectors;

  QPushButton *zoomInButton;
  QPushButton *zoomOutButton;
  QPushButton *normalSizeButton;

  // main boxes of the widget
  QGroupBox *bandBox;
  QGroupBox *modeBox;
  QGroupBox *paletteBox;

  // components of mode box
  QRadioButton *radioColor;
  QRadioButton *radioTwist;
  QLabel *currentColor;

  // components of palette box
  QPushButton **lstColors;

  /*
  int bandWidth;
  int patternLength;
  int nbPattern;
*/
  void createWidget();
  void createConnection();

  //void adjustScrollBar(QScrollBar *scrollBar);

public slots:  
  void setMode();

  void zoomIn();
  void zoomOut();
  void updatePalette(int colorId);

  void updateForNewCreationMode();
  void resizeScrollArea();

  // totally horrible but much simpler than defining a new widget
private slots:
  void chooseColor(quint8 numCol); // the general method
  void chooseColor1();
  void chooseColor2();
  void chooseColor3();
  void chooseColor4();
  void chooseColor5();
  void chooseColor6();
  void chooseColor7();
  void chooseColor8();
  void chooseColor9();
  void chooseColor10();
  void chooseColor11();
  void chooseColor12();
  void chooseColor13();
  void chooseColor14();
  void chooseColor15();
  void chooseColor16();  

  void changeColor(quint8 numCol);
  void changeColor1(QPoint p);
  void changeColor2(QPoint p);
  void changeColor3(QPoint p);
  void changeColor4(QPoint p);
  void changeColor5(QPoint p);
  void changeColor6(QPoint p);
  void changeColor7(QPoint p);
  void changeColor8(QPoint p);
  void changeColor9(QPoint p);
  void changeColor10(QPoint p);
  void changeColor11(QPoint p);
  void changeColor12(QPoint p);
  void changeColor13(QPoint p);
  void changeColor14(QPoint p);
  void changeColor15(QPoint p);
  void changeColor16(QPoint p);

  void updateScrollAreas(int dx);

signals:
  void currentColorChanged(quint8 numCol);
  void paletteChanged(quint8 numCol, QColor c);
};

#endif //__BANDWIDGET_HPP__
