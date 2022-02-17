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

#include <QDebug>
#include <QSqlQuery>

#include "QinIMCangjie.h"


QinCangjieIMBase::QinCangjieIMBase(QString xmlpath): QinTableIMBase(xmlpath) {}

void QinCangjieIMBase::doQuery(void)
{
    qDebug() << __LINE__ << __func__ << "doQuery仓颉";
    QString queryTemplate = getQueryTemplate();
    QString query = queryTemplate;
    QString strTemp = "";

    for (int i = 0; i < keyIndex; ++i) {
        strTemp += QString(tolower(keyStrokes[i]));
    }

    if (keyIndex != 0) {
        query = query.arg("'" + strTemp + "%'");
    }
    else {
        query = query.arg("''");
    }

    query = query.arg("limit " + QString::number(curpage * 10) + ", " + QString::number(10));
#ifdef DEBUG
    qDebug() << "DEBUG: query: " << query;
#endif
    results.clear();
    QSqlQuery queryResults = database.exec(query);

    while (queryResults.next()) {
        results += queryResults.value(0).toString();
    }

    qDebug() << __LINE__ << __func__ << results;
}

