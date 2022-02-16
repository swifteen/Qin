#include "QinGooglePinyin.h"
#include "QinIMBases.h"

#include <cstdio>
#include <cctype>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <vector>
#include <iterator>
#include <algorithm>
#include <QStringList>
#include <QDebug>
#include <QLatin1String>

#include "pinyinime.h"//google pinyin
#include "dictdef.h"//google pinyin
using namespace ime_pinyin;//google pinyin

#define GOOGLE_PINYIN_DEFALUT_DICT_PATH "/opt/usr/app/ime/google_pinyin/dict_pinyin.dat"
#define GOOGLE_PINYIN_USER_DICT_PATH "/opt/usr/app/ime/google_pinyin/usr_dict.dat"

QScopedPointer<PinyinDecoderService> PinyinDecoderService::_instance;

/*!
    \class QtVirtualKeyboard::PinyinDecoderService
    \internal
*/

PinyinDecoderService::PinyinDecoderService(QObject *parent) :
    QObject(parent),
    initDone(false)
{
}

PinyinDecoderService::~PinyinDecoderService()
{
    if (initDone) {
        im_close_decoder();
        initDone = false;
    }
}

PinyinDecoderService *PinyinDecoderService::getInstance()
{
    if (!_instance)
        _instance.reset(new PinyinDecoderService);
    if (!_instance->init())
        return NULL;
    return _instance.data();
}

bool PinyinDecoderService::init()
{
    if (initDone)
        return true;

#if 0
    QString sysDict(qEnvironmentVariable("QT_VIRTUALKEYBOARD_PINYIN_DICTIONARY"));
    if (!QFileInfo::exists(sysDict)) {
        sysDict = QLatin1String(":///QtQuick/VirtualKeyboard/3rdparty/pinyin/data/dict_pinyin.dat");
        if (!QFileInfo::exists(sysDict))
            sysDict = QLibraryInfo::location(QLibraryInfo::DataPath) + QLatin1String("/qtvirtualkeyboard/pinyin/dict_pinyin.dat");
    }

    QString usrDictPath = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation);
    QFileInfo usrDictInfo(usrDictPath + QLatin1String("/qtvirtualkeyboard/pinyin/usr_dict.dat"));
    if (!usrDictInfo.exists()) {
        qCWarning(lcPinyin) << "PinyinDecoderService::init(): creating directory for user dictionary" << usrDictInfo.absolutePath();
        QDir().mkpath(usrDictInfo.absolutePath());
    }

    initDone = im_open_decoder(sysDict.toUtf8().constData(), usrDictInfo.absoluteFilePath().toUtf8().constData());
    if (!initDone)
        qCWarning(lcPinyin) << "Could not initialize pinyin engine. sys_dict:" << sysDict << "usr_dict:" << usrDictInfo.absoluteFilePath();
#endif
	initDone = im_open_decoder(GOOGLE_PINYIN_DEFALUT_DICT_PATH, GOOGLE_PINYIN_USER_DICT_PATH);
qDebug()<< __FILE__ << __FUNCTION__ << __LINE__<<initDone;
    if (!initDone)
        qDebug() << "Could not initialize google pinyin engine.";
    return initDone;
}

//用于输入敏感词时，屏蔽用户自定义词典
void PinyinDecoderService::setUserDictionary(bool enabled)
{
    if (enabled == im_is_user_dictionary_enabled())
        return;
    if (enabled) {
//        QString usrDictPath = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation);
//        QFileInfo usrDictInfo(usrDictPath + QLatin1String("/qtvirtualkeyboard/pinyin/usr_dict.dat"));
        im_init_user_dictionary(GOOGLE_PINYIN_USER_DICT_PATH);
    } else {
        im_init_user_dictionary(NULL);
    }
}

bool PinyinDecoderService::isUserDictionaryEnabled() const
{
    return im_is_user_dictionary_enabled();
}

void PinyinDecoderService::setLimits(int maxSpsLen, int maxHzsLen)
{
    if (maxSpsLen <= 0)
        maxSpsLen = kMaxSearchSteps - 1;
    if (maxHzsLen <= 0)
        maxHzsLen = kMaxSearchSteps;
    im_set_max_lens(size_t(maxSpsLen), size_t(maxHzsLen));
}

