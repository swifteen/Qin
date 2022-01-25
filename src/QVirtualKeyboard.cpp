/*
 * Modified by Wei-Ning Huang
 *
 * Copyright 2010 Wei-Ning Huang <aitjcize@gmail.com>
 *
 * Copyright 2009 EMBITEL (http://www.embitel.com)
 * 
 * This file is part of Virtual Keyboard Project.
 * 
 * Virtual Keyboard is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation
 * 
 * Virtual Keyboard is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with Virtual Keyboard. If not, see <http://www.gnu.org/licenses/>.
 */

#include "QVirtualKeyboard.h"

#include <QDebug>
#include <QDesktopWidget>
#include <QFile>
#include <QPushButton>
#include <QSignalMapper>
#include <QTextStream>

#include "QinEngine.h"
#include "QinIMBases.h"
#define MAX_SELECT_PANEL_HEIGHT	(50)
QVirtualKeyboard::QVirtualKeyboard(QinEngine* im)
:QWidget(0, Qt::Tool | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint)
{
  /* setup UI */
  setupUi(this);
  this->move((QApplication::desktop()->width() - width())/2,
      QApplication::desktop()->height() - height());

  /* Setup selectPanel */
  QHBoxLayout* layout = new QHBoxLayout;
  layout->setContentsMargins(1, 1, 1, 0);
  layout->setSpacing(0);
  selectPanel = new QWidget(this, Qt::Tool |
                                  Qt::FramelessWindowHint |
                                  Qt::WindowStaysOnTopHint);
  selectPanel->move((QApplication::desktop()->width() - width())/2,
      QApplication::desktop()->height() - height() - MAX_SELECT_PANEL_HEIGHT);
  selectPanel->setMinimumSize(width(), MAX_SELECT_PANEL_HEIGHT);
  selectPanel->setMaximumSize(width(), MAX_SELECT_PANEL_HEIGHT);
  selectPanel->setLayout(layout);
  selectPanel->hide();

  QFile data(":/data/selectPanel.qss");
  if (data.open(QFile::ReadOnly)) {
    QTextStream ssin(&data);
    selectPanel->setStyleSheet(ssin.readAll());
    data.close();
  } else {
    qDebug() << "Error: failed to set style sheet for selectPanel!";
  }

  /* Setup members */
  imEngine = im;
  Capsed = false;
  Shifted = false;
  location = 0;
  IMIndex = 0;
  candSignalMapper = NULL;
//  opacitySlide->setRange(20, 100);

  /* Setup buttons */
  allButtons = findChildren<QToolButton*>();
  signalMapper = new QSignalMapper(this);

  for (int i = 0; i < allButtons.count(); i++) {
    connect(allButtons.at(i), SIGNAL(clicked()), signalMapper, SLOT(map()));
    signalMapper->setMapping(allButtons.at(i), i);
  }
  connect(signalMapper, SIGNAL(mapped(int)), this, SLOT(s_on_btn_clicked(int)));
}

QVirtualKeyboard::~QVirtualKeyboard() {
  delete signalMapper;
}

void QVirtualKeyboard::insertInputMethod(const QinIMBase* im) {
  regedIMs.push_back(im->name());
  btnIMToggle->setText(regedIMs[0]);
  imEngine->setCurrentIM(0);
}

void QVirtualKeyboard::on_opacitySlide_valueChanged(int value) {
  setWindowOpacity((120.0 - value) / 100.0);
}

void QVirtualKeyboard::hideAll(void) {
  clearCandStrBar();
  hide();
}

void QVirtualKeyboard::on_btnLoc_clicked(void) {
#if 0
  location = !location;
  if (location) {
    btnLoc->setText("↯");
    this->move((QApplication::desktop()->width() - width())/2, 0);
    selectPanel->move((QApplication::desktop()->width() - width())/2,
        height() + 1);
  } else {
    btnLoc->setText("↥");
    this->move((QApplication::desktop()->width() - width())/2,
        QApplication::desktop()->height() - height());
    selectPanel->move((QApplication::desktop()->width() - width())/2,
        QApplication::desktop()->height() - height() - MAX_SELECT_PANEL_HEIGHT);
  }
#endif
}

