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

#include "serdatactrlview.h"
#include <QApplication>
#include <QWindow>
#include <QGridLayout>
#include <QLabel>
#include <QMessageBox>
#include <QEvent>
#include <QFileDialog>
#include <QDateTime>
#include <QSettings>
#include <QDebug>


SerDataCtrlView::SerDataCtrlView(QWidget *parent, SerDataCtrl* pCtrl) : QWidget(parent)
{
   m_pCtrl = pCtrl;
   addWidgets();

   m_sSettingsFile = "SerDataColl.ini";
   loadSettings();

   m_timer = new QTimer(this);

   connect(m_pCtrl, SIGNAL(DataRcvd(uint32_t)), this, SLOT(OnDataRcvd(uint32_t)));
   connect(m_pCtrl, SIGNAL(StreamEnd()), this, SLOT(OnStreamEnd()));
   connect(m_pCtrl, SIGNAL(StreamStart()), this, SLOT(OnStreamStart()));
   connect(m_pCtrl, SIGNAL(StringOut(QString)), this, SLOT(OnOutputStr(QString)));
   connect(m_timer, SIGNAL(timeout()), this, SLOT(OnTimerExp()));


   // Force update of serial port configuration
   OnBaudSelected(m_baudCombo->currentIndex());
   OnBitsSelected(m_bitsCombo->currentIndex());
   OnFlowSelected(m_flowCombo->currentIndex());
   OnParitySelected(m_parityCombo->currentIndex());
   OnPortSelected(m_portCombo->currentIndex());
   OnStopSelected(m_stopCombo->currentIndex());

   m_timer->start(1000);
}


SerDataCtrlView::~SerDataCtrlView()
{
   saveSettings();
}

void SerDataCtrlView::addWidgets()
{
   m_portCombo = new QComboBox(this);
   connect(m_portCombo,SIGNAL(currentIndexChanged(int)),this,SLOT(OnPortSelected(int)));
   OnPortSelActivated();

   m_bitsCombo = new QComboBox(this);
   m_bitsCombo->setEditable(false);
   m_bitsCombo->addItem(tr("8"), QSerialPort::Data8);
   m_bitsCombo->addItem(tr("7"), QSerialPort::Data7);
   m_bitsCombo->addItem(tr("6"), QSerialPort::Data6);
   m_bitsCombo->addItem(tr("5"), QSerialPort::Data5);
   connect(m_bitsCombo,SIGNAL(currentIndexChanged(int)),this,SLOT(OnBitsSelected(int)));


   m_baudCombo = new QComboBox(this);
   m_baudCombo->setEditable(true);
   m_baudCombo->lineEdit()->setValidator( new QIntValidator(1, 1000000000, this));
   m_baudCombo->setInsertPolicy(QComboBox::InsertAtTop);
   m_baudCombo->addItem(tr("5000000"), 5000000);
   m_baudCombo->addItem(tr("1000000"), 1000000);
   m_baudCombo->addItem(tr("921600"), 921600);
   m_baudCombo->addItem(tr("460800"), 460800);
   m_baudCombo->addItem(tr("230400"), 230400);
   m_baudCombo->addItem(tr("115200"), QSerialPort::Baud115200);
   m_baudCombo->addItem(tr("9600"), QSerialPort::Baud9600);
   connect(m_baudCombo,SIGNAL(currentIndexChanged(int)),this,SLOT(OnBaudSelected(int)));


   m_stopCombo = new QComboBox(this);
   m_stopCombo->addItem(tr("1"), QSerialPort::OneStop);
   m_stopCombo->addItem(tr("1.5"), QSerialPort::OneAndHalfStop);
   m_stopCombo->addItem(tr("2"), QSerialPort::TwoStop);
   connect(m_stopCombo,SIGNAL(currentIndexChanged(int)),this,SLOT(OnStopSelected(int)));


   m_parityCombo = new QComboBox(this);
   m_parityCombo->addItem(tr("None"), QSerialPort::NoParity);
   m_parityCombo->addItem(tr("Even"), QSerialPort::EvenParity);
   m_parityCombo->addItem(tr("Odd"), QSerialPort::OddParity);
   m_parityCombo->addItem(tr("Space"), QSerialPort::SpaceParity);
   m_parityCombo->addItem(tr("Mark"), QSerialPort::MarkParity);
   connect(m_parityCombo,SIGNAL(currentIndexChanged(int)),this,SLOT(OnParitySelected(int)));


   m_flowCombo = new QComboBox(this);
   m_flowCombo->addItem(tr("None"), QSerialPort::NoFlowControl);
   m_flowCombo->addItem(tr("HW"), QSerialPort::HardwareControl);
   m_flowCombo->addItem(tr("SW"), QSerialPort::SoftwareControl);
   connect(m_flowCombo,SIGNAL(currentIndexChanged(int)),this,SLOT(OnFlowSelected(int)));

   m_filePathEdit = new QLineEdit(this);

   m_messageEdit = new QPlainTextEdit(this);
   m_messageEdit->setReadOnly(true);

   m_fileBrowse  = new QPushButton("Browse...", this);
   connect(m_fileBrowse, SIGNAL(clicked()), this, SLOT(OnBtnBrowse()));

   m_startBtn = new QPushButton("Start", this);
   connect(m_startBtn,SIGNAL(clicked()),this,SLOT(OnStart()));

   m_stopBtn = new QPushButton("Stop", this);
   connect(m_stopBtn,SIGNAL(clicked()),this,SLOT(OnStop()));

   QGridLayout* layout = new QGridLayout(this);
   uint32_t c = 0;
   uint32_t r = 0;
   layout->addWidget(new QLabel(tr("Port")),r,c++);
   layout->addWidget(m_portCombo,r,c++);
   c++;
   layout->addWidget(new QLabel(tr("Bits/wd")),r,c++);
   layout->addWidget(m_bitsCombo, r, c++);


   c = 0;
   r++;
   layout->addWidget(new QLabel(tr("Baud")),r,c++);
   layout->addWidget(m_baudCombo,r,c++);
   c++;
   layout->addWidget(new QLabel(tr("Stop")),r,c++);
   layout->addWidget(m_stopCombo,r,c++);


   c = 0;
   r++;
   layout->addWidget(new QLabel(tr("Parity")),r,c++);
   layout->addWidget(m_parityCombo,r,c++);
   c++;
   layout->addWidget(new QLabel(tr("Flow Ctl")),r,c++);
   layout->addWidget(m_flowCombo,r,c++);

   r++;
   layout->setRowStretch(r,1);

   c = 0;
   r++;
   layout->addWidget(new QLabel(tr("Destination")),r,c++);
   c = 0;
   r++;
   layout->addWidget(m_filePathEdit,r,c,1,4);
   c += 4;
   layout->addWidget(m_fileBrowse, r, c++);

   c = 0;
   r++;
   layout->addWidget(new QLabel(tr("KB Rcvd")),r,c++);
   m_bytesLabel = new QLabel(tr("0"));
   layout->addWidget(m_bytesLabel,r,c++);
   c++;
   layout->addWidget(new QLabel(tr("Duration")),r,c++);
   m_timeLabel = new QLabel(tr("0:0:0"));
   layout->addWidget(m_timeLabel,r,c++);

   c = 1;
   r++;
   layout->addWidget(m_startBtn,r,c++);
   c++;
   layout->addWidget(m_stopBtn,r,c++);

   c = 0;
   r++;
   layout->setRowStretch(r, 10);
   layout->addWidget(m_messageEdit,r, c, 1, 6);

   layout->setColumnStretch(2,2);

   setLayout(layout);
}

