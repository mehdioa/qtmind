/***********************************************************************
 *
 * Copyright (C) 2013 Omid Nikta <omidnikta@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#include "board.h"
#include "button.h"
#include "pegbox.h"
#include "pinbox.h"
#include "game.h"
#include "message.h"
#include <QMetaType>
#include <QDebug>

Board::Board(const QString &font_name, const int &font_size, QWidget *parent):
	QGraphicsView(parent),
	mState(GameState::None),
	mMode(GameMode::Master),
	mPegNumber(4),
	mColorNumber(6),
	mSameColor(true),
	mSetPins(false),
	mCloseRow(false),
	mAlgorithm(Algorithm::MostParts),
	mFontName(font_name),
	mFontSize(font_size),
	mGame(0)
{
	qRegisterMetaType<Algorithm>("Algorithm");
	auto scene = new QGraphicsScene(this);
	setScene(scene);
	scene->setSceneRect(0, 0, 320, 560);
	fitInView(sceneRect(), Qt::KeepAspectRatio);
	setCacheMode(QGraphicsView::CacheNone);
	setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
	setFrameStyle(QFrame::NoFrame);
	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setMinimumSize(320, 560);
}
//-----------------------------------------------------------------------------

Board::~Board()
{
	if (mGame)
	{
		emit interuptSignal();
		mGame->quit();
		mGame->wait();
		mGame->deleteLater();
	}

	scene()->clear();
}
//-----------------------------------------------------------------------------

void Board::codeRowFilled(const bool &filled)
{
	switch (mMode)
	{
	case GameMode::Breaker://-----------------------------------------
		switch (filled)
		{
		case true:
			mPinBoxes.first()->setBoxState(BoxState::Current);
			mState = GameState::WaittingPinBoxPress;
			mMessage->showMessage("Press The Pin Box ...");
			if (mCloseRow)
				onPinBoxPressed();
			break;
		default:
			mPinBoxes.first()->setBoxState(BoxState::Future);
			mMessage->showMessage(tr("Place Your Pegs ... "));
			break;
		}
		break;
	default://GameMode::Master-------------------------------------
		if (mDoneButton->isEnabled())//the user is not done putting the master code
		{
			switch (filled)
			{
			case true:
				mDoneButton->setVisible(true);
				mMessage->showMessage(tr("Press Done ..."));
				mState = GameState::WaittingDoneButtonPress;
				break;
			default:
				mDoneButton->setVisible(false);
				mMessage->showMessage(tr("Place Your Pegs ... "));
				mState = GameState::None;
				break;
			}
		}
		break;
	}
}
//-----------------------------------------------------------------------------

void Board::createBoxes()
{
	/*	This function creates boxes and put them in QLists. They are
	 *	stored in QList from bottom to top and left to right.
	 */
	mCodeBoxes.clear();
	mPinBoxes.clear();
	mPegBoxes.clear();
	mCurrentBoxes.clear();
	mMasterBoxes.clear();

	QPoint left_bottom_corner(4, 489);

	for (int i = 0; i < MAX_COLOR_NUMBER; ++i)
	{

		QPoint position = left_bottom_corner - QPoint(0, i*40);

		auto pinbox = new PinBox(mPegNumber, position);
		scene()->addItem(pinbox);
		mPinBoxes.append(pinbox);
		connect(pinbox, SIGNAL(pinBoxPressSignal()), this, SLOT(onPinBoxPressed()));

		position.setX(160-20*mPegNumber);

		for (int j = 0; j < mPegNumber; ++j)
		{
			auto pegbox = new PegBox(position+QPoint(j*40, 0));
			scene()->addItem(pegbox);
			mCodeBoxes.append(pegbox);
			pegbox->setPegState(PegState::Invisible);
		}

		position.setX(276);	//go to right corner for the peg boxes

		auto pegbox = new PegBox(position);
		scene()->addItem(pegbox);
		if (i < mColorNumber)
		{
			createPegForBox(pegbox, i, true);//just a background peg
			createPegForBox(pegbox, i);
		}
		pegbox->setPegState(PegState::Initial);
		mPegBoxes.append(pegbox);
	}

	for (int i = 0; i < mPegNumber; ++i)// the last code boxes are for the master code
	{
		auto box = new PegBox(QPoint(160-20*mPegNumber+i*40, 70));
		scene()->addItem(box);
		box->setPegState(PegState::Invisible);
		mMasterBoxes.append(box);
	}
}
//-----------------------------------------------------------------------------

