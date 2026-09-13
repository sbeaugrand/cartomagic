// fichier enregisté en UTF-8

#include "MainWindow.hpp"
#include "NewProjectDialog.hpp"
#include "ParamsProjectDialog.hpp"
#include "PreferencesDialog.hpp"
#include "InsertWarpDialog.hpp"
#include "RemoveWarpDialog.hpp"
#include "ColorWarpDialog.hpp"
#include "TwistWarpDialog.hpp"
#include "SetPatternDialog.hpp"
#include "RollWarpDialog.hpp"
#include "CopyWarpDialog.hpp"
#include "SwapGroupWarpDialog.hpp"
#include "CopyGroupWarpDialog.hpp"
#include "MirrorWarpDialog.hpp"
#include "LetteringWarpDialog.hpp"
#include "ColoringStyleDialog.hpp"


MainWindow::MainWindow(ProjectParams *_params, PatternGuesser *_guesser, DispatchControl *_dispatcher, GttExchanger *_xchgGtt) : QMainWindow() {

  params = _params;
  guesser = _guesser;
  xchgGtt = _xchgGtt;
  dispatcher = _dispatcher;
  design = NULL;

  projectPath = "";
  lastUsedPath = "";

  stack = new QStackedWidget();

  createActions();
  createMenus();
  createToolBars();  
  createConnections();

  labelAppli = new QLabel(tr("Cart'o Magic : guess cards threading for you !"));

  stack = new QStackedWidget();
  stack->addWidget(labelAppli);

  initialize();

  setCentralWidget(stack);

  resize(500,300);

}

void MainWindow::initialize() {
  params->clear();

  stack->setCurrentIndex(0);
  enableFileActions(true,PROJ_NEW | PROJ_OPEN | PROJ_IMPORTGTT, OP_RAZ);
  enableWarpActions(false, WARP_ALL,OP_RAZ);
  enablePatternActions(false);

  warpToolBar->setEnabled(false);
  patternToolBar->setEnabled(false);

  stack->setCurrentIndex(0);

  resize(500,300);
}

void MainWindow::enableFileActions(bool enbMenu, quint16 mask, quint8 op) {
  if (enbMenu) {
    fileMenu->setEnabled(true);
  }
  else {
    fileMenu->setEnabled(false);
  }

  if (op == OP_ADD) {
    fileMenuEnb = fileMenuEnb | mask;
  }
  else if (op == OP_REM) {
    fileMenuEnb = (fileMenuEnb | mask) ^ mask;
  }
  else if (op == OP_RAZ) {
    fileMenuEnb = mask;
  }


  if (fileMenuEnb & PROJ_NEW) {
    createProject->setEnabled(true);    
  }
  else {
    createProject->setEnabled(false);
  }
  if (fileMenuEnb & PROJ_OPEN) {
    openProject->setEnabled(true);
  }
  else {
    openProject->setEnabled(false);
  }
  if (fileMenuEnb & PROJ_SAVE) {
    saveProject->setEnabled(true);
  }
  else {
    saveProject->setEnabled(false);
  }
  if (fileMenuEnb & PROJ_SAVEAS) {
    saveAsProject->setEnabled(true);
  }
  else {
    saveAsProject->setEnabled(false);
  }
  if (fileMenuEnb & PROJ_CLOSE) {
    closeProject->setEnabled(true);
  }
  else {
    closeProject->setEnabled(false);
  }
  if (fileMenuEnb & PROJ_PRINT) {
    printProject->setEnabled(true);
  }
  else {
    printProject->setEnabled(false);
  }
  if (fileMenuEnb & PROJ_EXPJPG) {
    jpgProject->setEnabled(true);
  }
  else {
    jpgProject->setEnabled(false);
  }
  if (fileMenuEnb & PROJ_SETPARAMS) {
    setParamsProject->setEnabled(true);
  }
  else {
    setParamsProject->setEnabled(false);
  }
  if (fileMenuEnb & PROJ_IMPORTGTT) {
    importGtt->setEnabled(true);
  }
  else {
    importGtt->setEnabled(false);
  }
  if (fileMenuEnb & PROJ_SETPREFS) {
    setPreferences->setEnabled(true);
  }
  else {
    setPreferences->setEnabled(false);
  }
}