void QVirtualKeyboard::on_btnHide_clicked()
{
    hideAll();
}

void QVirtualKeyboard::s_on_btn_clicked(int btn) {
  QString strKeyId = allButtons.at(btn)->accessibleName();
#ifdef DEBUG
  qDebug()<< __FILE__ << __FUNCTION__ << __LINE__<<QObject::sender()<<btn<<strKeyId;
#endif
  bool isOk;
  int keyId = strKeyId.toInt(&isOk, 16);
  int involvedKeys = 1;
  bool istextkey = isTextKey(keyId);

  if (strKeyId.isEmpty() || !isOk)
    return;

  Qt::KeyboardModifiers Modifier = Qt::NoModifier;

  if (Shifted) {
    Modifier = Modifier | Qt::ShiftModifier;
    involvedKeys++;

    switch (keyId) {
      case Qt::Key_Comma://0x2c
      case Qt::Key_Period://0x2e
      case Qt::Key_Slash: //0x2f
	  		keyId += 0x10; //Qt::Key_Less Qt::Key_Greater Qt::Key_Question
	  break;
      case Qt::Key_Semicolon: //0x3b
	  	keyId = Qt::Key_Colon; //0x3a
		break;
      case Qt::Key_Apostrophe: //0x27
	  	keyId = Qt::Key_QuoteDbl;//0x22 
		break;
      case Qt::Key_BracketLeft://0x5b
      case Qt::Key_Backslash://0x5c
      case Qt::Key_BracketRight://0x5d
	  	keyId += 0x20; //Qt::Key_BraceLeft Qt::Key_Bar Qt::Key_BraceRight
	  break;
      case 0x31: keyId = 0x21; break;//Qt::Key_1  ==> Qt::Key_Exclam
      case 0x32: keyId = 0x40; break;//Qt::Key_2  ==> Qt::Key_At
      case 0x33: keyId = 0x23; break;//Qt::Key_3  ==> Qt::Key_NumberSign
      case 0x34: keyId = 0x24; break;//Qt::Key_4  ==> Qt::Key_Dollar
      case 0x35: keyId = 0x25; break;
      case 0x36: keyId = 0x5e; break;
      case 0x37: keyId = 0x26; break;
      case 0x38: keyId = 0x2a; break;
      case 0x39: keyId = 0x28; break;//Qt::Key_9  ==> Qt::Key_ParenLeft
      case 0x30: keyId = 0x29; break;//Qt::Key_0  ==> Qt::Key_ParenRight
      case 0x2d: keyId = 0x5f; break;//Qt::Key_Minus  ==> Qt::Key_Underscore
      case 0x3d: keyId = 0x2b; break;//Qt::Key_Equal  ==> Qt::Key_Plus
      default: keyId = tolower(keyId);
    }
  }

  QString ch = allButtons.at(btn)->text().trimmed();
#ifdef DEBUG
  qDebug()<< __FILE__ << __FUNCTION__ << __LINE__<<btn<<ch;
#endif
  int uni = ch.unicode()[0].unicode();

  if (!istextkey) {
    ch = QString();
    if (keyId == Qt::Key_Tab)
      uni = 9;
    else
      uni = 65535;
  }

  if (keyId == Qt::Key_Space)
    ch = QString(" ");
#ifdef DEBUG
	qDebug()<< __FILE__ << __FUNCTION__ << __LINE__<<uni<<keyId<<Modifier;
#endif
  QWSServer::sendKeyEvent(uni, keyId, Modifier, true, false);
//在shift状态下输入字符后，又回到原始状态
  if (istextkey) {
    btnShiftLeft->setChecked(false);
    btnShiftRight->setChecked(false);
  }
}

void QVirtualKeyboard::on_btnCaps_toggled(bool checked) {
  Capsed = checked;
  if (Capsed) {
    if (Shifted)
      changeNormalKeyMap(imEngine->currentIM);
    else
      changeShiftKeyMap(imEngine->currentIM);
  } else {
    if (Shifted)
      changeShiftKeyMap(imEngine->currentIM);
    else
      changeNormalKeyMap(imEngine->currentIM);
  }
}

