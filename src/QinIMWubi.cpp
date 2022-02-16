/**
 * @file   QinIMTables.h
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
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QTextStream>

#include "QinIMWubi.h"

QinWubiIMBase::QinWubiIMBase(QString xmlpath): QinTableIMBase(xmlpath) {
	qDebug()<<__LINE__<<__func__;
}

#if 1
void QinWubiIMBase::doQuery(void){
  int count = 0;
  QString queryTemplate = getQueryTemplate();
  QString query = queryTemplate;
  qDebug()<<__LINE__<<__func__<<maxKeyStrokes<<"XXXXXXXXXXXXXXXXXX";
  qDebug()<<__LINE__<<__func__<<keyIndex<<"XXXXXXXXXXXXXXXXXX";
  qDebug()<<__LINE__<<__func__<<query<<"XXXXXXXXXXXXXXXXXX0";
  
  for (int i = 0; i < maxKeyStrokes; ++i) {
    if (i < keyIndex) {
      query = query.arg("m%1=%2%3").arg(i).arg(keyTransform[tolower(keyStrokes[i])]);
	  //qDebug()<<__LINE__<<__func__<<query<<"XXXXXXXXXXXXXXXXXXX1";
      if (i != keyIndex -1)
        query = query.arg(" AND %1");
      else
        query = query.arg("");
	 qDebug()<<__LINE__<<__func__<<query<<"XXXXXXXXXXXXXXXXXXX2";
	}
  }

  //wf 五笔特有的属性
  //无法保证m1 is Null的情况
  //query = query.replace("=26", " like '%'");
  
  //wf z开头的特殊处理
  if(query.indexOf("m0=26") > -1){
     query = query.replace(" ORDER BY -freq", "");
	 qDebug()<<__LINE__<<__func__<<query<<"XXXXXXXXXXXXXXXXXXX21";
  }

/* */
 query = query.replace(QRegExp("m.=26"), "1=1");
  
#ifdef DEBUG
  qDebug() << "DEBUG: query: " << query;
#endif

  results.clear();
  QSqlQuery queryResults = database.exec(query);
  while (queryResults.next() && count++ < 10)
    results += queryResults.value(0).toString();

  qDebug()<<__LINE__<<__func__<<"XXXXXXXXXXXXXXXXXXX3";
  qDebug()<<results;
}
#endif