int PinyinDecoderService::search(const QString &spelling)
{
    QByteArray spellingBuf = spelling.toLatin1();
    return int(im_search(spellingBuf.constData(), spellingBuf.length()));
}

int PinyinDecoderService::deleteSearch(int pos, bool isPosInSpellingId, bool clearFixedInThisStep)
{
    if (pos <= 0)
        pos = 0;
    return int(im_delsearch(size_t(pos), isPosInSpellingId, clearFixedInThisStep));
}

void PinyinDecoderService::resetSearch()
{
    im_reset_search();
}

QString PinyinDecoderService::pinyinString(bool decoded)
{
    size_t py_len;
    const char *py = im_get_sps_str(&py_len);
    if (!decoded)
        py_len = strlen(py);
//    return QString(QLatin1String(py, (int)py_len));
	return QString::fromLatin1(py, (int)py_len);
}

int PinyinDecoderService::pinyinStringLength(bool decoded)
{
    size_t py_len;
    const char *py = im_get_sps_str(&py_len);
    if (!decoded)
        py_len = strlen(py);
    return (int)py_len;
}

QVector<int> PinyinDecoderService::spellingStartPositions()
{
    const unsigned short *spl_start;
    int len;
    // There will be len + 1 elements in the buffer when len > 0.
    len = (int)im_get_spl_start_pos(spl_start);

    QVector<int> arr;
    arr.resize(len + 2);
    arr[0] = len; // element 0 is used to store the length of buffer.
    for (int i = 0; i <= len; i++)
        arr[i + 1] = spl_start[i];
    return arr;
}

QString PinyinDecoderService::candidateAt(int index)
{
    Q_ASSERT(index >= 0);
    QVector<QChar> candidateBuf;
    candidateBuf.resize(kMaxSearchSteps + 1);
    if (!im_get_candidate(size_t(index), (char16 *)candidateBuf.data(), candidateBuf.size() - 1))
        return QString();
    candidateBuf.last() = '\0';
    return QString(candidateBuf.data());
}

QList<QString> PinyinDecoderService::fetchCandidates(int index, int count, int sentFixedLen)
{
    QList<QString> candidatesList;
    for (int i = index; i < index + count; i++) {
        QString retStr = candidateAt(i);
        if (0 == i)
            retStr.remove(0, sentFixedLen);
        candidatesList.append(retStr);
    }
    return candidatesList;
}

int PinyinDecoderService::chooceCandidate(int index)
{
    return int(im_choose(index));
}

int PinyinDecoderService::cancelLastChoice()
{
    return int(im_cancel_last_choice());
}

int PinyinDecoderService::fixedLength()
{
    return (int)im_get_fixed_len();
}

void PinyinDecoderService::flushCache()
{
    im_flush_cache();
}

QList<QString> PinyinDecoderService::predictionList(const QString &history)
{
    QList<QString> predictList;
    char16 (*predictItems)[kMaxPredictSize + 1] = NULL;
    int predictNum = int(im_get_predicts(history.utf16(), predictItems));
    predictList.reserve(predictNum);
    for (int i = 0; i < predictNum; i++)
        predictList.append(QString((QChar *)predictItems[i]));
    return predictList;
}

class ScopedCandidateListUpdate
{
    Q_DISABLE_COPY(ScopedCandidateListUpdate)
public:
    inline explicit ScopedCandidateListUpdate(QinGooglePinyin *d) :
        d(d),
        candidatesList(d->candidatesList),
        totalChoicesNum(d->totalChoicesNum),
        state(d->state)
    {
    }

    inline ~ScopedCandidateListUpdate()
    {
        if (totalChoicesNum != d->totalChoicesNum || state != d->state || candidatesList != d->candidatesList)
        {
			/*
				The input method emits this signal when the contents of the selection list has changed. 
				The type parameter specifies which selection list has changed
				The input method emits this signal when the current index has changed in the selection list identified by type.
			*/
#if 0//TODO
			emit q->selectionListChanged(QVirtualKeyboardSelectionListModel::Type::WordCandidateList);
			emit q->selectionListActiveItemChanged(QVirtualKeyboardSelectionListModel::Type::WordCandidateList,
												   totalChoicesNum > 0 && state == Input ? 0 : -1);
#endif
		}
    }

private:
    QinGooglePinyin *d;
    QList<QString> candidatesList;
    int totalChoicesNum;
    QinGooglePinyin::State state;
};

