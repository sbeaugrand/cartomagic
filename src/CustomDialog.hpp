/*-==============================================================-

file : CustomDialog.hpp

creation date : 08/02/2012

author : S. Domas (sdomas@iut-bm.univ-fcomte.fr)

description : 

supp. infos : saved in UTF-8 [éè]

-==============================================================-*/
#ifndef __CUSTOMDIALOG_HPP__
#define __CUSTOMDIALOG_HPP__

#include <iostream>
#include <fstream>

#include <QtCore>
#include <QtGui>

using namespace std;
using namespace Qt;

class CustomDialog : public QDialog {

  Q_OBJECT

public:

  CustomDialog(const QString &dialogTitle, const QString &boxTitle, QWidget *_parent = NULL);

protected:

  QGroupBox *box;
  QPushButton *okButton;
  QPushButton *cancelButton;

  void setContent(QVBoxLayout *boxLayout);

private:
  QHBoxLayout *layBottom;

protected slots:
  virtual void checkBeforeAccept();
  virtual bool confirmAccept();

};
#endif //__CUSTOMDIALOG_HPP__
