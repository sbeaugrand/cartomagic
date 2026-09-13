/*-==============================================================-

file : TwistWarpDialog.hpp

creation date : 09/02/2012

author : S. Domas (sdomas@iut-bm.univ-fcomte.fr)

description : 

supp. infos : saved in UTF-8 [éè]

-==============================================================-*/
#ifndef __TWISTWARPDIALOG_HPP__
#define __TWISTWARPDIALOG_HPP__

#include <iostream>
#include <fstream>

#include <QtCore>
#include <QtGui>

#include "BandDialog.hpp"

using namespace std;
using namespace Qt;

class TwistWarpDialog : public BandDialog {

  Q_OBJECT

public:

  TwistWarpDialog(ProjectParams *_params, DispatchControl *_control, QList<int> &_selIndex, QWidget *_parent = NULL);


private:
  QList<int> selIndex;

private slots:

  void checkBeforeAccept();  
			  
};
#endif //__TWISTWARPDIALOG_HPP__
