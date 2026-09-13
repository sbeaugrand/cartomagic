#include "MovesSelectorsView.hpp"

MovesSelectorsView::MovesSelectorsView(ProjectParams *_params, MovesModel *_model, QWidget *parent) : QWidget(parent) {
  
  params = _params;  
  model = _model;
  
  /* parameters to set the layout */

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

  /*
  a single warp is touch or slection/lock area changed
  */
  connect(model,SIGNAL(selectionChanged(int)),this,SLOT(updateSelectors(int)));
  connect(model,SIGNAL(selectionAllChanged()),this,SLOT(update()));

  connect(model,SIGNAL(allMovesChanged()), this, SLOT(updateView()));
}

void MovesSelectorsView::updateLayout() {

    moveWidth = params->moveWidth; // should be set to the same value than in MovesView
    marginUp  = 5;
    marginDown = 10;
    marginLeft = 10;
    marginRight = 10;

    gapToFirstSel = idWidth + 5; /* vertical gap in pixel between the left margin and the first selector
                        it must be in adequation of the MovesView */

    gapNumToSel  = 10; // horizontal gap in pixel between the warp number and the selectors

    numStartX = marginLeft+gapToFirstSel;
    numStartY  = marginUp;
    numEndY = marginUp+numHeight;

    setStartX  = marginLeft+gapToFirstSel;
    selStartY  = numEndY+gapNumToSel;
    selSize = moveWidth-6;

    totalWidth = marginLeft+gapToFirstSel+(params->bandWidth)*moveWidth+marginRight;
    totalHeight = selStartY+moveWidth+marginDown;
}


QSize MovesSelectorsView::minimumSizeHint() const {
  return QSize(totalWidth,totalHeight);
}

QSize MovesSelectorsView::sizeHint() const {
  return QSize(totalWidth,totalHeight);
}

void MovesSelectorsView::updateView() {

  // refresh the whole widget
  // need to resize it since it is not in a layout.
  updateLayout();
  resize(sizeHint());
  update();
}

void MovesSelectorsView::updateSelectors(int warpId) {

  update();
}

void MovesSelectorsView::changeMoveWidth(int width) {

  moveWidth = width;
  update();
}

void MovesSelectorsView::paintEvent(QPaintEvent *event) {
  
  quint32 i;
  
  QPainter p(this);  
  QPen pen(Qt::black);
  pen.setWidth(1);
  p.setPen(pen);  

  // paint warp numbers
  QFont fontNum("Arial",7);
  p.setFont(fontNum);

  QString warpNumber;
  for(i=0;i<params->bandWidth;i++) {
    warpNumber.setNum(i+1);
    p.drawText(numStartX+i*moveWidth+(moveWidth-numWidth)/2,numEndY,warpNumber);
  }

  // paint select all box
  bool sel;
  p.setBrush(Qt::NoBrush);
  pen.setWidth(1);
  p.setPen(pen);
  p.drawRect(marginLeft,selStartY,selSize,selSize);
  sel = model->allSelected;
  if (sel) {
    pen.setWidth(2);
    p.setPen(pen);
    p.drawLine(marginLeft+3,selStartY+3,marginLeft+selSize-3,selStartY+selSize-3);
    p.drawLine(marginLeft+3,selStartY+selSize-3,marginLeft+selSize-3,selStartY+3);
  }


  // paint selection boxes    
  for(i=0;i<(params->bandWidth);i++) {
    pen.setWidth(1);
    p.setPen(pen);
    if ((model->totallyLockedMoves).at(i)) {
      p.setBrush(Qt::black);
    }
    else {
      p.setBrush(Qt::NoBrush);
    }
    p.drawRect(setStartX+i*moveWidth+(moveWidth/2)-(selSize/2),selStartY,selSize,selSize);
    sel = (model->selectedMoves).at(i);
    if (sel) {
      pen.setWidth(2);
      p.setPen(pen);
      p.drawLine(setStartX+i*moveWidth+(moveWidth/2)-(selSize/2)+3,selStartY+3,setStartX+i*moveWidth+(moveWidth/2)+(selSize/2)-3,selStartY+selSize-3);
      p.drawLine(setStartX+i*moveWidth+(moveWidth/2)-(selSize/2)+3,selStartY+selSize-3,setStartX+i*moveWidth+(moveWidth/2)+(selSize/2)-3,selStartY+3);
    }    
  }
}

void MovesSelectorsView::mousePressEvent(QMouseEvent *event) {

  int x = event->x();
  int y = event->y();

  QPointF p(x,y);
  int warpId;

  if ((x>=setStartX) && (x<(setStartX+moveWidth*(int)(params->bandWidth)))) {

    warpId = (x-setStartX)/moveWidth;
    x = (x-setStartX)%moveWidth;


    // else click on selectors
    if ((y>=selStartY)&& (y<=(selStartY+selSize))) {
      if ((x>=(moveWidth/2)-(selSize/2))&& (x<=(moveWidth/2)+(selSize/2))) {
        if (!(model->totallyLockedMoves).at(warpId)) {
#ifdef DEBUG_PRINT
          cout << "warp n°" << warpId << " selected" << endl;
#endif
          emit warpSelected(warpId);
        }
      }
    }
  }
  // click on select all
  else if ((x>=marginLeft) && (x<marginLeft+selSize)) {
    if ((y>=selStartY)&& (y<=(selStartY+selSize))) {
#ifdef DEBUG_PRINT
      cout << "selected all" << endl;
#endif
      emit allWarpsSelected();
    }
  }

}
