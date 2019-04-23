//==============================================================================//
//                                                                              //
//    RDB Diplomaterv Monitor                                                   //
//    A monitor program for the RDB Diplomaterv project                         //
//    Copyright (C) 2018  András Gergő Kocsis                                   //
//                                                                              //
//    This program is free software: you can redistribute it and/or modify      //
//    it under the terms of the GNU General Public License as published by      //
//    the Free Software Foundation, either version 3 of the License, or         //
//    (at your option) any later version.                                       //
//                                                                              //
//    This program is distributed in the hope that it will be useful,           //
//    but WITHOUT ANY WARRANTY; without even the implied warranty of            //
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             //
//    GNU General Public License for more details.                              //
//                                                                              //
//    You should have received a copy of the GNU General Public License         //
//    along with this program.  If not, see <https://www.gnu.org/licenses/>.    //
//                                                                              //
//==============================================================================//



#include "measurement_data_protocol.hpp"



MeasurementDataProtocol::MeasurementDataProtocol() : DataProcessingInterface("Measurement Data Protocol MDP", ".mdp")
{
    processing_state = ProcessingStates::WaitingForStartLine;
}

std::string MeasurementDataProtocol::GetProtocolName(void)
{
    return protocol_name;
}

std::vector<std::shared_ptr<DiagramSpecialized> > MeasurementDataProtocol::ProcessData(const std::string& data_source, std::istream& input_data)
{
    std::vector<std::shared_ptr<DiagramSpecialized> > assembled_diagrams;
    std::string received_data;
    std::string actual_line;

    while(std::getline(input_data, actual_line))
    {
        std::smatch match_results;

        // Removing the whitespaces from the actual line
        actual_line.erase(std::remove_if(actual_line.begin(), actual_line.end(), isspace), actual_line.end());

        try
        {
            switch(processing_state)
            {
                case ProcessingStates::WaitingForStartLine:
                    // If a start line was found...
                    if(std::regex_match(actual_line, std::regex(regex_start_line)))
                    {
                        processing_state = ProcessingStates::ProcessingHeadline;
                    }
                    break;
                case ProcessingStates::ProcessingHeadline:
                    // If this is a headline but and a dataline
                    // (this is needed because with regex it is difficult to define the differences between the data and headlines)
                    if((std::regex_match(actual_line, std::regex(regex_headline))) &&
                       (!std::regex_match(actual_line, std::regex(regex_data_line))))
                    {
                        std::string headline = actual_line;
                        DataIndexType column_index = 0;

                        // Collecting the labels from the headline
                        while(std::regex_search(headline, match_results, std::regex(regex_headline_analyzer)))
                        {
                            if(0 == column_index)
                            {
                                auto current_date_and_time = std::time(nullptr);
                                actual_diagram = std::make_shared<DiagramSpecialized>(data_source + " - " + std::string(ctime(&current_date_and_time)) , match_results[1]);
                            }
                            else
                            {
                                actual_diagram->AddNewDataLine(match_results[1]);
                            }

                            ++column_index;
                            headline = match_results.suffix().str();
                        }

                        // A headline was found...
                        //actual_diagram = make_shared<DiagramSpecialized>(match_results.at());
                        processing_state = ProcessingStates::ProcessingDataLines;
                    }
                    else
                    {
                        processing_state = ProcessingStates::WaitingForStartLine;
                    }
                    break;
                case ProcessingStates::ProcessingDataLines:
                    if(std::regex_match(actual_line, std::regex(regex_data_line)))
                    {
                        std::string data_line = actual_line;
                        DataIndexType column_index = 0;
                        DataPointType data_point_x_value = 0;

                        // Collecting the labels from the headline
                        while(std::regex_search(data_line, match_results, std::regex(regex_data_line_analyzer)))
                        {
                            if(0 == column_index)
                            {
                                std::stringstream stringstream(match_results[1]);
                                stringstream >> data_point_x_value;
                            }
                            else
                            {
                                if((column_index - 1) < actual_diagram->GetTheNumberOfDataLines())
                                {
                                    std::stringstream stringstream(match_results[1]);
                                    DataPointType data_point_y_value;
                                    stringstream >> data_point_y_value;
                                    actual_diagram->AddNewDataPoint((column_index - 1), DataPointSpecialized(data_point_x_value, data_point_y_value));
                                }
                                else
                                {
                                    processing_state = ProcessingStates::WaitingForStartLine;
                                    break;
                                }
                            }

                            ++column_index;
                            data_line = match_results.suffix().str();
                        }
                        if((column_index - 1) != actual_diagram->GetTheNumberOfDataLines())
                        {
                            processing_state = ProcessingStates::WaitingForStartLine;
                        }
                    }
                    else
                    {
                        if(std::regex_match(actual_line, std::regex(regex_end_line)))
                        {
                            assembled_diagrams.push_back(actual_diagram);
                        }
                        processing_state = ProcessingStates::WaitingForStartLine;
                    }
                    break;
                default:
                    processing_state = ProcessingStates::WaitingForStartLine;
                    throw("The DataProcessor::ProcessData's statemachine switched to an undefined state: " + std::to_string(static_cast<std::underlying_type<ProcessingStates>::type>(processing_state)));
                    break;
            }
        }
        catch(const std::regex_error& exception)
        {
            throw("A regex exception was caught: " + std::to_string(exception.code()) + ": " + exception.what());
        }
    }

    return assembled_diagrams;
}

bool MeasurementDataProtocol::CanThisFileBeProcessed(const std::string path_to_file)
{
    bool bResult = false;

    std::string file_extension = std::filesystem::path(path_to_file).extension();

    if(native_file_extension == file_extension)
    {
        bResult = true;
    }
    return bResult;
}
