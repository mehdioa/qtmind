#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class Board;
class QMenu;
class QAction;
class QToolBar;
class QActionGroup;
class QComboBox;
class Board;

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();

protected:
	void closeEvent(QCloseEvent* event); /** Override parent function to save window geometry. */

private slots:
	void newGameSlot();
//	void restartGameSlot();
	void throwInTheTowelSlot();
	void toggleAllowSameColorSlot();
	void setPinsCloseRowAutomatically();
	void doItForMeSlot();
	void codeMasterModeSlot();
	void codeBreakModeSlot();
	void about();

private:
//	void startGame();
	void setPegsComboBox();
	void setColorsComboBox();
	void setSolvingAlgorithmsComboBox();


private:
	Board* gameBoard;
	int m_mode;
	int m_colors;
	int m_pegs;
	int m_algorithm;
	bool m_allowSameColor ;
	bool m_setPins;
	bool m_closeRow;

	void createBoard();
	void createMenuBar();
//	void createToolBar();
	void DrawBoardBackground();

	/* Actions and Menus*/
//	QMenu* gameMenu;
//	QMenu* rowMenu;
//	QMenu* settingsMenu;
//	QMenu* helpMenu;

//	QToolBar* mainToolbar;

//	QAction* newGameAction;
//	QAction* restartGameAction;
//	QAction* throwInTheTowelAction;
//	QAction* fillInTheRowAction;
//	QAction* doItForMeAction;
	QActionGroup* gameModeActions;
	QAction* toggleAllowSameColorAction;
	QAction* setPinsAutomaticallyAction;
	QAction* closeRowAutomaticallyAction;

	QComboBox* pegsNumberComboBox;
	QComboBox* colorsNumberComboBox;
	QComboBox* solvingAlgorithmsComboBox;


};

#endif // MAINWINDOW_H
