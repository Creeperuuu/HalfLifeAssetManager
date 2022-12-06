#include <QObject>

#include "ui/Credits.hpp"

namespace ui
{
std::string GetSharedCredits()
{
	return std::string( 
		"This product contains software technology licensed from Id Software, Inc.\n"
		"( \"Id Technology\" ). Id Technology � 1996 Id Software, Inc.\n"
		"All Rights Reserved.\n\n"
		"Copyright � 1998-2002, Valve LLC.\n"
		"All rights reserved.\n\n"
		"Uses OpenAL Soft\n"
		"Uses Ogg Vorbis\n"
		"Uses Libnyquist, Copyright (c) 2019, Dimitri Diakopoulos All rights reserved.\n"
		"Uses The OpenGL Mathemathics library (GLM)\n"
		"Copyright � 2005 - 2016 G-Truc Creation\n\n"
		"Uses Qt ") + QT_VERSION_STR + "\n\n"
		+ "Build Date: " + + __DATE__ + "\n";
}
}