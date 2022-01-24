/**
 * @file   QinChewing.cpp
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

#include "QinChewing.h"
#include "QinIMBases.h"

#include <stdio.h>
#include <cctype>
#include <cstdlib>
#include <cstring>
#include <cmath>

#include <QStringList>
#include <chewing.h>
#include <QDebug>
QinChewing::QinChewing(void): QinIMBase(":/data/Chewing.xml") {
  int keys[] = SELKEYS;
#if 0
  chewContext = chewing_new();
  chewing_Init(QIN_CHEWING_DATA_PATH, QIN_CHEWING_HASH_PATH);
#else  
  chewContext = chewing_new2(QIN_CHEWING_DATA_PATH, NULL, NULL, NULL);
#endif

  chewing_set_ChiEngMode(chewContext, CHINESE_MODE);
  chewing_set_ShapeMode(chewContext, HALFSHAPE_MODE);
  chewing_set_candPerPage(chewContext, QIN_CHEWING_CAND_PER_PAGE);
  chewing_set_maxChiSymbolLen(chewContext, QIN_ENGINE_MAX_CHINESE_SYMBOL_LEN);
  chewing_set_spaceAsSelection(chewContext, true);
  chewing_set_escCleanAllBuf(chewContext, true);
  chewing_set_autoShiftCur(chewContext, true);
  chewing_set_phraseChoiceRearward(chewContext, true);
  chewing_set_selKey(chewContext, keys, SELKEY_COUNT);
  chewing_set_easySymbolInput(chewContext, 0);
}

QinChewing::~QinChewing(void) {
#if 0
  chewing_Terminate();
#endif
  chewing_delete(chewContext);
}

/**
 * @brief 判断是否存在预编辑
 *
 * @return 返回true代表存在
 */
bool QinChewing::isPreEditing(void) {
  char* preedit = getPreEditString();
  bool ret = strlen(preedit);
  delete preedit;
  return ret;
}

/**
 * @brief This function checks if there are more candidates to enumerate.
  Note: The 'chewing_cand_hasNext' function checks the end of
  total choices instead of the end of current page.
 * @return 返回true代表存在
 */
bool QinChewing::getDoPopUp(void) {
	qDebug("DEBUG: chewing_cand_TotalChoice: %d, %d", chewing_cand_TotalChoice(chewContext),chewing_cand_hasNext(chewContext));
    chewing_cand_Enumerate(chewContext);
	
  return chewing_cand_hasNext(chewContext);
}

/**
 * @brief 获取侯选词列表
 *
 * @return 返回true代表存在
 */
QStringList QinChewing::getPopUpStrings(void) {
  QStringList cands;
  const char* word = NULL;
  int choices = chewing_cand_ChoicePerPage(chewContext);//returns the number of the choices per page.

  if (chewing_cand_TotalChoice(chewContext)) {//returns the number of the available choices.
    chewing_cand_Enumerate(chewContext);

  /*
  -- Function: int chewing_cand_hasNext (ChewingContext *CTX)
	  This function checks if there are more candidates to enumerate.
  
		   Note: The 'chewing_cand_hasNext' function checks the end of
		   total choices instead of the end of current page.
  */
    while (choices-- && chewing_cand_hasNext(chewContext)) {
      word = chewing_cand_String_static(chewContext);
      cands += word;
//      free(word);
    }
  }
  qDebug()<< __FILE__ << __FUNCTION__ << __LINE__<<cands;

  
#if 0
  chewing_cand_open(chewContext);
  
	if (!chewing_cand_CheckDone(chewContext)) {
		// Get candidate words
		chewing_cand_Enumerate(chewContext);
		while (chewing_cand_hasNext(chewContext)) {
			char *chewingCand = chewing_cand_String(chewContext);
			QString candidate(chewingCand);
			m_candidates.append(choppedBuffer + candidate);
			chewing_free(chewingCand);
		}
	}
  
	if (chewing_buffer_Len(chewContext) <= chewing_cursor_Current(chewContext)) {	 
		// Insert bopomofo string
		m_candidates.prepend(buffer + QString(chewing_bopomofo_String_static(chewContext)));
	}
  
	chewing_cand_close(chewContext);
#endif
  return cands;
}

