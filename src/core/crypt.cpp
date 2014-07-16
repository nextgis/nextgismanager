/******************************************************************************
 * Project:  wxGIS
 * Purpose:  base crypt functions.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2011 Dmitry Baryshnikov
*
*    This program is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 2 of the License, or
*    (at your option) any later version.
*
*    This program is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ****************************************************************************/

#include "wxgis/core/crypt.h"

#include "wxgisdefs.h"

#ifdef wxGIS_USE_OPENSSL

#include "wxgis/core/config.h"

#include "cpl_string.h"
#include <openssl/evp.h>
#include <openssl/rand.h>

#define EVP_KEY_SIZE	32
#define EVP_IV_SIZE		8
#define BUFSIZE			1024

wxString GetRandomKey(short nLen)
{
	GByte *key = new GByte[nLen];
	if(!RAND_bytes(key, nLen))
    {
        wxDELETE(key);
		return wxEmptyString;
    }
	CPLString pszKey(CPLBinaryToHex(nLen, key));
    wxDELETE(key);
	return wxString(pszKey, wxConvUTF8);
}

/** \fn bool CreateRandomData(void)
 *  \brief Create key and iv data and store it in config.
 *  \return true if succeeded, false otherwise
 */
//bool CreateRandomData(void);
//GByte *GetKey(void);
//GByte *GetIV(void);
//EVP_CIPHER_CTX* CreateCTX(GByte* pabyKey, GByte* pabyIV, bool bDecrypt);

bool CreateRandomData(void)
{
	wxGISAppConfig oConfig = GetConfig();
	if(!oConfig.IsOk())
		return false;

	GByte key[EVP_KEY_SIZE];
	if(!RAND_bytes(key, sizeof(key)))
		return false;
	CPLString pszKey(CPLBinaryToHex(EVP_KEY_SIZE, key));
	if(!oConfig.Write(enumGISHKCU, wxString(wxT("wxGISCommon/crypt/key")), wxString(pszKey, wxConvUTF8)))
 		return false;
	GByte iv[EVP_IV_SIZE];
	if(!RAND_bytes(iv, sizeof(iv)))
		return false;
	CPLString pszIV(CPLBinaryToHex(EVP_IV_SIZE, iv));
	return oConfig.Write(enumGISHKCU, wxString(wxT("wxGISCommon/crypt/iv")), wxString(pszIV, wxConvUTF8));
}

GByte *GetKey(void)
{
	wxGISAppConfig oConfig = GetConfig();
	if(!oConfig.IsOk())
		return NULL;
    //try get key data from config
	wxString sKey = oConfig.Read(enumGISHKCU, wxString(wxT("wxGISCommon/crypt/key")), wxString(ERR));
	if(sKey.CmpNoCase(wxString(ERR)) == 0)
	{
		if(!CreateRandomData())//create random key data
            return NULL;
		//second try get key data from config
		sKey = oConfig.Read(enumGISHKCU, wxString(wxT("wxGISCommon/crypt/key")), wxString(ERR));
		if(sKey.CmpNoCase(wxString(ERR)) == 0)
			return NULL;
	}

	int nKeyBytes;
	GByte *pabyKey = CPLHexToBinary( sKey.mb_str(wxConvUTF8), &nKeyBytes );
    return pabyKey;
}

GByte *GetIV(void)
{
	wxGISAppConfig oConfig = GetConfig();
	if(!oConfig.IsOk())
		return NULL;

	//try get iv data from config
	wxString sIV = oConfig.Read(enumGISHKCU, wxString(wxT("wxGISCommon/crypt/iv")), wxString(ERR));
	if(sIV.CmpNoCase(wxString(ERR)) == 0)
	{
		if(!CreateRandomData())//create random key data
            return NULL;
		//second try get iv data from config
		sIV = oConfig.Read(enumGISHKCU, wxString(wxT("wxGISCommon/crypt/iv")), wxString(ERR));
		if(sIV.CmpNoCase(wxString(ERR)) == 0)
			return NULL;
	}

	int nIVBytes;
	GByte *pabyIV = CPLHexToBinary( sIV.mb_str(wxConvUTF8), &nIVBytes );
	return pabyIV;
}