void MainWindow::enableWarpActions(bool enbMenu, quint16 mask, quint8 op) {
  if (enbMenu) {
    warpMenu->setEnabled(true);
  }
  else {
    warpMenu->setEnabled(false);
  }

  if (op == OP_ADD) {
    warpMenuEnb = warpMenuEnb | mask;
  }
  else if (op == OP_REM) {
    warpMenuEnb = (warpMenuEnb | mask) ^ mask;
  }
  else if (op == OP_RAZ) {
    warpMenuEnb = mask;
  }

  if (warpMenuEnb & WARP_INSERT) {
    insertWarp->setEnabled(true);
  }
  else {
    insertWarp->setEnabled(false);
  }
  if (warpMenuEnb & WARP_REMOVE) {
    removeWarp->setEnabled(true);
  }
  else {
    removeWarp->setEnabled(false);
  }
  /* if (warpMenuEnb & WARP_COPY) {
    copyWarp->setEnabled(true);
  }
  else {
    copyWarp->setEnabled(false);
  } */
  if (warpMenuEnb & WARP_GROUPCOPY) {
    copyGroupWarp->setEnabled(true);
  }
  else {
    copyGroupWarp->setEnabled(false);
  }
  /*
  if (warpMenuEnb & WARP_SWAP) {
    swapWarp->setEnabled(true);
  }
  else {
    swapWarp->setEnabled(false);
  }
  */
  if (warpMenuEnb & WARP_GROUPSWAP) {
    swapGroupWarp->setEnabled(true);
  }
  else {
    swapGroupWarp->setEnabled(false);
  }
  if (warpMenuEnb & WARP_TWIST) {
    twistWarp->setEnabled(true);
  }
  else {
    twistWarp->setEnabled(false);
  }
  if (warpMenuEnb & WARP_COLORIZE) {
    colorWarp->setEnabled(true);
  }
  else {
    colorWarp->setEnabled(false);
  }
  if (warpMenuEnb & WARP_GUESSIDLE) {
    forceIdleGuess->setEnabled(true);
  }
  else {
    forceIdleGuess->setEnabled(false);
  }
  if (warpMenuEnb & WARP_ADVANCED) {
    advancedWarpMenu->setEnabled(true);
  }
  else {
    advancedWarpMenu->setEnabled(false);
  }
  if (warpMenuEnb & WARP_COLORMODE) {
    coloringStyle->setEnabled(true);
  }
  else {
    coloringStyle->setEnabled(false);
  }
  if (warpMenuEnb & WARP_LETTERING) {
    letteringWarp->setEnabled(true);
  }
  else {
    letteringWarp->setEnabled(false);
  }
}

void MainWindow::enablePatternActions(bool enbMenu) {

  if (enbMenu) {
    patternMenu->setEnabled(true);
  }
  else {
    patternMenu->setEnabled(false);
  }
}

/******************************************/

