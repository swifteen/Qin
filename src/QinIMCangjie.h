#ifndef __QIN_SRC_QIN_IM_CANGJIE_H__
#define __QIN_SRC_QIN_IM_CANGJIE_H__

#include "QinIMTables.h"

/**
 * @name  QinTableIMBase
 * @brief Base class for table input methods.
 */
class QinCangjieIMBase: public QinTableIMBase
{
public:
    /** Public methods **/
    QinCangjieIMBase(QString xmlpath);
    virtual void doQuery(void);
};

#endif /* __QIN_SRC_QIN_IM_TABLE_H__ */
