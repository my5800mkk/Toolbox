/////////////////////////////////////////////////////////////////////////*
//Ink Studios Source File.
//Copyright (C), Ink Studios, 2013.
//////////////////////////////////////////////////////////////////////////
// Button class to handle click events.
//////////////////////////////////////////////////////////////////////////
// 01/06/2013 : Created by Filip 'i59' Lundgren
////////////////////////////////////////////////////////////////////////*/
#pragma once

#include "Toolbox.h"

class CButton
{
public:
	CButton(ITexture *pTexture, Vec2 position);
	~CButton();

	void Draw();

protected:
	ITexture *m_pTexture;

	Vec2 m_position;
};