void Board::createPegForBox(PegBox *box, int color, bool backPeg)
{
	QPoint pos = box->sceneBoundingRect().topLeft().toPoint();
	if (box->hasPeg())
	{
		box->setPegColor(color);
	} else
	{
		auto peg = new Peg(pos, color, mIndicator);
		scene()->addItem(peg);
		connect(this, SIGNAL(IndicatorTypeChangeSignal(IndicatorType)), peg, SLOT(onIndicatorChanged(IndicatorType)));

		if(backPeg)
		{
			peg->setMovable(false);
		}
		else
		{
			box->setPeg(peg);
			connect(peg, SIGNAL(mouseReleaseSignal(QPoint, int)), this, SLOT(onPegMouseReleased(QPoint, int)));
			connect(peg, SIGNAL(mouseDoubleClickSignal(Peg*)), this, SLOT(onPegMouseDoubleClicked(Peg*)));
		}
	}
}
//-----------------------------------------------------------------------------

void Board::initializeScene()
{
	mCodeBoxes.clear();
	mPinBoxes.clear();
	mPegBoxes.clear();
	mMasterBoxes.clear();
	mCurrentBoxes.clear();

	scene()->clear();
	setInteractive(true);

	mOkButton = new Button(38, tr("OK"), mFontName, mFontSize);
	scene()->addItem(mOkButton);
	mOkButton->setZValue(2);
	connect(mOkButton, SIGNAL(buttonPressed()), this, SLOT(onOkButtonPressed()));
	mOkButton->setEnabled(false);
	mOkButton->setVisible(false);

	mDoneButton = new Button(160, tr("Done"), mFontName, mFontSize);
	mDoneButton->setPos(79, 118);
	mDoneButton->setVisible(false);
	mDoneButton->setZValue(2);
	mDoneButton->setEnabled(false);
	connect(mDoneButton, SIGNAL(buttonPressed()), this, SLOT(onDoneButtonPressed()));
	scene()->addItem(mDoneButton);

	mMessage = new Message("#303030", mFontName, mFontSize);
	scene()->addItem(mMessage);
	mMessage->setPos(20, 0);

	mInformation = new Message("#808080", mFontName, qMax(mFontSize - 2, 1));
	scene()->addItem(mInformation);
	mInformation->setPos(20, 506);
	mInformation->showMessage(QString("%1: %2   %3: %4   %5: %6").arg(tr("Slots")).
							  arg(mLocale.toString(mPegNumber)).arg(tr("Colors")).
							  arg(mLocale.toString(mColorNumber)).arg(tr("Same Color")).
							  arg(mSameColor ? tr("Yes"): tr("No")));
	createBoxes();
	scene()->update();
}
//-----------------------------------------------------------------------------

void Board::onPegMouseReleased(const QPoint &position, const int &color)
{
	//if same color is not allowed and there is already a color-peg visible, we just ignore drop
	if(!mSameColor)
	{
		foreach (PegBox *box, mCurrentBoxes)
		{
			if(!box->sceneBoundingRect().contains(position) &&
					box->isPegVisible() && box->getPegColor() == color)
				return;
		}
	}

	static bool rowFillState = false;
	bool newRowFillState = true;

	// conversion from float to integer may cause double drop on middle. Flag to do it just once
	bool dropOnlyOnce = true;

	foreach (PegBox *box, mCurrentBoxes)
	{
		if (box->sceneBoundingRect().contains(position) && box->getPegState() != PegState::Initial && dropOnlyOnce)
		{
			dropOnlyOnce = false;

			if(box->hasPeg())
				box->setPegColor(color);
			else
				createPegForBox(box, color);

			box->setPegState(PegState::Visible);
		}//	end if

		if (box->getPegState() != PegState::Visible)
			newRowFillState = false;
	} //	end foreach

	//	if (master) code row state changed, go to codeRowFilled
	if (newRowFillState != rowFillState)
	{
		rowFillState = newRowFillState;
		codeRowFilled(rowFillState);
	}
}
//-----------------------------------------------------------------------------

void Board::onPegMouseDoubleClicked(Peg *peg)
{
	foreach (PegBox *box, mCurrentBoxes)
	{
		if (!box->hasPeg() || box->getPegState() == PegState::Invisible)
		{
			peg->setPos(box->sceneBoundingRect().topLeft().toPoint());
			break;
		}
	}
}
//-----------------------------------------------------------------------------

