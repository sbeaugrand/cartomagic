/*-==============================================================-

file : DesignWidget.hpp

creation date : 31/01/2012

author : S. Domas (sdomas@iut-bm.univ-fcomte.fr)

description : 

supp. infos : saved in UTF-8 [éè]

-==============================================================-*/
#ifndef __DESIGNWIDGET_HPP__
#define __DESIGNWIDGET_HPP__

#include <iostream>
#include <fstream>

#include <QtCore>
#include <QtGui>

#include "ProjectParams.hpp"
#include "BandWidget.hpp"
#include "ThreadingWidget.hpp"
#include "MovesWidget.hpp"

using namespace std;
using namespace Qt;

class DesignWidget : public QWidget {

  Q_OBJECT

public:

  DesignWidget(ProjectParams *_params, QWidget *parent = NULL); 
  void initialize();

  BandWidget *band;
  ThreadingWidget *threading;
  MovesWidget *moves;

public slots:
  void updateForNewCreationMode();

private:

  ProjectParams *params;  

  void createWidget();
  void createConnection();

};

#endif //__DESIGNWIDGET_HPP__
