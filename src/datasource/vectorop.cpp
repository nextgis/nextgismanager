/******************************************************************************
 * Project:  wxGIS
 * Purpose:  vector operations.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2011-2013 Dmitry Baryshnikov
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
#include "wxgis/datasource/vectorop.h"


IMPLEMENT_DYNAMIC_CLASS(wxFeatureDSEvent, wxEvent)

wxDEFINE_EVENT( wxDS_FEATURES_ADDED, wxFeatureDSEvent );
wxDEFINE_EVENT( wxDS_FEATURE_ADDED, wxFeatureDSEvent );
wxDEFINE_EVENT( wxDS_FEATURE_DELETED, wxFeatureDSEvent );
wxDEFINE_EVENT( wxDS_FEATURE_CHANGED, wxFeatureDSEvent );
wxDEFINE_EVENT( wxDS_CLOSED, wxFeatureDSEvent );
wxDEFINE_EVENT( wxDS_CHANGED, wxFeatureDSEvent );

//--------------------------------------------------------------------------

void IncreaseEnvelope(OGREnvelope &Env, double dSize)
{
	double dRatio = dSize / 2;
	double dWidth = (Env.MaxX - Env.MinX) * dRatio;
	double dHeight = (Env.MaxY - Env.MinY) * dRatio;
	Env.MinX -= dWidth;
	Env.MinY -= dHeight;
	Env.MaxX += dWidth;
	Env.MaxY += dHeight;
}

void SetEnvelopeRatio(OGREnvelope &Env, double dRatio)
{
	double dWidth = (Env.MaxX - Env.MinX) / 2;
	double dHeight = (Env.MaxY - Env.MinY) / 2;
	double dCenterX = Env.MinX + dWidth;
	double dCenterY = Env.MinY + dHeight;

	double dEnvRatio = dWidth / dHeight;

	if(IsDoubleEquil(dRatio, dEnvRatio))
		return;

	//if(dEnvRatio <= 1.0 && dRatio > 1.0) || (dEnvRatio > 1.0 && dRatio < 1.0))
	if(dRatio > dEnvRatio) //increase width
	{
		dWidth = dHeight * dRatio;
		Env.MaxX = dCenterX + dWidth;
		Env.MinX = dCenterX - dWidth;
	}
	else					//increase height
	{
		dHeight = dWidth / dRatio;
		Env.MaxY = dCenterY + dHeight;
		Env.MinY = dCenterY - dHeight;
	}

#ifdef _DEBUG
	dWidth = (Env.MaxX - Env.MinX) / 2;
	dHeight = (Env.MaxY - Env.MinY) / 2;
	dCenterX = Env.MinX + dWidth;
	dCenterY = Env.MinY + dHeight;

	dEnvRatio = dWidth / dHeight;

//    wxASSERT(IsDoubleEquil(dRatio, dEnvRatio));
#endif //_DEBUG

}

wxGISGeometry EnvelopeToGeometry(const OGREnvelope &Env, const wxGISSpatialReference &SpaRef)
{
	if(!Env.IsInit())
		return wxGISGeometry();
	OGRLinearRing ring;
	ring.addPoint(Env.MinX, Env.MinY);
	ring.addPoint(Env.MinX, Env.MaxY);
	ring.addPoint(Env.MaxX, Env.MaxY);
	ring.addPoint(Env.MaxX, Env.MinY);
    ring.closeRings();

    OGRPolygon* pRgn = new OGRPolygon();
    pRgn->addRing(&ring);
    pRgn->flattenTo2D();
    if(SpaRef.IsOk())
		pRgn->assignSpatialReference(SpaRef.Clone());
	return wxGISGeometry(static_cast<OGRGeometry*>(pRgn));
}

void MoveEnvelope(OGREnvelope &MoveEnv, const OGREnvelope &Env)
{
	double dMoveWidth = (MoveEnv.MaxX - MoveEnv.MinX) / 2;
	double dMoveHeight = (MoveEnv.MaxY - MoveEnv.MinY) / 2;

	double dWidth = (Env.MaxX - Env.MinX) / 2;
	double dHeight = (Env.MaxY - Env.MinY) / 2;
	double dCenterX = Env.MinX + dWidth;
	double dCenterY = Env.MinY + dHeight;

	MoveEnv.MinX = dCenterX - dMoveWidth;
	MoveEnv.MaxX = dCenterX + dMoveWidth;
	MoveEnv.MinY = dCenterY - dMoveHeight;
	MoveEnv.MaxY = dCenterY + dMoveHeight;
}

