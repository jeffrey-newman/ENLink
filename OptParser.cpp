/*
 * File:   EpaNetLink.cpp
 * Author: jnewman
 *
 * Created on April 6, 2010, 2:24 PM
 */

#define FUSION_MAX_VECTOR_SIZE 20

#include <iostream>
#include <fstream>
#include <boost/spirit/include/qi.hpp>
#include <boost/config/warning_disable.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>
#include <boost/fusion/include/std_pair.hpp>
//#include <bits/stl_algobase.h>
#include "OptParser.h"


#include <exception>


IFException::IFException() : std::runtime_error("input file error")
{
}

IFException::IFException(std::string what) : std::runtime_error(what)
{
}

ObjectiveInputError::ObjectiveInputError() : IFException()
{
}

ObjectiveInputError::ObjectiveInputError(std::string what)
        : IFException(what)
{
}

OptFileParser::OptFileParser() :
        OptFileParser::base_type(start), optData(new OptData()) {
    this->init();
}

OptFileParser::OptFileParser(boost::shared_ptr <OptData> _optData) :
        OptFileParser::base_type(start), optData(_optData) {
    this->init();
}

Skipper::Skipper() : Skipper::base_type(skip_it)
{
    comment_char = ';';
    skip_it = (qi::space | comment);
    comment = qi::lexeme[qi::lit(comment_char) >> *(qi::char_ - qi::eol) >> qi::eol ];
}

No_eol_skip::No_eol_skip() : No_eol_skip::base_type(skip_it)
{
    comment_char = ';';
    //Skip spaces or comments, except if they are end of line spaces.
    skip_it = ((qi::space - qi::eol) | comment);
    comment = qi::lexeme[qi::lit(comment_char) >> *(qi::char_ - qi::eol)];
}

