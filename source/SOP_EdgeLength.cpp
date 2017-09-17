/*
	Modify edge length in arbitrary positions.

	IMPORTANT! ------------------------------------------	
	-----------------------------------------------------

	Author: 	SWANN
	Email:		sebastianswann@outlook.com

	LICENSE ------------------------------------------

	Copyright (c) 2016-2017 SWANN
	All rights reserved.

	Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

	1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
	2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
	3. The names of the contributors may not be used to endorse or promote products derived from this software without specific prior written permission.

	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

/* -----------------------------------------------------------------
INCLUDES                                                           |
----------------------------------------------------------------- */

// SESI
#include <UT/UT_Interrupt.h>
#include <OP/OP_AutoLockInputs.h>
#include <CH/CH_Manager.h>
#include <PRM/PRM_Parm.h>
#include <PRM/PRM_Error.h>
#include <PRM/PRM_Include.h>
#include <sys/SYS_Math.h>

// hou-hdk-common
#include <Macros/ParameterList.h>
#include <Utility/ParameterAccessing.h>
#include <Utility/EdgeGroupAccessing.h>

// this
#include "Parameters.h"
#include "ProcessModeOption.h"
#include "LengthModeOption.h"
#include "StartFromOption.h"

/* -----------------------------------------------------------------
DEFINES                                                            |
----------------------------------------------------------------- */

#define SOP_Operator			GET_SOP_Namespace()::SOP_EdgeLength
#define SOP_SmallName			"modeling::edgelength::1.0"
#define SOP_Input_Name_0		"Geometry"
#define SOP_Input_Name_1		"Positions"
#define SOP_Icon_Name			"nodeway_short_dark_WB.png"
#define SOP_Base_Operator		SOP_Node
#define MSS_Selector			GET_SOP_Namespace()::MSS_EdgeLengthSelector

// very important
#define SOP_GroupFieldIndex_0	1

#define UI						GET_SOP_Namespace()::UI
#define PRM_ACCESS				GET_Base_Namespace()::Utility::PRM
#define GRP_ACCESS				GET_Base_Namespace()::Utility::Groups

/* -----------------------------------------------------------------
PARAMETERS                                                         |
----------------------------------------------------------------- */

PARAMETERLIST_Start(SOP_Operator)

	UI::filterSectionSwitcher_Parameter,
	UI::input0EdgeGroup_Parameter,
	UI::processModeChoiceMenu_Parameter,
	UI::edgeIslandErrorModeChoiceMenu_Parameter,

	UI::mainSectionSwitcher_Parameter,
	UI::lengthModeChoiceMenu_Parameter,
	UI::lengthOverrideValueFloat_Parameter,
	UI::lengthPercentageValueFloat_Parameter,
	UI::lengthCustomValueFloat_Parameter,
	UI::startFromChoiceMenu_Parameter,
	UI::positionPointVector3_Parameter,

	UI::additionalSectionSwitcher_Parameter,
	UI::setMorphToggle_Parameter,
	UI::setMorphSeparator_Parameter,
	UI::morphValueFloat_Parameter,
	PARAMETERLIST_DescriptionPRM(UI),

PARAMETERLIST_End()

