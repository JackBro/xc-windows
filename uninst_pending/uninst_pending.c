/*
 * Copyright (c) 2009 Citrix Systems, Inc.
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <windows.h>
#pragma warning(push, 3)
#include <winioctl.h>
#pragma warning(pop)
#include <stdio.h>

#include "xs_ioctl.h"

int __cdecl
main()
{
    HANDLE h;
    DWORD ignore;

    h = CreateFile("\\\\.\\XenBus", GENERIC_READ | GENERIC_WRITE, 0,
                   NULL, OPEN_EXISTING, 0, NULL);
    if (h == INVALID_HANDLE_VALUE) {
        /* No drivers present -> can't have an uninstallation
         * pending. */
        return 0;
    }

    if (!DeviceIoControl(h, IOCTL_XS_UNINST_IN_PROGRESS, NULL, 0, NULL, 0,
                         &ignore, NULL)) {
        if (GetLastError() == ERROR_INVALID_FUNCTION) {
            /* Installed drivers don't support the ioctl.  Assume that
               means there's no uninstallation pending. */
            return 0;
        }
        if (GetLastError() == ERROR_GEN_FAILURE) {
            /* Drivers confirm no uninstallation pending */
            return 0;
        }
        /* Unknown error.  Assume that means uninstallation
         * pending. */
        return 1;
    }

    /* Success -> uninstallation is in progress */

    return 1;
}