void MainWindow::createActions()
{

  //=============== FOR FILE MENU ==================
  createProject = new QAction(QIcon(":/icons/filenew.png"),tr("Create"),this);
  createProject->setIconVisibleInMenu(true);
  //createProject->setShortcut(QKeySequence("Ctrl+N"));
  createProject->setStatusTip(tr("Create a new project"));
  connect(createProject, SIGNAL(triggered()), this, SLOT(slotNewProject()));

  openProject = new QAction(QIcon(":/icons/fileopen.png"),tr("Open"), this);
  openProject->setIconVisibleInMenu(true);
  //openProject->setShortcut(QKeySequence("Ctrl+O"));
  openProject->setStatusTip(tr("Open an existing project"));
  connect(openProject, SIGNAL(triggered()), this, SLOT(slotOpenProject()));

  saveProject = new QAction(QIcon(":/icons/filesave.png"),tr("Save"), this);
  saveProject->setIconVisibleInMenu(true);
  //saveProject->setShortcut(QKeySequence("Ctrl+S"));
  saveProject->setStatusTip(tr("Save the current project"));
  connect(saveProject, SIGNAL(triggered()), this, SLOT(slotSaveProject()));

  saveAsProject = new QAction(QIcon(":/icons/filesaveas.png"),tr("Save As"), this);
  saveAsProject->setIconVisibleInMenu(true);
  //saveProject->setShortcut(QKeySequence("Ctrl+S"));
  saveAsProject->setStatusTip(tr("Save the current project under a new name"));
  connect(saveAsProject, SIGNAL(triggered()), this, SLOT(slotSaveAsProject()));

  closeProject = new QAction(QIcon(":/icons/fileclose.png"),tr("Close"), this);
  closeProject->setIconVisibleInMenu(true);
  //closeProject->setShortcut(QKeySequence("Ctrl+W"));
  closeProject->setStatusTip(tr("Close the current project"));
  connect(closeProject, SIGNAL(triggered()), this, SLOT(slotCloseProject()));

  printProject = new QAction(QIcon(":/icons/print.png"),tr("Print"), this);
  printProject->setIconVisibleInMenu(true);
  //printProject->setShortcut(QKeySequence("Ctrl+W"));
  printProject->setStatusTip(tr("Print rotations, threading and band of the current project"));
  connect(printProject, SIGNAL(triggered()), this, SLOT(slotPrintProject()));

  jpgProject = new QAction(QIcon(":/icons/exportjpg.png"),tr("Export as images"), this);
  jpgProject->setIconVisibleInMenu(true);
  //printProject->setShortcut(QKeySequence("Ctrl+W"));
  jpgProject->setStatusTip(tr("Save rotations, threading and band of the current project in jpg images"));
  connect(jpgProject, SIGNAL(triggered()), this, SLOT(slotJpgProject()));

  setParamsProject = new QAction(QIcon(":/icons/params.png"),tr("Project parameters"), this);
  setParamsProject->setIconVisibleInMenu(true);
  setParamsProject->setStatusTip(tr("Set values for some parameters of the project"));
  connect(setParamsProject, SIGNAL(triggered()), this, SLOT(slotSetParamsProject()));

  importGtt = new QAction(QIcon(":/icons/fileopen.png"),tr("Import Gtt file"), this);
  importGtt->setIconVisibleInMenu(true);
  importGtt->setStatusTip(tr("Import a Gtt file"));
  connect(importGtt, SIGNAL(triggered()), this, SLOT(slotImportGtt()));

  setPreferences = new QAction(QIcon(":/icons/params.png"),tr("Options"), this);
  setPreferences->setIconVisibleInMenu(true);
  setPreferences->setStatusTip(tr("Set values for some options"));
  connect(setPreferences, SIGNAL(triggered()), this, SLOT(slotSetPreferences()));

  //=============== FOR WARP MENU ==================

  colorWarp = new QAction(QIcon(":/icons/warp-colorize.png"),tr("Colorize"),this);
  colorWarp->setIconVisibleInMenu(true);
  //colorWarp->setShortcut(QKeySequence("Ctrl+I"));
  colorWarp->setStatusTip(tr("paint all picks of selected warps with the current color"));

  twistWarp = new QAction(QIcon(":/icons/warp-twist.png"),tr("Twist"),this);
  twistWarp->setIconVisibleInMenu(true);
  //twistWarp->setShortcut(QKeySequence("Ctrl+T"));
  twistWarp->setStatusTip(tr("Twist all picks of selected warps"));

  insertWarp = new QAction(QIcon(":/icons/warp-insert.png"),tr("Insert"),this);
  insertWarp->setIconVisibleInMenu(true);
  //insertWarp->setShortcut(QKeySequence("Ctrl+I"));
  insertWarp->setStatusTip(tr("Insert warps before or after selected warps"));

  removeWarp = new QAction(QIcon(":/icons/warp-remove.png"),tr("Remove"),this);
  removeWarp->setIconVisibleInMenu(true);
  //removeWarp->setShortcut(QKeySequence("Ctrl+R"));
  removeWarp->setStatusTip(tr("Remove selected warps"));

  // swapWarp = new QAction(QIcon(":/icons/warp-swap.png"),tr("Swap"),this);
  // swapWarp->setIconVisibleInMenu(true);
  // swapWarp->setShortcut(QKeySequence("Ctrl+S"));
  // swapWarp->setStatusTip(tr("Swap two selected warps"));

  swapGroupWarp = new QAction(QIcon(":/icons/warp-groupswap.png"),tr("Swap"),this);
  swapGroupWarp->setIconVisibleInMenu(true);
  //swapGroupWarp->setShortcut(QKeySequence("Ctrl+S"));
  swapGroupWarp->setStatusTip(tr("Swap warps/rotations"));

  // copyWarp = new QAction(QIcon(":/icons/warp-copy.png"),tr("Copy"),this);
  // copyWarp->setIconVisibleInMenu(true);
  // copyWarp->setShortcut(QKeySequence("Ctrl+C"));
  // copyWarp->setStatusTip(tr("Copy warp(s) to other"));

  copyGroupWarp = new QAction(QIcon(":/icons/warp-groupcopy.png"),tr("Copy"),this);
  copyGroupWarp->setIconVisibleInMenu(true);
  //copyGroupWarp->setShortcut(QKeySequence("Ctrl+C"));
  copyGroupWarp->setStatusTip(tr("Copy warps/rotations to others"));


  //mirrorWarp = new QAction(QIcon(":/icons/warp-mirror.png"),tr("Mirroring"),this);
  //mirrorWarp->setIconVisibleInMenu(true);
  //copyGroupWarp->setShortcut(QKeySequence("Ctrl+C"));
  //mirrorWarp->setStatusTip(tr("Swap symetrically several warps"));

  forceIdleGuess = new QAction(QIcon(":/icons/guess-idle.png"),tr("Guess with idles"),this);
  forceIdleGuess->setIconVisibleInMenu(true);
  //copyWarp->setShortcut(QKeySequence("Ctrl+C"));
  forceIdleGuess->setStatusTip(tr("Guess where are idles when auto-guess is desactivated"));

  letteringWarp = new QAction(QIcon(":/icons/warp-lettering.png"),tr("Pick lettering"),this);
  letteringWarp->setIconVisibleInMenu(true);
  //copyGroupWarp->setShortcut(QKeySequence("Ctrl+C"));
  letteringWarp->setStatusTip(tr("Set if hole letters are shown on picks or not"));

  coloringStyle = new QAction(QIcon(":/icons/warp-colorize.png"),tr("Set coloring style"),this);
  coloringStyle->setIconVisibleInMenu(true);
  //copyGroupWarp->setShortcut(QKeySequence("Ctrl+C"));
  coloringStyle->setStatusTip(tr("Set the coloring style of warps"));

  //=============== FOR PATTERN MENU ==================
  rollPattern = new QAction(QIcon(":/icons/warp-roll.png"),tr("Roll up/down"),this);
  rollPattern->setIconVisibleInMenu(true);
  //rollPattern->setShortcut(QKeySequence("Ctrl+O"));
  rollPattern->setStatusTip(tr("Roll up/down X times selected warps/rotations"));

  rollUpPattern = new QAction(QIcon(":/icons/warp-rollup.png"),tr("Roll up"),this);
  rollUpPattern->setIconVisibleInMenu(true);
  rollUpPattern->setStatusTip(tr("Roll up one time selected warps/rotations"));

  rollDownPattern = new QAction(QIcon(":/icons/warp-rolldown.png"),tr("Roll down"),this);
  rollDownPattern->setIconVisibleInMenu(true);
  rollDownPattern->setStatusTip(tr("Roll down one time selected warps/rotations"));

  setPattern = new QAction(QIcon(":/icons/pattern-twist.png"),tr("Set pattern twists"),this);
  setPattern->setIconVisibleInMenu(true);
  //twistPatterns->setShortcut(QKeySequence("Ctrl+T"));
  setPattern->setStatusTip(tr("Set the twist for each pick of the pattern, for selected warps"));

  //=============== FOR CREATION MODE ==================
  switchMode = new QAction(tr("Rotations click mode"),this);
  switchMode->setStatusTip(tr("Change to click-on-rotations mode"));
  connect(switchMode,SIGNAL(triggered()),this,SLOT(slotChangeCreationMode()));
}

