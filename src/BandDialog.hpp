/*-==============================================================-

file : BandDialog.hpp

creation date : 08/02/2012

author : S. Domas (sdomas@iut-bm.univ-fcomte.fr)

description : 

supp. infos : saved in UTF-8 [éè]

-==============================================================-*/
#ifndef __BANDDIALOG_HPP__
#define __BANDDIALOG_HPP__

#include <iostream>
#include <fstream>

#include <QtCore>
#include <QtGui>

#include "ProjectParams.hpp"
#include "BandModel.hpp"
#include "CustomDialog.hpp"
#include "DispatchControl.hpp"
class DispatchControl;

using namespace std;
using namespace Qt;

class BandDialog : public CustomDialog {

  Q_OBJECT

public:

  BandDialog(const QString &dialogTitle, const QString &boxTitle, ProjectParams *_params, DispatchControl *_control, QWidget *_parent = NULL);

protected:

  ProjectParams *params;  
  DispatchControl *control;
			  
};
#endif //__BANDDIALOG_HPP__
