/***********************************************************************
 *
 * Copyright (C) 2013 Mehdi Omidal <mehdioa@gmail.com>
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
#include "peg.h"
#include "button.h"
#include "pegbox.h"
#include "emptybox.h"
#include "pinbox.h"
#include "game.h"
#include "message.h"

Board::Board(QWidget* parent):
	QGraphicsView(parent),
	mState(STATE_NONE),
	mMode(MODE_MASTER),
	mPegNumber(4),
	mColorNumber(6),
	mSameColor(true),
	mSetPins(false),
	mCloseRow(false),
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

void Board::onPegMouseRelease(QPoint position, int color)
{
	if(!mSameColor)//if same color is not allowed and there is already a color-peg visible, we just ignore drop
		foreach (PegBox* box, mCurrentBoxes)
			if(!box->sceneBoundingRect().contains(position) && box->isPegVisible() && box->getPegColor() == color)
				return;

	static bool rowFillState = false;
	bool newRowFillState = true;

	bool dropOnlyOnce = true; // conversion from float to integer may cause double drop on middle. Flag to do it just once

	foreach (PegBox* box, mCurrentBoxes) {
		if (box->sceneBoundingRect().contains(position) && box->getPegState() != PEG_INITIAL && dropOnlyOnce) {
			dropOnlyOnce = false;

			if(box->hasPeg())
				box->setPegColor(color);
			else
				createPegForBox(box, color);

			box->setPegState(PEG_FILLED);
		}//	end if

		if (box->getPegState() != PEG_FILLED)
			newRowFillState = false;
	} //	end foreach

	if (newRowFillState != rowFillState){//	if (master) code row state changed, emit appropriate signal
		codeRowFilled(newRowFillState);
	}
	rowFillState = newRowFillState;
}
//-----------------------------------------------------------------------------

void Board::codeRowFilled(bool filled)
{
	switch (mMode) {
	case MODE_MASTER://-----------------------------------------
		switch (filled) {
		case true:
			mPinBoxes.first()->setState(BOX_CURRENT);
			mMessage->showMassage("Press The Pin Box ...");
			break;
		default:
			mPinBoxes.first()->setState(BOX_PAST);
			mMessage->showMassage("Place Your Pegs ... ");
			break;
		}
		break;
	default://MODE_BREAKER-------------------------------------
		if (mDoneButton->isEnabled()){//the user is not done putting the master code
			switch (filled) {
			case true:
				mDoneButton->setVisible(true);
				mMessage->showMassage("Press Done ...");
				break;
			default:
				mDoneButton->setVisible(false);
				mMessage->showMassage("Place Your Pegs ... ");
				break;
			}
		}
		break;
	}
}
//-----------------------------------------------------------------------------

void Board::playCodeBreaker()
{
	generate();
	mState = STATE_RUNNING;
	mMode = MODE_BREAKER;
	mDoneButton->setZValue(2);
	mDoneButton->setVisible(false);
	mDoneButton->setEnabled(true);

	if (!mGame) {
		mGame = new Game(mPegNumber, mColorNumber, mSameColor);
	}
	mGame->reset(mPegNumber, mColorNumber, mSameColor);

	mMessage->showMassage("Place Your Pegs ... ");

	for(int i = 0; i < mPegNumber; ++i) {//initializing currentrow
		mCurrentBoxes.append(mMasterBoxes.first());
		mMasterBoxes.first()->setState(BOX_CURRENT);
		mMasterBoxes.removeFirst();
	}
	/*	Nothing happening from here till the user fill the master code
	 *	row and press the done button. After the done button is pressed,
	 *	the onDoneButtonPressed is continuing the game
	 */
}
//-----------------------------------------------------------------------------

void Board::playCodeMaster()
{
	generate();
	mState = STATE_RUNNING;

	foreach (PegBox* box, mMasterBoxes) {//creating a master code to be guessed
		int color = qrand() % mColorNumber;
		createPegForBox(box, color);
		box->setPegState(PEG_EMPTY);
		box->setState(BOX_NONE);
	}

	for(int i = 0; i < mPegNumber; ++i) {//initializing currentrow
		mCurrentBoxes.append(mCodeBoxes.first());
		mCodeBoxes.first()->setState(BOX_CURRENT);
		mCodeBoxes.removeFirst();
	}

	mMessage->showMassage("Place Your Pegs ... ");

	// from now on the onPinBoxPushed function is continuing the game.
}
//-----------------------------------------------------------------------------

