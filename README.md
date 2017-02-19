# Serial-Data-Collocater
A Qt-based serial data recorder.
This application provides a quick way to record data from a PC COM port to a file.  Baud rates are configurable to non-standard values; 1MBaud and 5MBaud are already included.
Potential improvements:

1.  Make bits/wd and stop bits configurable (to non-standard) as well
2.  Change window title when port changes
3.  Update port combo widget when COM ports change

  a.  An easier route might be to provide a "Refresh" button
4.  Integrate and test on Linux
5.  Implement drag and drop to select file
6.  Provide option to save the status log to a file
7.  Provide controls for a limited capture time - either by nbytes or by time period
8.  Provide a KB/s calculation and display
9.  Implement Wait Cursor after pushing Start or Stop button.