void MainWindow::createMenus() {
  allMenuBar = menuBar();

  fileMenu = allMenuBar->addMenu(tr("Project"));
  warpMenu = allMenuBar->addMenu(tr("Warps"));
  advancedWarpMenu = new QMenu(tr("Advanced modes"));
  patternMenu = allMenuBar->addMenu(tr("Pattern"));

  allMenuBar->addAction(switchMode);
  switchMode->setEnabled(false);

  fileMenu->addAction(createProject);
  fileMenu->addAction(openProject);
  fileMenu->addAction(saveProject);
  fileMenu->addAction(saveAsProject);
  fileMenu->addAction(closeProject);
  fileMenu->addSeparator();
  fileMenu->addAction(setParamsProject);
  fileMenu->addAction(setPreferences);
  fileMenu->addSeparator();
  fileMenu->addAction(importGtt);
  fileMenu->addAction(printProject);
  fileMenu->addAction(jpgProject);  

  advancedWarpMenu->addAction(letteringWarp);
  advancedWarpMenu->addAction(coloringStyle);

  warpMenu->addAction(colorWarp);
  warpMenu->addAction(twistWarp);
  warpMenu->addSeparator();
  //warpMenu->addAction(copyWarp);
  //warpMenu->addAction(swapWarp);
  warpMenu->addAction(copyGroupWarp);
  warpMenu->addAction(swapGroupWarp);
  //warpMenu->addAction(mirrorWarp);
  warpMenu->addSeparator();
  warpMenu->addAction(insertWarp);
  warpMenu->addAction(removeWarp);
  warpMenu->addSeparator();
  warpMenu->addAction(forceIdleGuess);
  warpMenu->addSeparator();
  warpMenu->addMenu(advancedWarpMenu);

  patternMenu->addAction(rollPattern);
  patternMenu->addAction(setPattern);

}

