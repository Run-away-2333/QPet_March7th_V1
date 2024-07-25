#pragma once
#include "Widget.h"

template<class Mol, class Den>  // molecule denominator
double Ratio_(Mol mol, Den den) {
	return mol / den;
}
