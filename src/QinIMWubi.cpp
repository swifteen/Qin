#include <QDebug>
#include <QSqlQuery>

#include "QinIMWubi.h"

QinWubiIMBase::QinWubiIMBase(QString xmlpath): QinTableIMBase(xmlpath) {}

void QinWubiIMBase::doQuery(void){
  qDebug()<<__LINE__<<__func__<<"doQuery五笔";
  QString queryTemplate = getQueryTemplate();
  QString query = queryTemplate;

  for (int i = 0; i < maxKeyStrokes; ++i) {
    if (i < keyIndex) {
      query = query.arg("m%1=%2%3").arg(i).arg(keyTransform[tolower(keyStrokes[i])]);
      if (i != keyIndex -1)
        query = query.arg(" AND %1");
      else
        query = query.arg("");
	}
  }

  //wf 五笔特有的属性
  //无法保证m1 is Null的情况
  //query = query.replace("=26", " like '%'");
  
  //wf z开头的特殊处理
  if(query.indexOf("m0=26") > -1){
     query = query.replace(" ORDER BY -freq", "");
  }

 query = query.replace(QRegExp("m.=26"), "1=1");
 query = query.replace(";", " limit " + QString::number(curpage * 10) + ", " + QString::number(10) + ";");
 
#ifdef DEBUG
  qDebug() << "DEBUG: query: " << query;
#endif

   results.clear();
   QSqlQuery queryResults = database.exec(query);
   while (queryResults.next())
   		results += queryResults.value(0).toString();	

    qDebug()<<__LINE__<<__func__<<results;
}

