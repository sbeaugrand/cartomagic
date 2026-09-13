#include "BandSelectorsView.hpp"

BandSelectorsView::BandSelectorsView(ProjectParams *_params, BandModel *_model, QWidget *parent) : QWidget(parent) {
  
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

  connect(model,SIGNAL(allWarpsChanged()),this,SLOT(updateView()));
}

void BandSelectorsView::updateLayout() {

  pickWidth = params->pickWidth;
  marginUp  = 5;
  marginDown = 10;
  marginLeft = 10;
  marginRight = 10;

  gapToFirstSel = idWidth + 5; /* vertical gap in pixel between the left margin and the first selector
                      it must be in adequation of the BandView */

  gapNumToSel  = 10; // horizontal gap in pixel between the warp number and the selectors

  numStartX = marginLeft+gapToFirstSel;
  numStartY  = marginUp;
  numEndY = marginUp+numHeight;

  setStartX  = marginLeft+gapToFirstSel;
  selStartY  = numEndY+gapNumToSel;
  selSize = pickWidth-6;

  totalWidth = marginLeft+gapToFirstSel+(params->bandWidth)*pickWidth+marginRight;
  totalHeight = selStartY+pickWidth+marginDown;

}

QSize BandSelectorsView::minimumSizeHint() const {
  return QSize(totalWidth,totalHeight);
}

QSize BandSelectorsView::sizeHint() const {
  return QSize(totalWidth,totalHeight);
}

void BandSelectorsView::updateView() {

  // refresh the whole widget
  // need to resize it since it is not in a layout.
  updateLayout();
  resize(sizeHint());
  update();
}

void BandSelectorsView::updateSelectors(int warpId) {

  update();
}

void BandSelectorsView::changePickWidth(int width) {

  pickWidth = width;  
  update();
}

void BandSelectorsView::paintEvent(QPaintEvent *event) {
  
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
    p.drawText(numStartX+i*pickWidth+(pickWidth-numWidth)/2,numEndY,warpNumber);
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
    if ((model->totallyLockedWarps).at(i)) {
      p.setBrush(Qt::black);
    }
    else if ((model->shapeLockedWarps).at(i)) {
      p.setBrush(Qt::gray);
    }
    else {
      p.setBrush(Qt::NoBrush);
    }
    p.drawRect(setStartX+i*pickWidth+(pickWidth/2)-(selSize/2),selStartY,selSize,selSize);
    sel = (model->selectedWarps).at(i);
    if (sel) {
      pen.setWidth(2);
      p.setPen(pen);
      p.drawLine(setStartX+i*pickWidth+(pickWidth/2)-(selSize/2)+3,selStartY+3,setStartX+i*pickWidth+(pickWidth/2)+(selSize/2)-3,selStartY+selSize-3);
      p.drawLine(setStartX+i*pickWidth+(pickWidth/2)-(selSize/2)+3,selStartY+selSize-3,setStartX+i*pickWidth+(pickWidth/2)+(selSize/2)-3,selStartY+3);
    }    
  }
}

void BandSelectorsView::mousePressEvent(QMouseEvent *event) {

  int x = event->x();
  int y = event->y();

  QPointF p(x,y);
  int warpId;

  if ((x>=(int)setStartX) && (x<(int)(setStartX+pickWidth*params->bandWidth))) {

    warpId = (x-setStartX)/pickWidth;
    x = (x-setStartX)%pickWidth;


    // else click on selectors
    if ((y>=(int)selStartY)&& (y<=(int)(selStartY+selSize))) {
      if ((x>=(pickWidth/2)-(selSize/2))&& (x<=(pickWidth/2)+(selSize/2))) {
        if (!(model->totallyLockedWarps).at(warpId)) {
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
    if ((y>=(int)selStartY)&& (y<=(int)(selStartY+selSize))) {
#ifdef DEBUG_PRINT
      cout << "selected all" << endl;
#endif
      emit allWarpsSelected();
    }
  }

}
