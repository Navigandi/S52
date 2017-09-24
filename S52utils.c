// S52utils.c: utility
//
// Project:  OpENCview

/*
    This file is part of the OpENCview project, a viewer of ENC.
    Copyright (C) 2000-2017 Sylvain Duclos sduclos@users.sourceforge.net

    OpENCview is free software: you can redistribute it and/or modify
    it under the terms of the Lesser GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    OpENCview is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    Lesser GNU General Public License for more details.

    You should have received a copy of the Lesser GNU General Public License
    along with OpENCview.  If not, see <http://www.gnu.org/licenses/>.
*/


#include "S52utils.h"

#include <glib.h>          // g_get_current_time()
#include <glib/gprintf.h>  // g_strrstr()
#include <glib/gstdio.h>   // FILE

#include <stdlib.h>        // atoi(), atof()
#include <string.h>        // strlen()
#include <unistd.h>        // write()

// debug - configuration file
#ifdef S52_USE_ANDROID
#define CFG_NAME   "/sdcard/s52droid/s52.cfg"
#else
#define CFG_NAME   "s52.cfg"
#endif

// user provided lob msg callback
static S52_log_cb _log_cb  = NULL;

//#if defined(S52_DEBUG) || defined(S52_USE_LOGFILE)
//static GTimeVal   _now;

#ifdef S52_USE_LOGFILE
static gint       _logFile = 0;
typedef void (*GPrintFunc)(const gchar *string);
static GPrintFunc _oldPrintHandler = NULL;
#endif  // S52_USE_LOGFILE
//#endif  // S52_DEBUG || S52_USE_LOGFILE


// internal libS52.so version + build def's
static const char _version[] = S52_VERSION
#ifdef  _MINGW
      ",_MINGW"
#endif
#ifdef  S52_USE_GV
      ",S52_USE_GV"
#endif
#ifdef  GV_USE_DOUBLE_PRECISION_COORD
      ",GV_USE_DOUBLE_PRECISION_COORD"
#endif
#ifdef  S52_USE_OGR_FILECOLLECTOR
      ",S52_USE_OGR_FILECOLLECTOR"
#endif
#ifdef  S52_USE_PROJ
      ",S52_USE_PROJ"
#endif
#ifdef  S52_USE_SUPP_LINE_OVERLAP
      ",S52_USE_SUPP_LINE_OVERLAP"
#endif
#ifdef  S52_DEBUG
      ",S52_DEBUG"
#endif
#ifdef  S52_USE_LOGFILE
      ",S52_USE_LOGFILE"
#endif
#ifdef  S52_USE_DBUS
      ",S52_USE_DBUS"
#endif
#ifdef  S52_USE_SOCK
      ",S52_USE_SOCK"
#endif
#ifdef  S52_USE_BACKTRACE
      ",S52_USE_BACKTRACE"
#endif
#ifdef  S52_USE_EGL
      ",S52_USE_EGL"
#endif 
#ifdef  S52_USE_GL1
      ",S52_USE_GL1"
#endif
#ifdef  S52_USE_OPENGL_VBO
      ",S52_USE_OPENGL_VBO"
#endif
#ifdef  S52_USE_GLSC1
      ",S52_USE_GLSC1"
#endif
#ifdef  S52_USE_GL2
      ",S52_USE_GL2"
#endif
#ifdef  S52_USE_GLES2
      ",S52_USE_GLES2"
#endif
#ifdef  S52_USE_GLSC2
      ",S52_USE_GLSC2"
#endif
#ifdef  S52_USE_ANDROID
      ",S52_USE_ANDROID"
#endif
#ifdef  S52_USE_TEGRA2
      ",S52_USE_TEGRA2"
#endif
#ifdef  S52_USE_ADRENO
      ",S52_USE_ADRENO"
#endif
#ifdef  S52_USE_COGL
      ",S52_USE_COGL"
#endif
#ifdef  S52_USE_FREETYPE_GL
      ",S52_USE_FREETYPE_GL"
#endif
#ifdef  S52_USE_SYM_AISSEL01
      ",S52_USE_SYM_AISSEL01"
#endif
#ifdef  S52_USE_WORLD
      ",S52_USE_WORLD"
#endif
//#ifdef  S52_USE_SYM_VESSEL_DNGHL
//      ",S52_USE_SYM_VESSEL_DNGHL"
//#endif
#ifdef  S52_USE_TXT_SHADOW
      ",S52_USE_TXT_SHADOW"