void QVirtualKeyboard::handelShift(bool checked) {
	Shifted = checked;
	if (Shifted) {
	  if (Capsed)
		changeNormalKeyMap(imEngine->currentIM);
	  else
		changeShiftKeyMap(imEngine->currentIM);
	} else {
	  if (Capsed)
		changeShiftKeyMap(imEngine->currentIM);
	  else
		changeNormalKeyMap(imEngine->currentIM);
	}
}

void QVirtualKeyboard::on_btnShiftLeft_toggled(bool checked) {
	handelShift(checked);
//  qDebug()<< __FILE__ << __FUNCTION__ << __LINE__<<btnShiftLeft->isChecked()<<btnShiftRight->isChecked();
  btnShiftRight->setChecked(btnShiftLeft->isChecked());
}

void QVirtualKeyboard::on_btnShiftRight_toggled(bool checked) {
	handelShift(checked);
//	qDebug()<< __FILE__ << __FUNCTION__ << __LINE__<<btnShiftLeft->isChecked()<<btnShiftRight->isChecked();
	btnShiftLeft->setChecked(btnShiftRight->isChecked());
}

/**
 * @brief 左边数字和符号输入切换按钮
 */
void QVirtualKeyboard::on_btnIM_NumLeftToggle_clicked()
{
	imEngine->setCurrentNumAndSymbolIM(0);//目前只考虑中文下的符号输入，在其它语种下，符号输入界面存在不同
	Capsed = false;
	Shifted = false;
	btnCaps->setChecked(false);
	btnShiftLeft->setChecked(false);
	btnShiftRight->setChecked(false);
	changeNormalKeyMap(imEngine->currentIM);
}

/**
 * @brief 右边数字和符号输入切换按钮
 */
void QVirtualKeyboard::on_btnIM_NumRightToggle_clicked()
{
	on_btnIM_NumLeftToggle_clicked();
}

void QVirtualKeyboard::on_btnIMToggle_clicked(void) {
	if(!imEngine->getCurrentNumAndSymbolState())
	{
		IMIndex = (IMIndex + 1) % regedIMs.size();
	}
  imEngine->setCurrentIM(IMIndex);
  btnIMToggle->setText(imEngine->currentIM->name());
	//根据XML的customkeymap标识来判断是否启用默认的英文布局还是XML中指定的自定义布局
  if (imEngine->currentIM->getUseCustomKeyMap()) {
    if (Capsed || Shifted)
      changeShiftKeyMap(imEngine->currentIM);
    else
      changeNormalKeyMap(imEngine->currentIM);
  } else {
    if (Capsed || Shifted)
      changeShiftKeyMap(imEngine->defaultIM);
    else
      changeNormalKeyMap(imEngine->defaultIM);
  }
}