void MainWindow::createToolBars() {
  warpToolBar = addToolBar(tr("Warps"));

  warpToolBar->addAction(colorWarp);
  warpToolBar->addAction(twistWarp);

  patternToolBar = addToolBar(tr("Pattern"));
  patternToolBar->addAction(rollUpPattern);
  patternToolBar->addAction(rollDownPattern);
  patternToolBar->addAction(setPattern);
}

void MainWindow::createConnections() {  

  connect(insertWarp, SIGNAL(triggered()), this, SLOT(slotInsertWarp()));
  connect(removeWarp, SIGNAL(triggered()),this, SLOT(slotRemoveWarp()));
  connect(copyGroupWarp, SIGNAL(triggered()), this, SLOT(slotCopyGroupWarp()));
  connect(swapGroupWarp, SIGNAL(triggered()), this, SLOT(slotSwapGroupWarp()));
  // connect(swapWarp, SIGNAL(triggered()), this, SLOT(slotSwapWarp()));
  // connect(copyWarp, SIGNAL(triggered()), this, SLOT(slotCopyWarp()));
  connect(twistWarp, SIGNAL(triggered()), this, SLOT(slotTwistWarp()));
  connect(setPattern, SIGNAL(triggered()), this, SLOT(slotSetPattern()));
  connect(colorWarp, SIGNAL(triggered()), this, SLOT(slotColorWarp()));
  connect(letteringWarp, SIGNAL(triggered()), this, SLOT(slotLetteringWarp()));
  connect(coloringStyle, SIGNAL(triggered()), this, SLOT(slotSetColoringStyle()));
  connect(rollPattern, SIGNAL(triggered()), this, SLOT(slotRollPattern()));
  connect(rollUpPattern, SIGNAL(triggered()), this, SLOT(slotRollUpPattern()));
  connect(rollDownPattern, SIGNAL(triggered()), this, SLOT(slotRollDownPattern()));
  connect(forceIdleGuess,SIGNAL(triggered()), this, SLOT(slotForceIdleGuess()));
}


void MainWindow::closeEvent(QCloseEvent *event) {

  if (params->createMode == MODE_FROMMOVES) {
    event->ignore();
  }
  else {

    if (design != NULL) {

      if (params->mustBeSaved) {

        int ret = QMessageBox::warning(this,tr("Project is not saved"),tr("Project has been modified. would you like to save it ?"),QMessageBox::Save | QMessageBox::Cancel | QMessageBox::Discard,QMessageBox::Save);

        if (ret == QMessageBox::Cancel) {
          event->ignore();
        }
        else {
          if (ret == QMessageBox::Save) {
            slotSaveProject();
          }
          projectPath = "";
          destroyDesignWidget();
          delete guesser;
          delete params;
          delete xchgGtt;
          event->accept();
        }
      }
      else {
        projectPath = "";
        destroyDesignWidget();
        delete guesser;
        delete params;
        delete xchgGtt;
        event->accept();
      }
    }
    else {

      delete guesser;
      delete params;
      delete xchgGtt;
      event->accept();
    }
  }
}

void MainWindow::slotChangeCreationMode() {

  dispatcher->toggleCreationMode();

  QPalette p = this->palette();
  if (params->createMode == MODE_FROMMOVES) {

    // change to FROMMOVES mode
    bgColor = p.color(QPalette::Window);
    switchMode->setText(tr("Band click mode"));
    switchMode->setStatusTip(tr("Change to click-on-band mode"));
    setPattern->setText(tr("Set pattern rotations"));
    setPattern->setStatusTip(tr("Set the rotation F/B for each pick of the pattern, for selected warps"));
    p.setColor(QPalette::Window,QColor(240,240,200));
    enableFileActions(false);
    enableWarpActions(true,WARP_COLORIZE,OP_REM);
    enableWarpActions(true,WARP_GUESSIDLE,OP_REM);
  }
  else if (params->createMode == MODE_FROMBAND) {

    switchMode->setText(tr("Rotations click mode"));
    switchMode->setStatusTip(tr("Change to click-on-rotations mode"));
    setPattern->setText(tr("Set pattern twists"));
    setPattern->setStatusTip(tr("Set the twist for each pick of the pattern, for selected warps"));
    p.setColor(QPalette::Window,bgColor);
    enableFileActions(true);
    enableWarpActions(true,WARP_COLORIZE,OP_ADD);
    enableWarpActions(true,WARP_GUESSIDLE,OP_ADD);
  }
  this->setPalette(p);  

  update();
}

