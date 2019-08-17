/*
$Id: Measurement.h,v 1.1.1.1 2012/03/25 23:06:55 camac Exp $
*/

class DOScreen;

/** Implementing MeasureDialog_fb */
class Measurement
{
protected:
   wxString m_name;
   int      m_index;
   double   m_value;
   double   m_statArray[10000];
   int      m_statIndex;
   int      m_nMeasured;
   int      m_nAverage;
   double   m_vsum;
   double   m_vvsum;
   double   m_min;
   double   m_max;
   
public:
   /** Constructor */
   Measurement(int index);

   wxString GetName();
   wxString GetUnit();
   double Measure(double *x1, double *y1, double *x2, double *y2, int n, bool update, DOScreen *s);
   void Measure(double *x1, double *y1, double *x2, double *y2, int n);
   wxString GetString();
   wxString GetStat();
   void SetNAverage(int n);
   void ResetStat();

   static const int N_MEASUREMENTS = 10;

private:
   double MLevel(double *x1, double *y1, int n, DOScreen *s);
   double MPkPk(double *x1, double *y1, int n, DOScreen *s);
   double MRMS(double *x1, double *y1, int n, DOScreen *s);
   double MFreq(double *x1, double *y1, int n, DOScreen *s);
   double MPeriod(double *x1, double *y1, int n, DOScreen *s);
   double MRise(double *x1, double *y1, int n, DOScreen *s);
   double MFall(double *x1, double *y1, int n, DOScreen *s);
   double MPosWidth(double *x1, double *y1, int n, DOScreen *s);
   double MNegWidth(double *x1, double *y1, int n, DOScreen *s);
   double MChnDelay(double *x1, double *y1, double *x2, double *y2, int n, DOScreen *s);
};