void Board::onPinBoxPressed()
{
	/*	This function is the running engine of the Breaker mode.
	 *	If b and c are the number of blacks and whites, then
	 *			w = sum(min(c[i], g[i]) -b
	 *	where c[i] and g[i] are the number of times that the color i
	 *	appears in code/guess, and sum runs over all colors.
	 *	http://mathworld.wolfram.com/Mastermind.html
	 */
	mState = GameState::Running;

	mGuess = "";
	for(int i = 0; i < mPegNumber; ++i)
		mGuess.append(QString::number(mCurrentBoxes.at(i)->getPegColor()));

	mPinBoxes.first()->setPins(mMasterCode, mGuess, mColorNumber);
	bool isDone = (mPinBoxes.first()->getValue() == (mPegNumber + 1)*(mPegNumber + 2)/2 - 1);
	mPinBoxes.first()->setBoxState(BoxState::Past);
	mPinBoxes.removeFirst();	// the pinbox goes to past

	//old row of codes goes to past and are removed from current
	for(int i = 0; i < mPegNumber; ++i)
	{
		mCurrentBoxes.first()->setBoxState(BoxState::Past);
		mCurrentBoxes.removeFirst();
	}

	if (isDone) //success here
	{
		mState = GameState::Win;
		mMessage->showMessage(tr("Success! You Win"));

		setBoxStateOfList(&mMasterBoxes, BoxState::Past);
		setBoxStateOfList(&mCurrentBoxes, BoxState::Past);
		setBoxStateOfList(&mPinBoxes, BoxState::Future);
		setBoxStateOfList(&mPegBoxes, BoxState::Future);

	}
	else if (mCodeBoxes.isEmpty()) //out of more row of codes, a fail
	{
		mState = GameState::Lose;
		mMessage->showMessage(tr("Game Over! You Failed"));
		setBoxStateOfList(&mMasterBoxes, BoxState::Past);
		setBoxStateOfList(&mCurrentBoxes, BoxState::Past);
		setBoxStateOfList(&mPinBoxes, BoxState::Future);
		setBoxStateOfList(&mPegBoxes, BoxState::Future);
	}
	else // continue the game
	{
		mMessage->showMessage(tr("Place Your Pegs ... "));
		for(int i = 0; i < mPegNumber; ++i) //put the next codeboxes in action
		{
			mCurrentBoxes.append(mCodeBoxes.first());
			mCodeBoxes.first()->setBoxState(BoxState::Current);
			mCodeBoxes.removeFirst();
			mState = GameState::WaittingPinBoxPress;
		}
	}
}
//-----------------------------------------------------------------------------

void Board::onIndicatorTypeChanged(const IndicatorType &indicator_n)
{
	if(mIndicator != indicator_n)
	{
		mIndicator = indicator_n;
		emit IndicatorTypeChangeSignal(mIndicator);
	}
}
//-----------------------------------------------------------------------------

void Board::onRevealOnePeg()
{
	foreach (PegBox *box, mMasterBoxes)
	{
		if(box->getBoxState() != BoxState::Past)
		{
			if (box == mMasterBoxes.last())
			{
				mState = GameState::Running;
				onResigned();
			}
			else
			{
				box->setBoxState(BoxState::Past);
				return;
			}
		}
	}
}
//-----------------------------------------------------------------------------

void Board::onResigned()
{
	if (mMode == GameMode::Breaker && (mState == GameState::Running || mState == GameState::WaittingPinBoxPress))
	{
		mState = GameState::None;
		mMessage->showMessage(tr("You Resign"));

		setBoxStateOfList(&mMasterBoxes, BoxState::Past);
		setBoxStateOfList(&mCurrentBoxes, BoxState::Future);
		setBoxStateOfList(&mPinBoxes, BoxState::Future);
		setBoxStateOfList(&mPegBoxes, BoxState::Future);
	}
}
//-----------------------------------------------------------------------------