void
OptFileParser::init() {
    using qi::double_;
    using qi::int_;
    using qi::char_;
    using qi::eol;
    using qi::lexeme;
    using qi::space;
    using qi::lit;
    using qi::_val;
    using qi::_1;
    using qi::eps;
    using qi::omit;
    using qi::no_case;
    using phoenix::ref;
    using phoenix::push_back;
    using phoenix::insert;

    //Some general parsers we make frequent use of
    headers = lit("[SPECIFICATION]") //
              | lit("[PIPE_DATA]") //
              | lit("[LINK_GROUPS]") //
              | lit("[PIPE_CHOICES]") //
              | lit("[PENALTIES]") //
              | lit("[EVALUATION]") //
              | lit("[VELOCITIES]") //
              | lit("[PRESSURES]") //
              | lit("[TREATMENTS]") //
              | lit("[PUMPING]") //
              | lit("[HEADS]") //
              | lit("[VSP]") //
              | lit("[STORAGE_TANK_CHOICES]") //
              | lit("[STORAGE_TANK_DATA]") //
              | lit("[NODE_GROUPS]");
    //white_seperated_string = lexeme[+(char_ - (space | '['))];
    white_seperated_string = lexeme[(+(char_ - space))] - headers;
    white_seperated_string_then_eol = lexeme[(+(char_ - space))
            >> omit[*(space - eol)] >> eol] - headers;
    //list_white_seperated_string = lexeme[*white_seperated_string >> *(space - eol)) >> eol] ;
    list_white_seperated_string = lexeme[*(+(char_ - (space | '['))
            >> omit[*(space - eol)]) >> eol];

    //lexeme[skip(customSkipper)]
    double_then_end_of_line = (lexeme[double_[_val = _1]
            >> *(space - eol) >> eol])[_val += 0];
    int_then_end_of_line = (lexeme[int_[_val = _1] >> *(space - eol)
                                                   >> eol])[_val += 0];

    isDistSystem_rule = no_case[lit("Distribution")[_val = true]];
    isCollSystem_rule = no_case[lit("Collection")[_val = true]];
    isWtrBalSystem_rule = no_case[lit("WaterBalance")[_val = true]];
    isOptPipes_rule = no_case[lit("Pipes")[_val = true]];
    isOptVSP_rule = no_case[lit("VSPStations")[_val = true]];
    isOptTanks_rule = no_case[lit("Tanks")[_val = true]];
    allTanksSameSize_rule = no_case[lit("All consistent size")[_val = true]];
    allowVariableSize_rule = no_case[lit("Allow variable sizes")[_val = false]];

    isCostObj_rule = no_case[lit("Cost")[_val = true]];
    isNetworkResiliency_rule = no_case[lit("NetworkResiliency")[_val=true]];
    isEnergyObj_rule = no_case[lit("Energy")[_val = true]];
    isWtrObj_rule = no_case[lit("WaterSavings")[_val = true]];
    isPressureConstraint_rule = no_case[lit("Pressure")[_val = true]];
    isHeadConstraint_rule = no_case[lit("Head")[_val = true]];
    isVelocityConstraint_rule = no_case[lit("Velocity")[_val = true]];
    en_inp_rule =
            lit("EpanetFile:")
                    >> lexeme['\"' >> +(char_ - '\"') >> '\"' /*>> *(space - eol) >> eol*/]
                       - headers;

//    working_dir_rule =
//            lit("EpanetDylibLoc")
//                    >> lexeme['\"' >> +(char_ - '\"') >> '\"' /*>> *(space - eol) >> eol*/]
//                       - headers;


    iuwmod_inp_rule =
            lit("IUWModFile:")
                    >> lexeme['\"' >> +(char_ - '\"') >> '\"' /*>> *(space - eol) >> eol*/]
                       - headers;

    systemRule = no_case[lit("Systems:")] >> //
                                          *( //
                                                  isDistSystem_rule[phoenix::ref(optData->isDistSystem) = _1] //
                                                  | isCollSystem_rule[phoenix::ref(optData->isCollSystem) = _1] //
                                                  | isWtrBalSystem_rule[phoenix::ref(optData->isWtrBalSystem) = _1] //
                                                  | lit(",") //
                                          );
    OptimiseRule = no_case[lit("Optimise:")] >> //
                                             *( //
                                                     isOptPipes_rule[phoenix::ref(optData->isOptPipes) = _1] //
                                                     | isOptVSP_rule[phoenix::ref(optData->isOptVSP) = _1] //
                                                     | (isOptTanks_rule[phoenix::ref(optData->isOptTanks) = _1]
                                                             >> (allTanksSameSize_rule[phoenix::ref(
                                                                     optData->allTankChoicesTheSame) = _1]
                                                                 | allowVariableSize_rule[phoenix::ref(
                                                                     optData->allTankChoicesTheSame) = _1]))//
                                                     | lit(",") //
                                             );
    ObjectivesRule = no_case[lit("Objectives:")] >> //
                                                 *( //
                                                         isCostObj_rule[phoenix::ref(optData->isCostObj) = _1] //
                                                         | isNetworkResiliency_rule[phoenix::ref(optData->isNetworkResiliencyObj) = _1] //
                                                         | isEnergyObj_rule[phoenix::ref(optData->isEnergyObj) = _1] //
                                                         | isWtrObj_rule[phoenix::ref(optData->isWtrObj) = _1] //
                                                         | lit(",") //
                                                 );
    ConstraintsRule = no_case[lit("Constraints:")] >> //
                                                   *( //
                                                           isPressureConstraint_rule[phoenix::ref(
                                                                   optData->isPressureConstraint) =
                                                                                             _1] //
                                                           | isHeadConstraint_rule[phoenix::ref(
                                                                   optData->isHeadConstraint) = _1] //
                                                           | isVelocityConstraint_rule[phoenix::ref(
                                                                   optData->isVelocityConstraint) = _1] //
                                                           | lit(",") //
                                                   );

    specification_header = lexeme[lit("[SPECIFICATION]")
            >> *(space - eol) >> eol];
    specification_rule = specification_header
            >> *(systemRule | OptimiseRule | ObjectivesRule
                 | ConstraintsRule
                 | en_inp_rule[phoenix::ref(optData->epanetFile) = _1]
                 | iuwmod_inp_rule[phoenix::ref(optData->iuwmodFile) = _1]);

    //Pipe data parser rules
    pipe_data_header = lexeme[lit("[PIPE_DATA]") >> *(space - eol)
                                                 >> eol];
    pipe_datum_id = white_seperated_string;
    pipe_data_description = white_seperated_string;
    pipe_datum_info = pipe_data_description >> double_ // roughness (mm)
                                            >> double_ // diameter (mm)
                                            >> double_ // cost of pipe per m
                                            >> double_ // repair cost of pipe per m per year
                                            >> double_ // embodied energy of pipe per m
                                            >> double_ // embodied energy for repairs per year per m
                                            >> double_ // greenhouse gas emissions of pipe per m
                                            >> double_ // required top cover of pipe (mm)
                                            >> double_ // required side cover of pipe (mm)
                                            >> double_then_end_of_line; // required bottom fill of pipe (mm)
    pipe_datum = pipe_datum_id >> pipe_datum_info;
    pipe_data = pipe_data_header
            >> *(pipe_datum[insert(phoenix::ref(optData->pipe_data_map), _1)]);

    //Treatments parser rules
    treatments_header = lexeme[lit("[TREATMENTS]") >> *(space - eol)
                                                   >> eol];
    treatment_id = white_seperated_string;
    treatment_description = white_seperated_string;
    capacity = double_;
    capital_cost = double_;
    operate_cost = double_then_end_of_line;
    treatment_info = treatment_description >> capacity >> capital_cost
                                           >> operate_cost;
    treatment_datum = treatment_id >> treatment_info;
    treatment_data =
            treatments_header
                    >> *(treatment_datum[insert(
                            phoenix::ref(optData->treatments_data), _1)]);

    tanks_header = lexeme[lit("[STORAGE_TANK_DATA]") >> *(space - eol)
                                                     >> eol];
    tank_id = white_seperated_string;
    //            tank_info1 = qi::double_ >> qi::double_ >> qi::double_
    //                  >> qi::attr(0) >> qi::attr(0) >> qi::attr(0) >> qi::attr(0)
    //                  >> qi::attr(0);
    //            tank_info2 = qi::double_ >> qi::double_ >> qi::double_
    //                  >> qi::double_ >> qi::double_ >> double_ >> qi::attr(0)
    //                  >> qi::attr(0);
    tank_info3 = qi::double_ // initial storage
            >> qi::double_ // surface area
            >> qi::double_ // height
            >> qi::double_ // peakAllotmentDemand
            >> qi::double_ // peak Supply
            >> qi::double_ // peak external supply per allotment
            >> qi::double_ // avg Supply
            >> qi::double_ // avg Allotment Demand
            >> qi::double_ // avg external supply per allotment
            >> qi::double_ // net External Supply per Allotment
            >> qi::double_ // cost of tank
            >> qi::double_ // embodied energy of tank
            >> qi::double_ // other costs (dist. coll. networks)
            >> qi::double_ // other EE (ditto)
            >> int_then_end_of_line; // Number of allotments that this tank services.

    tank_datum = tank_id >> tank_info3;
    //            tank_datum = tank_id >> (tank_info3 | tank_info2 | tank_info1);
    //            tank_datum = tank_id >> tank_info2;
    tank_data = tanks_header
            >> *(tank_datum[insert(phoenix::ref(optData->storage_tank_data), _1)]);

    //Pumping parser rules
    pumping_header = lexeme[lit("[PUMPING]") >> *(space - eol) >> eol];
    pump_id = white_seperated_string;
    pump_description = white_seperated_string;
    head = double_;
    efficiency = double_then_end_of_line;
    pump_info = pump_description >> head >> efficiency;
    pump_datum = pump_id >> pump_info;
    pump_data =
            pumping_header
                    >> *(pump_datum[push_back(
                            phoenix::ref(optData->pump_options_data), _1)]);

    //Pipe group rules
    pipe_group_header = lexeme[lit("[LINK_GROUPS]") >> *(space - eol)
                                                    >> eol];
    pipe_group_id = white_seperated_string;
    epanet_pipe_ids = list_white_seperated_string;
    pipe_group = pipe_group_id >> epanet_pipe_ids;
    pipe_groups = pipe_group_header
            >> *(pipe_group[insert(phoenix::ref(optData->link_groups_data), _1)]);

    //Pipe choices rules
    pipe_choices_header = lexeme[lit("[PIPE_CHOICES]") >> *(space - eol)
                                                       >> eol];
    pipe_data_ids = list_white_seperated_string;
    pipe_choice = pipe_group_id >> pipe_data_ids;
    pipe_choices =
            pipe_choices_header
                    >> *(pipe_choice[insert(
                            phoenix::ref(optData->pipe_availOpt_data), _1)]);

    //Node group rules
    node_group_header = lexeme[lit("[NODE_GROUPS]") >> *(space - eol)
                                                    >> eol];
    node_group_id = white_seperated_string;
    epanet_node_ids = list_white_seperated_string;
    node_group = node_group_id >> epanet_node_ids;
    node_groups = node_group_header
            >> *(node_group[insert(phoenix::ref(optData->node_groups_data), _1)]);

    // tank chocies
    tank_choices_header = lexeme[lit("[STORAGE_TANK_CHOICES]")
            >> *(space - eol) >> eol];
    potDemandPatternID_rule = white_seperated_string;
    potNodeGroup_rule = white_seperated_string;
    rcylDemandPatternID_rule = white_seperated_string;
    rcylNodeGroup_rule = white_seperated_string;
    tank_data_ids = lexeme[*(+((char_ - (space | '[' | ']')) - ']')
            >> omit[*(space - eol)] - ']')];
    tank_choice_info = potDemandPatternID_rule >> potNodeGroup_rule
                                               >> rcylDemandPatternID_rule >> rcylNodeGroup_rule
                                               >> lit("[")
                                               >> tank_data_ids
                                               >> lit("]");
    tank_choice_datum = white_seperated_string >> tank_choice_info;
    tank_choices = tank_choices_header
            >> *(tank_choice_datum[insert(
                    phoenix::ref(optData->tank_availOpt_data), _1)]);

    dist_groups_header = lexeme[lit("[DIST_NETWORKS]") >> *(space - eol) >> eol];
    dist_group_ID_rule = white_seperated_string;
    groups_in_sys_ids_rule = lexeme[*(+((char_ - (space | '[' | ']')) - ']')
            >> omit[*(space - eol)] - ']')];
    dist_groups_info = int_ >> lit("[") >> groups_in_sys_ids_rule >> lit("]");
    dist_groups_datum = dist_group_ID_rule >> dist_groups_info;
    dist_groups = dist_groups_header
            >> *(dist_groups_datum[insert(
                    phoenix::ref(optData->dist_groups_data), _1)]);


    penalties_header = lexeme[lit("[PENALTIES]") >> *(space - eol)
                                                 >> eol];
    pressureConstraintPenalty_rule = lit("PressureConstraintPenalty")
            >> double_[phoenix::ref(optData->PressurePenaltyCost) = _1]
            >> double_[phoenix::ref(optData->PressurePenaltyEE) = _1]
            >> double_then_end_of_line[phoenix::ref(optData->PressurePenaltyWtr) =
                                               _1];
    headConstraintPenalty_rule = lit("HeadConstraintPenalty")
            >> double_[phoenix::ref(optData->HeadPenaltyCost) = _1]
            >> double_[phoenix::ref(optData->HeadPenaltyEE) = _1]
            >> double_then_end_of_line[phoenix::ref(optData->HeadPenaltyWtr) = _1];
    velocityConstraintPenalty_rule = lit("VelocityConstraintPenalty")
            >> double_[phoenix::ref(optData->VelocityPenaltyCost) = _1]
            >> double_[phoenix::ref(optData->VelocityPenaltyEE) = _1]
            >> double_then_end_of_line[phoenix::ref(optData->VelocityPenaltyWtr) =
                                               _1];
    treatmentConstraintPenalty_rule = lit("TreatmentConstraintPenalty")
            >> double_[phoenix::ref(optData->TreatmentPenaltyCost) = _1]
            >> double_[phoenix::ref(optData->TreatmentPenaltyEE) = _1]
            >> double_then_end_of_line[phoenix::ref(optData->TreatmentPenaltyWtr) =
                                               _1];
    penalties = penalties_header
            >> *(pressureConstraintPenalty_rule
                 | velocityConstraintPenalty_rule
                 | treatmentConstraintPenalty_rule
                 | headConstraintPenalty_rule);

    // Variable speed pumping rules
    vsp_header = lexeme[lit("[VSP]") >> *(space - eol) >> eol];
    vsp_avg_rule = no_case[lit("Average")[_val = "Average"]];
    vsp_ts_rule = no_case[lit("Timestep")[_val = "Timestep"]];
    vsp_info1 = white_seperated_string // VSP description
            >> white_seperated_string // EPANET Reservoir ID
            >> white_seperated_string // EPANET Pressure reducing valve ID
            >> white_seperated_string // EPANET Link ID
            >> white_seperated_string // EPANET Intermediate node ID (connects reservoir outlet pipe to PRV)
            >> white_seperated_string // EPANET Outlet node ID (outlet from PRV)
            >> vsp_ts_rule
            >> int_ //Now we need to know how to calculate average pumping costs.... this could be based on average of other time steps, or based on a particular time step
            >> white_seperated_string; //THE NODE GROUP THAT THIS VSP INFLUENCES

    vsp_info2 = white_seperated_string // VSP description
            >> white_seperated_string // EPANET Reservoir ID
            >> white_seperated_string // EPANET Pressure reducing valve ID
            >> white_seperated_string // EPANET Link ID
            >> white_seperated_string // EPANET Intermediate node ID (connects reservoir outlet pipe to PRV)
            >> white_seperated_string // EPANET Outlet node ID (outlet from PRV)
            >> vsp_avg_rule >> qi::attr(
            int(-9)) //Now we need to know how to calculate average pumping costs.... this could be based on average of other time steps, or based on a particular time step
            >> white_seperated_string; //THE NODE GROUP THAT THIS VSP INFLUENCES

    vsp_datum1 = white_seperated_string >> vsp_info1; // First entity is the ID of the VSP
    vsp_datum2 = white_seperated_string >> vsp_info2; // First entity is the ID of the VSP

    vsp_data_rule = vsp_header
            >> *(vsp_datum1[insert(phoenix::ref(optData->vsp_data), _1)]
                 | vsp_datum2[insert(phoenix::ref(optData->vsp_data), _1)]);

    evaluation_header = lexeme[lit("[EVALUATION]") >> *(space - eol)
                                                   >> eol];
    trenchingCost_rule = lit("TrenchingCost")
            >> double_then_end_of_line[phoenix::ref(optData->TrenchingCost) = _1];
    fittingsCost_rule = lit("FittingsCost")
            >> double_then_end_of_line[phoenix::ref(optData->FittingsCost) = _1];
    fittingsEE_rule = lit("FittingsEE")
            >> double_then_end_of_line[phoenix::ref(optData->FittingsEE) = _1];
    greywaterSysCost_rule =
            lit("GreywaterSysCost")
                    >> double_then_end_of_line[phoenix::ref(
                            optData->GreywaterSysCost) = _1];
    houseConnectionsCost_rule =
            lit("HouseConnectionsCost")
                    >> double_then_end_of_line[phoenix::ref(
                            optData->HouseConnectionsCost) = _1];
    greywaterEE_rule = lit("GreywaterEE")
            >> double_then_end_of_line[phoenix::ref(optData->GreywaterEE) = _1];
    //en_dll_rule = lit("EpanetDylibLoc") >> lexeme[lit("\"") >> (char_ - lit("\"")) >> lit("\"") >> *(space - eol) >> eol];/*[phoenix::ref(epanet_dylib_loc) = _1]*/;
    //en_dll_rule = lit("EpanetDylibLoc") >> white_seperated_string;/*[phoenix::ref(epanet_dylib_loc) = _1]*/;

    en_lib_rule = lit("EpanetLib:") >> (lit("OWA_EPANET_Version2:")[phoenix::ref(optData->en_lib_version) = OWA_EN2]
                  | lit("OWA_EPANET_Version3:")[phoenix::ref(optData->en_lib_version) = OWA_EN3]
                  | lit("Ibanevs_EPANET:")[phoenix::ref(optData->en_lib_version) = Ibanev_EN2])
                          >> en_lib_path_rule[phoenix::ref(optData->epanet_dylib_loc) = _1];

    en_lib_path_rule = lexeme['\"' >> +(char_ - '\"') >> '\"' /*>> *(space - eol) >> eol*/]
                       - headers;
    iuwm_nodeext_rule =
            lit("IUWMod_NodeExt_Loc")
                    >> lexeme['\"' >> +(char_ - '\"') >> '\"' /*>> *(space - eol) >> eol*/]
                       - headers;
    iuwm_linkext_rule =
            lit("IUWMod_LinkExt_Loc")
                    >> lexeme['\"' >> +(char_ - '\"') >> '\"' /*>> *(space - eol) >> eol*/]
                       - headers;

    //en_dll_rule = lit("EpanetDylibLoc") >> lexeme[ +(char_ - eol) >> eol] - headers;
    trenchEE_rule = lit("TrenchEE")
            >> double_then_end_of_line[phoenix::ref(optData->trenchEE) = _1];
    discountEE_rule = lit("DiscoutEE")
            >> double_then_end_of_line[phoenix::ref(optData->drateEE) = _1];
    discountCost_rule = lit("DiscoutCost")
            >> double_then_end_of_line[phoenix::ref(optData->drateCost) = _1];
    netWaterDemand_rule = lit("NetWaterDemand")
            >> double_then_end_of_line[phoenix::ref(optData->netWaterDemand) = _1];
    greywaterContrib_rule =
            lit("GreywaterContrib")
                    >> double_then_end_of_line[phoenix::ref(
                            optData->greywaterContrib) = _1];
    numAllotments_rule = lit("numAllotments")
            >> int_then_end_of_line[phoenix::ref(optData->numAllotments) = _1];
    energyCost_rule = lit("EnergyCost")
            >> double_then_end_of_line[phoenix::ref(optData->energyCost) = _1];
    planPeriod_rule = lit("PlanPeriod")
            >> int_then_end_of_line[phoenix::ref(optData->planHorizon) = _1];

    peakPumpEfficiency_rule = lit("PeakPumpEfficiency")
            >> double_then_end_of_line[phoenix::ref(optData->peakPumpEfficiency) =
                                               _1];
    avgPumpEfficiency_rule = lit("AvgPumpEfficiency")
            >> double_then_end_of_line[phoenix::ref(optData->avgPumpEfficiency) =
                                               _1];
    pumpElectricsLife_rule = lit("PumpElectricsLife")
            >> int_then_end_of_line[phoenix::ref(optData->pumpElectricsLife) = _1];
    pumpMechanicsLife_rule = lit("PumpMechanicsLife")
            >> int_then_end_of_line[phoenix::ref(optData->pumpMechanicsLife) = _1];
    pumpCivilLife_rule = lit("PumpCivilLife")
            >> int_then_end_of_line[phoenix::ref(optData->pumpCivilLife) = _1];
    treatmentLife_rule = lit("TreatmentLife")
            >> int_then_end_of_line[phoenix::ref(optData->treatmentLife) = _1];

    dailyPumpOpHours_rule = lit("DailyPumpOperationHours")
            >> double_then_end_of_line[phoenix::ref(optData->dailyPumpOpHours) =
                                               _1];
    dayPeak_hourPeakRatio_rule = lit("DayHourPeakRatio")
            >> double_then_end_of_line[phoenix::ref(optData->dayPeak_hourPeakRatio) =
                                               _1];

    treatLogReduction_rule = lit("TreatmentLogReduction")
            >> double_then_end_of_line[phoenix::ref(
                    optData->treatementLogReduction) = _1];

    iuwm_topupNde_rule =
            lit("IUWMod_topup_name")
                    >> lexeme['\"' >> +(char_ - '\"') >> '\"' /*>> *(space - eol) >> eol*/]
                       - headers;

    en_pat_id_rule =
            lit("Epanet_pattern_id")
                    >> lexeme['\"' >> +(char_ - '\"') >> '\"' /*>> *(space - eol) >> eol*/]
                       - headers;

    extWaterEnergyRule = lit("ExternalWaterEnergyRequirement")
            >> double_then_end_of_line[phoenix::ref(optData->externalWaterEnergy) = _1];

    extTreatEnergyRule = lit("ExternalTreatmentEnergyRequirement")
            >> double_then_end_of_line[phoenix::ref(optData->externalTreatmentEnergy) = _1];

    extTreatCostRule = lit("ExternalTreatmentCostRequirement")
            >> double_then_end_of_line[phoenix::ref(optData->externalTreatmentCost) = _1];

    tarifPairRule = qi::lit("(") >> qi::int_ >> qi::lit(",") >> qi::double_ >> qi::lit(")");

    extWaterCostRule = qi::lit("waterTarifStructure")
            >> +(tarifPairRule[phoenix::insert(phoenix::ref(optData->waterTariffs), qi::_1)]);

    evaluation = evaluation_header >> *(trenchingCost_rule //
                                        | fittingsCost_rule //
                                        | fittingsEE_rule //
                                        | greywaterSysCost_rule //
                                        | houseConnectionsCost_rule | greywaterEE_rule //
                                        | en_lib_rule //
                                        | iuwm_nodeext_rule[phoenix::ref(optData->iuwmod_nodeext_loc) = _1] //
                                        | iuwm_linkext_rule[phoenix::ref(optData->iuwmod_linkext_loc) = _1] //
                                        | trenchEE_rule //
                                        | discountEE_rule //
                                        | discountCost_rule //
                                        | netWaterDemand_rule //
                                        | greywaterContrib_rule //
                                        | numAllotments_rule //
                                        | energyCost_rule //
                                        | planPeriod_rule //
                                        | peakPumpEfficiency_rule //
                                        | avgPumpEfficiency_rule //
                                        | pumpElectricsLife_rule //
                                        | pumpMechanicsLife_rule //
                                        | pumpCivilLife_rule //
                                        | treatmentLife_rule //
                                        | dailyPumpOpHours_rule //
                                        | treatLogReduction_rule //
                                        | dayPeak_hourPeakRatio_rule //
                                        | iuwm_topupNde_rule[phoenix::ref(optData->pot_topup_node_name) = _1] //
                                        | extTreatEnergyRule //
                                        | extTreatCostRule //
                                        | extWaterCostRule //
                                        | extWaterEnergyRule //
                                        | en_pat_id_rule[phoenix::ref(optData->en_pat_id) = _1]);

    pressures_header = lexeme[lit("[PRESSURES]") >> *(space - eol)
                                                 >> eol];
    node_id = white_seperated_string;
    min_pressure = double_;
    max_pressure = double_then_end_of_line;
    mm_pressure_constraint = min_pressure >> max_pressure;
    pressure_constraint = node_id >> mm_pressure_constraint;
    pressure_constraints_r = pressures_header
            >> *(pressure_constraint[insert(
                    phoenix::ref(optData->pressure_constraints), _1)]);

    heads_header = lexeme[lit("[HEADS]") >> *(space - eol) >> eol];
    min_head = double_;
    max_head = double_then_end_of_line;
    mm_head_constraint = min_head >> max_head;
    head_constraint = node_id >> mm_head_constraint;
    head_constraints_r = heads_header
            >> *(head_constraint[insert(phoenix::ref(optData->head_constraints),
                                        _1)]);

    velocities_header = lexeme[lit("[VELOCITIES]") >> *(space - eol)
                                                   >> eol];
    pipe_id = white_seperated_string;
    max_velocity = double_then_end_of_line;
    velocity_constraint = pipe_id >> max_velocity;
    velocity_constraints_r = velocities_header
            >> *(velocity_constraint[insert(
                    phoenix::ref(optData->velocity_constraints), _1)]);

    start = *(pipe_data //
              | pipe_groups //
              | pipe_choices //
              | penalties //
              | evaluation //
              | pressure_constraints_r //
              | velocity_constraints_r //
              | treatment_data //
              | pump_data //
              | specification_rule //
              | head_constraints_r //
              | vsp_data_rule //
              | tank_data //
              | tank_choices //
              | dist_groups //
              | node_groups);
}