#endif
#ifdef  S52_USE_RADAR
      ",S52_USE_RADAR"
#endif
#ifdef  S52_USE_RASTER
      ",S52_USE_RASTER"
#endif
#ifdef  S52_USE_DUAL_MON
      ",S52_USE_DUAL_MON"
#endif
#ifdef  S52_USE_C_AGGR_C_ASSO
      ",S52_USE_C_AGGR_C_ASSO"
#endif
#ifdef  S52_USE_LCMS2
      ",S52_USE_LCMS2"
#endif
"\n";

CCHAR   *S52_utils_version(void)
{
    return _version;
}

int      S52_utils_getConfig(CCHAR *label, char *vbuf)
// return TRUE and string value in vbuf for label, FALSE if fail
{
   FILE *fp;
   //int  ret;
   int  nline = 1;
   //char lbuf[PATH_MAX];
   //char frmt[PATH_MAX];
   char lbuf[MAXL];
   char frmt[MAXL];
   char str [MAXL];
   char *pstr = NULL;

   fp = g_fopen(CFG_NAME, "r");
   if (NULL == fp) {
       PRINTF("WARNING: .cfg not found: %s\n", CFG_NAME);
       return FALSE;
   }

   // prevent buffer overflow
   SNPRINTF(frmt, MAXL, "%s%i%s", " %s %", MAXL-1, "[^\n]s");
   //printf("frmt:%s\n", frmt);

   pstr = fgets(str, MAXL, fp);
   while (NULL != pstr) {
       // debug
       //printf("%i - label:%s value:%s\n", nline, lbuf, vbuf);

       if ('#' != str[0]) {
           //ret = sscanf(str, frmt, lbuf, vbuf);
           sscanf(str, frmt, lbuf, vbuf);
           if (0 == g_strcmp0(lbuf, label)) {
               PRINTF("--->>> label:%s value:%s \n", lbuf, vbuf);
               fclose(fp);
               return TRUE;
           }
       }

       ++nline;
       pstr = fgets(str, MAXL, fp);
   }

   fclose(fp);

   vbuf[0] = '\0';

   return FALSE;
}

#if defined(S52_DEBUG) || defined(S52_USE_LOGFILE)
void _printf(const char *file, int line, const char *function, const char *frmt, ...)
// FIXME: filter msg type: NOTE:, DEBUG:, FIXME:, WARNING:, ERROR:
{
    int  MAX = 1024;
    char buf[MAX];
    char headerfrmt[] = "%s:%i in %s(): ";
    int  size = snprintf(buf, MAX, headerfrmt, file, line, function);

    if (size < MAX) {
        va_list argptr;
        va_start(argptr, frmt);
        int n = vsnprintf(&buf[size], (MAX-size), frmt, argptr);
        va_end(argptr);

        char bufFinal[size + n + 1];
        memcpy(bufFinal, buf, size);
        va_start(argptr, frmt);
        //int nn = vsnprintf(&buf[size], (MAX-size), frmt, argptr);
        int nn = vsnprintf(&bufFinal[size], (n+1), frmt, argptr);
        va_end(argptr);

        printf("%s", bufFinal);

#if !defined(S52_USE_LOGFILE)
        // if user set a callback .. call it,
        // unless logging to file witch will call the cb
        if (NULL != _log_cb) {
            _log_cb(bufFinal);
        }
#endif

        if (nn > (n+1)) {
        //if (n > (MAX-size)) {
            //g_print("WARNING: _printf(): string buffer FULL, str len:%i, buf len:%i\n", n, (MAX-size));
            g_message("WARNING: _printf(): string buffer FULL, str len:%i, buf len:%i\n", nn, (n+1));
            g_assert(0);
        }
    } else {
        // FIXME: use printf() or g_message / g_error !
        g_message("WARNING: _printf(): buf FULL, str size:%i, buf len:%i\n", size, MAX);
        g_assert(0);
    }

    return;
}

#ifdef S52_USE_LOGFILE
static void     _S52_printf(cchar *string)
{
    char str[MAXL];
    //static
    GTimeVal now;
    g_get_current_time(&now);

    snprintf(str, MAXL-1, "%s %s", g_time_val_to_iso8601(&now), string);

    // if user set a callback .. call it
    if (NULL != _log_cb) {
        _log_cb(str);
    }

//#ifdef S52_USE_LOGFILE
    // log to file
    if (NULL != _logFile) {
        write(_logFile, str, strlen(str));
    }
//#endif

    // STDOUT
    g_printf("%s", str);
}
#endif

