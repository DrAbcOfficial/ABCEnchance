#pragma once
#include "mathlib/vector.h"

void V_CalcViewModelLag(ref_params_t* pparams, Vector& origin, Vector& angles, const Vector& original_angles);
void V_CalcModelSlide(ref_params_t* pparams);