bool 
SOP_Operator::updateParmsFlags()
{
	DEFAULTS_UpdateParmsFlags(SOP_Base_Operator)

	// is input connected?
	const exint is0Connected = getInput(0) == nullptr ? 0 : 1;

	/* ---------------------------- Set Global Visibility ---------------------------- */

	visibilityState = is0Connected ? 1 : 0; // TODO: do I still need this?

	/* ---------------------------- Set States --------------------------------------- */		

	PRM_ACCESS::Get::IntPRM(this, activeState, UI::lengthModeChoiceMenu_Parameter, currentTime);
	changed |= setVisibleState(UI::lengthOverrideValueFloat_Parameter.getToken(), activeState == 0);
	changed |= setVisibleState(UI::lengthPercentageValueFloat_Parameter.getToken(), activeState == 1);
	changed |= setVisibleState(UI::lengthCustomValueFloat_Parameter.getToken(), activeState == 2);

	PRM_ACCESS::Get::IntPRM(this, activeState, UI::processModeChoiceMenu_Parameter, currentTime);
	if (activeState == 1)
	{
		changed |= setVisibleState(UI::startFromChoiceMenu_Parameter.getToken(), 0);
		changed |= setVisibleState(UI::positionPointVector3_Parameter.getToken(), 0);
	}
	else
	{
		PRM_ACCESS::Get::IntPRM(this, activeState, UI::startFromChoiceMenu_Parameter, currentTime);
		changed |= setVisibleState(UI::positionPointVector3_Parameter.getToken(), activeState >= 3);
		changed |= setVisibleState(UI::startFromChoiceMenu_Parameter.getToken(), 1);		
	}

	PRM_ACCESS::Get::IntPRM(this, visibilityState, UI::setMorphToggle_Parameter, currentTime);
	changed |= setVisibleState(UI::morphValueFloat_Parameter.getToken(), visibilityState);

	// update description active state
	UPDATE_DescriptionPRM_ActiveState(this, UI)

	return changed;
}

IMPLEMENT_DescriptionPRM_Callback(SOP_Operator, UI)

int
SOP_Operator::CallbackProcessMode(void* data, int index, float time, const PRM_Template* tmp)
{
	const auto me = reinterpret_cast<SOP_Operator*>(data);
	if (!me) return 0;

	// TODO: figure out why restoreFactoryDefaults() doesn't work
	exint processModeState;
	PRM_ACCESS::Get::IntPRM(me, processModeState, UI::processModeChoiceMenu_Parameter, time);

	if (processModeState)
	{
		auto defVal = UI::startFromChoiceMenu_Parameter.getFactoryDefaults()->getFloat();
		PRM_ACCESS::Set::FloatPRM(me, defVal, UI::startFromChoiceMenu_Parameter, time);
		CallbackStartFrom(data, index, time, tmp);
	}

	return 1;
}

int
SOP_Operator::CallbackLenghtMode(void* data, int index, float time, const PRM_Template* tmp)
{
	const auto me = reinterpret_cast<SOP_Operator*>(data);
	if (!me) return 0;

	// TODO: figure out why restoreFactoryDefaults() doesn't work
	exint lenghtModeState;
	PRM_ACCESS::Get::IntPRM(me, lenghtModeState, UI::lengthModeChoiceMenu_Parameter, time);

	if (lenghtModeState == static_cast<exint>(LengthModeOption::OVERRIDE))
	{
		auto defVal = UI::lengthOverrideValueFloat_Parameter.getFactoryDefaults()->getFloat();
		PRM_ACCESS::Set::FloatPRM(me, defVal, UI::lengthOverrideValueFloat_Parameter, time);
	}
	else if (lenghtModeState == static_cast<exint>(LengthModeOption::PERCENTAGE))
	{
		auto defVal = UI::lengthPercentageValueFloat_Parameter.getFactoryDefaults()->getFloat();
		PRM_ACCESS::Set::FloatPRM(me, defVal, UI::lengthPercentageValueFloat_Parameter, time);
	}
	else if (lenghtModeState == static_cast<exint>(LengthModeOption::CUSTOM))
	{
		auto defVal = UI::lengthCustomValueFloat_Parameter.getFactoryDefaults()->getFloat();
		PRM_ACCESS::Set::FloatPRM(me, defVal, UI::lengthCustomValueFloat_Parameter, time);
	}

	return 1;
}

int
SOP_Operator::CallbackStartFrom(void* data, int index, float time, const PRM_Template* tmp)
{
	const auto me = reinterpret_cast<SOP_Operator*>(data);
	if (!me) return 0;

	// TODO: figure out why restoreFactoryDefaults() doesn't work
	auto position = UT_Vector3R(0, 0, 0);
	PRM_ACCESS::Set::VectorPRM(me, position, UI::positionPointVector3_Parameter, time);

	return 1;
}

