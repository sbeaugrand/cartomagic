/*-==============================================================-

file : MainWindow.hpp

creation date : 31/01/2012

author : S. Domas (sdomas@iut-bm.univ-fcomte.fr)

description : 

supp. infos : saved in UTF-8 [éè]

-==============================================================-*/

#ifndef __MAINWINDOW_HPP__
#define __MAINWINDOW_HPP__

#include <iostream>
#include <fstream>

#include <QtCore>
#include <QtGui>
#include <QMainWindow>
#include <QStackedWidget>

#include "DesignWidget.hpp"
#include "ProjectParams.hpp"
#include "PatternGuesser.hpp"
#include "GttExchanger.hpp"
#include "DispatchControl.hpp"
class DispatchControl;

// defines for menus
#define FILE_MENU (quint8)1
#define WARPS_MENU (quint8)2

// defines for actions
#define NONE_ACT (quint16)0

#define PROJ_NEW (quint16)1
#define PROJ_OPEN (quint16)2
#define PROJ_SAVE (quint16)4
#define PROJ_SAVEAS (quint16)8
#define PROJ_CLOSE (quint16)16
#define PROJ_PRINT (quint16)32
#define PROJ_EXPJPG (quint16)64
#define PROJ_SETPARAMS (quint16)128
#define PROJ_IMPORTGTT (quint16)256
#define PROJ_SETPREFS (quint16)512
#define PROJ_ALL (quint16)1023

#define WARP_COLORIZE (quint16)1
#define WARP_TWIST (quint16)2
#define WARP_COPY (quint16)4
#define WARP_SWAP (quint16)8
#define WARP_GROUPCOPY (quint16)16
#define WARP_GROUPSWAP (quint16)32
#define WARP_INSERT (quint16)64
#define WARP_REMOVE (quint16)128
#define WARP_GUESSIDLE (quint16)256
#define WARP_ADVANCED (quint16)512
#define WARP_LETTERING (quint16)1024
#define WARP_COLORMODE (quint16)2048
#define WARP_ALL (quint16)4095

#define OP_ADD (quint8)0
#define OP_REM (quint8)1
#define OP_RAZ (quint8)2

using namespace std;
using namespace Qt;

class MainWindow : public QMainWindow
{
  Q_OBJECT
  
public:
  MainWindow(ProjectParams *_params, PatternGuesser *_guesser, DispatchControl *_dispatcher, GttExchanger *_xchgGtt);
  void initialize();  

private:

  ProjectParams *params; 
  DesignWidget *design;
  PatternGuesser *guesser;
  GttExchanger *xchgGtt;
  DispatchControl *dispatcher;

  quint16 fileMenuEnb;
  quint16 warpMenuEnb;

  QString projectPath;
  QString lastUsedPath;

  void createActions();
  void createMenus();
  void createToolBars();
  void createConnections();

  void createDesignWidget();
  void destroyDesignWidget();

  void resetCentralWindow();
  void closeEvent(QCloseEvent *event);

  void save(bool askPath);

  QList<int> getSelected();

  /* Menus */
  QMenuBar *allMenuBar;

  QMenu *fileMenu;
  QMenu *warpMenu;
  QMenu *advancedWarpMenu;
  QMenu *patternMenu;

  
  // action for Project menu
  QAction *createProject;
  QAction *openProject;
  QAction *saveProject;  
  QAction *saveAsProject;
  QAction *closeProject;
  QAction *printProject;
  QAction *jpgProject;
  QAction *setParamsProject;
  QAction *importGtt;
  QAction *setPreferences;

  // action for setting up warps
  QAction *insertWarp;
  QAction *removeWarp;
  // QAction *swapWarp;: useless since group swap does the same
  QAction *swapGroupWarp;
  // QAction *copyWarp; : useless since group copy does the same
  QAction *copyGroupWarp;
  // QAction *mirrorWarp; : useless since group copy does the same
  QAction *twistWarp;
  QAction *colorWarp;
  QAction *forceIdleGuess;
  QAction *letteringWarp;
  QAction *coloringStyle;

  // actions for pattern menu
  QAction *changeSizePattern;
  QAction *setPattern;
  QAction *copyPattern;
  QAction *rollPattern;
  QAction *rollUpPattern;
  QAction *rollDownPattern;

  // action to change creation mode
  QAction *switchMode;

  /* tool bar */
  QToolBar *warpToolBar;
  QToolBar *patternToolBar;

  QLabel *labelAppli;

  QStackedWidget *stack;

  QColor bgColor;

public slots:
  void enableFileActions(bool enbMenu, quint16 mask = 0, quint8 op = 0); // default : add nothing
  void enableWarpActions(bool enbMenu, quint16 mask = 0, quint8 op = 0); // default : add nothing
  void enablePatternActions(bool enbMenu);

private slots:

  void slotNewProject();  
  void slotOpenProject();
  void slotSaveProject();
  void slotSaveAsProject();
  void slotCloseProject();  
  void slotPrintProject();
  void slotJpgProject();
  void slotSetParamsProject();
  void slotImportGtt();
  void slotChangeCreationMode();
  void slotSetPreferences();

  // slot related to action on warps/moves/patterns
  void slotInsertWarp();
  void slotRemoveWarp();
  void slotCopyGroupWarp();
  void slotSwapGroupWarp();
  void slotColorWarp();
  void slotTwistWarp();
  void slotLetteringWarp();
  void slotSetColoringStyle();

  void slotRollPattern();
  void slotRollUpPattern();
  void slotRollDownPattern();
  void slotSetPattern();


  /*
  void slotSwapWarp();
  void slotSwapGroupWarp();  
  void slotColorWarp();
  void slotTwistWarp();
  void slotTwistPattern();
  void slotLetteringWarp();
  void slotRollPattern();
  void slotRollUpPattern();
  void slotRollDownPattern();  
  void slotSetColoringStyle();
*/
  void slotForceIdleGuess();

};

#endif //__MAINWINDOW_HPP__
