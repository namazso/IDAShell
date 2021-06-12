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
#include "identify.h"

class CShellExt : public IShellExtInit, IContextMenu
{
  wchar_t _file[PATHCCH_MAX_CCH]{};
	ULONG _refs{};
  IDAType _type = IDAType::Unsupported;

public:
  CShellExt();
  virtual ~CShellExt();

  // ========== IUnknown
  HRESULT STDMETHODCALLTYPE QueryInterface(
    REFIID riid,
    _COM_Outptr_ void** ppv
  ) override;
  ULONG STDMETHODCALLTYPE AddRef() override;
  ULONG STDMETHODCALLTYPE Release() override;

	// IShellExtInit
	HRESULT STDMETHODCALLTYPE Initialize(
    _In_opt_  PCIDLIST_ABSOLUTE folder,
    _In_opt_  IDataObject*      data,
    _In_opt_  HKEY              prog_id
  ) override;

	// IContextMenu
	HRESULT STDMETHODCALLTYPE QueryContextMenu(
    HMENU menu,
    UINT index_menu,
    UINT cmd_first,
    UINT cmd_last,
    UINT flags
  ) override;
  HRESULT STDMETHODCALLTYPE InvokeCommand(
    CMINVOKECOMMANDINFO* pici
  ) override;
  HRESULT STDMETHODCALLTYPE GetCommandString(
    UINT_PTR cmd,
    UINT type,
    UINT* /*reserved*/,
    CHAR* name,
    UINT name_size
  ) override;
};
