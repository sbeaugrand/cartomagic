/*-==============================================================-

file : ColorWarpDialog.hpp

creation date : 09/02/2012

author : S. Domas (sdomas@iut-bm.univ-fcomte.fr)

description : 

supp. infos : saved in UTF-8 [éè]

-==============================================================-*/
#ifndef __COLORWARPDIALOG_HPP__
#define __COLORWARPDIALOG_HPP__

#include <iostream>
#include <fstream>

#include <QtCore>
#include <QtGui>

#include "BandDialog.hpp"

using namespace std;
using namespace Qt;

class ColorWarpDialog : public BandDialog {

  Q_OBJECT

public:

  ColorWarpDialog(ProjectParams *_params, DispatchControl *_control, QList<int> &_selIndex, QWidget *_parent = NULL);

private:
  QList<int> selIndex;

private slots:
  void checkBeforeAccept();  
			  
};
#endif //__COLORWARPDIALOG_HPP__