void SerDataCtrlView::updateStartStop()
{
   if(m_pCtrl->isStopped())
   {
      m_startBtn->setEnabled(true);
   }
   else
   {
      m_startBtn->setEnabled(false);

   }
}

void SerDataCtrlView::OnPortSelActivated()
{
   foreach (const QSerialPortInfo &p, QSerialPortInfo::availablePorts())
   {
      m_portCombo->addItem(p.portName());
   }
}

void SerDataCtrlView::OnPortSelected(int boxIndex)
{
   m_pCtrl->SetPortName(m_portCombo->itemText(boxIndex));
   // TODO: emit signal so parent window can update title bar
}

void SerDataCtrlView::OnBaudSelected(int idx)
{
   bool ok;
   uint32_t baud = m_baudCombo->itemText(idx).toUInt(&ok);
   if(ok)
   {
      m_pCtrl->SetBaudRate(baud);
   }
   else
   {
      addMsg(tr("Failed to convert baud rate to an int."));
   }
}

void SerDataCtrlView::OnStart(void)
{
   m_pCtrl->SetFilePath(m_filePathEdit->text());
   m_messageEdit->clear();
   m_totalBytes = 0;

   if(!m_pCtrl->Run())
   {
      QMessageBox::information(this, tr("Start Fail"), tr("Serial streaming failed to start"));
   }
}

void SerDataCtrlView::OnStop(void)
{
   m_pCtrl->Stop();
}

void SerDataCtrlView::OnBitsSelected(int idx)
{
   bool ok;
   uint32_t nbits = m_bitsCombo->itemData(idx).toUInt(&ok);
   if(ok)
   {
      m_pCtrl->SetWdBits(nbits);
   }
   else
   {
      addMsg(tr("Failed to convert word bits to an int."));
   }
}

void SerDataCtrlView::OnStopSelected(int idx)
{
   bool ok;
   QSerialPort::StopBits stop = static_cast<QSerialPort::StopBits>(m_stopCombo->itemData(idx).toUInt(&ok));
   if(ok)
   {
      m_pCtrl->SetStopBits(stop);
   }
   else
   {
      addMsg(tr("Failed to convert stop bits to an int."));
   }
}

void SerDataCtrlView::OnParitySelected(int idx)
{
   bool ok;
   QSerialPort::Parity par = static_cast<QSerialPort::Parity>(m_parityCombo->itemData(idx).toUInt(&ok));
   if(ok)
   {
      m_pCtrl->SetParity(par);
   }
   else
   {
      addMsg(tr("Failed to convert parity selection to an enumerated UINT."));
   }
}

