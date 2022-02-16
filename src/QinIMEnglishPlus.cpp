/**
 * @file   QinIMBases.cpp
 * @brief  
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

#include "QinIMEnglishPlus.h"

/* QinIMBase methods implementation */

QinIMEnglishPlus::QinIMEnglishPlus(QString xmlpath):  QinIMBase(xmlpath){
  QDomElement root = xml.documentElement();
  keyIndex = 0;
  maxKeyStrokes = root.firstChildElement("maxkeystrokes").text().toInt();
  keyStrokes = new int[maxKeyStrokes + 1]();
}

QinIMEnglishPlus::~QinIMEnglishPlus() {
	delete keyStrokes;
}

QStringList QinIMEnglishPlus::getPopUpStrings(void) {
qDebug()<<__LINE__<<__func__;
  return results;
}

bool QinIMEnglishPlus::isPreEditing(void) {
  return keyIndex != 0;
}

char* QinIMEnglishPlus::getPreEditString(void) {
  qDebug()<<__LINE__<<__func__;

  char* preEditStr = NULL;
  const char* cstr = NULL;

  if (!results.isEmpty()) {
    cstr = results[0].toStdString().c_str();
    preEditStr = new char[strlen(cstr) + 1];
    memcpy(preEditStr, cstr, strlen(cstr));
    preEditStr[strlen(cstr)] = 0;
  }

#ifdef DEBUG
  if (preEditStr) {
    qDebug() << "DEBUG: results[0]: " << results[0];
    qDebug("DEBUG: preEditStr: %s", preEditStr);
  }
#endif

  return preEditStr;
}

char* QinIMEnglishPlus::getCommitString(void) {
	qDebug()<<__LINE__<<__func__;

#if 0
	char* commitStr = NULL;
	const char* cstr = NULL;
	
	if (!m_commitStr.isEmpty()) {
	  cstr = m_commitStr.toStdString().c_str();
	  commitStr = new char[strlen(cstr) + 1];
	  memcpy(commitStr, cstr, strlen(cstr));
	  commitStr[strlen(cstr)] = 0;
	  m_commitStr.clear();
	}
	return commitStr;
#endif

	char* commitStr = NULL;
  const char* cstr = NULL;

  if (!commitString.isEmpty()) {
    cstr = commitString.toStdString().c_str();
    commitStr = new char[strlen(cstr) + 1];
    memcpy(commitStr, cstr, strlen(cstr));
    commitStr[strlen(cstr)] = 0;
    commitString.clear();
  }
  return commitStr;


}

int QinIMBase::cursorCurrent(void) {
  return -1;
}

void QinIMBase::setCursor(int) {}
void QinIMBase::reset(void) {}
void QinIMBase::update(void) {}
void QinIMBase::handle_Alt(void) {}
void QinIMBase::handle_Backspace(void) {}
void QinIMBase::handle_Capslock(void) {}
void QinIMBase::handle_Ctrl(void) {}

void QinIMBase::handle_Default(int keyId) {
  qDebug()<<__LINE__<<__func__<<"English"<<keyId;

  int keys[] = SELKEYS;
  if (results.size()) {
    for (size_t i = 0; i < SELKEY_COUNT; ++i)
      if (keyId == keys[i]) {
        commitString = results[i];
        results.clear();
        keyIndex = 0;
        return;
      }
  }

  keyStrokes[keyIndex++] = keyId;
  doQuery();
}

void QinIMBase::handle_Del(void) {}
void QinIMBase::handle_Down(void) {}
void QinIMBase::handle_End(void) {}
void QinIMBase::handle_Enter(void) {}
void QinIMBase::handle_Esc(void) {}
void QinIMBase::handle_Home(void) {}
void QinIMBase::handle_Left(void) {}
void QinIMBase::handle_PageDown(void) {}
void QinIMBase::handle_PageUp(void) {}
void QinIMBase::handle_Right(void) {}
void QinIMBase::handle_Space(void) {}
void QinIMBase::handle_Tab(void) {}
void QinIMBase::handle_Up(void) {}

void QinIMBase::doQuery(void){
    /* 英文联想逻辑 QStemp => results; */
	//Trie_Tree tree("../data/count_1w.txt");
	//qDebug()<<tree.getWordsForPrefix("good").size()<<"XXXXXXXXXX";
	qDebug()<<__LINE__<<__func__<<keyIndex;

	results.clear();
	QString QStemp = "";
	for(int i = 0; i < keyIndex;i++)
		QStemp += QString(QChar(keyStrokes[i]));

	results += QStemp;
	// 获取候选列表并展示
	//for(int i = 0; i < 9; i++)
	//	results += QString(('A' + i));
	
	qDebug()<<__LINE__<<__func__<<results;
}
