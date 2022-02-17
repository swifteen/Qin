#ifndef __QIN_SRC_PLUGIN_QIN_GOOGLE_PINYIN_H__
#define __QIN_SRC_PLUGIN_QIN_GOOGLE_PINYIN_H__

#include "QinConfig.h"
#include "QinIMBases.h"
#include <QPointer>
#include <QVector>
#include <QWSInputMethod>

#define PREEDIT_LENGTH_MAX 50

class PinyinDecoderService : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(PinyinDecoderService)
    explicit PinyinDecoderService(QObject* parent = NULL);

public:
    ~PinyinDecoderService();

    static PinyinDecoderService* getInstance();

    bool init();
    void setUserDictionary(bool enabled);
    bool isUserDictionaryEnabled() const;
    void setLimits(int maxSpelling, int maxHzsLen);
    int search(const QString& spelling);
    int deleteSearch(int pos, bool isPosInSpellingId, bool clearFixedInThisStep);
    void resetSearch();
    QString pinyinString(bool decoded);
    int pinyinStringLength(bool decoded);
    QVector<int> spellingStartPositions();
    QString candidateAt(int index);
    QList<QString> fetchCandidates(int index, int count, int sentFixedLen);
    int chooceCandidate(int index);
    int cancelLastChoice();
    int fixedLength();
    void flushCache();
    QList<QString> predictionList(const QString& history);

private:
    static QScopedPointer<PinyinDecoderService> _instance;
    bool initDone;
};


class QinGooglePinyin: public QinIMBase
{
public:
    enum State {
        Idle,
        Input,
        Predict
    };
    QinGooglePinyin(QWSInputMethod* ime);
    virtual ~QinGooglePinyin(void);

    virtual bool isPreEditing(void);
    virtual bool getDoPopUp(void);
    virtual QStringList getPopUpStrings(void);


    /** I/O related **/
    virtual char* getPreEditString(void);
    virtual char* getCommitString(void);
    virtual void reset(void);
    virtual void update(void);
    /** Key handling APIs **/
    virtual void handle_Default(int unicode, int keycode);
    virtual void handle_Space(void);
    virtual void handle_Enter(void);
    virtual void handle_Backspace(void);
    virtual void handle_Left(void);
    virtual void handle_Right(void);
private:
    void resetToIdleState();
    bool addSpellingChar(QChar ch, bool reset);
    bool removeSpellingChar();
    void chooseAndUpdate(int candId);
    bool chooseAndFinish();
    int candidatesCount();
    QString candidateAt(int index);
    void chooseDecodingCandidate(int candId);
    void choosePredictChoice(int choiceId);
    QString getComposingStrActivePart();
    void resetCandidates();
    bool canDoPrediction();
    void tryPredict();
public:
    State state;
    int totalChoicesNum;
    QList<QString> candidatesList;
private:
    QString preeditStr;
    QString commitStr;

    typedef struct {
        int pos;//起始索引
        int length;//长度
    } SelectRange;
    QVector<SelectRange> m_selectRangeCache;//缓存候选列表每一页的范围
    int m_iCurSelectPage;//候选词列表显示页
    QWSInputMethod* m_ime;//用于获取输入框光标位置和文字
private:
    QScopedPointer<PinyinDecoderService> pinyinDecoderService;
    QString surface;
    int fixedLen;
    QString composingStr;
    int activeCmpsLen;
    bool finishSelection;
    int posDelSpl;
    bool isPosInSpl;
};

#endif /* __QIN_SRC_PLUGIN_QIN_PINYIN_H__ */