void Board::onPinBoxPressed()
{
	/*	This function is the running engine of the CODEMASTER mode.
	 *	If b and c are the number of blacks and whites, then
	 *			w = sum(min(c[i], g[i]) -b
	 *	where c[i] and g[i] are the number of times that the color i
	 *	appears in code/guess, and sum runs over all colors.
	 *	http://mathworld.wolfram.com/Mastermind.html
	 */
	int c[mColorNumber];
	int g[mColorNumber];

	for(int i = 0; i < mColorNumber; ++i) {
		c[i] = g[i] = 0;
	}

	int blacks = 0;

	for(int i = 0; i < mPegNumber; ++i){
		int currentColor = mCurrentBoxes.at(i)->getPegColor();
		int masterColor = mMasterBoxes.at(i)->getPegColor();

		if ( currentColor == masterColor) {
			++blacks;
		}
		++c[currentColor];
		++g[masterColor];
	}

	int total = 0;

	for(int i = 0; i < mColorNumber; ++i)
		total += std::min(c[i], g[i]);

	mPinBoxes.first()->setPins(blacks, total - blacks);
	mPinBoxes.first()->setState(BOX_PAST);
	mPinBoxes.removeFirst();	// the pinbox goes to past

	for(int i = 0; i < mPegNumber; ++i) {//old row of codes goes to past and are removed from current
		mCurrentBoxes.first()->setState(BOX_PAST);
		mCurrentBoxes.removeFirst();
	}

	if (blacks == mPegNumber) {//success here
		mState = STATE_WON;
		mMessage->showMassage("Success! You Win");
		foreach (PegBox* box, mMasterBoxes) {//show master code pegs
			box->setState(BOX_PAST);
		}
		scene()->update();
	} else if (mCodeBoxes.isEmpty()) {//out of more row of codes, a fail
		mState = STATE_LOST;
		mMessage->showMassage("Game Over! You Failed");
		scene()->update();
	} else {// continue the game
		mMessage->showMassage("Place Your Pegs ... ");
		for(int i = 0; i < mPegNumber; ++i) {//put the next codeboxes in action
			mCurrentBoxes.append(mCodeBoxes.first());
			mCodeBoxes.first()->setState(BOX_CURRENT);
			mCodeBoxes.first()->setPegState(PEG_EMPTY);
			mCodeBoxes.removeFirst();
		}
	}
}

void Board::onChangeIndicators(const int &indicator_n)
{
	setIndicatorType(indicator_n);
	emit changePegIndicator(mIndicator);
}

void Board::onChangeMode(const int &mode_n)
{
	mMode = (GAME_MODE) mode_n;
}
//-----------------------------------------------------------------------------

void Board::onOkButtonPressed()
{
	/*	passing user inputed blacks and whites to mGame */
	int resp = mPinBoxes.first()->getValue();
	if(!mGame->setResponse(resp)){
		mMessage->showMassage("Not Possible, Try Again...");
		return;
	}

	mOkButton->setVisible(false);

	/*	we are done with the pinbox. Make it past and remove it from mPinBoxes */
	mPinBoxes.first()->setState(BOX_PAST);
	mPinBoxes.removeFirst();

	/*	Here we check if user user input 4 blacks*/
	if (mGame->done()){
		mMessage->showMassage("Yeah I found It");
		mState = STATE_WON;
		scene()->update();
		return;
	}

	/* getting guess from mGame */
	mGuess = mGame->getGuess(mAlgorithm);

	if (mCodeBoxes.isEmpty()) {
		mState = STATE_LOST; // Hehe, I can solve it always
		mMessage->showMassage("I Lose");
		scene()->update();
		return;
	} else {
		for(int i = 0; i < mPegNumber; ++i) {
			createPegForBox(mCodeBoxes.first(), mGuess[i].digitValue());
			mCodeBoxes.first()->setState(BOX_PAST);
			mCodeBoxes.removeFirst();
		}

		mMessage->showMassage("Please Put Pins And Press OK");
		mPinBoxes.first()->setState(BOX_NONE);
		mOkButton->setEnabled(true);
		mOkButton->setVisible(true);
		mOkButton->setPos(mPinBoxes.first()->pos()-QPoint(0, 39));
	}

	if (mSetPins) {
		mPinBoxes.first()->setPins(mMasterCode, mGuess, mPegNumber, mColorNumber);
	}
}
//-----------------------------------------------------------------------------