void Board::onOkButtonPressed()
{
	/*	passing user inputed blacks and whites to mGame */
	int resp = mPinBoxes.first()->getValue();
	if(!mGame->setResponse(resp))
	{
		mMessage->showMessage(tr("Not Possible, Try Again..."));
		return;
	}

	mOkButton->setVisible(false);

	/*	we are done with the pinbox. Make it past and remove it from mPinBoxes */
	mPinBoxes.first()->setBoxState(BoxState::Past);
	mPinBoxes.removeFirst();

	/*	Here we check if the user inputs 4 blacks*/
	if (mGame->done())
	{
		mMessage->showMessage(tr("Ready To Play"));
		mState = GameState::None;
		return;
	}

	mMessage->showMessage(tr("Let Me Think..."));
	mState = GameState::Guessing;

	emit startGuessingSignal(mAlgorithm);
}
//-----------------------------------------------------------------------------

void Board::onDoneButtonPressed()
{
	mState = GameState::Running;

	//	we are done with the done button
	mDoneButton->setVisible(false);
	mDoneButton->setEnabled(false);

	setBoxStateOfList(&mCurrentBoxes, BoxState::Past);	//	master code boxes become past for no interaction
	setBoxStateOfList(&mPegBoxes, BoxState::Future);	//	freezing the peg boxes

	mMasterCode = "";
	foreach(PegBox *box, mCurrentBoxes)
		mMasterCode.append(QString::number(box->getPegColor()));

	mCurrentBoxes.clear();
	mMessage->showMessage(tr("Let Me Think..."));

	mState = GameState::Guessing;
	emit startGuessingSignal(mAlgorithm);
}
//-----------------------------------------------------------------------------

void Board::onGuessReady(const Algorithm &alg, const QString &guess,
						 const int &possibleSize, const qreal &lastWeight)
{
	mState = GameState::Running;
	mGuess = guess;
	showTranslatedInformation(alg, possibleSize, lastWeight);

	if (mCodeBoxes.isEmpty())
	{
		mState = GameState::Lose; // Hehe, I can solve it always
		mMessage->showMessage(tr("I Lose"));
		return;
	}
	else
	{
		for(int i = 0; i < mPegNumber; ++i)
		{
			createPegForBox(mCodeBoxes.first(), mGuess[i].digitValue());
			mCodeBoxes.first()->setBoxState(BoxState::Past);
			mCodeBoxes.removeFirst();
		}
		mMessage->showMessage(tr("Please Put Pins And Press OK"));
		mPinBoxes.first()->setBoxState(BoxState::None);
		mOkButton->setEnabled(true);
		mOkButton->setVisible(true);
		mOkButton->setPos(mPinBoxes.first()->pos()-QPoint(0, 39));
	}

	mState = GameState::WaittingOkButtonPress;

	if (mSetPins)
	{
		mPinBoxes.first()->setPins(mMasterCode, mGuess, mColorNumber);
	}
}

//-----------------------------------------------------------------------------

void Board::play()
{
	if (mGame)
	{
		emit interuptSignal();
		mGame->quit();
		mGame->wait();
	}
	initializeScene();
	mState = GameState::None;
	switch (mMode) {
	case GameMode::Master:
		playCodeMaster();
		break;
	default:
		playCodeBreaker();
		break;
	}
}
//-----------------------------------------------------------------------------

void Board::playCodeMaster()
{
	mDoneButton->setZValue(2);
	mDoneButton->setVisible(false);
	mDoneButton->setEnabled(true);

	if (!mGame)
	{
		mGame = new Game(mPegNumber, mColorNumber, mSameColor, this);
		connect(mGame, SIGNAL(guessDoneSignal(Algorithm, QString, int, qreal)),
				this, SLOT(onGuessReady(Algorithm, QString, int, qreal)));
		connect(this, SIGNAL(startGuessingSignal(Algorithm)), mGame, SLOT(onStartGuessing(Algorithm)));
		connect(this, SIGNAL(resetGameSignal(int,int,bool)), mGame, SLOT(onReset(int,int,bool)));
		connect(this, SIGNAL(interuptSignal()), mGame, SLOT(onInterupt()));
	}
	emit interuptSignal();
	emit resetGameSignal(mPegNumber, mColorNumber, mSameColor);

	mMessage->showMessage(tr("Place Your Pegs ... "));

	for(int i = 0; i < mPegNumber; ++i) //initializing currentrow
	{
		mCurrentBoxes.append(mMasterBoxes.first());
		mMasterBoxes.first()->setBoxState(BoxState::Current);
		mMasterBoxes.removeFirst();
	}

	mState = GameState::WaittingFirstRowFill;
	/*	Nothing happening from here till the user fill the master code
	 *	row and press the done button. After the done button is pressed,
	 *	the onDoneButtonPressed is continuing the game
	 */
}
//-----------------------------------------------------------------------------

