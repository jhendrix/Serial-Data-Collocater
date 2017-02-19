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

#include "serdatactrl.h"
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QList>
#include <QFile>
#include <QThread>
#include <QtConcurrent/QtConcurrent>


SerDataCtrl::SerDataCtrl(QObject *parent) : QObject(parent)
{

}

bool SerDataCtrl::Run()
{
   QtConcurrent::run(this, &SerDataCtrl::stream);
   return true;
}

void SerDataCtrl::stream(void)
{
   bool retVal = true;
   QFile file(m_fileStr);

   // Open serial port
   m_pPort = new QSerialPort(m_portName);
   m_pPort->setBaudRate(m_baudRate, QSerialPort::Input);
   m_pPort->setDataBits(static_cast<QSerialPort::DataBits>(m_bitsWd));
   m_pPort->setParity(m_parity);
   m_pPort->setFlowControl(m_flowCtrl);
   m_pPort->setStopBits(m_stopBits);
   m_pPort->setReadBufferSize(0);


   m_pPort->close();
   m_pPort->clearError();

   if (!m_pPort->open(QIODevice::ReadOnly))
   {
       emit(StringOut(QObject::tr("Failed to open port %1, error: %2").arg(m_pPort->portName()).arg(m_pPort->errorString())));
       retVal = false;
   }   

   if(retVal)
   {
      // Open file
      if(!file.open(QIODevice::WriteOnly))
      {
         QString str = QObject::tr("Failed to open file %1, error: %2").arg(file.fileName()).arg(file.errorString());
         emit(StringOut(str));
         retVal = false;
      }
      else
      {
         m_pPort->clear();
         emit StreamStart();
      }

      //while not done and no fatal errors, read and write
      while(!m_stop && retVal)
      {
         m_stopped = false;
         if(m_pPort->waitForReadyRead(25))
         {
            QByteArray ary = m_pPort->readAll();
            if(!ary.isEmpty())
            {
               m_totalBytes += ary.size();
               emit DataRcvd(ary.size());
               file.write(ary);
            }
            else
            {
               // sleep a little
               if(QSerialPort::NoError != m_pPort->error())
               {
                  emit(StringOut(m_pPort->errorString()));
                  m_pPort->clearError();
               }
            }
         }
         else
         {
            QSerialPort::SerialPortError err = m_pPort->error();
            if(QSerialPort::NoError != err && QSerialPort::TimeoutError != err)
            {
               emit(StringOut(m_pPort->errorString() + " after timeout"));
               m_pPort->clearError();
            }
         }
      }
   }


   m_pPort->close();
   file.close();

   m_stop = false;
   m_stopped = true;

   emit StreamEnd();

   return;
}