int
SOP_Operator::CallbackSetMorph(void* data, int index, float time, const PRM_Template* tmp)
{
	const auto me = reinterpret_cast<SOP_Operator*>(data);
	if (!me) return 0;

	// TODO: figure out why restoreFactoryDefaults() doesn't work
	auto defVal = UI::morphValueFloat_Parameter.getFactoryDefaults()->getFloat();
	PRM_ACCESS::Set::FloatPRM(me, defVal, UI::morphValueFloat_Parameter, time);

	return 1;
}

/* -----------------------------------------------------------------
OPERATOR INITIALIZATION                                            |
----------------------------------------------------------------- */

SOP_Operator::~SOP_EdgeLength() { }
SOP_Operator::SOP_EdgeLength(OP_Network* network, const char* name, OP_Operator* op) 
: SOP_Base_Operator(network, name, op), 
_edgeGroupInput0(nullptr)
{ op->setIconName(SOP_Icon_Name); }

OP_Node* 
SOP_Operator::CreateMe(OP_Network* network, const char* name, OP_Operator* op) { return new SOP_Operator(network, name, op); }

const char* 
SOP_Operator::inputLabel(unsigned input) const 
{ return SOP_Input_Name_0; }

OP_ERROR
SOP_Operator::cookInputGroups(OP_Context &context, int alone)
{ return cookInputEdgeGroups(context, this->_edgeGroupInput0, alone, true, SOP_GroupFieldIndex_0, -1, true, 0); }

/* -----------------------------------------------------------------
HELPERS                                                            |
----------------------------------------------------------------- */

void
SOP_Operator::WhenSmallerOrBigger(const exint lengthmode, const exint startfrom, GA_EdgeIsland& edgeisland, fpreal lengthvalue) const
{
	const auto smallerOffset = edgeisland.First() < edgeisland.Last() ? edgeisland.First() : edgeisland.Last();
	const auto biggerOffset = edgeisland.First() == smallerOffset ? edgeisland.Last() : edgeisland.First();
	auto direction = this->gdp->getPos3(biggerOffset) - this->gdp->getPos3(smallerOffset);
	const auto distance = direction.length();

	direction.normalize();

	if (startfrom == static_cast<exint>(StartFromOption::BIGGER_NUMBER))
	{
		switch (lengthmode)
		{
			case static_cast<exint>(LengthModeOption::OVERRIDE): { this->gdp->setPos3(smallerOffset, this->gdp->getPos3(smallerOffset) + (-direction * lengthvalue)); } break;
			case static_cast<exint>(LengthModeOption::PERCENTAGE): { this->gdp->setPos3(smallerOffset, this->gdp->getPos3(biggerOffset) + (-direction * (distance * lengthvalue))); } break;
			case static_cast<exint>(LengthModeOption::CUSTOM): { this->gdp->setPos3(smallerOffset, this->gdp->getPos3(biggerOffset) + (-direction * lengthvalue)); } break;
		}
	}
	else if (startfrom == static_cast<exint>(StartFromOption::SMALLER_NUMBER))
	{
		switch (lengthmode)
		{
			case static_cast<exint>(LengthModeOption::OVERRIDE): { this->gdp->setPos3(biggerOffset, this->gdp->getPos3(biggerOffset) + direction * lengthvalue); } break;
			case static_cast<exint>(LengthModeOption::PERCENTAGE): { this->gdp->setPos3(biggerOffset, this->gdp->getPos3(smallerOffset) + direction * (distance * lengthvalue)); } break;
			case static_cast<exint>(LengthModeOption::CUSTOM): { this->gdp->setPos3(biggerOffset, this->gdp->getPos3(smallerOffset) + direction * lengthvalue); } break;
		}
	}
}

