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
 * qVideoCodecPlatformSpecific.h
 * QVideoCodecPlugin
 *
 * Platform- and API-specific functions that must be implemented in 
 * each plugin that "inherits" from QVideoCodecPlugin
 */

#ifndef __Q_VIDEO_CODEC_PLATFORM_SPECIFIC_H__
#define __Q_VIDEO_CODEC_PLATFORM_SPECIFIC_H__

#ifdef __cplusplus
extern "C"
{
#endif

void qInitPlatform();
void qShutdownPlatform();

#ifdef __cplusplus
} //extern "C"
#endif

#endif //#ifndef __Q_VIDEO_CODEC_PLATFORM_SPECIFIC_H__
