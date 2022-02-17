/**
 * @file   QinEngine.h
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

#ifndef __QIN_SRC_QIN_ENGINE_H__
#define __QIN_SRC_QIN_ENGINE_H__

#include <QWSInputMethod>
#include "QVirtualKeyboard.h"
#include "QinIMBases.h"

class QVirtualKeyboard;

class QinEngine: public QWSInputMethod {
  Q_OBJECT

  friend class QVirtualKeyboard;

  public:
    QinEngine();
    ~QinEngine();
    void regInputMethod(QinIMBase* imb);
	void regNumAndSymbolInputMethod(QinIMBase* imb);
    void setCurrentIM(int index);
	void setCurrentNumAndSymbolIM(int index);
	bool getCurrentNumAndSymbolState();
	void sendIMQuery(int property);
private:	
  bool filter(int uni, int keyId, int mod, bool isPress, bool autoRepeat);
  bool filter(const QPoint& point, int state, int wheel);
  virtual void queryResponse(int property, const QVariant& result);
  void updateHandler(int type);
  void mouseHandler(int offset, int state);
  void updateCommitString();
  void updatePreEditBuffer(void);
  void selectPreEditWord(int index);
  private:
    QVirtualKeyboard* vkeyboard;
    QString inputBuffer;
    QVector<QinIMBase*> numAndSymbolInputMethods;//符号和数字输入在不同的语言下存在差异
    QVector<QinIMBase*> inputMethods;
    QinIMBase* defaultIM;
    QinIMBase* currentIM;
    int selected;
	bool m_bNumAndSymbol;//当前状态是否为符号和数字输入
	int m_cursorPosition;
	QString m_surroundingText;
};

#endif /* __QIN_SRC_QIN_ENGINE_H__ */
