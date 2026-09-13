/*-==============================================================-

file : LetteringWarpDialog.hpp

creation date : 08/02/2012

author : S. Domas (sdomas@iut-bm.univ-fcomte.fr)

description : 

supp. infos : saved in UTF-8 [éè]

-==============================================================-*/
#ifndef __LETTERINGWARPDIALOG_HPP__
#define __LETTERINGWARPDIALOG_HPP__

#include <iostream>
#include <fstream>

#include <QtCore>
#include <QtGui>

#include "BandDialog.hpp"

using namespace std;
using namespace Qt;

class LetteringWarpDialog : public BandDialog {

  Q_OBJECT

public:

  LetteringWarpDialog(ProjectParams *_params, DispatchControl *_control, QWidget *_parent = NULL);

private:

  QPushButton *selectAllBut;
  QPushButton *deselectAllBut;
  QList<QCheckBox *> lstLettered;
  
private slots:
  void checkBeforeAccept();  
  void selectAll();
  void deselectAll();
			  
};
#endif //__LETTERINGWARPDIALOG_HPP__
