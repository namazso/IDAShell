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
#include "bitmap.h"

#include <Uxtheme.h>

#pragma comment(lib, "Uxtheme.lib")

static void InitBitmapInfo(BITMAPINFO* pbmi, ULONG cbInfo, LONG cx, LONG cy, WORD bpp)
{
  ZeroMemory(pbmi, cbInfo);
  pbmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
  pbmi->bmiHeader.biPlanes = 1;
  pbmi->bmiHeader.biCompression = BI_RGB;

  pbmi->bmiHeader.biWidth = cx;
  pbmi->bmiHeader.biHeight = cy;
  pbmi->bmiHeader.biBitCount = bpp;
}

HRESULT Create32BitHBITMAP(HDC hdc, const SIZE* psize, void** ppvBits, HBITMAP* phBmp)
{
  *phBmp = nullptr;

  BITMAPINFO bmi;
  InitBitmapInfo(&bmi, sizeof(bmi), psize->cx, psize->cy, 32);

  const HDC hdcUsed = hdc ? hdc : GetDC(nullptr);
  if (hdcUsed)
  {
    *phBmp = CreateDIBSection(hdcUsed, &bmi, DIB_RGB_COLORS, ppvBits, nullptr, 0);
    if (hdc != hdcUsed)
    {
      ReleaseDC(nullptr, hdcUsed);
    }
  }
  return (nullptr == *phBmp) ? E_OUTOFMEMORY : S_OK;
}

HRESULT ConvertToPARGB32(HDC hdc, ARGB* pargb, HBITMAP hbmp, SIZE& sizImage, int cxRow)
{
  BITMAPINFO bmi;
  InitBitmapInfo(&bmi, sizeof(bmi), sizImage.cx, sizImage.cy, 32);

  HRESULT hr = E_OUTOFMEMORY;
  const HANDLE hHeap = GetProcessHeap();
  void* pvBits = HeapAlloc(hHeap, 0, bmi.bmiHeader.biWidth * 4 * bmi.bmiHeader.biHeight);
  if (pvBits)
  {
    hr = E_UNEXPECTED;
    if (GetDIBits(hdc, hbmp, 0, bmi.bmiHeader.biHeight, pvBits, &bmi, DIB_RGB_COLORS) == bmi.bmiHeader.biHeight)
    {
      const ULONG cxDelta = cxRow - bmi.bmiHeader.biWidth;
      auto pargbMask = static_cast<ARGB*>(pvBits);

      for (ULONG y = bmi.bmiHeader.biHeight; y; --y)
      {
        for (ULONG x = bmi.bmiHeader.biWidth; x; --x)
        {
          if (*pargbMask++)
          {
            // transparent pixel
            *pargb++ = 0;
          }
          else
          {
            // opaque pixel
            *pargb++ |= 0xFF000000;
          }
        }

        pargb += cxDelta;
      }

      hr = S_OK;
    }

    HeapFree(hHeap, 0, pvBits);
  }

  return hr;
}

static bool HasAlpha(ARGB* pargb, SIZE& sizImage, int cxRow)
{
  const ULONG cxDelta = cxRow - sizImage.cx;
  for (ULONG y = sizImage.cy; y; --y)
  {
    for (ULONG x = sizImage.cx; x; --x)
    {
      if (*pargb++ & 0xFF000000)
      {
        return true;
      }
    }

    pargb += cxDelta;
  }

  return false;
}

static HRESULT ConvertBufferToPARGB32(HPAINTBUFFER hPaintBuffer, HDC hdc, HICON hicon, SIZE& sizIcon)
{
  RGBQUAD* prgbQuad;
  int cxRow;
  HRESULT hr = GetBufferedPaintBits(hPaintBuffer, &prgbQuad, &cxRow);
  if (SUCCEEDED(hr))
  {
    const auto pargb = reinterpret_cast<ARGB*>(prgbQuad);
    if (!HasAlpha(pargb, sizIcon, cxRow))
    {
      ICONINFO info;
      if (GetIconInfo(hicon, &info))
      {
        if (info.hbmMask)
        {
          hr = ConvertToPARGB32(hdc, pargb, info.hbmMask, sizIcon, cxRow);
        }

        DeleteObject(info.hbmColor);
        if (info.hbmMask)
          DeleteObject(info.hbmMask);
      }
    }
  }

  return hr;
}

HBITMAP IconToBitmapPARGB32(HICON hIcon, DWORD cx, DWORD cy)
{
  HRESULT hr = E_OUTOFMEMORY;
  HBITMAP hBmp = nullptr;

  if (!hIcon)
    return nullptr;

  SIZE sizIcon;
  sizIcon.cx = cx;
  sizIcon.cy = cy;

  RECT rcIcon;
  SetRect(&rcIcon, 0, 0, sizIcon.cx, sizIcon.cy);

  const HDC hdcDest = CreateCompatibleDC(nullptr);
  if (hdcDest)
  {
    hr = Create32BitHBITMAP(hdcDest, &sizIcon, nullptr, &hBmp);
    if (SUCCEEDED(hr))
    {
      hr = E_FAIL;

      const auto hbmpOld = (HBITMAP)SelectObject(hdcDest, hBmp);
      if (hbmpOld)
      {
        BLENDFUNCTION bfAlpha = {AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
        BP_PAINTPARAMS paintParams = {0, 0, nullptr, nullptr};
        paintParams.cbSize = sizeof(paintParams);
        paintParams.dwFlags = BPPF_ERASE;
        paintParams.pBlendFunction = &bfAlpha;

        HDC hdcBuffer;
        const HPAINTBUFFER hPaintBuffer = BeginBufferedPaint(hdcDest, &rcIcon, BPBF_DIB, &paintParams, &hdcBuffer);
        if (hPaintBuffer)
        {
          if (DrawIconEx(hdcBuffer, 0, 0, hIcon, sizIcon.cx, sizIcon.cy, 0, nullptr, DI_NORMAL))
          {
            // If icon did not have an alpha channel, we need to convert buffer to PARGB.
            hr = ConvertBufferToPARGB32(hPaintBuffer, hdcDest, hIcon, sizIcon);
          }

          // This will write the buffer contents to the destination bitmap.
          EndBufferedPaint(hPaintBuffer, TRUE);
        }
        SelectObject(hdcDest, hbmpOld);
      }
    }
    DeleteDC(hdcDest);
  }

  DestroyIcon(hIcon);
  if (SUCCEEDED(hr))
  {
    return hBmp;
  }
  if (hBmp)
    DeleteObject(hBmp);
  return nullptr;
}