QinGooglePinyin::QinGooglePinyin(void): 
	QinIMBase(":/data/Pinyin.xml"),	
	state(Idle),
	totalChoicesNum(0),
	candidatesList(),
	pinyinDecoderService(PinyinDecoderService::getInstance()),
	surface(),
	fixedLen(0),
	composingStr(),
	activeCmpsLen(0),
	finishSelection(true),
	posDelSpl(-1),
	isPosInSpl(false)
{
	candidates.clear();
	preeditStr.clear();
	commitStr.clear();
}

QinGooglePinyin::~QinGooglePinyin(void) {
}

void QinGooglePinyin::resetToIdleState()
{
#if 0//TODO
	// Disable the user dictionary when entering sensitive data
	if (inputContext) {
		bool userDictionaryEnabled = !inputContext->inputMethodHints().testFlag(Qt::ImhSensitiveData);
		if (userDictionaryEnabled != pinyinDecoderService->isUserDictionaryEnabled())
			pinyinDecoderService->setUserDictionary(userDictionaryEnabled);
	}
#endif

	if (state == Idle)
		return;

	state = Idle;
	surface.clear();
	fixedLen = 0;
	finishSelection = true;
	composingStr.clear();
	qDebug()<< __FILE__ << __FUNCTION__ << __LINE__<<preeditStr;
	preeditStr.clear();
	activeCmpsLen = 0;
	posDelSpl = -1;
	isPosInSpl = false;

	resetCandidates();
}

bool QinGooglePinyin::addSpellingChar(QChar ch, bool reset)
{
	if (reset) {
		surface.clear();
		pinyinDecoderService->resetSearch();
	}
	if (ch == '\'') {
		if (surface.isEmpty())
			return false;
		if (surface.endsWith(ch))
			return true;
	}
	surface.append(ch);
	return true;
}

bool QinGooglePinyin::removeSpellingChar()
{
	if (surface.isEmpty())
		return false;
	QVector<int> splStart = pinyinDecoderService->spellingStartPositions();
	isPosInSpl = (surface.length() <= splStart[fixedLen + 1]);
	posDelSpl = isPosInSpl ? fixedLen - 1 : surface.length() - 1;
	return true;
}

void QinGooglePinyin::chooseAndUpdate(int candId)
{
	if (state == Predict)
		choosePredictChoice(candId);
	else
		chooseDecodingCandidate(candId);

	if (composingStr.length() > 0) {
		if ((candId >= 0 || finishSelection) && composingStr.length() == fixedLen) {
			QString resultStr = getComposingStrActivePart();
			tryPredict();
		qDebug()<< __FILE__ << __FUNCTION__ << __LINE__<<resultStr;
//			q->inputContext()->commit(resultStr);
			commitStr = resultStr;
		} else if (state == Idle) {
			state = Input;
		}
	} else {
		tryPredict();
	}
}

bool QinGooglePinyin::chooseAndFinish()
{
	if (state == Predict || !totalChoicesNum)
		return false;

	chooseAndUpdate(0);
	if (state != Predict && totalChoicesNum > 0)
		chooseAndUpdate(0);

	return true;
}

int QinGooglePinyin::candidatesCount()
{
	return totalChoicesNum;
}

QString QinGooglePinyin::candidateAt(int index)
{
	if (index < 0 || index >= totalChoicesNum)
		return QString();
	if (index >= candidatesList.size()) {
		int fetchMore = qMin(index + 20, totalChoicesNum - candidatesList.size());
		candidatesList.append(pinyinDecoderService->fetchCandidates(candidatesList.size(), fetchMore, fixedLen));
		if (index == 0 && totalChoicesNum == 1) {
			int surfaceDecodedLen = pinyinDecoderService->pinyinStringLength(true);
			if (surfaceDecodedLen < surface.length())
				candidatesList[0] = candidatesList[0] + surface.mid(surfaceDecodedLen).toLower();
		}
	}
	return index < candidatesList.size() ? candidatesList[index] : QString();
}