std::ostream &
operator<<(std::ostream &os, const OptData &optdata) {

    os << "; OPTIMISER FILE GENERATED" << std::endl;
    os << "; THE AUTHOR DOES NOT WARRANTEE THE PROGRAM FOR ANY PURPOSE "
       << std::endl;
    os << "; (c) JEFFREY NEWMAN 2010 " << std::endl;
    os
            << "; INTELECTUAL PROPERTY (IP) BELONGS TO JEFF NEWMAN, PROF. HOLGER MAIER, PROF. GRAEME DANDY "
            << std::endl;
    os << "; AND THE UNIVERSITY OF ADELAIDE " << std::endl;
    os
            << "; DISTRIBUTION OF EXECUTABLES AND SOURCE CODE IS STRICTLY FORBIDDEN WITHOUT CONSENT FROM THE IP OWNERS "
            << std::endl << std::endl;
    os << std::endl << std::endl << std::endl;

    os << "[SPECIFICATION]" << std::endl;
    os << "Systems:" << "\t";
    if (optdata.isDistSystem) os << "Distribution" << "\t";
    if (optdata.isCollSystem) os << "Collection" << "\t";
    if (optdata.isWtrBalSystem) os << "WaterBalance" << "\t";
    os << std::endl;
    os << "Optimise:" << "\t";
    if (optdata.isOptPipes) os << "Pipes" << "\t";
    if (optdata.isOptVSP) os << "VSPStations" << "\t";
    if (optdata.isOptTanks) os << "Tanks" << "\t";
    os << std::endl;
    os << "Objectives:" << "\t";
    if (optdata.isCostObj) os << "Cost" << "\t";
    if (optdata.isEnergyObj) os << "Energy" << "\t";
    if (optdata.isWtrObj) os << "WaterSavings" << "\t";
    os << std::endl;
    os << "Constraints:" << "\t";
    if (optdata.isPressureConstraint) os << "Pressure" << "\t";
    if (optdata.isHeadConstraint) os << "Head" << "\t";
    if (optdata.isVelocityConstraint) os << "Velocity" << "\t";
    os << std::endl;
    if (!(optdata.epanetFile.empty()))
        os << "EpanetFile:" << "\t\""
           << optdata.epanetFile << "\"\t" << std::endl;
    if (!(optdata.iuwmodFile.empty()))
        os << "IUWModFile:" << "\t\""
           << optdata.iuwmodFile << "\"\t" << std::endl;
    os << std::endl << std::endl;

    os << "[PIPE_DATA]" << std::endl;
    os
            << ";-----------------------------------------------------------------------------------------------------------------------------"
            << std::endl;
    os
            << "; PipeOptionID  Desc  roughness    diam.    cost    repairCost    Emb.Energy    GHG    topCover    sideCover    bottomFill    "
            << std::endl;
    os
            << ";-----------------------------------------------------------------------------------------------------------------------------"
            << std::endl;
    for (PipeDataType::const_iterator pipe =
            optdata.pipe_data_map.begin();
         pipe != optdata.pipe_data_map.end(); ++pipe) {
        os << pipe->first << "\t" << pipe->second.description << "\t"
           << pipe->second.roughnessHeight << "\t"
           << pipe->second.diameter << "\t" << pipe->second.cost
           << "\t" << pipe->second.repairCost << "\t"
           << pipe->second.embodiedEnergy << "\t" << pipe->second.repairEE << "\t" << pipe->second.ghg
           << "\t" << pipe->second.topCover << "\t"
           << pipe->second.sideCover << "\t"
           << pipe->second.bottomCover << "\t" << std::endl;
    }
    os << std::endl;

    os << "[LINK_GROUPS]" << std::endl;
    os
            << ";-----------------------------------------------------------------------------------------------------------------------------"
            << std::endl;
    os << "; LinkGroupID EpanetPipeIDs" << std::endl;
    os
            << ";-----------------------------------------------------------------------------------------------------------------------------"
            << std::endl;
    for (LinkGroupsType::const_iterator lnGroup =
            optdata.link_groups_data.begin();
         lnGroup != optdata.link_groups_data.end(); ++lnGroup) {
        os << lnGroup->first << "\t";
        for (std::vector<std::string>::const_iterator ln =
                lnGroup->second.begin(); ln != lnGroup->second.end(); ++ln) {
            os << *ln << "\t";
        }
        os << std::endl;
    }
    os << std::endl;

    os << "[NODE_GROUPS]" << std::endl;
    os
            << ";-----------------------------------------------------------------------------------------------------------------------------"
            << std::endl;
    os << "; NodeGroupID EpanetNodeIDs" << std::endl;
    os
            << ";-----------------------------------------------------------------------------------------------------------------------------"
            << std::endl;
    for (NodeGroupsType::const_iterator ndGroup =
            optdata.node_groups_data.begin();
         ndGroup != optdata.node_groups_data.end(); ++ndGroup) {
        os << ndGroup->first << "\t";
        for (std::vector<std::string>::const_iterator nd =
                ndGroup->second.begin(); nd != ndGroup->second.end(); ++nd) {
            os << *nd << "\t";
        }
        os << std::endl;
    }
    os << std::endl;

    os << "[PIPE_CHOICES]" << std::endl;
    os
            << ";-----------------------------------------------------------------------------------------------------------------------------"
            << std::endl;
    os << "; LinkGroupID    PipeOptionID(List)...     " << std::endl;
    os
            << ";-----------------------------------------------------------------------------------------------------------------------------"
            << std::endl;
    for (PipeAvailOptionsType::const_iterator optn =
            optdata.pipe_availOpt_data.begin();
         optn != optdata.pipe_availOpt_data.end(); ++optn) {
        os << optn->first << "\t";
        for (std::vector<std::string>::const_iterator optnId =
                optn->second.begin(); optnId != optn->second.end();
             ++optnId) {
            os << *optnId << "\t";
        }
        os << std::endl;
    }
    os << std::endl;

    os << "[STORAGE_TANK_CHOICES]" << std::endl;
    os
            << ";-----------------------------------------------------------------------------------------------------------------------------"
            << std::endl;
    os
            << "; StorageID      PotableDemandPatternID     PotableNodeGroup      RecycledPatternID       RecycledNodeGroup      TankOptionID(List)...     "
            << std::endl;
    os
            << ";-----------------------------------------------------------------------------------------------------------------------------"
            << std::endl;
    for (TankAvailableOptionsType::const_iterator optn =
            optdata.tank_availOpt_data.begin();
         optn != optdata.tank_availOpt_data.end(); ++optn) {
        os << optn->first << "\t";
        os << optn->second.potDemandPatternID << "\t";
        os << optn->second.potNodeGroup << "\t";
        os << optn->second.rcylDemandPatternID << "\t";
        os << optn->second.rcylNodeGroup << "\t";
        for (std::vector<std::string>::const_iterator optnId =
                optn->second.tankOptionIDs.begin();
             optnId != optn->second.tankOptionIDs.end(); ++optnId) {
            os << *optnId << "\t";
        }
        os << std::endl;
    }
    os << std::endl;

    //Now write down the available pipe choices for each pipe group.
    os << "[TREATMENTS]" << std::endl;
    os
            << ";-----------------------------------------------------------------------------------------------------------------------------"
            << std::endl;
    os
            << "; Treat.ID    Description(single_word)    Capacity    cap_cost    ongoing_cost         "
            << std::endl;
    os
            << ";-----------------------------------------------------------------------------------------------------------------------------"
            << std::endl;
    for (TreatmentsType::const_iterator treat =
            optdata.treatments_data.begin();
         treat != optdata.treatments_data.end(); ++treat) {
        os << treat->first << "\t" << treat->second.description << "\t"
           << treat->second.waterload << "\t"
           << treat->second.capitalcost << "\t"
           << treat->second.annualOpCost << "\t" << std::endl;
    }
    os << std::endl;

    //Now write down the available pipe choices for each pipe group.
    os << "[STORAGE_TANK_DATA]" << std::endl;
    os
            << ";-----------------------------------------------------------------------------------------------------------------------------"
            << std::endl;
    os << "; Tank.ID    init_storage    surface_area    height "
       << std::endl;
    os
            << ";-----------------------------------------------------------------------------------------------------------------------------"
            << std::endl;
    for (TanksType::const_iterator tank =
            optdata.storage_tank_data.begin();
         tank != optdata.storage_tank_data.end(); ++tank) {
        os << tank->first << "\t" //
           << tank->second.init_storage << "\t" //
           << tank->second.surface_area << "\t" //
           << tank->second.height << "\t" //
           << tank->second.peakAllotmentDemand << "\t" //
           << tank->second.avgAllotmentDemand << "\t" //
           << tank->second.peakSupply << "\t" //
           << tank->second.avgSupply << "\t" //
           << tank->second.peakExternalSupplyPerAllot << "/t" //
           << tank->second.avgExternalSupplyPerAllot << "/t" //
           << tank->second.netExternalSupplyPerAllot << "\t" //
           << tank->second.cost << "\t" //
           << tank->second.embodied_energy << "\t" //
           << tank->second.numAllots << "\t" //
           << std::endl;
    }
    os << std::endl;

    os << "[DIST_NETWORKS]" << std::endl;
    os
            << ";-----------------------------------------------------------------------------------------------------------------------------"
            << std::endl;
    os << "; Dist.Sys.ID    numAllots    LinkGroupIDs "
       << std::endl;
    os
            << ";-----------------------------------------------------------------------------------------------------------------------------"
            << std::endl;
    typedef const std::pair<const std::string, DistSysPipeGroups> DistSysPair;
    BOOST_FOREACH(DistSysPair & sysData, optdata.dist_groups_data)
    {
        os << sysData.first << "\t" << sysData.second.numAllotments;
        BOOST_FOREACH(
        const std::string &grpID, sysData.second.pipeGroupIDs)
        {
            os << "\t" << grpID;
        }
        os << std::endl;

    }

    os << "[PUMPING]" << std::endl;
    os
            << ";-----------------------------------------------------------------------------------------------------------------------------"
            << std::endl;
    os
            << "; PumpID    Description(single_word)    Head    Efficiency    "
            << std::endl;
    os
            << ";-----------------------------------------------------------------------------------------------------------------------------"
            << std::endl;
    for (PumpingOptionsType::const_iterator pump =
            optdata.pump_options_data.begin();
         pump != optdata.pump_options_data.end(); ++pump) {
        os << pump->first << "\t" << pump->second.description << "\t"
           << pump->second.head << "\t" << pump->second.efficiency
           << "\t" << std::endl;
    }
    os << std::endl;

    os << "[VSP]" << std::endl;
    os
            << ";-----------------------------------------------------------------------------------------------------------------------------"
            << std::endl;
    os
            << "; VSPID    Description(single_word)  EN_Res_ID   EN_PRV_ID    EN_Link_ID    EN_IntNode_ID    EN_OutletNode_ID    T"
            << std::endl;
    os
            << ";-----------------------------------------------------------------------------------------------------------------------------"
            << std::endl;
    for (VSPType::const_iterator vsp = optdata.vsp_data.begin();
         vsp != optdata.vsp_data.end(); ++vsp) {
        os << vsp->first << "\t" << vsp->second.description << "\t"
           << vsp->second.resID << "\t" << vsp->second.prvID << "\t"
           << vsp->second.linkID << "\t" << vsp->second.interNode
           << "\t" << vsp->second.outletNode << "\t"
           << vsp->second.determineAvgBy << "\t";
        if (vsp->second.determineAvgBy == "Timestep") {
            os << vsp->second.timestep << "\t";
        }
        os << vsp->second.NodeGroupID << "\t" << std::endl;
    }
    os << std::endl;

    //Now write the pressure constraints - these apply at demand nodes.
    os << "[HEADS]" << std::endl;
    os
            << ";-----------------------------------------------------------------------------------------------------------------------------"
            << std::endl;
    os << "; NODE_ID        Min_Head    Max_Head" << std::endl;
    os
            << ";-----------------------------------------------------------------------------------------------------------------------------"
            << std::endl;
    for (NodeConstraintsT::const_iterator hconst =
            optdata.head_constraints.begin();
         hconst != optdata.head_constraints.end(); ++hconst) {
        os << hconst->first << "\t" << hconst->second.first << "\t"
           << hconst->second.second << "\t" << std::endl;
    }
    os << std::endl;

    //Now write the pressure constraints - these apply at demand nodes.
    os << "[PRESSURES]" << std::endl;
    os
            << ";-----------------------------------------------------------------------------------------------------------------------------"
            << std::endl;
    os << "; NODE_ID	Min_Pressure	Max_Pressure" << std::endl;
    os
            << ";-----------------------------------------------------------------------------------------------------------------------------"
            << std::endl;
    for (NodeConstraintsT::const_iterator pconst =
            optdata.pressure_constraints.begin();
         pconst != optdata.pressure_constraints.end(); ++pconst) {
        os << pconst->first << "\t" << pconst->second.first << "\t"
           << pconst->second.second << "\t" << std::endl;
    }
    os << std::endl;

    //Now write the velocity constraints - these apply at all links.
    os << "[VELOCITIES]" << std::endl;
    os
            << ";-----------------------------------------------------------------------------------------------------------------------------"
            << std::endl;
    os << "; PIPE_ID	Max_Velocity" << std::endl;
    os
            << ";-----------------------------------------------------------------------------------------------------------------------------"
            << std::endl;
    for (VelocityConstraintsT::const_iterator vconst =
            optdata.velocity_constraints.begin();
         vconst != optdata.velocity_constraints.end(); ++vconst) {
        os << vconst->first << "\t" << vconst->second << "\t"
           << std::endl;
    }
    os << std::endl;

    os << "[PENALTIES]" << std::endl;
    os << "PressureConstraintPenalty" << "\t"
       << optdata.PressurePenaltyCost << "\t"
       << optdata.PressurePenaltyEE << "\t"
       << optdata.PressurePenaltyWtr << "\t" << std::endl;
    os << "VelocityConstraintPenalty" << "\t"
       << optdata.VelocityPenaltyCost << "\t"
       << optdata.VelocityPenaltyEE << "\t"
       << optdata.VelocityPenaltyWtr << "\t" << std::endl;
    os << "TreatmentConstraintPenalty" << "\t"
       << optdata.TreatmentPenaltyCost << "\t"
       << optdata.TreatmentPenaltyEE << "\t"
       << optdata.TreatmentPenaltyWtr << "\t" << std::endl;
    os << "HeadConstraintPenalty" << "\t" << optdata.HeadPenaltyCost
       << "\t" << optdata.HeadPenaltyEE
       << "\t" << optdata.HeadPenaltyWtr << "\t" << std::endl;
    os << std::endl;

    // Now we add to the Evaluation section
    os << std::endl << std::endl << std::endl;
    os << "[EVALUATION]" << std::endl;
    if (optdata.TrenchingCost != -9)
        os << "TrenchingCost" << "\t"
           << optdata.TrenchingCost << "\t" << std::endl;
    if (optdata.FittingsCost != -9)
        os << "FittingsCost" << "\t"
           << optdata.FittingsCost << "\t" << std::endl;
    if (optdata.FittingsEE != -9)
        os << "FittingsEE" << "\t"
           << optdata.FittingsEE << "\t" << std::endl;
    if (optdata.GreywaterSysCost != -9)
        os << "GreywaterSysCost" << "\t"
           << optdata.GreywaterSysCost << "\t" << std::endl;
    if (optdata.HouseConnectionsCost != -9)
        os << "HouseConnectionsCost"
           << "\t" << optdata.HouseConnectionsCost << "\t" << std::endl;
    if (optdata.GreywaterEE != -9)
        os << "GreywaterEE" << "\t"
           << optdata.GreywaterEE << "\t" << std::endl;
    if (!optdata.epanet_dylib_loc.empty())
        os << "EpanetDylibLoc"
           << "\t\"" << optdata.epanet_dylib_loc << "\"\t" << std::endl;
    if (optdata.trenchEE != -9)
        os << "TrenchEE" << "\t"
           << optdata.trenchEE << "\t" << std::endl;
    if (optdata.drateEE != -9)
        os << "DiscoutEE" << "\t"
           << optdata.drateEE << "\t" << std::endl;
    if (optdata.drateCost != -9)
        os << "DiscoutCost" << "\t"
           << optdata.drateCost << "\t" << std::endl;
    if (optdata.netWaterDemand != -9)
        os << "NetWaterDemand" << "\t"
           << optdata.netWaterDemand << "\t" << std::endl;
    if (optdata.greywaterContrib != -9)
        os << "GreywaterContrib" << "\t"
           << optdata.greywaterContrib << "\t" << std::endl;
    if (optdata.numAllotments != -9)
        os << "numAllotments" << "\t"
           << optdata.numAllotments << "\t" << std::endl;
    if (optdata.energyCost != -9)
        os << "EnergyCost" << "\t"
           << optdata.energyCost << "\t" << std::endl;
    if (optdata.planHorizon != -9)
        os << "PlanPeriod" << "\t"
           << optdata.planHorizon << "\t" << std::endl;
    if (optdata.peakPumpEfficiency != -9)
        os << "PeakPumpEfficiency"
           << "\t" << optdata.peakPumpEfficiency << "\t" << std::endl;
    if (optdata.avgPumpEfficiency != -9)
        os << "AvgPumpEfficiency"
           << "\t" << optdata.avgPumpEfficiency << "\t" << std::endl;
    if (optdata.pumpElectricsLife != -9)
        os << "PumpElectricsLife"
           << "\t" << optdata.pumpElectricsLife << "\t" << std::endl;
    if (optdata.pumpMechanicsLife != -9)
        os << "PumpMechanicsLife"
           << "\t" << optdata.pumpMechanicsLife << "\t" << std::endl;
    if (optdata.pumpCivilLife != -9)
        os << "PumpCivilLife" << "\t"
           << optdata.pumpCivilLife << "\t" << std::endl;
    os << std::endl;

    os << std::endl << std::endl;

    return (os);

}


