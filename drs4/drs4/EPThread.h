/*
 * EPThread.h
 * DRS oscilloscope event processor header file
 * $Id: EPThread.h,v 1.1.1.1 2012/03/25 23:06:55 camac Exp $
 */

class EPThread : public wxThread
{
public:
   EPThread(DOFrame *o);
   ~EPThread();
   void *Entry();
   float *GetTime()            { return m_time; }
   float *GetWaveform(int c)   { return m_waveform[c]; }
   bool IsFinished()           { return m_finished; }

private:
   DOFrame *m_frame;
   Osci    *m_osci;
   bool     m_finished;
   float    m_waveform[4][2048];
   float    m_time[2048];
};
