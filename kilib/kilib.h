//--- K.I.LIB ---
// kilib.h : main-header of  K.I.LIB

#ifndef AFX_KILIB_H__89998F34_A9FE_4A27_A159_671F85AA9383__INCLUDED_
#define AFX_KILIB_H__89998F34_A9FE_4A27_A159_671F85AA9383__INCLUDED_

#include "kl_str.h"
#include "kl_misc.h"
#include "kl_cmd.h"
#include "kl_wnd.h"
#include "kl_file.h"
#include "kl_app.h"
#include "kl_reg.h"
#include "kl_find.h"
#include "kl_wcmn.h"
#include "kl_dnd.h"

extern kiApp* app();
extern void kilib_create_new_app();

#ifdef KILIB_LOG
#define Log(_s_) app()->log((const char*)(_s_))
#else
#define Log(_s_)
#endif

#endif