int      S52_utils_initLog(S52_log_cb log_cb)
// set print handler
// set tmp log file
{
    if (NULL != log_cb) {
        log_cb("S52_utils_initLog(): init logging\n");
        _log_cb = log_cb;
    }

#ifdef S52_USE_LOGFILE
    GError *error = NULL;
    _logFile = g_file_open_tmp("XXXXXX", NULL, &error);
    if (-1 == _logFile) {
        PRINTF("WARNING: g_file_open_tmp(): failed\n");
    } else {
        PRINTF("DEBUG: logfile tmp dir:%s\n", g_get_tmp_dir());
    }
    if (NULL != error) {
        g_printf("WARNING: g_file_open_tmp() failed (%s)\n", error->message);
        g_error_free(error);
    }

    _oldPrintHandler = g_set_print_handler(_S52_printf);

#else
    PRINTF("DEBUG: no LOGFILE, compiler flags 'S52_USE_LOGFILE' not set\n");
#endif

    return TRUE;
}

int      S52_utils_doneLog()
{
    _log_cb = NULL;

#ifdef S52_USE_LOGFILE
    g_set_print_handler(_oldPrintHandler);
    _oldPrintHandler = NULL;

    if (0 != _logFile)
        close(_logFile);
#endif

    return TRUE;
}
#endif  // S52_DEBUG || S52_USE_LOGFILE

int      S52_atoi(CCHAR *str)
// safe atoi()
// use for parsing the PLib and S57 attribute
{
    if (NULL == str) {
        PRINTF("WARNING: NULL string\n");
        g_assert(0);
        return 0;
    }

    if (0 == strlen(str)) {
        PRINTF("WARNING: zero length string\n");
        g_assert(0);
        return 0;
    }

    // the to (int) might not be such a great idea!  (no rounding)
    //return (int)S52_atof(str);
    //return (int)g_strtod(str, NULL);
    return (int)g_ascii_strtod(str, NULL);
    //return atoi(str);
}

double   S52_atof(CCHAR *str)
// safe atof()
{
    if (NULL == str) {
        PRINTF("WARNING: NULL string\n");
        g_assert(0);
        return 0;
    }

    if (0 == strlen(str)) {
        PRINTF("WARNING: zero length string\n");
        g_assert(0);
        return 0;
    }

    //return g_strtod(str, NULL);
    return g_ascii_strtod(str, NULL);
    //return atof(str);
}

char *S52_utils_new0(size_t sz, int n)
// debug - tally heap mem
//
// idea: - draw pixels to represent each mem bloc (sz*n)
//       - then hightlight pixels when blocs are accessed (read/write)
{
    char *ptr = g_malloc0(sz*n);

    PRINTF("DEBUG: >>>>>>>>>>>>>>>>>>>>>>>>>> ptr:%p sz:%i\n", ptr, sz*n);

    return ptr;
}

/*
//////////////////////
//
// from GIMP xyz2rgb.c
//
static double m2[3][3] =
{
  {  3.240479, -1.537150, -0.498535 },
  { -0.969256,  1.875992,  0.041556 },
  {  0.055648, -0.204043,  1.057311 }
};

int      S52_xyL2rgb(double *xr, double *yg, double *Lb)
{
  double R, G, B;

  double Y = *Lb;
  double X = (*xr / *yg) * Y;
  double Z = ((1 - *xr - *yg) / *yg) * Y;

  X = X / 100.0;
  Y = Y / 100.0;
  Z = Z / 100.0;


  R = m2[0][0] * X + m2[0][1] * Y + m2[0][2] * Z;
  G = m2[1][0] * X + m2[1][1] * Y + m2[1][2] * Z;
  B = m2[2][0] * X + m2[2][1] * Y + m2[2][2] * Z;

  //printf ("RGB = (%f %f %f) \n", R, G, B);

  R = (R<0.0)? 0.0 : (R>1.0)? 1.0 : R;
  G = (G<0.0)? 0.0 : (G>1.0)? 1.0 : G;
  B = (B<0.0)? 0.0 : (B>1.0)? 1.0 : B;

  *xr = R*255.0;
  *yg = G*255.0;;
  *Lb = B*255.0;;

  return 1;
}
*/