void Board::playCodeBreaker()
{
	QString digits = "0123456789";
	digits.left(mColorNumber);
	int remainingNumbers = mColorNumber;
	qsrand(time(NULL));
	mMasterCode = "";

	foreach (PegBox *box, mMasterBoxes) //creating a master code to be guessed
	{
		int color = qrand() % remainingNumbers;
		int realcolor = digits.at(color).digitValue();
		mMasterCode.append(QString::number(realcolor));
		createPegForBox(box, realcolor);
		box->setPegState(PegState::Invisible);
		box->setBoxState(BoxState::None);
		if(!mSameColor)
		{
			digits.remove(color, 1);
			--remainingNumbers;
		}
	}

	for(int i = 0; i < mPegNumber; ++i) //initializing currentrow
	{
		mCurrentBoxes.append(mCodeBoxes.first());
		mCodeBoxes.first()->setBoxState(BoxState::Current);
		mCodeBoxes.removeFirst();
	}

	mMessage->showMessage(tr("Place Your Pegs ... "));
	mState = GameState::WaittingFirstRowFill;

	// from now on the onPinBoxPushed function continue the game, after the code row is filled
}
//-----------------------------------------------------------------------------

void Board::reset(const int &peg_n, const int &color_n, const GameMode &mode_n, const bool &samecolor,
				  const Algorithm &algorithm_n, const bool &set_pins, const bool &close_row, QLocale locale_n,
				  const IndicatorType &indicator_n)
{
	mPegNumber = peg_n;
	mColorNumber = color_n;
	mSetPins = set_pins;
	mCloseRow = close_row;
	mMode = mode_n;
	mIndicator = indicator_n;
	mAlgorithm = algorithm_n;
	mLocale = locale_n;
	mSameColor = samecolor;
}
//-----------------------------------------------------------------------------

void Board::resizeEvent(QResizeEvent *event)
{
	fitInView(sceneRect(), Qt::KeepAspectRatio);
	QGraphicsView::resizeEvent(event);
}

//-----------------------------------------------------------------------------

void Board::setAlgorithm(const Algorithm &algorithm_n)
{
	mAlgorithm = algorithm_n;
}
//-----------------------------------------------------------------------------

void Board::setBoxStateOfList(QList<PegBox *> *boxlist, const BoxState &state_t)
{
	foreach (PegBox *box, *boxlist)
	{
		box->setBoxState(state_t);
	}
}
//-----------------------------------------------------------------------------

void Board::setBoxStateOfList(QList<PinBox *> *boxlist, const BoxState &state_t)
{
	foreach (PinBox *box, *boxlist)
	{
		box->setBoxState(state_t);
	}
}
//-----------------------------------------------------------------------------

void Board::showTranslatedInformation(const Algorithm &alg, const int &possibleSize, const qreal &minWeight)
{

	if (possibleSize == 1)
	{
		mInformation->showMessage(tr("The Code Is Cracked!"));
	}
	else if (possibleSize > 10000)
	{
		mInformation->showMessage(QString("%1    %2: %3").arg(tr("Random Guess")).
							  arg(tr("Remaining")).arg(mLocale.toString(possibleSize)));
	}
	else
	{
		switch (alg) {
		case Algorithm::MostParts:
			mInformation->showMessage(QString("%1: %2    %3: %4").arg(tr("Most Parts")).
									  arg(mLocale.toString(minWeight)).arg(tr("Remaining")).
									  arg(mLocale.toString(possibleSize)));
			break;
		case Algorithm::WorstCase:
			mInformation->showMessage(QString("%1: %2    %3: %4").arg(tr("Worst Case")).
									  arg(mLocale.toString(minWeight)).arg(tr("Remaining")).
									  arg(mLocale.toString(possibleSize)));
			break;
		default:
			mInformation->showMessage(QString("%1: %2    %3: %4").arg(tr("Expected Size")).
									  arg(mLocale.toString(minWeight)).arg(tr("Remaining")).
									  arg(mLocale.toString(possibleSize)));
			break;
		}

	}

}
//-----------------------------------------------------------------------------

