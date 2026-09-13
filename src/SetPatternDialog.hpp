/*-==============================================================-

file : SetPatternDialog.hpp

creation date : 08/02/2012

author : S. Domas (sdomas@iut-bm.univ-fcomte.fr)

description : 

supp. infos : saved in UTF-8 [éè]

-==============================================================-*/
#ifndef __TWISTPATTERNDIALOG_HPP__
#define __TWISTPATTERNDIALOG_HPP__

#include <iostream>
#include <fstream>

#include <QtCore>
#include <QtGui>

#include "BandDialog.hpp"

using namespace std;
using namespace Qt;

class SetPatternDialog : public BandDialog {

  Q_OBJECT

public:

  SetPatternDialog(ProjectParams *_params, DispatchControl *_control, QList<int> &_selIndex, QWidget *_parent = NULL);

private:
  QList<int> selIndex;
  QGroupBox *choiceBox;
  QRadioButton *rightOri;
  QRadioButton *leftOri;

  QList<QCheckBox *> lstTwisted;

  QComboBox * listClassics;

private slots:
  void checkBeforeAccept();  
  void setClassicTwists(int index);
			  
};
#endif //__TWISTPATTERNDIALOG_HPP__
