#include <QDebug>
#include "QinIMEnglishPlus.h"
#define maxKeyStrokes 20
/* QinIMBase methods implementation */

QinIMEnglishPlus::QinIMEnglishPlus(QString xmlpath):  QinIMBase(xmlpath){
  //QDomElement root = xml.documentElement();
  keyIndex = 0;
  //maxKeyStrokes = root.firstChildElement("maxkeystrokes").text().toInt();
  keyStrokes = new int[maxKeyStrokes + 1]();
  //preEditable = true;
}

QinIMEnglishPlus::~QinIMEnglishPlus() {
	delete keyStrokes;
}

bool QinIMEnglishPlus::getDoPopUp(void){
	qDebug()<<__LINE__<<__func__;
	return true;
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

void QinIMEnglishPlus::handle_Default(int keyId) {
  qDebug()<<__LINE__<<__func__<<"English plus"<<keyId;

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

void QinIMEnglishPlus::doQuery(void){
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

void QinIMEnglishPlus::handle_Backspace(void) {
  if (keyIndex > 0)
    --keyIndex;
  doQuery();
}

