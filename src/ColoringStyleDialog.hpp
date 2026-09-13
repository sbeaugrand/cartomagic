/*-==============================================================-

file : ColoringStyleDialog.hpp

creation date : 08/02/2012

author : S. Domas (sdomas@iut-bm.univ-fcomte.fr)

description : 

supp. infos : saved in UTF-8 [éè]

-==============================================================-*/
#ifndef __COLORINGSTYLEDIALOG_HPP__
#define __COLORINGSTYLEDIALOG_HPP__

#include <iostream>
#include <fstream>

#include <QtCore>
#include <QtGui>

#include "BandDialog.hpp"

using namespace std;
using namespace Qt;

class ColoringStyleDialog : public BandDialog {

  Q_OBJECT

public:

  ColoringStyleDialog(ProjectParams *_params, DispatchControl *_control, QWidget *_parent = NULL);

private:
  QGroupBox *choiceBox;
  QRadioButton *cyclic;
  QRadioButton *free;
  QRadioButton *holes;

private slots:
  void checkBeforeAccept();  
			  
};
#endif //__COLORINGSTYLEDIALOG_HPP__