void Board::onDoneButtonPressed()
{
	//	we are done with the done button
	mDoneButton->setVisible(false);
	mDoneButton->setEnabled(false);

	//	first let the master code boxes become past for no interaction
	foreach (PegBox* box, mCurrentBoxes) {
		box->setState(BOX_PAST);
	}

	//	freezing the peg boxes
	foreach (PegBox* box, mPegBoxes) {
		box->setState(BOX_FUTURE);
	}

	mMasterCode = "";
	foreach(PegBox* box, mCurrentBoxes){
		QString str;
		str.setNum(box->getPegColor());
		mMasterCode.append(str);
	}

	// getting the first guess and puting it in the current row
	mGuess = mGame->getGuess();

	//set the first row of current
	mCurrentBoxes.clear();
	for(int i = 0; i < mPegNumber; ++i) {
		createPegForBox(mCodeBoxes.first(), mGuess.at(i).digitValue());
		mCodeBoxes.first()->setState(BOX_CURRENT);
		mCurrentBoxes.append(mCodeBoxes.first());
		mCodeBoxes.removeFirst();
	}

	// getting the first guess and puting it in the current row
	mGuess = mGame->getGuess();
	for(int i = 0; i < mPegNumber; ++i){
		createPegForBox(mCurrentBoxes.at(i), mGuess.at(i).digitValue());
		mCurrentBoxes.at(i)->setState(BOX_PAST);
	}

	mPinBoxes.first()->setState(BOX_NONE);
	mMessage->showMassage("Please Put Pins And Press OK");
	mOkButton->setPos(mPinBoxes.first()->pos()-QPoint(0, 39));
	mOkButton->setEnabled(true);
	mOkButton->setVisible(true);

	if (mSetPins) {//the program puts pins for the lazy ass user
		mPinBoxes.first()->setPins(mMasterCode, mGuess, mPegNumber, mColorNumber);
	}
	/*	We are done here. The onOkButtonPressed is continuing the game from now on
	 */
}
//-----------------------------------------------------------------------------

void Board::generate()
{
	mState = STATE_NONE;
	mCodeBoxes.clear();
	mPinBoxes.clear();
	mPegBoxes.clear();

	scene()->clear();
	setInteractive(true);

	mOkButton = new Button(38, "OK");
	scene()->addItem(mOkButton);
	mOkButton->setZValue(2);
	connect(mOkButton, SIGNAL(buttonPressed()), this, SLOT(onOkButtonPressed()));
	mOkButton->setEnabled(false);
	mOkButton->setVisible(false);

	mDoneButton = new Button(160, "Done");
	mDoneButton->setPos(79, 118);
	mDoneButton->setVisible(false);
	mDoneButton->setZValue(2);
	mDoneButton->setEnabled(false);
	connect(mDoneButton, SIGNAL(buttonPressed()), this, SLOT(onDoneButtonPressed()));
	scene()->addItem(mDoneButton);

	mMessage = new Message;
	scene()->addItem(mMessage);
	mMessage->setPos(20, 0);

	createBoxes();

	scene()->update();
}
//-----------------------------------------------------------------------------

void Board::reset(const int& peg_n, const int& color_n, const int &mode_n, const bool &samecolor,
				  const int &algorithm_n, const bool &set_pins, const bool &close_row, const int &indicator_n)
{

	mPegNumber = peg_n;
	mColorNumber = color_n;
	mSetPins = set_pins;
	mCloseRow = close_row;
	mMode = (mode_n == 0) ? MODE_MASTER : MODE_BREAKER;
	setIndicatorType(indicator_n);
	setAlgorithmType(algorithm_n);

	mSameColor = samecolor;

	if (peg_n < 2 || peg_n > 6 || color_n < 2 || color_n > 10) {// for safety, fall back to standard in out-range inputs
		mPegNumber = 4;
		mColorNumber = 6;
	}
}
//-----------------------------------------------------------------------------

void Board::createPegForBox(PegBox *box, int color, bool backPeg)
{
	QPoint pos = box->sceneBoundingRect().topLeft().toPoint();
	if (box->hasPeg()){
		box->setPegColor(color);
	} else {
		auto peg = new Peg(pos, color, mIndicator);
		scene()->addItem(peg);
		connect(this, SIGNAL(changePegIndicator(INDICATOR_TYPE)), peg, SLOT(onChangeIndicators(INDICATOR_TYPE)));

		if(backPeg)
			peg->setMovable(false);
		else {
			box->setPeg(peg);
			connect(peg, SIGNAL(mouseReleasedSignal(QPoint, int)), this, SLOT(onPegMouseRelease(QPoint, int)));
		}
	}

}

//-----------------------------------------------------------------------------

void Board::play(const int &mode)
{
	switch (mode) {
	case 0:
		playCodeMaster();
		break;
	default:
		playCodeBreaker();
		break;
	}
}

