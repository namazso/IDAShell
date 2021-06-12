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
#include "CShellExt.h"

#include "bitmap.h"
#include "dllmain.h"
#include "execute.h"
#include "resource.h"

CShellExt::CShellExt()
{
  ++g_refs;
}

CShellExt::~CShellExt()
{
  --g_refs;
}

HRESULT STDMETHODCALLTYPE CShellExt::QueryInterface(
  REFIID riid,
  _COM_Outptr_ void** ppv
)
{
  *ppv = nullptr;
  if (IsEqualIID(riid, IID_IUnknown) || IsEqualIID(riid, IID_IShellExtInit))
  {
    *ppv = (IShellExtInit*)this;
  }
  else if (IsEqualIID(riid, IID_IContextMenu))
  {
    *ppv = (IContextMenu*)this;
  }
  if (*ppv)
  {
    AddRef();
    return NOERROR;
  }
  return E_NOINTERFACE;
}

ULONG STDMETHODCALLTYPE CShellExt::AddRef()
{
  return ++_refs;
}

ULONG STDMETHODCALLTYPE CShellExt::Release()
{
  if (--_refs)
    return _refs;
  delete this;
  return 0;
}

HRESULT STDMETHODCALLTYPE CShellExt::Initialize(
  _In_opt_  PCIDLIST_ABSOLUTE folder,
  _In_opt_  IDataObject*      data,
  _In_opt_  HKEY              prog_id
)
{
  UNREFERENCED_PARAMETER(folder);
  UNREFERENCED_PARAMETER(prog_id);
  
  // Read the list of folders from the data object. They're stored in HDROP form, so just get the HDROP handle and then
  // use the drag 'n' drop APIs on it.
  FORMATETC etc
  {
    CF_HDROP,
    nullptr,
    DVASPECT_CONTENT,
    -1,
    TYMED_HGLOBAL
  };
  STGMEDIUM stg;
  if (FAILED(data->GetData(&etc, &stg)))
    return E_INVALIDARG;

  // Get an HDROP handle.
  const auto drop = static_cast<HDROP>(GlobalLock(stg.hGlobal));

  if (!drop)
  {
    ReleaseStgMedium(&stg);
    return E_INVALIDARG;
  }

  // Determine how many files are involved in this operation.
  const auto file_count = DragQueryFileW(
    drop,
    0xFFFFFFFF,
    nullptr,
    0
  );

  // TODO: maybe multiple files?
  if (file_count == 1 && 0 != DragQueryFileW(drop, 0, _file, (UINT)std::size(_file)))
    _type = try_identify(_file);

  // Release resources.
  GlobalUnlock(stg.hGlobal);
  ReleaseStgMedium(&stg);

  // If we found any files we can work with, return S_OK. Otherwise, return E_FAIL so we don't get called again for this
  // right-click operation.
  return _type == IDAType::Unsupported ? E_FAIL : S_OK;
}

HRESULT STDMETHODCALLTYPE CShellExt::QueryContextMenu(
  HMENU menu,
  UINT index_menu,
  UINT cmd_first,
  UINT cmd_last,
  UINT flags
)
{
  // If the flags include CMF_DEFAULTONLY then we shouldn't do anything.
  if (flags & CMF_DEFAULTONLY)
    return S_OK;
  
  const auto w = GetSystemMetrics(SM_CXMENUCHECK);
  const auto h = GetSystemMetrics(SM_CYMENUCHECK);
  const auto hicon = (HICON)LoadImageW(
    (HINSTANCE)&__ImageBase,
    MAKEINTRESOURCE(IDI_ICON1),
    IMAGE_ICON,
    w,
    h,
    LR_DEFAULTCOLOR
  );
  const auto icon = IconToBitmapPARGB32(hicon, w, h);
  DestroyIcon(hicon);
  MENUITEMINFOW mii{ sizeof(mii) };
  mii.cbSize = sizeof(mii);
  mii.fMask = MIIM_BITMAP | MIIM_STRING | MIIM_ID;
  mii.dwTypeData = (LPWSTR)L"Open in IDA...";
  mii.wID = cmd_first;
  mii.hbmpItem = icon;

  InsertMenuItemW(
    menu,
    index_menu,
    true,
    &mii
  );

  return MAKE_HRESULT(SEVERITY_SUCCESS, FACILITY_NULL, 1);
}

HRESULT STDMETHODCALLTYPE CShellExt::InvokeCommand(
  CMINVOKECOMMANDINFO* pici
)
{
  if (IS_INTRESOURCE(pici->lpVerb) && (UINT_PTR)pici->lpVerb == 0)
  {
    execute_ida(_type == IDAType::IDA64, _file);

    return S_OK;
  }

  return E_INVALIDARG;
}

HRESULT STDMETHODCALLTYPE CShellExt::GetCommandString(
  UINT_PTR cmd,
  UINT type,
  UINT* /*reserved*/,
  CHAR* name,
  UINT name_size
)
{
  // Check cmd, it must be 0 since we have only one menu item.
  if (0 != cmd)
    return E_INVALIDARG;

  if (type == GCS_HELPTEXTW)
  {
    wcscpy_s((LPWSTR)name, name_size, L"Is this even displayed anywhere??");
    return S_OK;
  }

  return E_INVALIDARG;
}