void
printOptToFile(const OptData &optdata, std::string fileName) {
    std::cout << fileName << "\n";
    std::string cpy = fileName;
    std::ofstream fout(cpy.c_str());
    if (fout.is_open()) {
        fout << optdata;
    }
    if (fout.is_open()) {
        fout.close();
    }
}


void
printOptToFile_CStr(const OptData &optdata, const char *fileName) {
    std::cout << fileName << "\n";
    std::ofstream fout(fileName);
    if (fout.is_open()) {
        fout << optdata;
    }
    if (fout.is_open()) {
        fout.close();
    }
}

OptData::OptData() :
        PressurePenaltyCost(-9), PressurePenaltyEE(-9), HeadPenaltyCost(-9),
        HeadPenaltyEE(-9), VelocityPenaltyCost(-9), VelocityPenaltyEE(-9),
        TreatmentPenaltyCost(-9), TreatmentPenaltyEE(-9), TrenchingCost(-9),
        FittingsCost(-9), FittingsEE(-9), GreywaterSysCost(-9),
        HouseConnectionsCost(-9), GreywaterEE(-9), trenchEE(-9), drateEE(-9),
        drateCost(-9), netWaterDemand(-9), greywaterContrib(-9), energyCost(-9),
        planHorizon(-9), numAllotments(-9), peakPumpEfficiency(-9),
        avgPumpEfficiency(-9), pumpElectricsLife(-9), pumpMechanicsLife(-9),
        pumpCivilLife(-9), isDistSystem(false), isCollSystem(false),
        isWtrBalSystem(false), isOptPipes(false), isOptVSP(false),
        isOptTanks(false), isCostObj(false), isEnergyObj(false), isWtrObj(false),
        isPressureConstraint(false), isHeadConstraint(false),
        isVelocityConstraint(false), dailyPumpOpHours(24.0) {
}

