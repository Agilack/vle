/**
 * @file value/String.cpp
 * @author The VLE Development Team.
 * @brief A string Value.
 */

/*
 * Copyright (C) 2003-2007 - The vle Development Team
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#include <vle/value/String.hpp>



namespace vle { namespace value {

String StringFactory::create(const std::string& value)
{
    return String(new StringFactory(value));
}

Value StringFactory::clone() const
{
    return Value(new StringFactory(m_value));
}

std::string StringFactory::toXML() const
{
    std::string val("<string>");
    val += m_value;
    val += "</string>";
    return val;
}

String toStringValue(const Value& value)
{
    Assert(utils::InternalError, value->getType() == ValueBase::STRING,
           "Value is not a String");
    return boost::static_pointer_cast < StringFactory >(value);
}

const std::string& toString(const Value& value)
{
    Assert(utils::InternalError, value->getType() == ValueBase::STRING,
           "Value is not a String");
    return boost::static_pointer_cast < StringFactory >(value)->stringValue();
}

}} // namespace vle value
