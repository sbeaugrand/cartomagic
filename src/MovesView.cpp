#include "MovesView.hpp"

MovesView::MovesView(ProjectParams *_params, MovesModel *_model, QWidget *parent) : QWidget(parent) {
  
  params = _params;  
  model = _model;
  
  QFont fontId("Arial",10);
  QFontMetrics fmId(fontId);
  idWidth = fmId.width("55");
  idHeight = fmId.height();
  
  QFont fontNum("Arial",7);
  QFontMetrics fmNum(fontNum);
  numWidth = fmNum.width("55");
  numHeight = fmNum.height();
  
  updateLayout();
  
  setBackgroundRole(QPalette::Base);
  setAutoFillBackground(true);
  setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
  
  connect(model,SIGNAL(moveChanged(int)),this,SLOT(updateMove(int)));
  connect(model,SIGNAL(allMovesChanged()),this,SLOT(updateView()));
}

void MovesView::updateLayout() {

  moveSize = params->moveWidth;
  marginUp = 10;
  marginDown = 10;
  marginLeft = 10;
  marginRight = 10;
  gapNumToMove = 5;
  //gapMoveToNum = 5;
  gapIdToMove = 5;
  
  gapToFirstMove = marginLeft+idWidth+gapIdToMove;
    
  twistSize  = 6;
  
  numUpStartX = gapToFirstMove;
  numUpStartY  = marginUp+numHeight;
  
  moveStartX = gapToFirstMove;
  moveStartY = numUpStartY+gapNumToMove+(params->patternLength)*moveSize;

  //numDownStartX = gapToFirstMove;
  //numDownStartY  = moveStartY+gapMoveToNum+numHeight;
  
  idStartX = marginLeft;
  idStartY = numUpStartY+gapNumToMove+(params->patternLength)*moveSize;
  
  totalWidth = gapToFirstMove+(params->bandWidth)*moveSize+marginRight;
  //totalHeight = numDownStartY+marginDown;
  totalHeight = moveStartY+marginDown;
  
}

QSize MovesView::minimumSizeHint() const {
  return QSize(totalWidth,totalHeight);
}

QSize MovesView::sizeHint() const {
  return QSize(totalWidth,totalHeight);
}

void MovesView::updateView() {
  
  updateLayout();
  resize(sizeHint());
  //updateGeometry();
  
  update();
}

void MovesView::updateMove(int warpId) {
  
  update();
}

void MovesView::updateCyclic(int warpId) {
  
  update();
}

void MovesView::paintEvent(QPaintEvent *event) {
  
  QFont fontNum("Arial",7);
  QFont fontId("Arial",10);
  
  quint8* lstMoves;
  quint8 pick;
  
  QBrush coloredPick(Qt::black);
  
  quint32 i,j;
  
  QPainter p(this);  
  QPen pen(Qt::black);
  pen.setWidth(1);
  p.setPen(pen);
  
  // paint holes names  
  QString pickName("1");
  p.setFont(fontId);
  
  for(i=0;i<(params->patternLength);i++) {
    pickName.setNum(i+1);
    p.drawText(idStartX,idStartY-i*moveSize-((moveSize-idHeight)/2),pickName);
  }
  
  // paint warp numbers  
  p.setFont(fontNum);
  
  QString warpNumber;
  for(i=0;i<params->bandWidth;i++) {
    warpNumber.setNum(i+1);
    p.drawText(numUpStartX+i*moveSize+(moveSize-numWidth)/2,numUpStartY,warpNumber);
  }
  
  // paint coloredsquare F/B and twist circle
  
  for(i=0;i<(params->bandWidth);i++) {
    
    if (params->createMode == MODE_FROMBAND) {

      if ((params->mount).at(i) == S_MOUNT) {
        lstMoves = (model->startInS).at(i);
      }
      else {
        lstMoves = (model->startInZ).at(i);
      }
    }
    else {
      lstMoves = (model->movesForMoveMode).at(i);
    }
    
    for(j=0;j<(params->patternLength);j++) {
      
      pick = lstMoves[j];
      
      if (pick == NO_MOVE) {
        p.setBrush(Qt::Dense6Pattern);
      }
      else if (PICK_MOVE(pick) == FORWARD){
        coloredPick.setColor(Qt::white);
        p.setBrush(coloredPick);
      }
      else if (PICK_MOVE(pick) == BACKWARD){
        coloredPick.setColor(Qt::black);
        p.setBrush(coloredPick);
      }
      else if (PICK_MOVE(pick) == IDLE){
        coloredPick.setColor(Qt::green);
        p.setBrush(coloredPick);
      }
      pen.setWidth(2);
      p.setPen(pen);
      
      p.drawRect(moveStartX+i*moveSize,moveStartY-(j+1)*moveSize,moveSize,moveSize);
      
      if ((pick != NO_MOVE) && (PICK_TWIST(pick) == TWIST)) {
        pen.setWidth(1);
        p.setPen(pen);
        coloredPick.setColor(Qt::red);
        p.setBrush(coloredPick);
        p.drawRect(moveStartX+i*moveSize+(moveSize-twistSize)/2,moveStartY-(j+1)*moveSize+(moveSize-twistSize)/2,twistSize,twistSize);
      }
    }   
  } 
}

void MovesView::mousePressEvent(QMouseEvent *event) {

  int warpId,moveId;
  int x = event->x();
  int y = event->y();
  bool rightButton = false;
  if(event->button() == Qt::RightButton) {
    rightButton = true;
  }

  warpId = 0;
  moveId = 0;

  if (params->createMode == MODE_FROMMOVES) {

    if ((x>=moveStartX) && (x<(moveStartX+moveSize*(int)(params->bandWidth))) && (y>=numUpStartY+gapNumToMove) && (y <= moveStartY)) {
      
      warpId = (x-moveStartX)/moveSize;
      moveId = (moveStartY-y)/moveSize;

      //cout << "click on move " << moveId << " of warp " << warpId << endl;
            
      emit moveClicked(warpId,moveId,rightButton);
    }
  }
}


