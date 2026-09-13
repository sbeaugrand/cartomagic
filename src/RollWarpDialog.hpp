/*-==============================================================-

file : RollWarpDialog.hpp

creation date : 09/02/2012

author : S. Domas (sdomas@iut-bm.univ-fcomte.fr)

description : 

supp. infos : saved in UTF-8 [éè]

-==============================================================-*/
#ifndef __ROLLWARPDIALOG_HPP__
#define __ROLLWARPDIALOG_HPP__

#include <iostream>
#include <fstream>

#include <QtCore>
#include <QtGui>
#include <QSpinBox>

#include "BandDialog.hpp"

using namespace std;
using namespace Qt;

class RollWarpDialog : public BandDialog {

  Q_OBJECT

public:

  RollWarpDialog(ProjectParams *_params, DispatchControl *_control, QList<int> &_selIndex, QWidget *_parent = NULL);

private:
  QList<int> selIndex;
  QGroupBox *choiceBox;
  QRadioButton *up;
  QRadioButton *down;
  QSpinBox *nbRoll;

private slots:
  void checkBeforeAccept();  
			  
};
#endif //__ROLLWARPDIALOG_HPP__