void QVirtualKeyboard::changeNormalKeyMap(QinIMBase* imb) {
  btnTilt->setText(imb->fromStdKB("`"));
  btn1->setText(imb->fromStdKB("1"));
  btn2->setText(imb->fromStdKB("2"));
  btn3->setText(imb->fromStdKB("3"));
  btn4->setText(imb->fromStdKB("4"));
  btn5->setText(imb->fromStdKB("5"));
  btn6->setText(imb->fromStdKB("6"));
  btn7->setText(imb->fromStdKB("7"));
  btn8->setText(imb->fromStdKB("8"));
  btn9->setText(imb->fromStdKB("9"));
  btn0->setText(imb->fromStdKB("0"));
  btnHiphen->setText(imb->fromStdKB("-"));

  btnQ->setText(imb->fromStdKB("q"));
  btnW->setText(imb->fromStdKB("w"));
  btnE->setText(imb->fromStdKB("e"));
  btnR->setText(imb->fromStdKB("r"));
  btnT->setText(imb->fromStdKB("t"));
  btnY->setText(imb->fromStdKB("y"));
  btnU->setText(imb->fromStdKB("u"));
  btnI->setText(imb->fromStdKB("i"));
  btnO->setText(imb->fromStdKB("o"));
  btnP->setText(imb->fromStdKB("p"));
  btnStartSquare->setText(imb->fromStdKB("["));
  btnCloseSquare->setText(imb->fromStdKB("]"));
  btnBckSlash->setText(imb->fromStdKB("\\"));

  btnA->setText(imb->fromStdKB("a"));
  btnS->setText(imb->fromStdKB("s"));
  btnD->setText(imb->fromStdKB("d"));
  btnF->setText(imb->fromStdKB("f"));
  btnG->setText(imb->fromStdKB("g"));
  btnH->setText(imb->fromStdKB("h"));
  btnJ->setText(imb->fromStdKB("j"));
  btnK->setText(imb->fromStdKB("k"));
  btnL->setText(imb->fromStdKB("l"));
  btnSemiColon->setText(imb->fromStdKB(";"));
  btnSp->setText(imb->fromStdKB("'"));

  btnZ->setText(imb->fromStdKB("z"));
  btnX->setText(imb->fromStdKB("x"));
  btnC->setText(imb->fromStdKB("c"));
  btnV->setText(imb->fromStdKB("v"));
  btnB->setText(imb->fromStdKB("b"));
  btnN->setText(imb->fromStdKB("n"));
  btnM->setText(imb->fromStdKB("m"));
  btnComma->setText(imb->fromStdKB(","));
  btnPeriod->setText(imb->fromStdKB("."));
  btnSlash->setText(imb->fromStdKB("/"));
}

void QVirtualKeyboard::changeShiftKeyMap(QinIMBase* imb) {
  btnTilt->setText(imb->fromShiftStdKB("`"));
  btn1->setText(imb->fromShiftStdKB("1"));
  btn2->setText(imb->fromShiftStdKB("2"));
  btn3->setText(imb->fromShiftStdKB("3"));
  btn4->setText(imb->fromShiftStdKB("4"));
  btn5->setText(imb->fromShiftStdKB("5"));
  btn6->setText(imb->fromShiftStdKB("6"));
  btn7->setText(imb->fromShiftStdKB("7"));
  btn8->setText(imb->fromShiftStdKB("8"));
  btn9->setText(imb->fromShiftStdKB("9"));
  btn0->setText(imb->fromShiftStdKB("0"));
  btnHiphen->setText(imb->fromShiftStdKB("-"));

  btnQ->setText(imb->fromShiftStdKB("q"));
  btnW->setText(imb->fromShiftStdKB("w"));
  btnE->setText(imb->fromShiftStdKB("e"));
  btnR->setText(imb->fromShiftStdKB("r"));
  btnT->setText(imb->fromShiftStdKB("t"));
  btnY->setText(imb->fromShiftStdKB("y"));
  btnU->setText(imb->fromShiftStdKB("u"));
  btnI->setText(imb->fromShiftStdKB("i"));
  btnO->setText(imb->fromShiftStdKB("o"));
  btnP->setText(imb->fromShiftStdKB("p"));
  btnStartSquare->setText(imb->fromShiftStdKB("["));
  btnCloseSquare->setText(imb->fromShiftStdKB("]"));
  btnBckSlash->setText(imb->fromShiftStdKB("\\"));

  btnA->setText(imb->fromShiftStdKB("a"));
  btnS->setText(imb->fromShiftStdKB("s"));
  btnD->setText(imb->fromShiftStdKB("d"));
  btnF->setText(imb->fromShiftStdKB("f"));
  btnG->setText(imb->fromShiftStdKB("g"));
  btnH->setText(imb->fromShiftStdKB("h"));
  btnJ->setText(imb->fromShiftStdKB("j"));
  btnK->setText(imb->fromShiftStdKB("k"));
  btnL->setText(imb->fromShiftStdKB("l"));
  btnSemiColon->setText(imb->fromShiftStdKB(";"));
  btnSp->setText(imb->fromShiftStdKB("'"));

  btnZ->setText(imb->fromShiftStdKB("z"));
  btnX->setText(imb->fromShiftStdKB("x"));
  btnC->setText(imb->fromShiftStdKB("c"));
  btnV->setText(imb->fromShiftStdKB("v"));
  btnB->setText(imb->fromShiftStdKB("b"));
  btnN->setText(imb->fromShiftStdKB("n"));
  btnM->setText(imb->fromShiftStdKB("m"));
  btnComma->setText(imb->fromShiftStdKB(","));
  btnPeriod->setText(imb->fromShiftStdKB("."));
  btnSlash->setText(imb->fromShiftStdKB("/"));
}

