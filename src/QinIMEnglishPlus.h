#ifndef __QIN_SRC_QIN_IM_ENGLISH_H__
#define __QIN_SRC_QIN_IM_ENGLISH_H__


#include "QinIMBases.h"

/**
 * @name  QinIMBase
 * @brief Base class for all input method for Qin. New input methods should
 * derive from QinIMBase. 
 */
class QinIMEnglishPlus : public QinIMBase{
		//int keyIndex;
	//int* keyStrokes;
 //   int maxKeyStrokes;
	public:
	    QinIMEnglishPlus(QString xmlpath);
	    virtual ~QinIMEnglishPlus();
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
	    virtual void handle_Default(int keyId);
	    virtual void handle_Enter(void);
	    virtual void handle_Space(void);
	    virtual void handle_Backspace(void);
}
  
#endif /* __QIN_SRC_QIN_IM_ENGLISH_H__ */
