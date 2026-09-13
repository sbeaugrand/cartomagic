/*-==============================================================-

file : PreferencesDialog.hpp

creation date : 31/01/2012

author : S. Domas (sdomas@iut-bm.univ-fcomte.fr)

description : 

supp. infos : saved in UTF-8 [éè]

-==============================================================-*/
#ifndef __PREFERENCESDIALOG_HPP__
#define __PREFERENCESDIALOG_HPP__

#include <iostream>
#include <fstream>

#include <QtCore>
#include <QtGui>
#include <QDialog>
#include <QSpinBox>

#include "ProjectParams.hpp"
#include "DispatchControl.hpp"

using namespace std;
using namespace Qt;

class PreferencesDialog : public QDialog {

  Q_OBJECT

public:

  PreferencesDialog(ProjectParams *_params, DispatchControl *_control, QWidget *parent = NULL);

private:  
  DispatchControl *control;
  ProjectParams *params;

  QTabWidget *tabs;

  QPushButton *okButton;
  QPushButton *cancelButton;

  QWidget *optBand;
  QWidget *optPrint;

  // determine the visual size of each pick/move/card
  QSpinBox *pickWidth;
  QSpinBox *pickBaseLength;
  QSpinBox *pickTotalLength;
  QSpinBox *moveWidth;
  QSpinBox *threadWidth;

  QCheckBox *printVertSep;
  QPushButton *advSepBut;

private slots:
  void setVerticalSepLocation();
  void checkBeforeAccept();


};

#endif //__PREFERENCESDIALOG_HPP__
