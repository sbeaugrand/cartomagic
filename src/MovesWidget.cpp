/*-==============================================================-
  
file : MovesWidget.cpp

creation date : 30/01/2012

author : S. Domas (sdomas@iut-bm.univ-fcomte.fr)

description : 

supp. infos : saved in UTF-8 [éè]

-==============================================================-*/
#include <QScrollBar>
#include <QVBoxLayout>
#include "MovesWidget.hpp"

MovesWidget::MovesWidget(ProjectParams *_params, QWidget *parent) : QWidget(parent) {
  
  params = _params;
  
  model = new MovesModel(params);
  view = new MovesView(params,model);

  lockers = new MovesLockersView(params,model);
  selectors = new MovesSelectorsView(params,model);
  szView = new MovesSZView(params,model);

  control = new MovesControl(params,model,view,selectors,lockers,szView);
  
  createWidget();
  createConnection();
}

MovesWidget::~MovesWidget() {
  model->deleteLater();
  control->deleteLater();
}

void MovesWidget::createWidget() {
  
  scrollMoves = new QScrollArea;
  scrollMoves->setBackgroundRole(QPalette::Dark);
  scrollMoves->setAlignment(Qt::AlignLeft | Qt::AlignBottom);
  scrollMoves->setSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::MinimumExpanding);

  QSize s = view->sizeHint();
  if (s.width() < 500) {
    scrollMoves->setMinimumWidth(s.width()+25);
  }
  else {
    scrollMoves->setMinimumWidth(s.width()/2);
  }
  scrollMoves->setMaximumWidth(s.width()+25);
  scrollMoves->setMinimumHeight(s.height()/4);
  scrollMoves->setMaximumHeight(s.height()+25);
  scrollMoves->setWidget(view);

  scrollLockers = new QScrollArea;
  scrollLockers->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  scrollLockers->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  scrollLockers->setSizePolicy(QSizePolicy::Ignored,QSizePolicy::Fixed);
  scrollLockers->setFixedHeight(lockers->sizeHint().height()+4);
  scrollLockers->setWidget(lockers);  
  scrollLockers->setVisible(false);

  scrollSelectors = new QScrollArea;
  scrollSelectors->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  scrollSelectors->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  scrollSelectors->setSizePolicy(QSizePolicy::Ignored,QSizePolicy::Fixed);
  scrollSelectors->setFixedHeight(selectors->sizeHint().height()+4);
  scrollSelectors->setWidget(selectors);
  scrollSelectors->setVisible(false);

  scrollSZ = new QScrollArea;
  scrollSZ->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  scrollSZ->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  scrollSZ->setSizePolicy(QSizePolicy::Ignored,QSizePolicy::Fixed);
  scrollSZ->setFixedHeight(szView->sizeHint().height()+4);
  scrollSZ->setWidget(szView);
  scrollSZ->setVisible(true);

  movesBox = new QGroupBox(tr("Rotations"));

  guessButton = new QPushButton(tr("Refresh"));

  guessIdle = new QCheckBox(tr("guess with idles"));
  guessIdle->setChecked(params->guessWithIdles);
  if (!params->guessWithIdles) {
    guessIdle->setEnabled(false);
  }  

  allowNoCyclic = new QCheckBox(tr("allow to have different card positions for start/end of sequences"));
  if (params->allowNoCyclic) {
    allowNoCyclic->setChecked(true);
  }
  else {
    allowNoCyclic->setChecked(false);
  }
  QVBoxLayout *layGuess = new QVBoxLayout();
  layGuess->addWidget(guessIdle);
  layGuess->addWidget(allowNoCyclic);
  //layGuess->addStretch();

  QHBoxLayout *layUp = new QHBoxLayout();
  layUp->addLayout(layGuess);
  layUp->addStretch();
  layUp->addWidget(guessButton);

  QVBoxLayout *layView = new QVBoxLayout();  
  layView->addLayout(layUp);
  layView->addWidget(scrollLockers);
  layView->addWidget(scrollMoves);
  layView->addWidget(scrollSelectors);
  layView->addWidget(scrollSZ);

  movesBox->setLayout(layView);

  QHBoxLayout *layAll = new QHBoxLayout();
  layAll->addWidget(movesBox);  
    
  setLayout(layAll);
}

void MovesWidget::createConnection() {

  /* signals that are caught by the dispatcher */
  connect(guessIdle,SIGNAL(clicked()),this,SIGNAL(guessIdleToggled()));
  connect(allowNoCyclic,SIGNAL(clicked()),this,SIGNAL(allowCyclicToggled()));
  connect(guessButton,SIGNAL(clicked()),this,SIGNAL(refreshClicked()));  

  connect(params,SIGNAL(bandWidthChanged()),this,SLOT(resizeScrollArea()));
  connect(params,SIGNAL(guessWithIdlesSet()),this,SLOT(slotUpdateGuessCheck()));
}

void MovesWidget::resizeScrollArea() {
  QSize s = view->sizeHint();
  if (s.width() < 500) {
    scrollMoves->setMinimumWidth(s.width()+25);
  }
  else {
    scrollMoves->setMinimumWidth(s.width()/2);
  }
  scrollMoves->setMaximumWidth(s.width()+25);
  scrollMoves->setMinimumHeight(s.height()/4);
  scrollMoves->setMaximumHeight(s.height()+25);

}

void MovesWidget::slotUpdateGuessCheck() {
  if (params->guessWithIdles) {
    guessIdle->setEnabled(true);
    guessIdle->setChecked(true);
  }
  else {
    guessIdle->setEnabled(false);
    guessIdle->setChecked(false);
  }
}

void MovesWidget::updateForNewCreationMode() {

  model->updateForNewCreationMode();
  if (params->createMode == MODE_FROMMOVES) {    
    guessIdle->setVisible(false);
    allowNoCyclic->setVisible(false);
    guessButton->setVisible(false);

    scrollSelectors->setVisible(true);
    scrollLockers->setVisible(true);
    connect(scrollMoves->horizontalScrollBar(),SIGNAL(valueChanged(int)),this,SLOT(slotUpdateScrollAreas(int)));
  }
  else {
    guessIdle->setVisible(true);
    allowNoCyclic->setVisible(true);
    guessButton->setVisible(true);

    disconnect(scrollMoves->horizontalScrollBar(),SIGNAL(valueChanged(int)),this,SLOT(slotUpdateScrollAreas(int)));
    scrollSelectors->setVisible(false);
    scrollLockers->setVisible(false);    
  }
  szView->update();
}

void MovesWidget::slotUpdateScrollAreas(int dx) {

  int maxBand = scrollMoves->horizontalScrollBar()->maximum();
  int maxLock = scrollLockers->horizontalScrollBar()->maximum();
  int maxSel = scrollSelectors->horizontalScrollBar()->maximum();
  int maxSZ = scrollSZ->horizontalScrollBar()->maximum();

  if (maxLock != maxBand) {
    scrollLockers->horizontalScrollBar()->setMaximum(maxBand);
  }
  if (maxSel != maxBand) {
    scrollSelectors->horizontalScrollBar()->setMaximum(maxBand);
  }
  if (maxSZ != maxBand) {
    scrollSZ->horizontalScrollBar()->setMaximum(maxBand);
  }

  scrollLockers->horizontalScrollBar()->setValue(dx);
  scrollSelectors->horizontalScrollBar()->setValue(dx);
  scrollSZ->horizontalScrollBar()->setValue(dx);
}




