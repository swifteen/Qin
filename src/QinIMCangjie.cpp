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


#include <cstring>
#include <QCoreApplication>
#include <QDebug>
#include <QDomDocument>
#include <QDomElement>
#include <QFile>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QTextStream>
#include <QVariant>

#include "QinIMCangjie.h"
#include <QDebug>

QinCangjieIMBase::QinCangjieIMBase(QString xmlpath): QinTableIMBase(xmlpath) {
	qDebug()<<__LINE__<<__func__;
	curpage = 0;
}

#if 1
void QinCangjieIMBase::doQuery(void){
  int count = 0;
  QString queryTemplate = getQueryTemplate();
  QString query = queryTemplate;
  qDebug()<<__LINE__<<__func__<<maxKeyStrokes;
  qDebug()<<__LINE__<<__func__<<keyIndex;
  qDebug()<<__LINE__<<__func__<<query<<"XXXXXXXXXXXXXXXXXXXXXX0";
  
  	QString strTemp = "";
	
    for (int i = 0; i < keyIndex; ++i) 
		strTemp += QString(tolower(keyStrokes[i]));
		
	qDebug()<<__LINE__<<__func__<<strTemp<<"XXXXXXXXXXXXXXXXXXXXXX1";

	if(keyIndex != 0)
	{
		//query = query.replace("%1", "'"+ str + "%'");
		query = query.arg("'"+ strTemp + "%'");
	}
	else
	{	
		//query = query.replace("%1", "''");
	    query = query.arg("''");
	}
	query = query.arg("limit " + QString::number(curpage * 10) + ", " + QString::number(10));
	//query = query + "limit "+QString::number(10, curpage*10)+"," + QString::number(10, 10);
	qDebug()<<__LINE__<<__func__<<query<<"XXXXXXXXXXXXXXXXXXXXXXXX2";
  
#ifdef DEBUG
  qDebug() << "DEBUG: query: " << query;
#endif

  results.clear();
  QSqlQuery queryResults = database.exec(query);
  //wf debug
  //qDebug()<<__LINE__<<__func__<<queryResults.next();
  //while (queryResults.next() && count++ < 10)
  //  results += queryResults.value(0).toString();

  while (queryResults.next())
    results += queryResults.value(0).toString();

  qDebug()<<__LINE__<<__func__<<"XXXXXXXXXXXXXXXXXXXXXXXX3";
  qDebug()<<results;

}

void QinCangjieIMBase::handle_Left(void){
	 qDebug()<<__LINE__<<__func__<<"XXXXXXXXXXXXXXXXXXXXXXXY0";
	 if(curpage > 0)
	 {  --curpage;
	 	doQuery();
	 }
}

void QinCangjieIMBase::handle_Right(void){
	 qDebug()<<__LINE__<<__func__<<"XXXXXXXXXXXXXXXXXXXXXXXY1";
	 ++curpage;
	 doQuery();
}

void QinCangjieIMBase::handle_Backspace(void){
	 qDebug()<<__LINE__<<__func__<<"XXXXXXXXXXXXXXXXXXXXXXXY2";
	 curpage = 0;
	 QinTableIMBase::handle_Backspace();
}

void QinCangjieIMBase::handle_Default(int num){
	qDebug()<<__LINE__<<__func__<<"XXXXXXXXXXXXXXXXXXXXXXXY3";
	curpage = 0;
	QinTableIMBase::handle_Default(num);
}

#endif


#if 0
void QinWubiIMBase::doQuery(void){
  qDebug()<<__LINE__<<__func__;
  int count = 0;
  QString queryTemplate = getQueryTemplate();
  QString query = queryTemplate;
  qDebug()<<__LINE__<<__func__<<maxKeyStrokes;
  qDebug()<<__LINE__<<__func__<<keyIndex;
  qDebug()<<__LINE__<<__func__<<query;
  
  for (int i = 0; i < maxKeyStrokes; ++i) {
    if (i < keyIndex) {
      query = query.arg("m%1=%2%3").arg(i).arg(
          keyTransform[tolower(keyStrokes[i])]);
	  qDebug()<<__LINE__<<__func__<<query;
      if (i != keyIndex -1)
        query = query.arg(" AND %1");
      else
        query = query.arg("");
	 qDebug()<<__LINE__<<__func__<<query;
	}
  }

  //wf 五笔特有的属性
  query = query.replace("=26", " like '%'");
  qDebug()<<__LINE__<<__func__<<query;

#ifdef DEBUG
  qDebug() << "DEBUG: query: " << query;
#endif

  results.clear();
  QSqlQuery queryResults = database.exec(query);
  //wf debug
  //qDebug()<<__LINE__<<__func__<<queryResults.next();
  while (queryResults.next() && count++ < 10)
    results += queryResults.value(0).toString();

  qDebug()<<__LINE__<<__func__<<results;

}
#endif