std::vector<std::string> *
OptData::getOrNewLinkGroup(const std::string &name) {
    return &(this->link_groups_data[name]);
}

std::vector<std::string> *
OptData::getOrNewPipeOptions(const std::string &groupName) {
    return &(this->pipe_availOpt_data[groupName]);
}

void
OptData::setOrNewPressureConstraint(const std::string &name,
                                    const double &minP, const double &maxP) {
    std::pair<double, double> &pConstraints =
            this->pressure_constraints[name];
    pConstraints.first = minP;
    pConstraints.second = maxP;
}

void
OptData::setOrNewHeadConstraint(const std::string &name,
                                const double &minH, const double &maxH) {
    std::pair<double, double> &hConstraints =
            this->head_constraints[name];
    hConstraints.first = minH;
    hConstraints.second = maxH;
}

void
OptData::setOrNewVelocityConstraint(const std::string &name,
                                    const double &maxV) {
    this->velocity_constraints[name] = maxV;
}




OptData_SPtr
getObjectiveInput(boost::filesystem::path optFile)
{
    //Make the parameter store and file parser.
    OptData_SPtr data(new OptData);

    OptParser_SPtr readParams(new OptFileParser(data));

    //Now we read the file.
    std::ifstream fin(optFile.string().c_str(), std::ios_base::in);
    if (fin.is_open())
    {
        //Start parsing the file - String to store the contents of the file
        std::string fileContents;
        //Copy contents of file to the string, make sure to include the white space
        fin.unsetf(std::ios::skipws);
        std::copy(std::istream_iterator<char>(fin),
                  std::istream_iterator<char>(),
                  std::back_inserter(fileContents));
        // The parser grammer requires a line ending after some things. Can cause problems
        // if one of these things is on the last line of the file. So we append another
        // new line just in case.
        fileContents.append("     \n");
        //Print the read-in contents to clog.

        //Now we parse the file. WE need (rewindable) iterators at the start and end of the string.
        Str_it it = fileContents.begin();
        Str_it end = fileContents.end();
        Skipper skipper;
        bool r = boost::spirit::qi::phrase_parse(it, end, *readParams, skipper);

        if (r && it == end)
        {
//			std::cout << "-------------------------\n";
//			std::cout << "Parsing succeeded\n";
//			std::cout << "-------------------------\n";
        }
        else
        {
            std::string rest(it, end);
            std::cout << "-------------------------\n";
            std::cout << "Parsing failed\n";
            std::cout << "stopped at: \"" << rest << "\"\n";
            std::cout << "-------------------------\n";
            throw ObjectiveInputError("Error while parsing file. Check format of optimisation specification file");
        }
        fin.close();
    }
    else
    {
        std::string errmsg = "Error while opening file. File with name " + optFile.string() + " does not exist?";
        throw ObjectiveInputError(errmsg.c_str());
    }

    return data;
};
