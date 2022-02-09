/**
 * @file   QinEngine.cpp
 * @author Wei-Ning Huang (AZ) <aitjcize@gmail.com>
 *
 * Copyright (C) 2010 -  Wei-Ning Huang (AZ) <aitjcize@gmail.com>
 * All Rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "QinEngine.h"

#include <QDebug>

//#include "plugins/QinPinyin.h"
#include "plugins/QinChewing.h"
#include "QinIMBases.h"
#include "QVirtualKeyboard.h"

QinEngine::QinEngine() {
  vkeyboard = new QVirtualKeyboard(this);
  regInputMethod(new QinIMBase(":/data/English.xml"));
  regInputMethod(new QinChewing());
//  regInputMethod(new QinPinyin());
  regInputMethod(new QinTableIMBase(":/data/Boshiamy.xml"));
  defaultIM = inputMethods[0];
	//注册数字和符号输入核心
  regNumAndSymbolInputMethod(new QinIMBase(":/data/num_symbol_ch.xml"));
	m_bNumAndSymbol = false;
}

QinEngine::~QinEngine() {
  delete vkeyboard;
  for (QVector<QinIMBase*>::iterator it = inputMethods.begin();
      it != inputMethods.end(); ++it)
    delete *it;
	  
	for (QVector<QinIMBase*>::iterator it = numAndSymbolInputMethods.begin();
		it != numAndSymbolInputMethods.end(); ++it)
	  delete *it;
}

void QinEngine::regInputMethod(QinIMBase* imb) {
  if (!imb) {
    qDebug("Error: no input method specified\n");
    return;
  }

  inputMethods.push_back(imb);
  vkeyboard->insertInputMethod(imb);
}

void QinEngine::regNumAndSymbolInputMethod(QinIMBase* imb) {
  if (!imb) {
    qDebug("Error: no input method specified\n");
    return;
  }

  numAndSymbolInputMethods.push_back(imb);
//  vkeyboard->insertInputMethod(imb);
}

void QinEngine::setCurrentIM(int index) {
  currentIM = inputMethods[index];
  currentIM->reset();
  m_bNumAndSymbol = false;
}

void QinEngine::setCurrentNumAndSymbolIM(int index) {
  currentIM = numAndSymbolInputMethods[index];
  currentIM->reset();
  m_bNumAndSymbol = true;
}

bool QinEngine::getCurrentNumAndSymbolState() {
  return m_bNumAndSymbol;
}

bool QinEngine::filter(int uni, int keyId, int mod, bool isPress,
    bool autoRepeat) {
  bool doSendEvent = true;
#ifdef DEBUG
	qDebug("DEBUG1: KeyPressed: %d, %x,isPress[%d],autoRepeat[%d]", uni, keyId,isPress,autoRepeat);
#endif

  if (!isPress)
    return false;

#ifdef DEBUG
  qDebug("DEBUG: KeyPressed: %d, %x", uni, keyId);
#endif

  if (!currentIM->getPreEditable() && (Qt::Key_Space != keyId))//用于解决英文输入下，按空格无效
    return false;

  switch (keyId) {
    case Qt::Key_Space:
      if (currentIM->isPreEditing()) doSendEvent = false;
      currentIM->handle_Space();
      break;
    case Qt::Key_Escape: currentIM->handle_Esc(); break;
    case Qt::Key_Enter:
    case Qt::Key_Return:
      if (currentIM->isPreEditing()) doSendEvent = false;
      currentIM->handle_Enter();
      break;
    case Qt::Key_Delete: currentIM->handle_Del(); break;
    case Qt::Key_Backspace:
      if (currentIM->isPreEditing()) doSendEvent = false;
      currentIM->handle_Backspace();
      break;
    case Qt::Key_Tab: currentIM->handle_Tab(); break;
    case Qt::Key_Left:
      if (currentIM->isPreEditing()) doSendEvent = false;
      currentIM->handle_Left();
      break;
    case Qt::Key_Right:
      if (currentIM->isPreEditing()) doSendEvent = false;
      currentIM->handle_Right();
      break;
    case Qt::Key_Up: currentIM->handle_Up(); break;
    case Qt::Key_Home: currentIM->handle_Home(); break;
    case Qt::Key_End: currentIM->handle_End(); break;
    case Qt::Key_PageUp: currentIM->handle_PageUp(); break;
    case Qt::Key_PageDown: currentIM->handle_PageDown(); break;
    case Qt::Key_Down: currentIM->handle_Down(); break;
    case Qt::Key_CapsLock: currentIM->handle_Capslock(); break;
    case Qt::Key_Control: currentIM->handle_Ctrl(); break;
    case Qt::Key_Alt: currentIM->handle_Alt(); break;
    default:
      if (keyId & Qt::Key_Escape)
        return true;
      currentIM->handle_Default(keyId);
      doSendEvent = false;
  }
//从输入法核心中获取Commit String，并上屏
  updateCommitString();
//从输入法核心中获取Preedit String，并上屏
  if (currentIM->getPreEditable())
    updatePreEditBuffer();
//判断是否存在侯选词，如果存在则从输入法核心中获取选词列表，并显示在侯选列表
  if (currentIM->getDoPopUp())
  {
	  vkeyboard->showCandStrBar(currentIM->getPopUpStrings());
	  qDebug()<< __FILE__ << __FUNCTION__ << __LINE__;
  }
  else
  {
  	vkeyboard->clearCandStrBar();
  }
//设置PreEdit String中的光标位置
  selectPreEditWord(currentIM->cursorCurrent());

  return !doSendEvent;//返回true，则阻止事件做进一步处理
}
	
bool QinEngine::filter(const QPoint& point, int state, int wheel)
{
	if(0 == state)//按下释放,解决手动隐藏虚拟键盘后，再次点击输入框，能够再次弹出键盘
	{
		if(!vkeyboard->isVisible())
		{
			currentIM->reset();
			vkeyboard->show();
		}			
	}
	return QWSInputMethod::filter(point,state,wheel);
}

void QinEngine::updateCommitString() {
  char* commit_str = currentIM->getCommitString();
  if (commit_str) {
    sendCommitString(commit_str);
    delete commit_str;
  }
}

void QinEngine::updatePreEditBuffer() {
  char* preedit = currentIM->getPreEditString();
  inputBuffer = QString(preedit);
  sendPreeditString(inputBuffer, 0);
  delete preedit;
}

void QinEngine::updateHandler(int type) {
  switch (type) {
    case QWSInputMethod::FocusIn:
      currentIM->reset();
      vkeyboard->show();
      break;

    case QWSInputMethod::FocusOut:
      inputBuffer.clear();
      currentIM->reset();
      vkeyboard->hideAll();
      break;
	  case QWSInputMethod::Update:
		  currentIM->update();
	  	break;
  }
}

void QinEngine::mouseHandler(int offset, int state) {
  if (state == QWSServer::MousePress && offset >= 0) {
    currentIM->setCursor(offset);
    sendPreeditString(inputBuffer, offset, 1);
    selected = offset;
  }
//  qDebug("mouseHandler offset[%d], state[%d",offset, state);
}

/**
 * @brief 设置PreEdit String中的光标位置
 * @param index 为cursorPosition
 */
void QinEngine::selectPreEditWord(int index) {
  if (index != -1)
    sendPreeditString(inputBuffer, index, 1);
}