void SerDataCtrlView::OnFlowSelected(int idx)
{
   bool ok;
   QSerialPort::FlowControl fc = static_cast<QSerialPort::FlowControl>(m_flowCombo->itemData(idx).toUInt(&ok));
   if(ok)
   {
      m_pCtrl->SetFlowControl(fc);
   }
   else
   {
      addMsg(tr("Failed to convert flow ctrl into an enumerated UINT."));
   }
}

void SerDataCtrlView::OnStreamEnd()
{
   updateStartStop();
   addMsg("Streaming ended.");
   // stop timer?, other cleanup?
}

void SerDataCtrlView::OnStreamStart()
{
   m_durationSecs = 0;
   addMsg("Streaming started.");
   updateStartStop();
}

void SerDataCtrlView::OnDataRcvd(uint32_t bytes)
{
   m_totalBytes += bytes;
   m_bytesLabel->setText(QString::number(m_totalBytes/1024));
}

void SerDataCtrlView::OnBtnBrowse(void)
{
   // Display file chooser
   QString str =
         QFileDialog::getSaveFileName(this, tr("Open Input File"), "", "");

   if(!str.isNull())
   {
      // Put path in edit box
      m_filePathEdit->setText(str);
   }
}
void SerDataCtrlView::OnTimerExp()
{
   if(!m_pCtrl->isStopped())
   {
      m_durationSecs++;
      m_timeLabel->setText(QDateTime::fromTime_t(m_durationSecs).toUTC().toString("hh:mm:ss"));
   }
}

void SerDataCtrlView::OnOutputStr(QString str)
{
   addMsg(str);
}

void SerDataCtrlView::loadSettings()
{
   QSettings settings(m_sSettingsFile, QSettings::IniFormat);

   int32_t idx = settings.value("portComboIndex",0).toInt();
   if(m_portCombo && idx > 0 && idx < m_portCombo->count())
   {
      m_portCombo->setCurrentIndex(idx);
   }

   idx = settings.value("portBitsIndex",0).toInt();
   if(m_bitsCombo && idx > 0 && idx < m_bitsCombo->count())
   {
      m_bitsCombo->setCurrentIndex(idx);
      OnBitsSelected(idx);
   }

   idx = settings.value("portBaudIndex",0).toInt();
   if(m_baudCombo && idx > 0 && idx < m_baudCombo->count())
   {
      m_baudCombo->setCurrentIndex(idx);
      OnBaudSelected(idx);
   }

   idx = settings.value("portParityIndex",0).toInt();
   if(m_parityCombo && idx > 0 && idx < m_parityCombo->count())
   {
      m_parityCombo->setCurrentIndex(idx);
      OnParitySelected(idx);
   }

   idx = settings.value("portStopIndex",0).toInt();
   if(m_stopCombo && idx > 0 && idx < m_stopCombo->count())
   {
      m_stopCombo->setCurrentIndex(idx);
      OnStopSelected(idx);
   }

   idx = settings.value("portFlowIndex",0).toInt();
   if(m_flowCombo && idx > 0 && idx < m_flowCombo->count())
   {
      m_flowCombo->setCurrentIndex(idx);
      OnFlowSelected(idx);
   }

   QString str = settings.value("filePath", "").toString();
   if(m_filePathEdit)
   {
      m_filePathEdit->setText(str);
   }

   window()->restoreGeometry(settings.value("geometry").toByteArray());
}

void SerDataCtrlView::saveSettings()
{
   QSettings settings(m_sSettingsFile, QSettings::IniFormat);

   int32_t idx = (m_portCombo) ? m_portCombo->currentIndex() : 0;
   settings.setValue("portComboIndex", idx);

   idx = (m_bitsCombo) ? m_bitsCombo->currentIndex() : 0;
   settings.setValue("portBitsIndex", idx);

   idx = (m_baudCombo) ? m_baudCombo->currentIndex() : 0;
   settings.setValue("portBaudIndex", idx);

   idx = (m_parityCombo) ? m_parityCombo->currentIndex() : 0;
   settings.setValue("portParityIndex", idx);

   idx = (m_stopCombo) ? m_stopCombo->currentIndex() : 0;
   settings.setValue("portStopIndex", idx);

   idx = (m_flowCombo) ? m_flowCombo->currentIndex() : 0;
   settings.setValue("portFlowIndex", idx);

   QString str = (m_filePathEdit) ? m_filePathEdit->text() : "";
   settings.setValue("filePath", str);

   settings.setValue("geometry", window()->saveGeometry());
}

void SerDataCtrlView::addMsg(const QString &str)
{
   if(m_timestampLog)
   {
      QString ts = QString("[%1]   %2").arg(QDateTime::currentDateTime().toString("hh:mm:ss:zzz"), str);
      m_messageEdit->appendPlainText(ts);
   }
   else
   {
      m_messageEdit->appendPlainText(str);
   }
}

