/*-==============================================================-

file : NewProjectDialog.hpp

creation date : 31/01/2012

author : S. Domas (sdomas@iut-bm.univ-fcomte.fr)

description : 

supp. infos : saved in UTF-8 [éè]

-==============================================================-*/
#ifndef __NEWPROJECTDIALOG_HPP__
#define __NEWPROJECTDIALOG_HPP__

#include <iostream>
#include <fstream>

#include <QtCore>
#include <QtGui>

#include "CustomDialog.hpp"
#include "ProjectParams.hpp"

using namespace std;
using namespace Qt;

class NewProjectDialog : public CustomDialog {

  Q_OBJECT

public:

  NewProjectDialog(ProjectParams *_params, QWidget *parent = NULL);  

private:

  ProjectParams *params;

  QLineEdit *nameProjEdit;
  QSpinBox *nbCards;
  QSpinBox *nbHoles;
  QSpinBox *patternLength;
  QSpinBox *nbPattern;

private slots:
  void checkBeforeAccept();


};

#endif //__NEWPROJECTDIALOG_HPP__ 