char* QinChewing::getPreEditString(void) {
  int preedit_len;
  const char* buf_str = chewing_buffer_String_static(chewContext);//returns the current output in the pre-edit buffer.
  const char* bopomofo_str = chewing_bopomofo_String_static(chewContext);//returns the phonetic characters in the pre-edit buffer.
  int max_len = strlen(bopomofo_str) + strlen(buf_str);
  char* preedit_str = new char[max_len + 1];
  memset(preedit_str, 0, max_len + 1);

#ifdef DEBUG
  qDebug("-------------------------------------------------");
  qDebug("DEBUG: Buf: %s", buf_str);
  qDebug("DEBUG: bopomofo_str: %s", bopomofo_str);
  qDebug("DEBUG: Commit: %d", chewing_commit_Check(chewContext));//hecks whether or not the commit buffer has a output to commit.
  qDebug("DEBUG: Cand TotalPage: %d", chewing_cand_TotalPage(chewContext));//returns the number of pages of the candidates.
  qDebug("DEBUG: Cand hasNext: %d", chewing_cand_hasNext(chewContext));//checks if there are more candidates to enumerate.checks the end of total choices instead of the end of current page.
  qDebug("DEBUG: Aux: %s", chewing_aux_String_static(chewContext));//returns the current auxiliary string.
  qDebug("DEBUG: Aux Check: %d", chewing_aux_Check(chewContext));//checks whether there is auxiliary string in the auxiliary buffer.
#endif
#if 0
  strncpy(preedit_str, buf_str, max_len);
  strncat(preedit_str, bopomofo_str, max_len - strlen(preedit_str));

  preedit_str[max_len] = 0;
#else  
  snprintf(preedit_str,max_len + 1,"%s%s",buf_str,bopomofo_str);
#endif

//  free(buf_str);
//  free(zuin_str);

  return preedit_str;
}

char* QinChewing::getCommitString(void) {
  int committed = chewing_commit_Check(chewContext);//checks whether or not the commit buffer has a output to commit.
  char* commit_str = NULL;
  if (committed)
    commit_str = chewing_commit_String(chewContext);//returns the string in the commit buffer. 
  return commit_str;
}

int QinChewing::cursorCurrent(void) {
  return chewing_cursor_Current(chewContext);
}

void QinChewing::setCursor(int index) {
  int diff = cursorCurrent() - index;
  for (int i = 0; i < abs(diff); ++i) {
    if (diff > 0)
      handle_Left();
    else
      handle_Right();
  }
}

void QinChewing::reset(void) {
  chewing_handle_Esc(chewContext);
}

void QinChewing::handle_Default(int keyId) {
  chewing_handle_Default(chewContext, tolower(keyId));

}

void QinChewing::handle_Space(void) {
  chewing_handle_Space(chewContext);
}

void QinChewing::handle_Esc(void) {
  chewing_handle_Esc(chewContext);
}

void QinChewing::handle_Enter(void) {
  chewing_handle_Enter(chewContext);
}

void QinChewing::handle_Del(void) {
  chewing_handle_Del(chewContext);
}

void QinChewing::handle_Backspace(void) {
  chewing_handle_Backspace(chewContext);
}

void QinChewing::handle_Tab(void) {
  chewing_handle_Tab(chewContext);
}

void QinChewing::handle_Left(void) {
	qDebug()<< __FILE__ << __FUNCTION__ << __LINE__;
  chewing_handle_Left(chewContext);
}

void QinChewing::handle_Right(void) {
	qDebug()<< __FILE__ << __FUNCTION__ << __LINE__;
  chewing_handle_Right(chewContext);
}

void QinChewing::handle_Up(void) {
  chewing_handle_Up(chewContext);
}

void QinChewing::handle_Home(void) {
  chewing_handle_Home(chewContext);
}

void QinChewing::handle_End(void) {
  chewing_handle_End(chewContext);
}

void QinChewing::handle_PageUp(void) {
  chewing_handle_PageUp(chewContext);
}

void QinChewing::handle_PageDown(void) {
  chewing_handle_PageDown(chewContext);
}

void QinChewing::handle_Down(void) {
  chewing_handle_Down(chewContext);
}

void QinChewing::handle_Capslock(void) {
  chewing_handle_Capslock(chewContext);
}