void QinGooglePinyin::chooseDecodingCandidate(int candId)
{
	Q_ASSERT(state != Predict);

	int result = 0;
	if (candId < 0) {
		if (surface.length() > 0) {
			if (posDelSpl < 0) {
				result = pinyinDecoderService->search(surface);
			} else {
				result = pinyinDecoderService->deleteSearch(posDelSpl, isPosInSpl, false);
				posDelSpl = -1;
			}
		}
	} else {
		if (totalChoicesNum > 1) {
			result = pinyinDecoderService->chooceCandidate(candId);
		} else {
			QString resultStr;
			if (totalChoicesNum == 1) {
				QString undecodedStr = candId < candidatesList.length() ? candidatesList.at(candId) : QString();
				resultStr = pinyinDecoderService->candidateAt(0).mid(0, fixedLen) + undecodedStr;
			}
			resetToIdleState();
			if (!resultStr.isEmpty())
			{
				qDebug()<< __FILE__ << __FUNCTION__ << __LINE__<<resultStr;
//				q->inputContext()->commit(resultStr);
				commitStr = resultStr;
			}
			return;
		}
	}

	resetCandidates();
	totalChoicesNum = result;

	surface = pinyinDecoderService->pinyinString(false);
	QVector<int> splStart = pinyinDecoderService->spellingStartPositions();
	QString fullSent = pinyinDecoderService->candidateAt(0);
	fixedLen = pinyinDecoderService->fixedLength();
	composingStr = fullSent.mid(0, fixedLen) + surface.mid(splStart[fixedLen + 1]);
	activeCmpsLen = composingStr.length();

	// Prepare the display string.
	QString composingStrDisplay;
	int surfaceDecodedLen = pinyinDecoderService->pinyinStringLength(true);
	if (!surfaceDecodedLen) {
		composingStrDisplay = composingStr.toLower();
		if (!totalChoicesNum)
			totalChoicesNum = 1;
	} else {
		activeCmpsLen = activeCmpsLen - (surface.length() - surfaceDecodedLen);
		composingStrDisplay = fullSent.mid(0, fixedLen);
		for (int pos = fixedLen + 1; pos < splStart.size() - 1; pos++) {
			composingStrDisplay += surface.mid(splStart[pos], splStart[pos + 1] - splStart[pos]).toUpper();
			if (splStart[pos + 1] < surfaceDecodedLen)
				composingStrDisplay += QLatin1String(" ");
		}
		if (surfaceDecodedLen < surface.length())
			composingStrDisplay += surface.mid(surfaceDecodedLen).toLower();
	}
	qDebug()<< __FILE__ << __FUNCTION__ << __LINE__<<composingStrDisplay;
//	q->inputContext()->setPreeditText(composingStrDisplay);
	preeditStr = composingStrDisplay;

	finishSelection = splStart.size() == (fixedLen + 2);
	if (!finishSelection)
		candidateAt(0);
}

void QinGooglePinyin::choosePredictChoice(int choiceId)
{
	Q_ASSERT(state == Predict);

	if (choiceId < 0 || choiceId >= totalChoicesNum)
		return;

	QString tmp = candidatesList.at(choiceId);

	resetCandidates();

	candidatesList.append(tmp);
	totalChoicesNum = 1;

	surface.clear();
	fixedLen = tmp.length();
	composingStr = tmp;
	activeCmpsLen = fixedLen;

	finishSelection = true;
}

QString QinGooglePinyin::getComposingStrActivePart()
{
	return composingStr.mid(0, activeCmpsLen);
}

void QinGooglePinyin::resetCandidates()
{
	candidatesList.clear();
	if (totalChoicesNum) {
		totalChoicesNum = 0;
	}
}

bool QinGooglePinyin::canDoPrediction()
{
#if 0
	return composingStr.length() == fixedLen &&
			inputContext &&
			!inputContext->inputMethodHints().testFlag(Qt::ImhNoPredictiveText);
#endif
	return (composingStr.length() == fixedLen);
}

void QinGooglePinyin::tryPredict()
{
	// Try to get the prediction list.
	if (canDoPrediction()) {
		if (state != Predict)
			resetToIdleState();
#if 0//TODO
		QVirtualKeyboardInputContext *inputContext = q->inputContext();
		int cursorPosition = inputContext->cursorPosition();
		int historyStart = qMax(0, cursorPosition - 3);
		QString history = inputContext->surroundingText().mid(historyStart, cursorPosition - historyStart);
		candidatesList = pinyinDecoderService->predictionList(history);
#endif
		totalChoicesNum = candidatesList.size();
		finishSelection = false;
		state = Predict;
	} else {
		resetCandidates();
	}

	if (!candidatesCount())
		resetToIdleState();
}


