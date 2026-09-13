/*-==============================================================-

file : InsertWarpDialog.hpp

creation date : 08/02/2012

author : S. Domas (sdomas@iut-bm.univ-fcomte.fr)

description : 

supp. infos : saved in UTF-8 [éè]

-==============================================================-*/
#ifndef __INSERTWARPDIALOG_HPP__
#define __INSERTWARPDIALOG_HPP__

#include <iostream>
#include <fstream>

#include <QtCore>
#include <QtGui>

#include "BandDialog.hpp"
#include "DispatchControl.hpp"

using namespace std;
using namespace Qt;

class InsertWarpDialog : public BandDialog {

  Q_OBJECT

public:

  InsertWarpDialog(ProjectParams *_params, DispatchControl *_control, QList<int> &selIndex, QWidget *_parent = NULL);

private:
  QGroupBox *choiceBox;
  QRadioButton *after;
  QRadioButton *before;
  QSpinBox *nbInsert;
  QList<int> selIndex;

private slots:
  void checkBeforeAccept();  
			  
};
#endif //__INSERTWARPDIALOG_HPP__
