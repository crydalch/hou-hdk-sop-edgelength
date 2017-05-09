/*
	Modify edge length in arbitrary positions.

	IMPORTANT! ------------------------------------------
	-----------------------------------------------------

	Author: 	SNOWFLAKE
	Email:		snowflake@outlook.com

	LICENSE ------------------------------------------

	Copyright (c) 2016-2017 SNOWFLAKE
	All rights reserved.

	Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

	1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
	2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
	3. The names of the contributors may not be used to endorse or promote products derived from this software without specific prior written permission.

	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#pragma once

#ifndef ____parameters_h____
#define ____parameters_h____

/* -----------------------------------------------------------------
INCLUDES                                                           |
----------------------------------------------------------------- */

// hou-hdk-common
#include <Macros/SwitcherPRM.h>
#include <Macros/GroupMenuPRM.h>
#include <Macros/FloatPRM.h>
#include <Macros/TogglePRM.h>
#include <Macros/VectorPRM.h>

// this
#include "SOP_EdgeLength.h"

/* -----------------------------------------------------------------
DEFINES                                                            |
----------------------------------------------------------------- */

#define SOP_Operator GET_SOP_Namespace()::SOP_EdgeLength

/* -----------------------------------------------------------------
USING                                                              |
----------------------------------------------------------------- */

// YOUR CODE GOES HERE...

/* -----------------------------------------------------------------
PARAMETERS                                                         |
----------------------------------------------------------------- */

DECLARE_SOP_Namespace_Start()

	namespace UI
	{			
		__DECLARE__Filter_Section_PRM(2)
		DECLARE_Default_EdgeGroup_Input_0_PRM(input0)		
		static auto		processModeChoiceMenuParm_Name = PRM_Name("processmode", "Process Mode");
		static auto		processModeChoiceMenuParm_Range = PRM_Range(PRM_RANGE_RESTRICTED, 0, PRM_RANGE_RESTRICTED, 2);
		static auto     processModeChoiceMenuParm_Default = PRM_Default(2);
		static PRM_Name processModeChoiceMenuParm_Choices[] =
		{
			PRM_Name("0", "Single"),
			PRM_Name("1", "Multi"),
			PRM_Name("2", "Mixed"),
			PRM_Name(0)
		};
		static auto		processModeChoiceMenuParm_ChoiceList = PRM_ChoiceList(PRM_CHOICELIST_SINGLE, processModeChoiceMenuParm_Choices);
		auto			processModeChoiceMenu_Parameter = PRM_Template(PRM_ORD, 1, &processModeChoiceMenuParm_Name, &processModeChoiceMenuParm_Default, &processModeChoiceMenuParm_ChoiceList, &processModeChoiceMenuParm_Range, &SOP_Operator::CallbackProcessMode, 0, 1, "Specify edge process mode.");


		__DECLARE_Main_Section_PRM(6)
		static auto		lengthModeChoiceMenuParm_Name = PRM_Name("lengthmode", "Length Mode");
		static auto		lengthModeChoiceMenuParm_Range = PRM_Range(PRM_RANGE_RESTRICTED, 0, PRM_RANGE_RESTRICTED, 2);
		static PRM_Name lengthModeChoiceMenuParm_Choices[] =
		{
			PRM_Name("0", "Override"),
			PRM_Name("1", "Percentage"),			
			PRM_Name("2", "Custom"),
			PRM_Name(0)
		};
		static auto		lengthModeChoiceMenuParm_ChoiceList = PRM_ChoiceList(PRM_CHOICELIST_SINGLE, lengthModeChoiceMenuParm_Choices);
		auto			lengthModeChoiceMenu_Parameter = PRM_Template(PRM_ORD, 1, &lengthModeChoiceMenuParm_Name, 0, &lengthModeChoiceMenuParm_ChoiceList, &lengthModeChoiceMenuParm_Range, &SOP_Operator::CallbackLenghtMode, 0, 1, "Specify length mode.");
		
		DECLARE_Custom_Float_MinU_to_MaxU_PRM("lengthoverridevalue", "-/+", -1, 1, 0, 0, "World Units.", lengthOverrideValue)
		DECLARE_Custom_Float_0R_to_MaxU_PRM("lengthpercentagevalue", "%", 200, 100, 0, "Percents.", lengthPercentageValue)
		DECLARE_Custom_Float_0R_to_MaxU_PRM("lengtcustomvalue", "Value", 2, 1, 0, "World Units.", lengthCustomValue)

		static auto		startFromChoiceMenuParm_Name = PRM_Name("startfrom", "Start From");
		static auto		startFromChoiceMenuParm_Range = PRM_Range(PRM_RANGE_RESTRICTED, 0, PRM_RANGE_RESTRICTED, 4);
		static auto     startFromChoiceMenuParm_Default = PRM_Default(2);
		static PRM_Name startFromChoiceMenuParm_Choices[] =
		{
			PRM_Name("0", "Smaller Number"),
			PRM_Name("1", "Bigger Number"),
			PRM_Name("2", "Averanged"),
			PRM_Name("3", "Closest To Position"),
			PRM_Name("4", "Farthest To Position"),
			PRM_Name(0)
		};
		static auto		startFromChoiceMenuParm_ChoiceList = PRM_ChoiceList(PRM_CHOICELIST_SINGLE, startFromChoiceMenuParm_Choices);
		auto			startFromChoiceMenu_Parameter = PRM_Template(PRM_ORD, 1, &startFromChoiceMenuParm_Name, &startFromChoiceMenuParm_Default, &startFromChoiceMenuParm_ChoiceList, &startFromChoiceMenuParm_Range, &SOP_Operator::CallbackStartFrom, 0, 1, "Specify direction of modification.");

		DECLARE_Vector3_000_PRM("positionpoint", "Position", "Lookat position.", positionPoint)

		__DECLARE_Additional_Section_PRM(7)
		DECLARE_Toggle_with_Separator_OFF_PRM("setmorph", "Morph", "setmorphseparator", &SOP_Operator::CallbackSetMorph, "Blend between original and modified position.", setMorph)
		DECLARE_Custom_Float_MinR_to_MaxU_PRM("morphvalue", "Value", 0, 100, 100, 0, "Percents", morphValue)
		DECLARE_DescriptionPRM(SOP_Operator)
	}
		
DECLARE_SOP_Namespace_End

/* -----------------------------------------------------------------
UNDEFINES                                                          |
----------------------------------------------------------------- */

#undef SOP_Operator

#endif // !____parameters_h____