//用于输入空格后判断是否需要处理空格按键
bool QinGooglePinyin::isPreEditing(void) {
	qDebug()<< __FILE__ << __FUNCTION__ << __LINE__<<preeditStr<<preeditStr.length();
  return preeditStr.length() > 0;
}

//判断是否存在侯选词，如果存在则从输入法核心中获取选词列表，并显示在侯选列表
bool QinGooglePinyin::getDoPopUp(void) {
	qDebug()<< __FILE__ << __FUNCTION__ << __LINE__<<candidatesCount();
  return (candidatesCount() > 0);
}

QStringList QinGooglePinyin::getPopUpStrings(void) {
	qDebug()<< __FILE__ << __FUNCTION__ << __LINE__<<candidates;
  return candidates;
}

char* QinGooglePinyin::getPreEditString(void) {
	qDebug()<< __FILE__ << __FUNCTION__ << __LINE__<<preeditStr;
  return strdup(preeditStr.toUtf8().data());
}

char* QinGooglePinyin::getCommitString(void) {
	qDebug()<< __FILE__ << __FUNCTION__ << __LINE__<<commitStr;
  QString str = commitStr;
  commitStr.clear();
  return strdup(str.toUtf8().data());
}

void QinGooglePinyin::reset(void) {
	qDebug()<< __FILE__ << __FUNCTION__ << __LINE__;
    ScopedCandidateListUpdate scopedCandidateListUpdate(this);
    Q_UNUSED(scopedCandidateListUpdate);
    resetToIdleState();
}

void QinGooglePinyin::update(void)
{
	qDebug()<< __FILE__ << __FUNCTION__ << __LINE__;
    ScopedCandidateListUpdate scopedCandidateListUpdate(this);
    Q_UNUSED(scopedCandidateListUpdate);
    chooseAndFinish();
    tryPredict();
}

void QinGooglePinyin::handle_Default(int unicode, int keycode) {
	qDebug()<< __FILE__ << __FUNCTION__ << __LINE__<<unicode<<keycode<<tolower(keycode)<<Qt::Key_A<<Qt::Key_Z<<QString("%1").arg(Qt::Key_A, 0, 16)<<QString("%1").arg(Qt::Key_Z, 0, 16);
//	keycode = keycode-0x20;
	if ((keycode >= Qt::Key_A && keycode <= Qt::Key_Z) || (keycode == Qt::Key_Apostrophe)) {
		
	qDebug()<< __FILE__ << __FUNCTION__ << __LINE__<<unicode<<keycode<<state;
		if (state == Predict)
			resetToIdleState();
		if (addSpellingChar(unicode, state == Idle)) //TODO
		{
			chooseAndUpdate(-1);
		}
	}
	else//其它输入则完成输入
	{
		chooseAndFinish();
	}
}

void QinGooglePinyin::handle_Space(void) {
	qDebug()<< __FILE__ << __FUNCTION__ << __LINE__<<state<<candidatesCount();

	if (state != Predict && candidatesCount() > 0) {
		chooseAndUpdate(0);
	}
}

void QinGooglePinyin::handle_Esc(void) {
}

void QinGooglePinyin::handle_Enter(void) {
	qDebug()<< __FILE__ << __FUNCTION__ << __LINE__<<state<<candidatesCount()<<surface;
  if (state != Predict && candidatesCount() > 0) {
	  commitStr = surface;
	  qDebug()<< __FILE__ << __FUNCTION__ << __LINE__<<surface;
	  resetToIdleState();  
//	  inputContext()->commit(surface);
  }
}

void QinGooglePinyin::handle_Del(void) {

}

void QinGooglePinyin::handle_Backspace(void) {
	if (removeSpellingChar()) {
		chooseAndUpdate(-1);
	}
}

void QinGooglePinyin::handle_Left(void) {
	qDebug()<< __FILE__ << __FUNCTION__ << __LINE__<<state<<candidatesCount()<<surface<<preeditStr;
  if (preeditStr.length() > 0) {

  }
}

void QinGooglePinyin::handle_Right(void) {
	qDebug()<< __FILE__ << __FUNCTION__ << __LINE__<<state<<candidatesCount()<<surface<<preeditStr;
  if (preeditStr.length() > 0) {

  }
}
