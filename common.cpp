
/*
		Description:Convert between ANSI and UTF-8
		Usage:
		convert UTF-8 to ANSI:
		CodePageConvert(CP_ACP,ansi_str_out,CP_UTF8,utf8_str_in);
		Conver ANSI to UTF-8:
		CodePageConvert(CP_UTF8,utf8_str_out,CP_ACP,ansi_str_in);
*/
#include "stdafx.h"
#include "common.h"

int CodeConvert(uint32_t uDestCodePage, void* pDestChar, uint32_t uSrcCodePage, void* pSrcChar)
{
	if (uDestCodePage == uSrcCodePage)
	{
		return -1;
	}
	if ((pDestChar == nullptr) || (pSrcChar == nullptr))
	{
		return -1;
	}
	//ANSI转UTF-8
	if ((uSrcCodePage == CP_ACP) && (uDestCodePage == CP_UTF8))
	{
		//先将ASCII码转换为Unicode编码
		int wcharlen = MultiByteToWideChar(CP_ACP, 0, (PCSTR)pSrcChar, -1, nullptr, 0);
		uint32_t cbWideCharByte = wcharlen * sizeof(wchar_t);
		void* pWideCharBuf = malloc(cbWideCharByte);
		memset(pWideCharBuf, 0x00, cbWideCharByte);
		MultiByteToWideChar(CP_ACP, 0, (PCSTR)pSrcChar, -1, (PWSTR)pWideCharBuf, cbWideCharByte);
		//将Unicode编码转换为UTF-8编码
		int u8Len = WideCharToMultiByte(CP_UTF8, 0, (LPCWCH)pWideCharBuf, -1, nullptr, 0, nullptr, FALSE);
		WideCharToMultiByte(CP_UTF8, 0, (LPCWCH)pWideCharBuf, cbWideCharByte, (LPSTR)pDestChar, u8Len, nullptr, FALSE);
		free(pWideCharBuf);
	}
	//UTF-8转ANSI
	if ((uSrcCodePage == CP_UTF8) && (uDestCodePage == CP_ACP))
	{
		//UTF-8转Unicode
		int wcharlen = MultiByteToWideChar(CP_UTF8, 0, (LPCCH)pSrcChar, -1, nullptr, 0);
		uint32_t wchar_byte = wcharlen * sizeof(wchar_t);
		void* pUnicodeBuf = malloc(wchar_byte);
		memset(pUnicodeBuf, 0x00, wchar_byte);
		MultiByteToWideChar(CP_UTF8, 0, (LPCCH)pSrcChar, -1, (LPWSTR)pUnicodeBuf, wcharlen);
		int aLen = WideCharToMultiByte(CP_ACP, 0, (PWSTR)pUnicodeBuf, -1, nullptr, 0, nullptr, FALSE);
		WideCharToMultiByte(CP_ACP, 0, (PWSTR)pUnicodeBuf, -1, (LPSTR)pDestChar, aLen, nullptr, FALSE);
		//Unicode转ANSI
		free(pUnicodeBuf);
	}
}
