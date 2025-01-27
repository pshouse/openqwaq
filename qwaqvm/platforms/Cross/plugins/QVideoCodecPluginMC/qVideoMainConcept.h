/**
 * Project OpenQwaq
 *
 * Copyright (c) 2005-2011, Teleplace, Inc., All Rights Reserved
 *
 * Redistributions in source code form must reproduce the above
 * copyright and this condition.
 *
 * The contents of this file are subject to the GNU General Public
 * License, Version 2 (the "License"); you may not use this file
 * except in compliance with the License. A copy of the License is
 * available at http://www.opensource.org/licenses/gpl-2.0.php.
 *
 */

/*
 * qVideoMainConcept.h
 * QVideoCodecPluginMC
 *
 * MainConcept-specific code that may be useful for both encoding
 * and decoding.
 */

#ifndef __Q_VIDEO_MAINCONCEPT_H__
#define __Q_VIDEO_MAINCONCEPT_H__

#include "qVideoMainConceptPlatformSpecific.h"


typedef struct SEQ_Params SEQ_Params;

// C-linkage printing-resource function for MC
#ifdef __cplusplus
extern "C" 
#endif
	void* qencoder_get_rc(char*);


#endif //#ifndef __Q_VIDEO_MAINCONCEPT_H__