void Board::autoPutPinsCloseRow(const bool &set_pins, const bool &closeRow)
{
	mSetPins = set_pins;
	mCloseRow = closeRow;
}
//-----------------------------------------------------------------------------

void Board::drawBackground(QPainter *painter, const QRectF &rect)
{
	painter->fillRect(rect, QColor(182, 182, 182));// set scene background color

	QLinearGradient topgrad(0, 16, 0, 129);
	topgrad.setColorAt(0.0, QColor(248, 248, 248));
	topgrad.setColorAt(0.6, QColor(184, 184, 184));
	topgrad.setColorAt(1, QColor(212, 212, 212));
	QBrush mTopGrad(topgrad);

	QLinearGradient botgrad(0, 530, 0, 557);
	botgrad.setColorAt(0.0, QColor(204, 204, 204));
	botgrad.setColorAt(0.3, QColor(206, 206, 206));
	botgrad.setColorAt(1.0, QColor(180, 180, 180));
	QBrush mBotGrad(botgrad);

	QLinearGradient lgrad(0, 190, 320, 370);
	lgrad.setColorAt(0.0, QColor(255, 255, 255, 160));
	lgrad.setColorAt(0.49, QColor(255, 255, 255, 160));
	lgrad.setColorAt(0.50, QColor(0, 0, 0, 128));
	lgrad.setColorAt(1.0, QColor(0, 0, 0, 128));
	QPen mFramePen = QPen(QBrush(lgrad), 1);

	QColor mPend(100, 100, 100);
	QColor mPenl(236, 236, 236);
	QColor mGrad0(204, 204, 204);
	QColor mGrad1(252, 252, 252);

	mPend.setAlpha(50);
	mPenl.setAlpha(80);
	mGrad0.setAlpha(50);
	mGrad1.setAlpha(50);

	painter->setPen(Qt::NoPen);

	QRectF cr(3, 3, 314, 554);
	QPainterPath cpath;
	cpath.addRoundedRect(cr, 10.1, 10.1);
	painter->setClipPath(cpath);
	painter->setClipping(true);

	painter->setBrush(mTopGrad);
	painter->drawRect(QRect(4, 4, 312, 125));
	painter->setBrush(QBrush(QColor(112, 112, 112)));
	painter->drawRect(QRect(4, 128, 318, 1));

	painter->setPen(Qt::NoPen);

	QLinearGradient rowgrad(4, 129, 4, 169);
	rowgrad.setColorAt(0.0, QColor(154, 154, 154));
	rowgrad.setColorAt(1.0, QColor(143, 143, 143));
	rowgrad.setSpread(QGradient::RepeatSpread);
	painter->setBrush(QBrush(rowgrad));
	painter->drawRect(QRectF(4, 129, 318, 400));

	painter->setPen(QColor(154, 154, 154).darker(115));
	for(int i = 0; i < 11; ++i)
	{
		painter->drawLine(5, 128+i*40, 321, 128+i*40);
	}

	painter->setBrush(mBotGrad);
	painter->drawRect(QRect(1, 528, 318, 28));
	painter->setBrush(QBrush(QColor(239, 239, 239)));

	painter->setClipping(false);

	QRectF rightShadow(3.5, 3.5, 313, 553);
	painter->setBrush(Qt::NoBrush);
	painter->setPen(mFramePen);
	painter->drawRoundedRect(rightShadow, 9.8, 9.8);

	QLinearGradient solgrad(50, 70, 50, 108);
	solgrad.setColorAt(0.0, QColor(154, 154, 154));
	solgrad.setColorAt(1.0, QColor(148, 148, 148));
	QBrush solBgBrush(solgrad);

	QLinearGradient framegrad = QLinearGradient(50, 70, 50, 110);
	framegrad.setColorAt(0.0, QColor(78, 78, 78));
	framegrad.setColorAt(1.0, QColor(235, 235, 235));
	QPen solFramePen(QBrush(framegrad), 1);

	painter->setBrush(solBgBrush);
	painter->setPen(solFramePen);

	QRectF codeRowContainer(41, 68, 235, 42);
	QRectF mRectC(42, 69, 235, 41.5);

	painter->drawRoundedRect(codeRowContainer, 21,21);
	painter->setBrush(solgrad);
	painter->drawRoundedRect(mRectC, 20, 20);
	painter->setRenderHint(QPainter::TextAntialiasing, true);
}
