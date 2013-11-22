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
#include "peg.h"
#include "pegbox.h"
#include "pinbox.h"
#include "game.h"
#include "message.h"

Board::Board(const QString &font_name, const int &font_size, QWidget* parent):
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
	auto scene = new QGraphicsScene(this);
	setScene(scene);
	scene->setSceneRect(0, 0, 320, 560);
	fitInView(sceneRect(), Qt::KeepAspectRatio);
	scene->setBackgroundBrush(QColor("#B6B6B6"));
	setScene(scene);
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
	scene()->clear();
	if (mGame) delete mGame;
}
//-----------------------------------------------------------------------------

void Board::codeRowFilled(bool filled)
{
	switch (mMode)
	{
	case GameMode::Master://-----------------------------------------
		switch (filled)
		{
		case true:
			mPinBoxes.first()->setBoxState(BoxState::Current);
			mMessage->showMessage("Press The Pin Box ...");
			if (mCloseRow)
				mPinBoxes.first()->fakePress();
			break;
		default:
			mPinBoxes.first()->setBoxState(BoxState::Future);
			mMessage->showMessage(tr("Place Your Pegs ... "));
			break;
		}
		break;
	default://GameMode::Breaker-------------------------------------
		if (mDoneButton->isEnabled())//the user is not done putting the master code
		{
			switch (filled)
			{
			case true:
				mDoneButton->setVisible(true);
				mMessage->showMessage(tr("Press Done ..."));
				break;
			default:
				mDoneButton->setVisible(false);
				mMessage->showMessage(tr("Place Your Pegs ... "));
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
	 *	stored in QList from bottom up and left to right.
	 */
	mCodeBoxes.clear();
	mPinBoxes.clear();
	mPegBoxes.clear();
	mCurrentBoxes.clear();
	mMasterBoxes.clear();

	QPoint left_bottom_corner(4, 490);

	for (int i = 0; i < MAX_COLOR_NUMBER; ++i)
	{

		QPoint position = left_bottom_corner - QPoint(0, i*40);

		auto pinbox = new PinBox(mPegNumber, position);
		scene()->addItem(pinbox);
		mPinBoxes.append(pinbox);
		connect(pinbox, SIGNAL(pinBoxPressed()), this, SLOT(onPinBoxPressed()));

		position.setX(160-20*mPegNumber);

		for (int j = 0; j < mPegNumber; ++j)
		{
			auto pegbox = new PegBox(position+QPoint(j*40, 0), this);
			scene()->addItem(pegbox);
			mCodeBoxes.append(pegbox);
			pegbox->setPegState(PegState::Empty);
		}

		position.setX(276);	//go to right corner for the peg boxes

		auto pegbox = new PegBox(position, this);
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
		auto box = new PegBox(QPoint(160-20*mPegNumber+i*40, 70), this);
		scene()->addItem(box);
		box->setPegState(PegState::Empty);
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
		connect(this, SIGNAL(changePegIndicator(IndicatorType)), peg, SLOT(onChangeIndicators(IndicatorType)));

		if(backPeg)
		{
			peg->setMovable(false);
		}
		else
		{
			box->setPeg(peg);
			connect(peg, SIGNAL(mouseReleasedSignal(QPoint, int)), this, SLOT(onPegMouseRelease(QPoint, int)));
		}
	}
}
//-----------------------------------------------------------------------------

void Board::initializeScene()
{
	mState = GameState::None;
	mCodeBoxes.clear();
	mPinBoxes.clear();
	mPegBoxes.clear();

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

	mMessage = new Message("#303133", mFontName, mFontSize);
	scene()->addItem(mMessage);
	mMessage->setPos(20, 0);

	mInformation = new Message("#808183", mFontName, qMax(mFontSize - 2, 1));
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

void Board::onPegMouseRelease(const QPoint &position, const int &color)
{
	if(!mSameColor)//if same color is not allowed and there is already a color-peg visible, we just ignore drop
	{
		foreach (PegBox* box, mCurrentBoxes)
		{
			if(!box->sceneBoundingRect().contains(position) && box->isPegVisible() && box->getPegColor() == color)
				return;
		}
	}

	static bool rowFillState = false;
	bool newRowFillState = true;

	bool dropOnlyOnce = true; // conversion from float to integer may cause double drop on middle. Flag to do it just once

	foreach (PegBox* box, mCurrentBoxes)
	{
		if (box->sceneBoundingRect().contains(position) && box->getPegState() != PegState::Initial && dropOnlyOnce)
		{
			dropOnlyOnce = false;

			if(box->hasPeg())
				box->setPegColor(color);
			else
				createPegForBox(box, color);

			box->setPegState(PegState::Filled);
		}//	end if

		if (box->getPegState() != PegState::Filled)
			newRowFillState = false;
	} //	end foreach

	if (newRowFillState != rowFillState)//	if (master) code row state changed, emit appropriate signal
	{
		codeRowFilled(newRowFillState);
	}
	rowFillState = newRowFillState;
}
//-----------------------------------------------------------------------------

void Board::onPinBoxPressed()
{
	/*	This function is the running engine of the Master mode.
	 *	If b and c are the number of blacks and whites, then
	 *			w = sum(min(c[i], g[i]) -b
	 *	where c[i] and g[i] are the number of times that the color i
	 *	appears in code/guess, and sum runs over all colors.
	 *	http://mathworld.wolfram.com/Mastermind.html
	 */
	mState = GameState::Running;

	int c[mColorNumber];
	int g[mColorNumber];

	for(int i = 0; i < mColorNumber; ++i)
	{
		c[i] = g[i] = 0;
	}

	int blacks = 0;

	for(int i = 0; i < mPegNumber; ++i)
	{
		int currentColor = mCurrentBoxes.at(i)->getPegColor();
		int masterColor = mMasterBoxes.at(i)->getPegColor();

		if ( currentColor == masterColor)
		{
			++blacks;
		}
		++c[currentColor];
		++g[masterColor];
	}

	int total = 0;

	for(int i = 0; i < mColorNumber; ++i)
		total += std::min(c[i], g[i]);

	mPinBoxes.first()->setPins(blacks, total - blacks);
	mPinBoxes.first()->setBoxState(BoxState::Past);
	mPinBoxes.removeFirst();	// the pinbox goes to past

	for(int i = 0; i < mPegNumber; ++i) //old row of codes goes to past and are removed from current
	{
		mCurrentBoxes.first()->setBoxState(BoxState::Past);
		mCurrentBoxes.removeFirst();
	}

	if (blacks == mPegNumber) //success here
	{
		mState = GameState::Win;
		mMessage->showMessage(tr("Success! You Win"));

		setBoxStateOfList(mMasterBoxes, BoxState::Past);
		setBoxStateOfList(mCurrentBoxes, BoxState::Past);
		setBoxStateOfList(mPinBoxes, BoxState::Future);
		setBoxStateOfList(mPegBoxes, BoxState::Future);

	} else if (mCodeBoxes.isEmpty()) //out of more row of codes, a fail
	{
		mState = GameState::Lose;
		mMessage->showMessage(tr("Game Over! You Failed"));
		setBoxStateOfList(mPegBoxes, BoxState::Future);
	} else // continue the game
	{
		mMessage->showMessage(tr("Place Your Pegs ... "));
		for(int i = 0; i < mPegNumber; ++i) //put the next codeboxes in action
		{
			mCurrentBoxes.append(mCodeBoxes.first());
			mCodeBoxes.first()->setBoxState(BoxState::Current);
			mCodeBoxes.first()->setPegState(PegState::Empty);
			mCodeBoxes.removeFirst();
		}
	}
}
//-----------------------------------------------------------------------------

void Board::onChangeIndicators(const IndicatorType &indicator_n)
{
	setIndicatorType(indicator_n);
	emit changePegIndicator(mIndicator);
}

void Board::onResign()
{
	if (mMode == GameMode::Master && mState == GameState::Running)
	{
		mState = GameState::Resign;
		mMessage->showMessage(tr("You Resign"));

		setBoxStateOfList(mMasterBoxes, BoxState::Past);
		setBoxStateOfList(mCurrentBoxes, BoxState::Future);
		setBoxStateOfList(mPinBoxes, BoxState::Future);
		setBoxStateOfList(mPegBoxes, BoxState::Future);
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

	/*	Here we check if user user input 4 blacks*/
	if (mGame->done())
	{
		mMessage->showMessage(tr("Yeah I found It"));
		mState = GameState::Win;
		scene()->update();
		return;
	}

	/* getting guess from mGame */
	mGuess = mGame->getGuess(mAlgorithm);

	showTranslatedInformation();

	if (mCodeBoxes.isEmpty())
	{
		mState = GameState::Lose; // Hehe, I can solve it always
		mMessage->showMessage(tr("I Lose"));
		scene()->update();
		return;
	} else
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

	if (mSetPins)
	{
		mPinBoxes.first()->setPins(mMasterCode, mGuess, mPegNumber, mColorNumber);
	}
}
//-----------------------------------------------------------------------------

void Board::onDoneButtonPressed()
{
	mState = GameState::Running;
	//	we are done with the done button
	mDoneButton->setVisible(false);
	mDoneButton->setEnabled(false);

	setBoxStateOfList(mCurrentBoxes, BoxState::Past);	//	first let the master code boxes become past for no interaction
	setBoxStateOfList(mPegBoxes, BoxState::Future);	//	freezing the peg boxes

	mMasterCode = "";
	foreach(PegBox* box, mCurrentBoxes)
	{
		QString str;
		str.setNum(box->getPegColor());
		mMasterCode.append(str);
	}

	mState = GameState::Guessing;
	// getting the first guess and puting it in the current row
	mGuess = mGame->getGuess();
	mState = GameState::Running;

	//set the first row of current
	mCurrentBoxes.clear();
	for(int i = 0; i < mPegNumber; ++i)
	{
		createPegForBox(mCodeBoxes.first(), mGuess.at(i).digitValue());
		mCodeBoxes.first()->setBoxState(BoxState::Past);
		mCurrentBoxes.append(mCodeBoxes.first());
		mCodeBoxes.removeFirst();
	}

	mPinBoxes.first()->setBoxState(BoxState::None);
	mMessage->showMessage(tr("Please Put Pins And Press OK"));
	mOkButton->setPos(mPinBoxes.first()->pos()-QPoint(0, 39));
	mOkButton->setEnabled(true);
	mOkButton->setVisible(true);

	mState = GameState::WaittingPins;

	if (mSetPins) //the program puts pins for the lazy ass user
	{
		mPinBoxes.first()->setPins(mMasterCode, mGuess, mPegNumber, mColorNumber);
		mState = GameState::Running;
	}
	/*	We are done here. The onOkButtonPressed is continuing the game from now on
	 */
}
//-----------------------------------------------------------------------------

void Board::play(const GameMode &mode)
{
	initializeScene();

	switch (mode) {
	case GameMode::Master:
		playCodeMaster();
		break;
	default:
		playCodeBreaker();
		break;
	}
}
//-----------------------------------------------------------------------------

void Board::playCodeBreaker()
{
	mMode = GameMode::Breaker;
	mDoneButton->setZValue(2);
	mDoneButton->setVisible(false);
	mDoneButton->setEnabled(true);

	if (!mGame)
	{
		mGame = new Game(mPegNumber, mColorNumber, mSameColor);
	}
	mGame->reset(mPegNumber, mColorNumber, mSameColor);

	mMessage->showMessage(tr("Place Your Pegs ... "));

	for(int i = 0; i < mPegNumber; ++i) //initializing currentrow
	{
		mCurrentBoxes.append(mMasterBoxes.first());
		mMasterBoxes.first()->setBoxState(BoxState::Current);
		mMasterBoxes.removeFirst();
	}

	mState = GameState::WaittingMasterCode;
	/*	Nothing happening from here till the user fill the master code
	 *	row and press the done button. After the done button is pressed,
	 *	the onDoneButtonPressed is continuing the game
	 */
}
//-----------------------------------------------------------------------------

void Board::playCodeMaster()
{

	QString digits = "0123456789";
	digits.left(mColorNumber);
	int remainingNumbers = mColorNumber;

	foreach (PegBox* box, mMasterBoxes) //creating a master code to be guessed
	{
		int color = qrand() % remainingNumbers;
		createPegForBox(box, digits.at(color).digitValue());
		box->setPegState(PegState::Empty);
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
	mState = GameState::WaittingCodeRow;

	// from now on the onPinBoxPushed function continue the game.
}

//-----------------------------------------------------------------------------

void Board::reset(const int& peg_n, const int& color_n, const GameMode &mode_n, const bool &samecolor,
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

	if (peg_n < 2 || peg_n > 6 || color_n < 2 || color_n > 10) // for safety, fallback to standard in out-range inputs
	{
		mPegNumber = 4;
		mColorNumber = 6;
	}
}
//-----------------------------------------------------------------------------

void Board::resizeEvent(QResizeEvent* event)
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

void Board::setBoxStateOfList(QList<PegBox *> boxlist, const BoxState state_t)
{
	foreach (PegBox* box, boxlist)
	{
		box->setBoxState(state_t);
	}
}
//-----------------------------------------------------------------------------

void Board::setBoxStateOfList(QList<PinBox *> boxlist, const BoxState state_t)
{
	foreach (PinBox* box, boxlist)
	{
		box->setBoxState(state_t);
	}
}
//-----------------------------------------------------------------------------

void Board::showTranslatedInformation()
{
	int possibleSize = mGame->getPossibleCodesSize();

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
		int minWeight = mGame->getLastMinWeight();
		switch (mAlgorithm) {
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

void Board::setPinsRow(const bool &set_pins, const bool &closeRow)
{
	mSetPins = set_pins;
	mCloseRow = closeRow;
}

//-----------------------------------------------------------------------------

void Board::setIndicatorType(const IndicatorType &indicator_n)
{
	mIndicator = indicator_n;
	emit changePegIndicator(mIndicator);
}
//-----------------------------------------------------------------------------

void Board::drawBackground(QPainter* painter, const QRectF& rect)
{
	painter->fillRect(rect, QColor("#B6B6B6"));// set scene background color

	QLinearGradient topgrad(0, 16, 0, 129);
	topgrad.setColorAt(0.0, QColor(0xf8, 0xf8, 0xf8));
	topgrad.setColorAt(0.6, QColor(0xb8, 0xb9, 0xbb));
	topgrad.setColorAt(1, QColor(0xd4, 0xd5, 0xd7));
	QBrush mTopGrad(topgrad);

	QLinearGradient botgrad(0, 530, 0, 557);
	botgrad.setColorAt(0.0, QColor("#d4d5d7"));
	botgrad.setColorAt(0.3, QColor("#cecfd1"));
	botgrad.setColorAt(1.0, QColor("#b0b1b3"));
	QBrush mBotGrad(botgrad);

	QLinearGradient lgrad(0, 190, 320, 370);
	lgrad.setColorAt(0.0, QColor(0xff, 0xff, 0xff, 0xa0));
	lgrad.setColorAt(0.49, QColor(0xff, 0xff, 0xff, 0xa0));
	lgrad.setColorAt(0.50, QColor(0, 0, 0, 0x80));
	lgrad.setColorAt(1.0, QColor(0, 0, 0, 0x80));
	QPen mFramePen = QPen(QBrush(lgrad), 1);

	QColor mPend("#646568");
	QColor mPenl("#ecedef");
	QColor mGrad0("#cccdcf");
	QColor mGrad1("#fcfdff");

	mPend.setAlpha(0x32);
	mPenl.setAlpha(0x50);
	mGrad0.setAlpha(0x32);
	mGrad1.setAlpha(0x32);

	painter->setPen(Qt::NoPen);

	QRectF cr(3, 3, 314, 554);
	QPainterPath cpath;
	cpath.addRoundedRect(cr, 10.1, 10.1);
	painter->setClipPath(cpath);
	painter->setClipping(true);

	painter->setBrush(mTopGrad);
	painter->drawRect(QRect(4, 4, 312, 125));
	painter->setBrush(QBrush(QColor("#707173")));
	painter->drawRect(QRect(4, 128, 318, 1));

	painter->setPen(Qt::NoPen);

	QLinearGradient rowgrad(4, 129, 4, 169);
	rowgrad.setColorAt(0.0, QColor("#9a9b9d"));
	rowgrad.setColorAt(1.0, QColor("#8f8f8f"));
	rowgrad.setSpread(QGradient::RepeatSpread);
	painter->setBrush(QBrush(rowgrad));
	painter->drawRect(QRectF(4, 129, 318, 400));

	painter->setPen(QColor("#9a9b9d").dark(120));
	for(int i = 0; i < 11; ++i)
	{
		painter->drawLine(5, 129+i*40, 321, 129+i*40);
	}

	painter->setBrush(mBotGrad);
	painter->drawRect(QRect(1, 530, 318, 27));
	painter->setBrush(QBrush(QColor("#eff0f2")));

	painter->setClipping(false);

	QRectF rightShadow(3.5, 3.5, 313, 553);
	painter->setBrush(Qt::NoBrush);
	painter->setPen(mFramePen);
	painter->drawRoundedRect(rightShadow, 9.8, 9.8);

	QLinearGradient solgrad(50, 70, 50, 108);
	solgrad.setColorAt(0.0, QColor("#9a9b9d"));
	solgrad.setColorAt(1.0, QColor("#949597"));
	QBrush solBgBrush(solgrad);

	QLinearGradient framegrad = QLinearGradient(50, 70, 50, 110);
	framegrad.setColorAt(0.0, QColor("#4e4f51"));
	framegrad.setColorAt(1.0, QColor("#ebecee"));
	QPen solFramePen(QBrush(framegrad), 1);

	painter->setBrush(solBgBrush);
	painter->setPen(solFramePen);

	QRectF codeRowContainer(41, 68, 235, 42);
	QRectF mRectC(42, 69, 235, 41);

	painter->drawRoundedRect(codeRowContainer, 21,21);
	painter->setBrush(solgrad);
	painter->drawRoundedRect(mRectC, 20, 20);
	painter->setRenderHint(QPainter::TextAntialiasing, true);
}