EVP_CIPHER_CTX* CreateCTX(GByte* pabyKey, GByte* pabyIV, bool bDecrypt)
{
    EVP_CIPHER_CTX* pstCTX = new EVP_CIPHER_CTX;
	const EVP_CIPHER * cipher = NULL;

	EVP_CIPHER_CTX_init(pstCTX);

    wxGISAppConfig oConfig = GetConfig();
   	if(oConfig.IsOk())
    {
        wxString sMode = oConfig.Read(enumGISHKCU, wxString(wxT("wxGISCommon/crypt/mode")), wxString(ERR));

#ifndef OPENSSL_NO_AES
        if(sMode.IsSameAs(wxString(wxT("AES")), false))
            cipher = EVP_aes_256_cfb();
#endif
#ifndef OPENSSL_NO_IDEA
        if(sMode.IsSameAs(wxString(wxT("IDEA")), false))
            cipher = EVP_idea_cbc();
#endif
#ifndef OPENSSL_NO_RC2
        if(sMode.IsSameAs(wxString(wxT("RC2")), false))
            cipher = EVP_rc2_cbc();
#endif
#ifndef OPENSSL_NO_BF
        if(sMode.IsSameAs(wxString(wxT("BF")), false))
            cipher = EVP_bf_cbc();
#endif
#ifndef OPENSSL_NO_CAST
        if(sMode.IsSameAs(wxString(wxT("CAST5")), false))
            cipher = EVP_cast5_cbc();
#endif
#ifndef OPENSSL_NO_DES
        if(NULL == cipher || sMode.IsSameAs(wxString(ERR)) || sMode.IsSameAs(wxString(wxT("DES")), false))
            cipher = EVP_des_cfb();
#endif
    }
    else
#ifndef OPENSSL_NO_DES
        cipher = EVP_des_cfb();
#else
        return NULL;
#endif

    if(NULL == cipher)
        return NULL;

	bool bResult;
	if(bDecrypt)
	    bResult = EVP_EncryptInit(pstCTX, cipher, pabyKey, pabyIV) == TRUE;
    else
	    bResult = EVP_DecryptInit(pstCTX, cipher, pabyKey, pabyIV) == TRUE;
	if(!bResult)
		return NULL;
	return pstCTX;
}

bool Crypt(const wxString &sText, wxString &sCryptText)
{

	GByte *pabyKey = GetKey();
	GByte *pabyIV = GetIV();

	EVP_CIPHER_CTX* ctx = CreateCTX(pabyKey, pabyIV, false);
	if(!ctx)
	{
		wxLogError(_("Crypt: Failed EVP_EncryptInit!"));
		CPLFree( pabyKey );
		CPLFree( pabyIV );
		return false;
	}

	CPLString pszText(sText.mb_str(wxConvUTF8));
	int outlen;
	unsigned char outbuf[BUFSIZE];

	bool bResult = EVP_EncryptUpdate(ctx, outbuf, &outlen, (const unsigned char*)pszText.data(), pszText.length() * sizeof(pszText[0]) + 1) == TRUE;

	if(!bResult)
	{
		wxLogError(_("Crypt: Failed EVP_EncryptUpdate!"));
		CPLFree( pabyKey );
		CPLFree( pabyIV );
		return bResult;
	}

	int nLen = outlen;
	bResult = EVP_EncryptFinal(ctx, &outbuf[outlen], &outlen) == TRUE;
	nLen += outlen;

	CPLString pszOutput(CPLBinaryToHex(nLen, outbuf));
	sCryptText = wxString(pszOutput, wxConvUTF8);

	CPLFree( pabyKey );
	CPLFree( pabyIV );
	EVP_CIPHER_CTX_cleanup(ctx);
	//EVP_CIPHER_CTX_free(ctx);

	return bResult;
}

