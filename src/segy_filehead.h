#ifndef _SEGY_H__
#define _SEGY_H__
//****************************** SGY **************************************//
/* the SEG-Y reel identification header */
struct SegyReelHdrStruct
{
    /* ASCII- or EBCDIC-coded block */
    char comment[3200]; /* 3200-byte text area */
    /* binary-coded block */
    long int  jobid;   /* job identification number                                */
    long int  lino;    /* line number (only one line per reel)                     */
    long int  reno;    /* reel number                                              */
    short int ntrpr;   /* number of data traces per record                         */
    short int nart;    /* number of auxiliary traces per record                    */
    short int hdt;     /* sample interval in pico secs for this reel               */
    short int dto;     /* same for original field recording                        */
    short int hns;     /* number of samples per trace for this reel                */
    short int nso;     /* number of samples per trace for original field recording */
    short int format;  /* data sample format code:
                                1 = floating point (4 bytes)
                                2 = fixed point (4 bytes)
                                3 = fixed point (2 bytes)
                                4 = fixed point w/gain code (4 bytes) */
    short int fold;    /* CDP fold expected per CDP ensemble */
    short int tsort;   /* trace sorting code:
                                1 = as recorded (no sorting)
                                2 = CDP ensemble
                                3 = single fold continuous profile
                                4 = horizontally stacked */
    short int vscode;  /* vertical sum code:
                                1 = no sum
                                2 = two sum ...
                                N = N sum (N = 32,767) */
    short int hsfs;    /* sweep frequency at start */
    short int hsfe;    /* sweep frequency at end   */
    short int hslen;   /* sweep length (ms)        */
    short int hstyp;   /* sweep type code:
                                1 = linear
                                2 = parabolic
                                3 = exponential
                                4 = other */
    short int schn;    /* trace number of sweep channel */
    short int hstas;   /* sweep trace taper length (msec) at start if
                          tapered (the taper starts at zero time
                          and is effective for this length) */
    short int hstae;   /* sweep trace taper length (msec) at end
                          (the ending taper starts at sweep length
                          minus the taper length at end) */
    short int htatyp;  /* sweep trace taper type code:
                                1 = linear
                                2 = cos-squared
                                3 = other */
    short int hcorr;   /* correlated data traces code:
                                1 = no
                                2 = yes */
    short int bgrcv;   /* binary gain recovered code:
                                1 = yes
                                2 = no */
    short int rcvm;    /* amplitude recovery method code:
                                1 = none
                                2 = spherical divergence
                                3 = AGC
                                4 = other */
    short int mfeet;   /* measurement system code:
                                1 = meters
                                2 = feet */
    short int polyt;   /* impulse signal polarity code:
                                1 = increase in pressure or upward
                                    geophone case movement gives
                                    negative number on tape
                                2 = increase in pressure or upward
                                    geophone case movement gives
                                    positive number on tape */
    short int vpol;    /* vibratory polarity code:
                                code    seismic signal lags pilot by
                                1    337.5 to  22.5 degrees
                                2     22.5 to  67.5 degrees
                                3     67.5 to 112.5 degrees
                                4    112.5 to 157.5 degrees
                                5    157.5 to 202.5 degrees
                                6    202.5 to 247.5 degrees
                                7    247.5 to 292.5 degrees
                                8    293.5 to 337.5 degrees */
    short int hunass[170];  /* unassigned */
};  /* 3600 bytes if tightly packed */

