#include "DesignWidget.hpp"

DesignWidget::DesignWidget(ProjectParams *_params, QWidget *parent) : QWidget(parent) {

  params = _params;
  createWidget();
  initialize();
  createConnection();  
}

void DesignWidget::createWidget() {

  band = new BandWidget(params);
  threading = new ThreadingWidget(params);
  moves = new MovesWidget(params);

  QVBoxLayout *layRight = new QVBoxLayout;
  layRight->addWidget(moves);
  layRight->addWidget(threading);  
  //layRight->addWidget(moves);

  QHBoxLayout *layAll = new QHBoxLayout;
  layAll->addWidget(band);
  layAll->addLayout(layRight);

  setLayout(layAll);  
  resize(600,400);
}

void DesignWidget::createConnection() {  

}

void DesignWidget::initialize() {
}

void DesignWidget::updateForNewCreationMode() {

  /* do threading update then move update so that real letter numbers
     appear when changing from BAND to MOVE mode */
  threading->updateForNewCreationMode();
  moves->updateForNewCreationMode();
  band->updateForNewCreationMode();

}