void MainWindow::slotJpgProject() {

#ifdef DEBUG_PRINT
  QString format = "";
  QList<QByteArray> lst = QImageWriter::supportedImageFormats();
  QListIterator<QByteArray> iter(lst);

  while(iter.hasNext()) {

    format.append(QString(iter.next().data()) + ",");

  }
  QMessageBox::about(this,"format image",format);
#endif


  QString where;
  if (lastUsedPath == "") {
    where = QDir::toNativeSeparators(QDir::homePath());
  }
  else {
    where = lastUsedPath;
  }
  lastUsedPath = QFileDialog::getExistingDirectory(this, tr("Open Directory"),where);

  QString jpgName = lastUsedPath;
  jpgName.append("/");
  jpgName.append(params->name);

  QString fileName;

  params->setExportForImage();

  // set width for an A4 landscape in 200 dpi
  int width = (int)(29.7*200/2.54);
  int height = (int)(21.0*200/2.54);
  QImage *img = new QImage(width,height,QImage::Format_RGB888);
  img->setDotsPerMeterX((int)(20000/2.54));
  img->setDotsPerMeterY((int)(20000/2.54));

  params->setExportForPrinter();
  QRect boundingBox;
  QImage imgClip;
  QPainter *painter = new QPainter;

  // save threading
  painter->begin(img);
  painter->setBrush(Qt::white);
  painter->drawRect(0,0,width,height);
  boundingBox = design->threading->model->draw(painter);
  painter->end();

  fileName = jpgName;
  fileName.append("-threading.jpg");
  QImageWriter writerThreading(fileName,"JPG");
  imgClip = img->copy(boundingBox);
  writerThreading.write(imgClip);

  //save moves
  if ((params->movesVertSep).isEmpty()) {
    params->setMovesVertSep(5);
  }
  painter->begin(img);
  painter->setBrush(Qt::white);
  painter->drawRect(0,0,width,height);
  boundingBox = design->moves->model->draw(painter);
  painter->end();

  fileName = jpgName;
  fileName.append("-moves.jpg");
  QImageWriter writerMoves(fileName,"JPG");
  imgClip = img->copy(boundingBox);
  writerMoves.write(imgClip);

  // save band
  painter->begin(img);
  painter->setBrush(Qt::white);
  painter->drawRect(0,0,width,height);
  boundingBox = design->band->model->draw(painter);
  painter->end();

  fileName = jpgName;
  fileName.append("-band.jpg");
  QImageWriter writerBand(fileName,"JPG");
  imgClip = img->copy(boundingBox);
  writerBand.write(imgClip);

  delete img;
  delete painter;
}

void MainWindow::slotPrintProject() {

  QPrinter * printer = new QPrinter();

#ifdef CARTO_LIN
  QString printName;
  if (lastUsedPath == "") {
    printName = QDir::toNativeSeparators(QDir::homePath());
  }
  else {
    printName = lastUsedPath;
  }
  printName.append("/");
  printName.append(params->name);
  printName.append(".pdf");
  printer->setOutputFileName(printName);
  printer->setFullPage(true);
#endif
  printer->setOrientation(QPrinter::Landscape);
  printer->setColorMode(QPrinter::Color);
  printer->setResolution(600);

  QPrintDialog *dialog = new QPrintDialog(printer);
  dialog->setWindowTitle(tr("Print rotations, threading and band"));
  if (dialog->exec() != QDialog::Accepted) {
    return;
  }

  params->setExportForPrinter();
  if ((params->movesVertSep).isEmpty()) {
    params->setMovesVertSep(5);
  }


  QPainter *painter = new QPainter;
  painter->begin(printer);
  design->threading->model->draw(painter);
  printer->newPage();
  design->moves->model->draw(painter);
  printer->newPage();  
  design->band->model->draw(painter);
  painter->end();

  delete painter;
  delete printer;
}

void MainWindow::slotNewProject() {

  params->clear();
  NewProjectDialog *dialog = new NewProjectDialog(params,this);
  int ret = dialog->exec();

  if (ret == 1) {

    if (design != NULL) {
      cerr << "Cannot create design widget" << endl;
    }
    else {
      params->mustBeSaved = true;
      createDesignWidget();
    }
  }
}

void MainWindow::slotSetParamsProject() {

  ParamsProjectDialog *dialog = new ParamsProjectDialog(params,dispatcher,this);
  dialog->exec();
  params->mustBeSaved = true;
}

void MainWindow::slotSetPreferences() {

  PreferencesDialog *dialog = new PreferencesDialog(params,dispatcher,this);
  dialog->exec();
  params->mustBeSaved = true;
}

