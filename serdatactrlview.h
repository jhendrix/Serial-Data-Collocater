/*
Copyright 2017 Codependable LLC

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef SERDATACTRLVIEW_H
#define SERDATACTRLVIEW_H

#include <QWidget>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QTimer>
#include <QPlainTextEdit>
#include "serdatactrl.h"

class SerDataCtrlView : public QWidget
{
   Q_OBJECT
public:
   explicit SerDataCtrlView(QWidget *parent = 0, SerDataCtrl* pCtrl = 0);
   ~SerDataCtrlView();
private:
   void addWidgets(void);
   void updateStartStop(void);
   void loadSettings();
   void saveSettings();
   void addMsg(const QString& str);

signals:

public slots:

private slots:
   void OnPortSelActivated();
   void OnPortSelected(int boxIndex);
   void OnBaudSelected(int idx);
   void OnStart(void);
   void OnStop(void);
   void OnBitsSelected(int idx);
   void OnStopSelected(int idx);
   void OnParitySelected(int idx);
   void OnFlowSelected(int idx);
   void OnStreamEnd(void);
   void OnStreamStart(void);
   void OnDataRcvd(uint32_t bytes);
   void OnBtnBrowse(void);
   void OnTimerExp(void);
   void OnOutputStr(QString str);

private:
   SerDataCtrl* m_pCtrl = 0;
   QComboBox*   m_portCombo    = 0;
   QComboBox*   m_bitsCombo    = 0;
   QComboBox*   m_baudCombo    = 0;
   QComboBox*   m_stopCombo    = 0;
   QComboBox*   m_parityCombo  = 0;
   QComboBox*   m_flowCombo    = 0;
   QLabel*      m_bytesLabel   = 0;
   QLabel*      m_timeLabel    = 0;
   QPushButton* m_startBtn     = 0;
   QPushButton* m_stopBtn      = 0;
   QPushButton* m_fileBrowse   = 0;
   QLineEdit*   m_filePathEdit = 0;
   QTimer*      m_timer        = 0;
   uint64_t     m_totalBytes   = 0;
   uint32_t     m_durationSecs = 0;
   QString      m_sSettingsFile;
   QPlainTextEdit*  m_messageEdit  = 0;
   bool         m_timestampLog = true;

};

#endif // SERDATACTRLVIEW_H
