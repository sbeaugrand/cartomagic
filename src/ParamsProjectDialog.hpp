/*-==============================================================-

file : ParamsProjectDialog.hpp

creation date : 31/01/2012

author : S. Domas (sdomas@iut-bm.univ-fcomte.fr)

description : 

supp. infos : saved in UTF-8 [éè]

-==============================================================-*/
#ifndef __PARAMSPROJECTDIALOG_HPP__
#define __PARAMSPROJECTDIALOG_HPP__

#include <iostream>
#include <fstream>

#include <QtCore>
#include <QtGui>
#include <QLineEdit>
#include <QSpinBox>

#include "BandDialog.hpp"

using namespace std;
using namespace Qt;

class ParamsProjectDialog : public BandDialog {

  Q_OBJECT

public:

  ParamsProjectDialog(ProjectParams *_params, DispatchControl *_control, QWidget *parent = NULL);

private:  

  QLineEdit *nameProjEdit;
  QSpinBox *nbPatterns;
  QSpinBox *patternLength;
  QSpinBox *nbHoles;

private slots:
  void checkBeforeAccept();


};

#endif //__PARAMSPROJECTDIALOG_HPP__ 
