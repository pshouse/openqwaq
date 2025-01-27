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

void testBufferPool_1(void); // Basic allocation and auto-cleanup.
void testBufferPool_2(void); // Test assignment between buffer pointers.
void testBufferPool_3(void); // Verify destructor throws exception when there are outstanding buffers
void testBufferPool_4(void); // Test multithreading (not yet implemented)
