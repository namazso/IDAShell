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
#include "pch.h"
#include "execute.h"

constexpr static wchar_t k_reg_path[] = L"Software\\IDAShell";

static int formatted_message_box(
  HWND hwnd,
  LPCWSTR caption,
  UINT type,
  _In_z_ _Printf_format_string_ LPCWSTR fmt,
  ...
)
{
  wchar_t buf[4096];
  va_list args;
  va_start(args, fmt);
  vswprintf_s(buf, fmt, args);
  va_end(args);
  return MessageBoxW(hwnd, buf, caption, type);
}

static const wchar_t* shell_exec_error(ULONG err)
{
  switch (err)
  {
  case 0: return L"The operating system is out of memory or resources.";
  case ERROR_FILE_NOT_FOUND: return L"The specified file was not found.";
  case ERROR_PATH_NOT_FOUND: return L"The specified path was not found.";
  case ERROR_BAD_FORMAT: return L"The .exe file is invalid (non-Win32 .exe or error in .exe image).";
  case SE_ERR_ACCESSDENIED: return L"The operating system denied access to the specified file.";
  case SE_ERR_ASSOCINCOMPLETE: return L"The file name association is incomplete or invalid.";
  case SE_ERR_DDEBUSY: return L"The DDE transaction could not be completed because other DDE transactions were being processed.";
  case SE_ERR_DDEFAIL: return L"The DDE transaction failed.";
  case SE_ERR_DDETIMEOUT: return L"The DDE transaction could not be completed because the request timed out.";
  case SE_ERR_DLLNOTFOUND: return L"The specified DLL was not found.";
  case SE_ERR_NOASSOC: return L"There is no application associated with the given file name extension. This error will also be returned if you attempt to print a file that is not printable.";
  case SE_ERR_OOM: return L"There was not enough memory to complete the operation.";
  case SE_ERR_SHARE: return L"A sharing violation occurred. ";
  default: return L"Unknown error";
  }
}

void execute_ida(bool is_64, const wchar_t* path)
{
  const auto name = is_64 ? L"ida64.exe" : L"ida.exe";
  wchar_t ida_path[PATHCCH_MAX_CCH]{};
  DWORD size = sizeof(ida_path);
  if (!(GetKeyState(VK_SHIFT) & 0x8000))
  {
    const auto status = RegGetValueW(
      HKEY_CURRENT_USER,
      k_reg_path,
      name,
      RRF_RT_REG_SZ,
      nullptr,
      ida_path,
      &size
    );

    if (status != ERROR_SUCCESS)
    {
      const auto ret = GetEnvironmentVariableW(
        L"IDADIR",
        ida_path,
        (DWORD)std::size(ida_path)
      );
      if (ret && ret < std::size(ida_path) - 10)
      {
        if (ida_path[ret - 1] != '\\')
          wcscat_s(ida_path, L"\\");
        wcscat_s(ida_path, name);
      }
      else
      {
        ida_path[0] = 0;
      }
    }
  }

  if (!*ida_path) // ida can't be located
  {
    wcscpy_s(ida_path, name);

    OPENFILENAMEW of = { sizeof(OPENFILENAME) };
    of.lpstrFile = ida_path;
    of.lpstrDefExt = L"exe";
    of.nMaxFile = static_cast<DWORD>(std::size(ida_path));
    of.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;

    const auto success = GetOpenFileNameW(&of);
    
    if (success)
    {
      RegSetKeyValueW(
        HKEY_CURRENT_USER,
        k_reg_path,
        name,
        REG_SZ,
        ida_path,
        (DWORD)(wcslen(ida_path) + 1) * sizeof(wchar_t)
      );
    }
    else
    {
      const auto error = CommDlgExtendedError();
      // if error is 0 the user just cancelled the action

      if (error)
        formatted_message_box(
          nullptr,
          L"Error",
          MB_ICONERROR | MB_OK,
          L"GetSaveFileName returned with error: %08X",
          error
        );

      return;
    }
  }

  wchar_t path_quoted[PATHCCH_MAX_CCH] = L"\"";
  wcscat_s(path_quoted, path);
  wcscat_s(path_quoted, L"\"");
  const auto ret = (ULONG)(ULONG_PTR)ShellExecuteW(
    nullptr,
    nullptr,
    ida_path,
    path_quoted,
    nullptr,
    SW_NORMAL
  );

  if (ret < 32)
    formatted_message_box(
      nullptr,
      L"Error",
      MB_ICONERROR | MB_OK,
      L"ShellExecute failed with %d (%s)",
      ret,
      shell_exec_error(ret)
    );
}
