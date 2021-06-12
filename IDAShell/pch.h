// Copyright 2021 namazso <admin@namazso.eu>
// This file is part of IDAShell.
// 
// IDAShell is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// IDAShell is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with IDAShell.  If not, see <https://www.gnu.org/licenses/>.
#pragma once

// Beware: Most of these macros and include ordering are figured out via trying until it compiles.

#ifndef STRICT
#define STRICT
#endif

#define NOMINMAX

// thanks, microsoft
#define Unknown Unknown_FROM_WINDOWS

#include <winsdkver.h>

// Windows 7
#define _WIN32_WINNT 0x0601

#include <sdkddkver.h>

#define PHNT_VERSION PHNT_WIN7

#define ISOLATION_AWARE_ENABLED 1
#define SIDEBYSIDE_COMMONCONTROLS 1

#include <ntstatus.h>

#define WIN32_NO_STATUS

// ATL
#define _ATL_APARTMENT_THREADED
#define _ATL_NO_AUTOMATIC_NAMESPACE
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// some CString constructors will be explicit
#define ATL_NO_ASSERT_ON_DESTROY_NONEXISTENT_WINDOW

#include <atlbase.h>
#include <atlcom.h>
#include <atlctl.h>

// PHNT
#include <Windows.h>
#include <windowsx.h>

// Windows
#include <WinUser.h>
#include <CommCtrl.h>
#include <VersionHelpers.h>
#include <PathCch.h>
#include <shlobj.h>
#include <shlwapi.h>

#undef Unknown

#include <atomic>
#include <xutility>
#include <optional>
