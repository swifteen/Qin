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

#ifndef __QIN_SRC_QIN_IM_TABLE_H__
#define __QIN_SRC_QIN_IM_TABLE_H__

#include <QHash>
#include <QSqlDatabase>
#include <QStringList>

#include "QinIMBases.h"

/**
 * @name  QinTableIMBase
 * @brief Base class for table input methods.
 */
class QinTableIMBase: public QinIMBase
{
protected:
    QString dbPath;
    QSqlDatabase database;
    QString queryTemplate;
    QHash<int, int> keyTransform;
    QString commitString;
    QStringList results;
    int* keyStrokes;
    int maxKeyStrokes;
    int keyIndex;
    int curpage;

public:
    /** Public methods **/
    QinTableIMBase(QString xmlpath);
    virtual ~QinTableIMBase();

    virtual bool isPreEditing(void);
    virtual bool getDoPopUp(void);
    virtual QStringList getPopUpStrings(void);
    virtual void doQuery(void);
    virtual QString getQueryTemplate(void);

    /** I/O related **/
    /* Caller must free it */
    virtual char* getPreEditString(void);
    virtual char* getCommitString(void);

    /** Key handling APIs **/
    virtual void handle_Default(int unicode, int keyId);
    virtual void handle_Enter(void);
    virtual void handle_Space(void);
    virtual void handle_Backspace(void);
    virtual void handle_Left(void);
    virtual void handle_Right(void);
};

#endif /* __QIN_SRC_QIN_IM_TABLE_H__ */
