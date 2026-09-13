/*-==============================================================-

file : SetSeparatorDialog.hpp

creation date : 08/02/2012

author : S. Domas (sdomas@iut-bm.univ-fcomte.fr)

description : 

supp. infos : saved in UTF-8 [éè]

-==============================================================-*/
#ifndef __SETSEPARATORDIALOG_HPP__
#define __SETSEPARATORDIALOG_HPP__

#include <iostream>
#include <fstream>

#include <QtCore>
#include <QtGui>
#include <QSpinBox>

#include "BandDialog.hpp"

using namespace std;
using namespace Qt;

class SetSeparatorDialog : public BandDialog {

  Q_OBJECT

public:

  SetSeparatorDialog(ProjectParams *_params, DispatchControl *_control, QWidget *_parent = NULL);

private:

  QGroupBox *regBox;
  QSpinBox *gap;
  QPushButton *setGap;
  QList<QCheckBox *> lstCheck;


private slots:
  void checkBeforeAccept();  
  void setRegularGap();
			  
};
#endif //__SETSEPARATORDIALOG_HPP__