void MainWindow::slotImportGtt() {

  bool stop = false;

  QString gttFile;
  QString where;
  if (lastUsedPath == "") {
    where = QDir::toNativeSeparators(QDir::homePath());
  }
  else {
    where = lastUsedPath;
  }
  gttFile = QFileDialog::getOpenFileName(this, tr("Select a File"),where,tr("GTT file (*.gtt *.GTT)"));
  lastUsedPath = QFileInfo(gttFile).canonicalPath();
  int ret = xchgGtt->importFromGtt(gttFile);

  if (ret == ERR_GTTFILE) {
    QMessageBox::critical(this,tr("Error on opening gtt file"),xchgGtt->getErrorString());
    stop = true;
  }
  else if (ret == WARN_GTTFILE) {
    QMessageBox::warning(this,tr("Warning on opening gtt file"),xchgGtt->getWarningString());
  }
  if (!stop) {
    createDesignWidget();
    //xchgGtt->printShapes();
    design->band->control->setModel(xchgGtt->warps);
    xchgGtt->clearWarps();
    design->band->model->refresh(true);
    params->mustBeSaved = true;

  }
}

void MainWindow::slotOpenProject() {

  bool stop = false;

  QString where;
  if (lastUsedPath == "") {
    where = QDir::toNativeSeparators(QDir::homePath());
  }
  else {
    where = lastUsedPath;
  }
  projectPath = QFileDialog::getOpenFileName(this, tr("Select a File"),where,tr("Cartomagic Project (*.cmp)"));
  lastUsedPath = QFileInfo(projectPath).canonicalPath();
  QFile file(projectPath);
  if (file.open(QIODevice::ReadOnly)) {
    QDataStream in(&file);
    try {
      in >> *params;
    }
    catch(int errCode) {
      if (errCode == BAD_MAGIC_STRING) {
        QMessageBox::critical(this,tr("Error on opening project file"),tr("The project file is corrupted or not a cartomagic project"));
        stop = true;
        file.close();
        params->clear();
      }
    }


    if (!stop) {
      createDesignWidget();
      in >> *(design->band->model);
      in >> *(design->moves->model); // NB : no effect if file < v0.4
      in >> *(design->threading->model); // NB : no effect if file < v0.4

      /* if file v >= 0.4, just translate warps */
      if ((params->cmpVersionMaj >= 0) && (params->cmpVersionMin >= 4)) {
        design->band->model->refresh(false);
      }
      else { /* refresh and guess all warps */

        design->band->model->refresh(true);
      }

      file.close();
      params->mustBeSaved = false;

    }
  }
}

void MainWindow::slotSaveProject() {

  if (projectPath.isEmpty()) {
    save(true);
  }
  else {
    save(false);
  }
}

void MainWindow::slotSaveAsProject() {

  save(true);
}

void MainWindow::slotCloseProject() {

  if (params->mustBeSaved) {
    int ret = QMessageBox::warning(this,tr("Project is not saved"),tr("Project has been modified. would you like to save it ?"),QMessageBox::Save | QMessageBox::Cancel | QMessageBox::Discard,QMessageBox::Save);

    if (ret != QMessageBox::Cancel) {

      if (ret == QMessageBox::Save) {
        slotSaveProject();
      }
      projectPath = "";
      if (design != NULL) {
        destroyDesignWidget();
      }
      initialize();
    }
  }
  else {
    projectPath = "";
    if (design != NULL) {
      destroyDesignWidget();
    }
    initialize();
  }
}

void MainWindow::save(bool askPath) {

  if (projectPath.isEmpty()) {

    QString where;
    if (lastUsedPath == "") {
      where = QDir::toNativeSeparators(QDir::homePath());
    }
    else {
      where = lastUsedPath;
    }
    projectPath = where.append("/"+params->name);
    projectPath.append(".cmp");
  }


  if (askPath) {
    projectPath = QFileDialog::getSaveFileName(this, tr("Save File"),projectPath,tr("Cartomagic projects (*.cmp)"));
    lastUsedPath = QFileInfo(projectPath).canonicalPath();
  }

  if (! projectPath.isEmpty()) {
    QFile file(projectPath);
    if (file.open(QIODevice::WriteOnly)) {
      QDataStream out(&file);
      out << *params;
      out << *(design->band->model);
      out << *(design->moves->model);
      out << *(design->threading->model);
      file.close();
    }
    params->mustBeSaved = false;
  }

}