void Board::setPinsRow(const bool &set_pins, const bool &closeRow)
{
	mSetPins = set_pins;
	mCloseRow = closeRow;
}
//-----------------------------------------------------------------------------

void Board::resizeEvent(QResizeEvent* event)
{
	fitInView(sceneRect(), Qt::KeepAspectRatio);
	QGraphicsView::resizeEvent(event);
}

void Board::setIndicatorType(const int indicator_n)
{
	switch (indicator_n) {
	case 1:
		mIndicator = TYPE_CHAR;
		break;
	case 2:
		mIndicator = TYPE_DIGIT;
		break;
	default:
		mIndicator = TYPE_NONE;
		break;
	}
	emit changePegIndicator(mIndicator);
}

void Board::setAlgorithmType(const int &algorithm_n)
{
	switch (algorithm_n) {
	case 0:
		mAlgorithm = WorstCase;
		break;
	case 1:
		mAlgorithm = ExpectedSize;
		break;
	default:
		mAlgorithm = MostParts;
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

	for (int i = 0; i < MAX_COLOR_NUMBER; ++i){

		QPoint position = left_bottom_corner - QPoint(0, i*40);

		auto pinbox = new PinBox(mPegNumber, position);
		scene()->addItem(pinbox);
		mPinBoxes.append(pinbox);
		connect(pinbox, SIGNAL(pinBoxPushed()), this, SLOT(onPinBoxPressed()));

		position.setX(160-20*mPegNumber);

		for (int j = 0; j < mPegNumber; ++j){
			auto pegbox = new PegBox(position+QPoint(j*40, 0), this);
			scene()->addItem(pegbox);
			mCodeBoxes.append(pegbox);
			pegbox->setPegState(PEG_EMPTY);
		}

		position.setX(276);	//go to right corner for the peg boxes

		auto pegbox = new PegBox(position, this);
		scene()->addItem(pegbox);
		if (i < mColorNumber) {
			createPegForBox(pegbox, i, true);//just a background peg
			createPegForBox(pegbox, i);
		}
		pegbox->setPegState(PEG_INITIAL);
		mPegBoxes.append(pegbox);
	}

	for (int i = 0; i < mPegNumber; ++i){// the last code boxes are for the master code
		auto box = new PegBox(QPoint(160-20*mPegNumber+i*40, 70), this);
		scene()->addItem(box);
		box->setPegState(PEG_EMPTY);
		mMasterBoxes.append(box);
	}
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

	QLinearGradient rowgrad(0, 129, 0, 169);
	rowgrad.setColorAt(0.0, QColor("#9a9b9d"));
	rowgrad.setColorAt(1.0, QColor("#8f8f8f"));
	rowgrad.setSpread(QGradient::RepeatSpread);
	painter->setBrush(QBrush(rowgrad));
	painter->drawRect(QRectF(1, 129, 318, 400));

	painter->setBrush(mBotGrad);
	painter->drawRect(QRect(1, 530, 318, 27));
	painter->setBrush(QBrush(QColor("#eff0f2")));
	painter->drawRect(QRect(1, 530, 318, 1));

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
//-----------------------------------------------------------------------------

//void Board::drawForeground(QPainter* painter, const QRectF&)
//{
//	if (mState == STATE_WON || mState == STATE_LOST) {
//		QString message((mState == STATE_WON) ? tr("Success") : tr("Game Over"));
//		QFontMetrics metrics(QFont("Sans", 24));
//		int w = metrics.width(message);
//		int h = metrics.height();
//		QPixmap pixmap(QSize(w + h, h * 2));
//		pixmap.fill(QColor(0, 0, 0, 0));
//		{
//			QPainter pixmap_painter(&pixmap);

//			pixmap_painter.setPen(Qt::NoPen);
//			pixmap_painter.setBrush(QColor(0, 0, 0, 200));
//			pixmap_painter.setRenderHint(QPainter::Antialiasing, true);
//			pixmap_painter.drawRoundedRect(0, 0, w + h, h * 2, 10, 10);

//			pixmap_painter.setFont(QFont("Sans", 24));
//			pixmap_painter.setPen(Qt::white);
//			pixmap_painter.setRenderHint(QPainter::TextAntialiasing, true);
//			pixmap_painter.drawText(h / 2, h / 2 + metrics.ascent(), message);
//		}

//		painter->save();
//		painter->resetTransform();
//		painter->drawPixmap((width() - pixmap.width()) / 2, (height() - pixmap.height()) / 2, pixmap);
//		painter->restore();
//	}
//}
