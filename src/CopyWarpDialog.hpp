/*-==============================================================-

file : CopyWarpDialog.hpp

creation date : 09/02/2012

author : S. Domas (sdomas@iut-bm.univ-fcomte.fr)

description : 

supp. infos : saved in UTF-8 [éè]

-==============================================================-*/
#ifndef __COPYWARPDIALOG_HPP__
#define __COPYWARPDIALOG_HPP__

#include <iostream>
#include <fstream>

#include <QtCore>
#include <QtGui>

#include "BandDialog.hpp"

using namespace std;
using namespace Qt;

class CopyWarpDialog : public BandDialog {

  Q_OBJECT

public:

  CopyWarpDialog(ProjectParams *_params, BandModel *_model, QWidget *_parent = NULL);

private:
  QGroupBox *choiceBox;
  QRadioButton *left;
  QRadioButton *right;
  int id1;
  int id2;

private slots:
  void checkBeforeAccept();  
			  
};
#endif //__COPYWARPDIALOG_HPP__
