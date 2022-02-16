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

#ifndef __QIN_SRC_QIN_IM_CANGJIE_H__
#define __QIN_SRC_QIN_IM_CANGJIE_H__

#include "QinIMTables.h"

/**
 * @name  QinTableIMBase
 * @brief Base class for table input methods.
 */
class QinCangjieIMBase: public QinTableIMBase {
private:
	int curpage;
  public:
    /** Public methods **/
    QinCangjieIMBase(QString xmlpath);
	virtual void doQuery(void);
	virtual void handle_Left(void);
	virtual void handle_Right(void);
	virtual void handle_Backspace(void);
	virtual void handle_Default(int num);
};

#endif /* __QIN_SRC_QIN_IM_TABLE_H__ */