bool QVirtualKeyboard::isTextKey(int keyId)
{
  return !(keyId == Qt::Key_Shift
      || keyId == Qt::Key_Control
      || keyId == Qt::Key_Tab
      || keyId == Qt::Key_Return
      || keyId == Qt::Key_Enter
      || keyId == Qt::Key_CapsLock
      || keyId == Qt::Key_Backspace
      || keyId == Qt::Key_Alt);
}

void QVirtualKeyboard::s_on_btnCands_clicked(QWidget* btn) {
  QPushButton* pBtn = qobject_cast < QPushButton*>(btn);
	if(pBtn)
	{
		  QString strKeyId = pBtn->accessibleName();
		  bool isOk;
		  int keyId = strKeyId.toInt(&isOk, 16);
		
#ifdef DEBUG
		  qDebug() << "DEBUG: selected = " << btn<<keyId<<isOk;
#endif
		
		  QWSServer::sendKeyEvent(0, keyId, Qt::NoModifier, true, false);
		  clearCandStrBar();
	}
}

void QVirtualKeyboard::clearCandStrBar(void) {
  for (int i = 0; i < candButtons.size(); ++i) {
    selectPanel->layout()->removeWidget(candButtons[i]);
    delete candButtons[i];
  }
  candButtons.clear();
  selectPanel->hide();
}

void QVirtualKeyboard::showCandStrBar(QStringList strlist) {
  QPushButton* button = NULL;
  int keys[] = SELKEYS;

#ifdef DEBUG
  qDebug() << "DEBUG: cands: " << strlist;
#endif

  /* Make surce previous is cleared */
  clearCandStrBar();

  if (!strlist.size()) return;

  selectPanel->show();

  for (int i = 0; i < strlist.size(); ++i) {
    button = new QPushButton(strlist[i]);
	button->setMaximumHeight(50);
  	button->setMinimumHeight(50);
//    button->setFont(QFont("WenQuanYiMicroHeiLight", 18));
    candButtons.push_back(button);
    selectPanel->layout()->addWidget(button);
    button->show();
  }
#if 0
  QPushButton* leftBtn = new QPushButton(">");
  leftBtn->setAccessibleName(QString("%1").arg(Qt::Key_Right,0,16));
  selectPanel->layout()->addWidget(leftBtn);
#endif

  /* Fix border for the rightmost color, the sequence of the CSS must be
   * border-right then border-style else it won't work */
  candButtons.last()->setStyleSheet("QPushButton { border-right: 1px "
      "#8A8A8A; border-style: groove; height:48;}");

  if (candSignalMapper) {
    delete candSignalMapper;
    candSignalMapper = NULL;
  }

  candSignalMapper = new QSignalMapper(selectPanel);

  for (int i = 0; i < candButtons.size(); i++) {
    candButtons[i]->setAccessibleName(QString("%1").arg(keys[i], 2, 16));
    connect(candButtons[i], SIGNAL(clicked()), candSignalMapper, SLOT(map()));
    candSignalMapper->setMapping(candButtons[i], candButtons[i]);
  }
  
#if 0
  connect(leftBtn, SIGNAL(clicked()), candSignalMapper, SLOT(map()));
  candSignalMapper->setMapping(leftBtn, leftBtn);
#endif
  
  connect(candSignalMapper, SIGNAL(mapped(QWidget*)), this,
      SLOT(s_on_btnCands_clicked(QWidget*)));
}