bool Decrypt(const wxString &sText, wxString &sDecryptText)
{

	GByte *pabyKey = GetKey();
	GByte *pabyIV = GetIV();

	EVP_CIPHER_CTX* ctx = CreateCTX(pabyKey, pabyIV, true);
	if(!ctx)
	{
		wxLogError(_("Decrypt: Failed EVP_DecryptInit!"));
		CPLFree( pabyKey );
		CPLFree( pabyIV );
		return false;
	}

	int nTextBytes;
	GByte *pabyText = CPLHexToBinary( sText.mb_str(wxConvUTF8), &nTextBytes );

	int outlen;
	unsigned char outbuf[BUFSIZE];

	bool bResult = EVP_DecryptUpdate(ctx, outbuf, &outlen, pabyText, nTextBytes) == TRUE;
	if(!bResult)
	{
		wxLogError(_("Decrypt: Failed EVP_DecryptUpdate!"));
		CPLFree( pabyKey );
		CPLFree( pabyIV );
		CPLFree( pabyText );
		return bResult;
	}

	int nLen = outlen;
	bResult = EVP_DecryptFinal(ctx, &outbuf[outlen], &outlen) == TRUE;
	nLen += outlen;
	outbuf[nLen] = 0;

	CPLString szCryptText((const char*)outbuf);
	sDecryptText = wxString(szCryptText, wxConvUTF8);

	CPLFree( pabyKey );
	CPLFree( pabyIV );
	CPLFree( pabyText );

	EVP_CIPHER_CTX_cleanup(ctx);
	//EVP_CIPHER_CTX_free(ctx);

	return bResult;
}

#else

#  ifndef ZCR_SEED2
#    define ZCR_SEED2 3141592654UL     /* use PI as default pattern */
#  endif

#define hibyte(a) ((a>>8) & 0xFF)
#define lobyte(a) ((a) & 0xFF)
#define PASSWD wxT("wxgispasswd")

wxString GetRandomKey(short nLen)
{
    static const char alphanum[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
        //"abcdefghijklmnopqrstuvwxyz"

    srand((unsigned)(time(NULL) ^ ZCR_SEED2));
    wxString sOut;
    for (int i = 0; i < nLen; ++i) {
        sOut += alphanum[rand() % (sizeof(alphanum) - 1)];
    }
    return sOut;
}

bool Crypt(const wxString &sText, wxString &sCryptText)
{
    sCryptText.Clear();
    const wxString sPass(PASSWD);
    size_t pos(0);
    for(size_t i = 0; i < sText.Len(); ++i)
    {
        if(pos == sPass.Len())
            pos = 0;
        wxUint32 val1 = sText[i];
        wxUint32 val2 = sPass[pos];
        wxUint32 Symbol = val1 ^ val2;
        char SymbolHi = hibyte(Symbol);
        char SymbolLo = lobyte(Symbol);//(Symbol >> 4) & 0xff;
		sCryptText += wxDecToHex(SymbolHi);
		sCryptText += wxDecToHex(SymbolLo);
        pos++;
    }
	return true;
}

bool Decrypt(const wxString &sText, wxString &sDecryptText)
{
    sDecryptText.Clear();
    const wxString sPass(PASSWD);
    size_t pos(0);
    for(size_t i = 0; i < sText.Len(); i += 4)
    {
        if(pos == sPass.Len())
            pos = 0;
		wxString sHex = sText[i];
		sHex += sText[i + 1];
        char SymbolHi = wxHexToDec(sHex);
        sHex = sText[i + 2];
		sHex += sText[i + 3];
        char SymbolLo = wxHexToDec(sHex);
        wxUint32 Symbol = (SymbolHi << 8) + SymbolLo;
        wxUint32 val2 = sPass[pos].GetValue();
        Symbol = Symbol ^ val2;
        sDecryptText += wxUniChar(Symbol);
        pos++;
    }
    return true;
}
#endif //wxGIS_USE_OPENSSL