void
SOP_Operator::WhenAveranged(const exint lengthmode, GA_EdgeIsland& edgeisland, fpreal lengthvalue) const
{
	const auto positionA = this->gdp->getPos3(edgeisland.Last());
	const auto positionB = this->gdp->getPos3(edgeisland.First());
	auto direction = positionA - positionB;
	auto distance = direction.length();

	direction.normalize();
	const auto middle = this->gdp->getPos3(edgeisland.First()) + direction * (distance * 0.5);

	auto directionA = positionA - middle;
	auto directionB = positionB - middle;
	directionA.normalize();
	directionB.normalize();

	switch (lengthmode)
	{
		case static_cast<exint>(LengthModeOption::OVERRIDE):
			{
				this->gdp->setPos3(edgeisland.Last(), positionA + directionA * (lengthvalue * 0.5));
				this->gdp->setPos3(edgeisland.First(), positionB + directionB * (lengthvalue * 0.5));
			}
			break;
		case static_cast<exint>(LengthModeOption::PERCENTAGE):
			{			
				distance = distance * 0.5;
				this->gdp->setPos3(edgeisland.Last(), positionA + directionA * ((distance * lengthvalue) - distance));
				this->gdp->setPos3(edgeisland.First(), positionB + directionB * ((distance * lengthvalue) - distance));
			}
			break;
		case static_cast<exint>(LengthModeOption::CUSTOM):
			{
				lengthvalue = lengthvalue * 0.5;
				this->gdp->setPos3(edgeisland.Last(), middle + (directionA * lengthvalue));
				this->gdp->setPos3(edgeisland.First(), middle + (directionB * lengthvalue));
			}
			break;
	}

}

void
SOP_Operator::WhenClosestOrFarthest(const exint lengthmode, const exint startfrom, UT_Vector3R position, GA_EdgeIsland& edgeisland, fpreal lengthvalue) const
{	
	const auto positionA = this->gdp->getPos3(edgeisland.First());
	const auto positionB = this->gdp->getPos3(edgeisland.Last());

	auto directionA = positionA - positionB;
	auto directionB = positionB - positionA;
	const auto distanceA = directionA.length();
	const auto distanceB = directionB.length();

	directionA.normalize();
	directionB.normalize();

	const auto measuredDistanceA = (positionA - static_cast<UT_Vector3>(position)).length();
	const auto measuredDistanceB = (positionB - static_cast<UT_Vector3>(position)).length();

	if (startfrom == static_cast<exint>(StartFromOption::CLOSEST_TO_POSITION))
	{
		switch (lengthmode)
		{
			case static_cast<exint>(LengthModeOption::OVERRIDE):
				{
					if (measuredDistanceA >= measuredDistanceB) this->gdp->setPos3(edgeisland.First(), this->gdp->getPos3(edgeisland.First()) + (directionA * lengthvalue));
					else this->gdp->setPos3(edgeisland.Last(), this->gdp->getPos3(edgeisland.Last()) + (directionB * lengthvalue));
				}
				break;
			case static_cast<exint>(LengthModeOption::PERCENTAGE):
				{
					if (measuredDistanceA >= measuredDistanceB) this->gdp->setPos3(edgeisland.First(), this->gdp->getPos3(edgeisland.First()) + directionA * ((distanceA * lengthvalue) - distanceA));
					else this->gdp->setPos3(edgeisland.Last(), this->gdp->getPos3(edgeisland.Last()) + directionB * ((distanceB * lengthvalue) - distanceB));
				}
				break;
			case static_cast<exint>(LengthModeOption::CUSTOM):
				{
					if (measuredDistanceA >= measuredDistanceB) this->gdp->setPos3(edgeisland.First(), this->gdp->getPos3(edgeisland.Last()) + (directionA * lengthvalue));
					else this->gdp->setPos3(edgeisland.Last(), this->gdp->getPos3(edgeisland.First()) + (directionB * lengthvalue));
				}
				break;
		}
	}
	else if (startfrom == static_cast<exint>(StartFromOption::FARTHEST_TO_POSITION))
	{
		switch (lengthmode)
		{
			case static_cast<exint>(LengthModeOption::OVERRIDE):
				{
					if (measuredDistanceA <= measuredDistanceB) this->gdp->setPos3(edgeisland.First(), this->gdp->getPos3(edgeisland.First()) + (directionA * lengthvalue));
					else this->gdp->setPos3(edgeisland.Last(), this->gdp->getPos3(edgeisland.Last()) + (directionB * lengthvalue));
				}
				break;
			case static_cast<exint>(LengthModeOption::PERCENTAGE):
			{
				if (measuredDistanceA <= measuredDistanceB) this->gdp->setPos3(edgeisland.First(), this->gdp->getPos3(edgeisland.First()) + directionA * ((distanceA * lengthvalue) - distanceA));
				else this->gdp->setPos3(edgeisland.Last(), this->gdp->getPos3(edgeisland.Last()) + directionB * ((distanceB * lengthvalue) - distanceB));
			}
				break;
			case static_cast<exint>(LengthModeOption::CUSTOM):
				{
					if (measuredDistanceA <= measuredDistanceB) this->gdp->setPos3(edgeisland.First(), this->gdp->getPos3(edgeisland.Last()) + (directionA * lengthvalue));
					else this->gdp->setPos3(edgeisland.Last(), this->gdp->getPos3(edgeisland.First()) + (directionB * lengthvalue));
				}
				break;
		}
	}
}

