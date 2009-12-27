/**
 * @file examples/dess/I2.hpp
 * @author The VLE Development Team
 * See the AUTHORS or Authors.txt file
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment
 * http://www.vle-project.org
 *
 * Copyright (C) 2007-2009 INRA http://www.inra.fr
 * Copyright (C) 2003-2009 ULCO http://www.univ-littoral.fr
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


#ifndef VLE_EXAMPLES_I2_HPP
#define VLE_EXAMPLES_I2_HPP

#include <vle/extension/differential-equation/DESS.hpp>

namespace vle { namespace examples { namespace dess {

class I2 : public extension::DifferentialEquation::DESS
{
public:
    I2(const devs::DynamicsInit& model,
       const devs::InitEventList& events);
    virtual ~I2()
        { }

    virtual double compute(const devs::Time& time) const;

private:
    double a;
    double m;
    double g;

    Var I;
    Ext E;
};

}}} // namespace vle examples dess

#endif
