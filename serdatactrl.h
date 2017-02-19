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

#ifndef SERDATACTRL_H
#define SERDATACTRL_H

#include <QObject>
#include "stdint.h"
#include <QSerialPort>
#include <QSerialPortInfo>

class SerDataCtrl : public QObject
{
   Q_OBJECT
public:
   explicit SerDataCtrl(QObject *parent = 0);
   bool Run(void);
   void Stop(void){m_stop = true;}
   void SetPortName(const QString name){m_portName = name;}
   void SetFilePath(const QString& str){m_fileStr = str;}
   void SetBaudRate(uint32_t br){m_baudRate = br;}
   void SetWdBits(uint32_t br){m_bitsWd = br;}
   void SetStopBits(QSerialPort::StopBits sb){m_stopBits = sb;}
   void SetParity(QSerialPort::Parity par){m_parity = par;}
   void SetFlowControl(QSerialPort::FlowControl fc){m_flowCtrl = fc;}
   bool isStopped(void){return m_stopped;}
private:
   void stream(void);

signals:   
   void StreamEnd(void);
   void StreamStart(void);
   void DataRcvd(uint32_t nbytes);
   void StringOut(QString str);

public slots:

private:
   bool m_stop = false;
   bool m_stopped = true;
   uint64_t m_totalBytes = 0;
   QString m_portName = "COM10";
   QSerialPort *m_pPort = NULL;
   QString m_fileStr;
   uint32_t m_baudRate = 921600;
   uint32_t m_bitsWd = 8;
   uint32_t m_dataWaitMs = 500;
   QSerialPort::StopBits m_stopBits    = QSerialPort::OneStop;
   QSerialPort::Parity m_parity        = QSerialPort::NoParity;
   QSerialPort::FlowControl m_flowCtrl = QSerialPort::NoFlowControl;

};

#endif // SERDATACTRL_H