OP_ERROR
SOP_Operator::SetLengthOfEachEdgeIsland(GA_EdgeIslandBundle& edgeislands, UT_AutoInterrupt progress, fpreal time)
{
	UT_Map<GA_Offset, UT_Vector3>	originalPositions;

	exint							processModeState;
	exint							lenghtModeState;
	fpreal							lenghtValue;
	exint							startFromState;
	UT_Vector3R						positionPointValue;
	bool							setMorphState;
	fpreal							morphValueState;
	exint							edgeIslandErrorLevelState;

	PRM_ACCESS::Get::IntPRM(this, processModeState, UI::processModeChoiceMenu_Parameter, time);

	PRM_ACCESS::Get::IntPRM(this, lenghtModeState, UI::lengthModeChoiceMenu_Parameter, time);
	switch (lenghtModeState)
	{
		case static_cast<exint>(LengthModeOption::OVERRIDE): { PRM_ACCESS::Get::FloatPRM(this, lenghtValue, UI::lengthOverrideValueFloat_Parameter, time); } break;
		case static_cast<exint>(LengthModeOption::PERCENTAGE):
			{
				PRM_ACCESS::Get::FloatPRM(this, lenghtValue, UI::lengthPercentageValueFloat_Parameter, time);
				lenghtValue = 0.01 * lenghtValue; // rescale
			}
			break;
		case static_cast<exint>(LengthModeOption::CUSTOM): { PRM_ACCESS::Get::FloatPRM(this, lenghtValue, UI::lengthCustomValueFloat_Parameter, time); } break;
	}

	PRM_ACCESS::Get::IntPRM(this, startFromState, UI::startFromChoiceMenu_Parameter, time);
	PRM_ACCESS::Get::VectorPRM(this, positionPointValue, UI::positionPointVector3_Parameter, time);

	PRM_ACCESS::Get::IntPRM(this, setMorphState, UI::setMorphToggle_Parameter, time);
	PRM_ACCESS::Get::FloatPRM(this, morphValueState, UI::morphValueFloat_Parameter, time);
	morphValueState = setMorphState ? 0.01 * morphValueState : 1.0;				// from percentage
	
	PRM_ACCESS::Get::IntPRM(this, edgeIslandErrorLevelState, UI::edgeIslandErrorModeChoiceMenu_Parameter, time);

#define THIS_AddWarning(node, message) { switch (edgeIslandErrorLevelState) { default: /* do nothing */ continue; case static_cast<exint>(HOU_NODE_ERROR_LEVEL::Warning) : { node->addWarning(SOP_ErrorCodes::SOP_MESSAGE, message); } continue; case static_cast<exint>(HOU_NODE_ERROR_LEVEL::Error) : { node->addError(SOP_ErrorCodes::SOP_MESSAGE, message); } return error(); } }
//#define THIS_AddWarning(node, message) { node->addWarning(SOP_ErrorCodes::SOP_MESSAGE, message); continue; }
#define THIS_ProgressEscape(node, message, passedprogress) if (passedprogress.wasInterrupted()) { node->addError(SOP_ErrorCodes::SOP_MESSAGE, message); return error(); }	
	for (auto island : edgeislands)
	{
		THIS_ProgressEscape(this, "Operation interrupted", progress)

#ifdef DEBUG_ISLANDS
		island.Report();
#endif // DEBUG_ISLANDS

		// ignore islands with no endpoints				
		if (!island.IsValid() && !island.HasEndPoints()) 
		{
			/*
				In theory we never should even hit the possibility that we have closed and with no endpoints edge islands passed here, 
				since selection automatically looks only for open islands. But to be safe we still check for it.
			 */

			const auto errorMessage = "Edge islands with no endpoints detected.";
			switch (edgeIslandErrorLevelState)
			{			
				default: /* do nothing */ continue;
				case static_cast<exint>(HOU_NODE_ERROR_LEVEL::Warning) : { addWarning(SOP_ErrorCodes::SOP_MESSAGE, errorMessage); } continue;
				case static_cast<exint>(HOU_NODE_ERROR_LEVEL::Error) : { addError(SOP_ErrorCodes::SOP_MESSAGE, errorMessage); } return error();				
			}
		}

		// clear everything to be sure that nothing is cached
		originalPositions.clear();

		// store original positions		
		auto it = island.Begin();
		for (it; !it.atEnd(); it.advance())
		{
			THIS_ProgressEscape(this, "Operation interrupted", progress)
			originalPositions[*it] = this->gdp->getPos3(*it);
		}

		if (processModeState == static_cast<exint>(ProcessModeOption::SINGLE) || processModeState == static_cast<exint>(ProcessModeOption::MIXED))
		{
			if (island.GetEdges().size() == 1)
			{
				if (startFromState < 2) WhenSmallerOrBigger(lenghtModeState, startFromState, island, lenghtValue);
				else if (startFromState == 2) WhenAveranged(lenghtModeState, island, lenghtValue);
				else WhenClosestOrFarthest(lenghtModeState, startFromState, positionPointValue, island, lenghtValue);
			}
			else if (processModeState != static_cast<exint>(ProcessModeOption::MIXED)) THIS_AddWarning(this, "Edge islands with more than one edge detected.")
		}		

		if (processModeState == static_cast<exint>(ProcessModeOption::MULTI) || processModeState == static_cast<exint>(ProcessModeOption::MIXED))
		{
			if (island.GetEdges().size() > 1)
			{
				// allow only multi edges with one common point
				if (!island.HasCommonOffset() && island.HasEndPoints()) THIS_AddWarning(this, "Edge islands with more than one common point detected.")

				for (auto edge : island.GetEdges())
				{
					const auto notCommonOffset = edge.p0() == island.CommonOffset() ? edge.p1() : edge.p0();
					auto direction = this->gdp->getPos3(notCommonOffset) - this->gdp->getPos3(island.CommonOffset());
					const auto distance = direction.length();
					direction.normalize();

					switch (lenghtModeState)
					{
						case static_cast<exint>(LengthModeOption::OVERRIDE): { this->gdp->setPos3(notCommonOffset, this->gdp->getPos3(notCommonOffset) + direction * lenghtValue); } break;
						case static_cast<exint>(LengthModeOption::PERCENTAGE) : { this->gdp->setPos3(notCommonOffset, this->gdp->getPos3(notCommonOffset) + direction * ((distance * lenghtValue) - distance)); } break;
						case static_cast<exint>(LengthModeOption::CUSTOM) : { this->gdp->setPos3(notCommonOffset, this->gdp->getPos3(island.CommonOffset()) + direction * lenghtValue); } break;
					}
				}
			}
			else if (processModeState != static_cast<exint>(ProcessModeOption::MIXED)) THIS_AddWarning(this, "Edge islands with one edge only detected.")
		}

		// morph
		if (!setMorphState) continue;

		it = island.Begin();
		for (it; !it.atEnd(); it.advance())
		{
			THIS_ProgressEscape(this, "Operation interrupted", progress)
			const auto newPosition = SYSlerp(originalPositions[*it], this->gdp->getPos3(*it), morphValueState);
			this->gdp->setPos3(*it, newPosition);
		}
	}
#undef THIS_ProgressEscape
#undef THIS_AddWarning

	return error();
}

