/*-==============================================================-

file : ThreadingWidget.hpp

creation date : 06/07/2011

author : S. Domas (sdomas@iut-bm.univ-fcomte.fr)

description : 

supp. infos : saved in UTF-8 [éè]

-==============================================================-*/
#ifndef __THREADINGWIDGET_HPP__
#define __THREADINGWIDGET_HPP__

#include <iostream>
#include <fstream>

#include <QtCore>
#include <QtGui>
#include <QScrollArea>
#include <QLabel>
#include <QRadioButton>
#include <QGroupBox>

#include "ProjectParams.hpp"
#include "ThreadingView.hpp"
#include "ThreadingControl.hpp"
#include "ThreadingModel.hpp"

using namespace std;
using namespace Qt;

class ThreadingWidget : public QWidget {

  Q_OBJECT

public:

  ThreadingWidget(ProjectParams *_params, QWidget *parent = NULL);
  ~ThreadingWidget();

  /* NB : the control is the only part accessible
     from "upper" widgets, i.e. widget that contains
     ThreadingWidget. In this case, it is the Design widget
    */
  ThreadingControl *control;
  ThreadingView *view;
  ThreadingModel *model;

private:

  ProjectParams *params;
  QScrollArea *scrollThreading;
  QLabel *numberingTypeLab;
  QRadioButton *horMount;
  QRadioButton *antihorMount;
  QPixmap *pixAntihor;
  QPixmap *pixHor;
  QRadioButton *upOrder;
  QRadioButton *downOrder;  

  // main boxes of the widget
  QGroupBox *threadingBox;    

  void createWidget();
  void createConnection();

public slots:  
  void updateForNewCreationMode();
  void changeLetteringType();
  void changeLetteringOrder();
  void resizeScrollArea();

signals:

  void letteringTypeChanged();
  void letteringOrderChanged();


};

#endif //__THREADINGWIDGET_HPP__
