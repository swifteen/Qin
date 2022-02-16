/**
 * @file   QinIMTables.cpp
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
#include <QCoreApplication>
#include <QDebug>
#include <QDomDocument>
#include <QDomElement>
#include <QFile>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QTextStream>

#include "QinIMTables.h"
/* QinTableIMBase methods implementation */

QinTableIMBase::QinTableIMBase(QString xmlpath): QinIMBase(xmlpath) {
	qDebug()<<__LINE__<<__func__;
  QFile file(xmlPath);
  QString xmlData, err;
  int line, column;

  if (file.open(QFile::ReadOnly)) {
    QTextStream styleIn(&file);
    xmlData = styleIn.readAll();
    file.close();
  } else {
    qDebug() << "Fatal error: can't open `" << xmlPath << "' ..."
      << "abort.";
    QCoreApplication::exit(1);
  }

  QDomDocument xml;
  if (!xml.setContent(xmlData, &err, &line, &column)) {
    qDebug() << "Fatal error: error while parsing `" << xmlPath << "', "
      << line << ", " << column << ": " << err;
    QCoreApplication::exit(1);
  }

  QDomElement root = xml.documentElement();

  maxKeyStrokes = root.firstChildElement("maxkeystrokes").text().toInt();
#ifdef DEBUG
  qDebug() << "DEBUG: maxKeyStrokes: " << maxKeyStrokes;
#endif

  dbPath = root.firstChildElement("database").text();
#ifdef DEBUG
  qDebug() << "DEBUG: dbPath: " << dbPath;
#endif

  queryTemplate = root.firstChildElement("querytemplate").text();
#ifdef DEBUG
  qDebug() << "DEBUG: queryTemplate: " << queryTemplate;
#endif

  QDomElement keytransform = root.firstChildElement("keytransform");
  QDomNode node = keytransform.firstChild();
  QDomElement nodeElement;
  QChar attr;
  while (!node.isNull()) {
    nodeElement = node.toElement();
    attr = nodeElement.attribute("value")[0];
    keyTransform[attr.toAscii()] = nodeElement.text().toInt();
    node = node.nextSibling();
  }

  /* Initialize members */
  keyIndex = 0;
  keyStrokes = new int[maxKeyStrokes + 1];

  database = QSqlDatabase::addDatabase("QSQLITE", dbPath);
  database.setDatabaseName(dbPath);
  if (!database.open()) {
    qDebug() << "Fatal error: can't find database `" << dbPath << "' ..."
      << "abort.";
    QCoreApplication::exit(1);
  }
  database.setConnectOptions("QSQLITE_OPEN_READONLY");
}

QinTableIMBase::~QinTableIMBase() {
  database.close();
  delete[] keyStrokes;
}

QString QinTableIMBase::getQueryTemplate(void) {
  return queryTemplate;
}

bool QinTableIMBase::isPreEditing(void) {
  qDebug()<<__LINE__<<__func__;
  return keyIndex != 0;
}

bool QinTableIMBase::getDoPopUp(void) {
  qDebug()<<__LINE__<<__func__;
  return true;
}

QStringList QinTableIMBase::getPopUpStrings(void) {
  qDebug()<<__LINE__<<__func__;
  return results;
}

void QinTableIMBase::doQuery(void) {
  int count = 0;
  QString queryTemplate = getQueryTemplate();
  QString query = queryTemplate;
  qDebug()<<__LINE__<<__func__<<maxKeyStrokes;
  qDebug()<<__LINE__<<__func__<<keyIndex;
  qDebug()<<__LINE__<<__func__<<query<<"XXXXXXXXXXXXXXXX0";
  
  for (int i = 0; i < maxKeyStrokes; ++i) {
    if (i < keyIndex) {
      query = query.arg("m%1=%2%3").arg(i).arg(
          keyTransform[tolower(keyStrokes[i])]);
	  //qDebug()<<__LINE__<<__func__<<query<<"XXXXXXXXXXXXXXXX1";
      if (i != keyIndex -1)
        query = query.arg(" AND %1");
      else
        query = query.arg("");
	 //qDebug()<<__LINE__<<__func__<<query<<"XXXXXXXXXXXXXXXX2";
	}
  }

#ifdef DEBUG
  qDebug() << "DEBUG: query: " << query;
#endif

  results.clear();
  QSqlQuery queryResults = database.exec(query);
  while (queryResults.next() && count++ < 10)
    results += queryResults.value(0).toString();

  qDebug()<<__LINE__<<__func__;
  qDebug()<<results;
}

char* QinTableIMBase::getPreEditString(void) {
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

char* QinTableIMBase::getCommitString(void) {
	qDebug()<<__LINE__<<__func__;

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

void QinTableIMBase::handle_Default(int keyId) {
  qDebug()<<__LINE__<<__func__<<"无虾米";
  int keys[] = SELKEYS;
  if (results.size()) {
    for (size_t i = 0; i < SELKEY_COUNT; ++i)
      if (keyId == keys[i]) {
        commitString = results[i];
		 qDebug()<<__LINE__<<__func__<<commitString;
        results.clear();
        keyIndex = 0;
        return;
      }
  }

  if (keyIndex == maxKeyStrokes){
  	  qDebug()<<__LINE__<<__func__<<keyIndex<<maxKeyStrokes<<"wwwwwwwwwwwwwwwwww1";
	  return;
  }

  if (keyTransform.find(tolower(keyId)) == keyTransform.end()){
	qDebug()<<__LINE__<<__func__<<"wwwwwwwwwwwwwwwwwwwwwwwwww";
	return;
  }

  keyStrokes[keyIndex++] = keyId;
  doQuery();
}

void QinTableIMBase::handle_Space(void) {
  doQuery();
  if (results.isEmpty())
    commitString.clear();
  else {
    commitString = results[0];
    results.clear();
  }

  /* reset keyStrokes */
  keyIndex = 0;
}

void QinTableIMBase::handle_Enter(void) {
  handle_Space();
}

void QinTableIMBase::handle_Backspace(void) {
  if (keyIndex > 0)
    --keyIndex;
  doQuery();
}
