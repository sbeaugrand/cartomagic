/*-==============================================================-
  
file : ThreadingWidget.cpp

creation date : 30/01/2012

author : S. Domas (sdomas@iut-bm.univ-fcomte.fr)

description : 

supp. infos : saved in UTF-8 [éè]

-==============================================================-*/
#include <QVBoxLayout>
#include "ThreadingWidget.hpp"

ThreadingWidget::ThreadingWidget(ProjectParams *_params, QWidget *parent) : QWidget(parent) {
  
  params = _params;
  
  model = new ThreadingModel(params);
  view = new ThreadingView(params,model);
  control = new ThreadingControl(params,model,view);
  
  createWidget();
  createConnection();
}

ThreadingWidget::~ThreadingWidget() {
  model->deleteLater();
  control->deleteLater();
}

void ThreadingWidget::createWidget() {  

  scrollThreading = new QScrollArea;
  scrollThreading->setBackgroundRole(QPalette::Dark);
  scrollThreading->setAlignment(Qt::AlignLeft | Qt::AlignBottom);
  scrollThreading->setSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::Fixed);

  QSize s = view->sizeHint();
  if (s.width() < 500) {
    scrollThreading->setMinimumWidth(s.width()+25);
  }
  else {
    scrollThreading->setMinimumWidth(s.width()/2);
  }
  scrollThreading->setMaximumWidth(s.width()+25);
  scrollThreading->setWidget(view);

  threadingBox = new QGroupBox(tr("Threading"));
  QVBoxLayout *layView = new QVBoxLayout();

  //guessButton = new QPushButton(tr("Refresh"));

  QGroupBox *choiceBox = new QGroupBox(tr("Holes numbering"));
  QVBoxLayout *layChoice = new QVBoxLayout;
  horMount = new QRadioButton(tr("clockwise"));
  antihorMount = new QRadioButton(tr("anti-clockwise [e.g. guntram]"));

  layChoice->addWidget(antihorMount);
  layChoice->addWidget(horMount);
  choiceBox->setLayout(layChoice);

  pixAntihor = new QPixmap(":/icons/card-mount-antihor.png");
  pixHor = new QPixmap(":/icons/card-mount-hor.png");
  numberingTypeLab = new QLabel("");

  if(params->numberingType == ANTICLOCKWISE) {
    antihorMount->setChecked(true);
    numberingTypeLab->setPixmap(*pixAntihor);
  }
  else {
    horMount->setChecked(true);
    numberingTypeLab->setPixmap(*pixHor);
  }

  QGroupBox *upDownBox = new QGroupBox(tr("Numbering order"));
  downOrder = new QRadioButton(tr("descending"));
  upOrder = new QRadioButton(tr("ascending"));
  QVBoxLayout *layUpDown = new QVBoxLayout;
  layUpDown->addWidget(upOrder);
  layUpDown->addWidget(downOrder);  
  upDownBox->setLayout(layUpDown);

  if (params->numberingOrder == DESCENDING) {
    downOrder->setChecked(true);
  }
  else {
    upOrder->setChecked(true);
  }

  QHBoxLayout *layGuess = new QHBoxLayout();  
  layGuess->addWidget(choiceBox);
  layGuess->addWidget(numberingTypeLab);
  layGuess->addWidget(upDownBox);
  //layGuess->addStretch();
  //layGuess->addWidget(guessButton);

  layView->addLayout(layGuess);
  layView->addWidget(scrollThreading);
  //layView->addWidget(view);

  threadingBox->setLayout(layView);

  QHBoxLayout *layAll = new QHBoxLayout();
  layAll->addWidget(threadingBox);
  //layAll->addStretch();
  
  setLayout(layAll);
}

void ThreadingWidget::createConnection() {  
  connect(horMount,SIGNAL(clicked()),this,SLOT(changeLetteringType()));
  connect(antihorMount,SIGNAL(clicked()),this,SLOT(changeLetteringType()));
  connect(upOrder,SIGNAL(clicked()),this,SLOT(changeLetteringOrder()));
  connect(downOrder,SIGNAL(clicked()),this,SLOT(changeLetteringOrder()));

  connect(params,SIGNAL(bandWidthChanged()),this,SLOT(resizeScrollArea()));
}

void ThreadingWidget::resizeScrollArea() {
  QSize s = view->sizeHint();
  if (s.width() < 500) {
    scrollThreading->setMinimumWidth(s.width()+25);
  }
  else {
    scrollThreading->setMinimumWidth(s.width()/2);
  }
  scrollThreading->setMaximumWidth(s.width()+25);
}

void ThreadingWidget::changeLetteringType() {
  if (horMount->isChecked()) {
    numberingTypeLab->setPixmap(*pixHor);
    if (params->numberingType == ANTICLOCKWISE) {
      emit letteringTypeChanged();
    }
  }
  else {
    numberingTypeLab->setPixmap(*pixAntihor);
    if (params->numberingType == CLOCKWISE) {
      emit letteringTypeChanged();
    }
  }
}

void ThreadingWidget::changeLetteringOrder() {
  if ((downOrder->isChecked()) && (params->numberingOrder == ASCENDING)) {
    emit letteringOrderChanged();
  }
  else if ((upOrder->isChecked()) && (params->numberingOrder == DESCENDING)) {
    emit letteringOrderChanged();
  }

}

void ThreadingWidget::updateForNewCreationMode() {

  model->updateForNewCreationMode();
}
