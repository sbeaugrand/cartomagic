/*-==============================================================-

file : MirrorWarpDialog.hpp

creation date : 08/02/2012

author : S. Domas (sdomas@iut-bm.univ-fcomte.fr)

description : 

supp. infos : saved in UTF-8 [éè]

-==============================================================-*/
#ifndef __MIRRORWARPDIALOG_HPP__
#define __MIRRORWARPDIALOG_HPP__

#include <iostream>
#include <fstream>

#include <QtCore>
#include <QtGui>

#include "BandDialog.hpp"

using namespace std;
using namespace Qt;

class MirrorWarpDialog : public BandDialog {

  Q_OBJECT

public:

  MirrorWarpDialog(ProjectParams *_params, BandModel *_model, QWidget *_parent = NULL);

private:
  QGroupBox *choiceBox;  
  QSpinBox *from;
  QSpinBox *to;
  QLabel *xchgLabel;
  QCheckBox *mirrorCheck;

  void setMessage();

private slots:
  void checkBeforeAccept();
  void fromChanged(int val);
  void toChanged(int val);
			  
};
#endif //__MIRRORWARPDIALOG_HPP__