/* the SEG-Y trace identification header */
struct SegyTraceHdrStruct
{
    long int  tracl;   /* trace sequence number within line */
    long int  tracr;   /* trace sequence number within reel */
    long int  fldr;    /* field record number               */
    long int  tracf;   /* trace number within field record  */
    long int  ep;      /* energy source point number        */
    long int  cdp;     /* CDP ensemble number               */
    long int  cdpt;    /* trace number within CDP ensemble  */
    short int trid;    /* trace identification code:
                                1 = seismic data
                                2 = dead
                                3 = dummy
                                4 = time break
                                5 = uphole
                                6 = sweep
                                7 = timing
                                8 = water break
                                9---, N = optional use (N = 32,767)
                                Following are CWP id flags:
                                9 = autocorrelation
                                10 = Fourier transformed - no packing
                                     xr[0],xi[0], ..., xr[N-1],xi[N-1]
                                11 = Fourier transformed - unpacked Nyquist
                                     xr[0],xi[0],...,xr[N/2],xi[N/2]
                                12 = Fourier transformed - packed Nyquist
                                      even N:
                                     xr[0],xr[N/2],xr[1],xi[1], ...,
                                   xr[N/2 -1],xi[N/2 -1]
                                   (note the exceptional second entry)
                                     odd N:
                                     xr[0],xr[(N-1)/2],xr[1],xi[1], ...,
                                   xr[(N-1)/2 -1],xi[(N-1)/2 -1],
                                   xi[(N-1)/2]
                                   (note the exceptional second & last entries)
                                13 = Complex signal in the time domain
                                     xr[0],xi[0], ..., xr[N-1],xi[N-1]
                                14 = Fourier transformed - amplitude/phase
                                     a[0],p[0], ..., a[N-1],p[N-1]
                                15 = Complex time signal - amplitude/phase
                                     a[0],p[0], ..., a[N-1],p[N-1]
                                16 = Real part of complex trace from 0 to
                                 Nyquist
                                17 = Imag part of complex trace from 0 to
                                 Nyquist
                                18 = Amplitude of complex trace from 0 to
                                 Nyquist
                                19 = Phase of complex trace from 0 to Nyquist
                                21 = Wavenumber time domain (k-t)
                                22 = Wavenumber frequency (k-omega)
                                30 = Depth-Range (z-x) traces
                               101 = Seismic data packed to bytes (by supack1)
                               102 = Seismic data packed to 2 bytes (by supack2)
                               200 = GPR data
                                */
    short int nvs;     /* number of vertically summed traces (see vscode
                          in reel header structure) */
    short int nhs;     /* number of horizontally summed traces (see vscode
                          in reel header structure) */
    short int duse;    /* data use:
                          1 = production
                          2 = test */
    long int offset;   /* distance from source point to receiver
                          group (negative if opposite to direction
                          in which the line was shot) */
    long int gelev;    /* receiver group elevation from sea level
                          (above sea level is positive) */
    long int selev;    /* source elevation from sea level
                          (above sea level is positive) */
    long int  sdepth;   /* source depth below surface (positive) */
    long int  gdel;     /* datum elevation at receiver group     */
    long int  sdel;     /* datum elevation at source             */
    long int  swdep;    /* water depth at source                 */
    long int  gwdep;    /* water depth at receiver group         */
    short int scalel;   /* scale factor for previous 7 entries
                          with value plus or minus 10 to the
                          power 0, 1, 2, 3, or 4 (if positive,
                          multiply, if negative divide) */
    short int scalco;  /* scale factor for next 4 entries
                          with value plus or minus 10 to the
                          power 0, 1, 2, 3, or 4 (if positive,
                          multiply, if negative divide) */
    long int  sx;       /* X source coordinate    */
    long int  sy;       /* Y source coordinate    */
    long int  gx;       /* X group coordinate     */
    long int  gy;       /* Y group coordinate     */
    short int counit;   /* coordinate units code:
                          for previous four entries
                          1 = length (meters or feet)
                          2 = seconds of arc (in this case, the
                          X values are longitude and the Y values
                          are latitude, a positive value designates
                          the number of seconds east of Greenwich
                          or north of the equator */
    short int wevel;   /* weathering velocity */
    short int swevel;  /* subweathering velocity */
    short int sut;     /* uphole time at source */
    short int gut;     /* uphole time at receiver group */
    short int sstat;   /* source static correction */
    short int gstat;   /* group static correction */
    short int tstat;   /* total static applied */
    short int laga;    /* lag time A, time in ms between end of 240-
                          byte trace identification header and time
                          break, positive if time break occurs after
                          end of header, time break is defined as
                          the initiation pulse which maybe recorded
                          on an auxiliary trace or as otherwise
                          specified by the recording system */
    short int lagb;    /* lag time B, time in ms between the time break
                          and the initiation time of the energy source,
                          may be positive or negative */
    short int delrt;   /* delay recording time, time in ms between
                          initiation time of energy source and time
                          when recording of data samples begins
                          (for deep water work if recording does not
                          start at zero time) */
    short int          muts;   /* mute time--start                     */
    short int          mute;   /* mute time--end                       */
    unsigned short int ns;     /* number of samples in this trace      */
    unsigned short int dt;     /* sample interval; in micro-seconds    */
    short int          gain;   /* gain type of field instruments code:
                          1 = fixed
                          2 = binary
                          3 = floating point
                          4 ---- N = optional use */
    short int igc;     /* instrument gain constant */
    short int igi;     /* instrument early or initial gain */
    short int corr;    /* correlated:
                          1 = no
                          2 = yes */
    short int sfs;     /* sweep frequency at start */
    short int sfe;     /* sweep frequency at end */
    short int slen;    /* sweep length in ms */
    short int styp;    /* sweep type code:
                          1 = linear
                          2 = parabolic
                          3 = exponential
                          4 = other */
    short int stas;    /* sweep trace taper length at start in ms */
    short int stae;    /* sweep trace taper length at end in ms */
    short int tatyp;   /* taper type: 1=linear, 2=cos^2, 3=other */
    short int afilf;   /* alias filter frequency if used */
    short int afils;   /* alias filter slope */
    short int nofilf;  /* notch filter frequency if used */
    short int nofils;  /* notch filter slope */
    short int lcf;     /* low cut frequency if used */
    short int hcf;     /* high cut frequncy if used */
    short int lcs;     /* low cut slope */
    short int hcs;     /* high cut slope */
    short int year;    /* year data recorded */
    short int day;     /* day of year */
    short int hour;    /* hour of day (24 hour clock) */
    short int minute;  /* minute of hour */
    short int sec;     /* second of minute */
    short int timbas;  /* time basis code:
                          1 = local
                          2 = GMT
                          3 = other */
    short int trwf;    /* trace weighting factor, defined as 1/2^N
                          volts for the least sigificant bit */
    short int grnors;  /* geophone group number of roll switch
                          position one */
    short int grnofr;  /* geophone group number of trace one within
                          original field record */
    short int grnlof;  /* geophone group number of last trace within
                          original field record */
    short int gaps;    /* gap size (total number of groups dropped) */
    short int otrav;   /* overtravel taper code:
                          1 = down (or behind)
                          2 = up (or ahead) */
    short int unass[28]; /* unassigned -- for optional info */
    short int marks_indicator;        /* marks indicator */
    short int mark_number;  /* mark number */
};  /* 240 bytes if tightly packed */
#endif