void MainWindow::createDesignWidget() {

  enableFileActions(true,PROJ_SAVE | PROJ_SAVEAS | PROJ_PRINT | PROJ_EXPJPG | PROJ_CLOSE | PROJ_SETPARAMS | PROJ_SETPREFS, OP_RAZ);
  enableWarpActions(true);
  enablePatternActions(true);
  switchMode->setEnabled(true);


  warpToolBar->setEnabled(true);
  patternToolBar->setEnabled(true);

  design = new DesignWidget(params);

  dispatcher->setDesign(design);

  stack->addWidget(design);
  stack->setCurrentIndex(1);

  resize (100,100);

}

void MainWindow::destroyDesignWidget() {
  // first return to FROMBAND mode

  if (params->createMode == MODE_FROMMOVES) {
    slotChangeCreationMode();
  }
  switchMode->setEnabled(false);

  stack->removeWidget(design);

  dispatcher->unsetDesign();

  //disconnect(params,SIGNAL(bandWidthChanged()),guesser,SLOT(update()));

  design->deleteLater();
  design = NULL;  

}

QList<int> MainWindow::getSelected() {

  QList<int> selIndex;

  quint32 i;
  bool isSel;

  for(i=0;i<params->bandWidth;i++) {
    if (params->createMode == MODE_FROMMOVES) {
      isSel = (design->moves->model->selectedMoves).at(i);
    }
    else {
      isSel = (design->band->model->selectedWarps).at(i);
    }
    if (isSel) {
      selIndex.append(i);
    }
  }

  return selIndex;
}

/*
    insertWarp() : opens a pop-up to select before or after selected warp
    then calls the appropriated model slot
*/
void MainWindow::slotInsertWarp() {

  QList<int> selIndex = getSelected();
  if (selIndex.size() > 0) {
    BandDialog *dialog = new InsertWarpDialog(params,dispatcher,selIndex);
    dialog->exec();
  }
}

void MainWindow::slotRemoveWarp() {

  QList<int> selIndex = getSelected();
  if (selIndex.size() > 0) {
    BandDialog *dialog = new RemoveWarpDialog(params,dispatcher,selIndex);
    dialog->exec();
  }

}

void MainWindow::slotCopyGroupWarp() {

  BandDialog *dialog = new CopyGroupWarpDialog(params,dispatcher);
  dialog->exec();
}

void MainWindow::slotSwapGroupWarp() {

  BandDialog *dialog = new SwapGroupWarpDialog(params,dispatcher);
  dialog->exec();
}

void MainWindow::slotColorWarp() {

  QList<int> selIndex = getSelected();
  if (selIndex.size() > 0) {
    BandDialog *dialog = new ColorWarpDialog(params,dispatcher,selIndex);
    dialog->exec();
  }
}

void MainWindow::slotTwistWarp() {

  QList<int> selIndex = getSelected();
  if (selIndex.size() > 0) {
    BandDialog *dialog = new TwistWarpDialog(params,dispatcher,selIndex);
    dialog->exec();
  }
}

void MainWindow::slotLetteringWarp() {

  BandDialog *dialog = new LetteringWarpDialog(params,dispatcher);
  dialog->exec();
  dispatcher->showLetterginWarning();
}

void MainWindow::slotSetColoringStyle() {

  BandDialog *dialog = new ColoringStyleDialog(params,dispatcher);
  dialog->exec();
}


void MainWindow::slotRollPattern() {

  QList<int> selIndex = getSelected();
  if (selIndex.size() > 0) {
    BandDialog *dialog = new RollWarpDialog(params,dispatcher,selIndex);
    dialog->exec();
  }
}

void MainWindow::slotRollUpPattern() {

  QList<int> selIndex = getSelected();

  int i;
  int nbSel = selIndex.size();
  int index;

  for(i=nbSel-1;i>=0;i--) {
    index = selIndex.at(i);
    dispatcher->rollWarp(index,1,false);
  }
}

void MainWindow::slotRollDownPattern() {

  QList<int> selIndex = getSelected();

  int i;
  int nbSel = selIndex.size();
  int index;

  for(i=nbSel-1;i>=0;i--) {
    index = selIndex.at(i);
    dispatcher->rollWarp(index,1,true);
  }
}

void MainWindow::slotSetPattern() {

  QList<int> selIndex = getSelected();
  if (selIndex.size() > 0) {
    BandDialog *dialog = new SetPatternDialog(params,dispatcher,selIndex);
    dialog->exec();
  }
}


void MainWindow::slotForceIdleGuess() {

  QList<int> selIndex = getSelected();
  quint32 i;
  int index;

  if (selIndex.size() > 0) {

    for(i=0;i<selIndex.size();i++) {
      index = selIndex.at(i);      
      if ((design->band->model->nbColoredPicks).at(index) == (params->bandLength)-1) {
        guesser->forcedGuessForWarp(index,10);
      }
    }
  }
}