/* -----------------------------------------------------------------
MAIN                                                               |
----------------------------------------------------------------- */

OP_ERROR 
SOP_Operator::cookMySop(OP_Context& context)
{	
	DEFAULTS_CookMySop()
	
	if (duplicateSource(0, context) < OP_ERROR::UT_ERROR_WARNING && error() < OP_ERROR::UT_ERROR_WARNING && cookInputGroups(context) < OP_ERROR::UT_ERROR_WARNING)
	{
		auto success = this->_edgeGroupInput0 && !this->_edgeGroupInput0->isEmpty();
		if ((success && error() >= OP_ERROR::UT_ERROR_WARNING) || (!success && error() >= OP_ERROR::UT_ERROR_NONE))
		{			
			//clearSelection();
			addWarning(SOP_ErrorCodes::SOP_ERR_BADGROUP);
			return error();
		}

		// edge selection can contain multiple separate edge island, so before we find them, we need to find their endpoints, so we could have some starting point
		auto edgeData = GA_EdgesData();
		edgeData.Clear();

		success = GRP_ACCESS::Edge::Break::PerPoint(this, this->_edgeGroupInput0, edgeData, progress);
		if ((success && error() >= OP_ERROR::UT_ERROR_WARNING) || (!success && error() >= OP_ERROR::UT_ERROR_NONE)) return error();

		// once we have endpoints, we can go thru our edge group and split it on edge islands	
		auto edgeIslands = GA_EdgeIslandBundle();
		edgeIslands.clear();

		success = GRP_ACCESS::Edge::Break::PerIsland(this, edgeData, edgeIslands, EdgeIslandType::OPEN, progress);
		if ((success && error() >= OP_ERROR::UT_ERROR_WARNING) || (!success && error() >= OP_ERROR::UT_ERROR_NONE)) return error();	

		// finally, we can go thru each edge island and set edge lengths
		return SetLengthOfEachEdgeIsland(edgeIslands, progress, currentTime);
	}	

	return error();
}

/* -----------------------------------------------------------------
SELECTOR IMPLEMENTATION                                            |
----------------------------------------------------------------- */

MSS_Selector::~MSS_EdgeLengthSelector() { }

MSS_Selector::MSS_EdgeLengthSelector(OP3D_View& viewer, PI_SelectorTemplate& templ) : MSS_ReusableSelector(viewer, templ, SOP_SmallName, CONST_EdgeGroupInput0_Name, 0, true)
{ this->setAllowUseExistingSelection(false); }

BM_InputSelector*
MSS_Selector::CreateMe(BM_View& viewer, PI_SelectorTemplate& templ)
{ return new MSS_Selector(reinterpret_cast<OP3D_View&>(viewer), templ); }

const char*
MSS_Selector::className() const
{ return "MSS_EdgeLengthSelector"; }

/* -----------------------------------------------------------------
UNDEFINES                                                          |
----------------------------------------------------------------- */

#undef PRM_ACCESS
#undef UI

#undef SOP_GroupFieldIndex_0

#undef MSS_Selector
#undef SOP_Base_Operator
#undef SOP_Icon_Name
#undef SOP_Input_Name_1
#undef SOP_Input_Name_0
#undef SOP_SmallName
#undef SOP_Operator