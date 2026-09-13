/*-==============================================================-

file : CopyGroupWarpDialog.hpp

creation date : 08/02/2012

author : S. Domas (sdomas@iut-bm.univ-fcomte.fr)

description : 

supp. infos : saved in UTF-8 [éè]

-==============================================================-*/
#ifndef __COPYGROUPWARPDIALOG_HPP__
#define __COPYGROUPWARPDIALOG_HPP__

#include <iostream>
#include <fstream>

#include <QtCore>
#include <QtGui>
#include <QSpinBox>

#include "BandDialog.hpp"

using namespace std;
using namespace Qt;

class CopyGroupWarpDialog : public BandDialog {

  Q_OBJECT

public:

  CopyGroupWarpDialog(ProjectParams *_params, DispatchControl *_control, QWidget *_parent = NULL);

private:
  QGroupBox *choiceBox;  
  QSpinBox *from;
  QSpinBox *nbWarp;
  QSpinBox *to;
  QLabel *cpLabel;
  QCheckBox *mirrorCheck;
  QCheckBox *threadCheck;

  int lastFromValue;
  int lastToValue;  

private slots:
  void checkBeforeAccept();
  void fromChanged(int val);
  void nbWarpChanged(int val);
  void toChanged(int val);
  void setMessage();
			  
};
#endif //__COPYGROUPWARPDIALOG_HPP__
