//==============================================================================//
//                                                                              //
//    RDB Monitor                                                               //
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



#include <memory>

#include "i_protocol.hpp"
#include "measurement_data_protocol.hpp"
#include "continous_measurement_data_protocol.hpp"



#ifndef PROTOCOL_FACTORY_HPP
#define PROTOCOL_FACTORY_HPP



class ProtocolFactory
{
public:
    static std::shared_ptr<I_Protocol> make(const QString& protocol_name)
    {
        std::shared_ptr<I_Protocol> protocol;

        if(protocol_name == QString(measurement_data_protocol_name))
        {
            protocol = std::make_shared<MeasurementDataProtocol>();
        }
        else if(protocol_name == QString(continous_measurement_data_protocol_name))
        {
            protocol = std::make_shared<ContinousMeasurementDataProtocol>();
        }
        else
        {
            throw ("ProtocolFactory::make(): Unknown protocol \"" + protocol_name.toStdString() + "\"");
        }

        return protocol;
    }
};


#endif // PROTOCOL_FACTORY_HPP
