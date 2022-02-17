#ifndef __QIN_SRC_QIN_IM_WUBI_H__
#define __QIN_SRC_QIN_IM_WUBI_H__

#include "QinIMTables.h"

class QinWubiIMBase: public QinTableIMBase {
  public:
    QinWubiIMBase(QString xmlpath);
	virtual void doQuery(void);
};

#endif /* __QIN_SRC_QIN_IM_TABLE_H__ */
