/**
 * @file vle/extension/FSA.cpp
 * @author The VLE Development Team
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment (http://vle.univ-littoral.fr)
 * Copyright (C) 2003 - 2008 The VLE Development Team
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


#include <vle/extension/FSA.hpp>
#include <vle/value/Value.hpp>

namespace vle { namespace extension {

void Base::initialState(int state)
{
    vle::Assert <vle::utils::InternalError>(
        existState(state), boost::format(
            "FSA::Base model, unknow state %1%") % state);

    mInitialState = state;
    mInit = true;
}

vle::devs::ExternalEvent* Base::cloneExternalEvent(
    vle::devs::ExternalEvent* event) const
{
    vle::devs::ExternalEvent* ee = new vle::devs::ExternalEvent(
        event->getPortName());
    vle::value::Map::const_iterator it = event->getAttributes().begin();

    while (it != event->getAttributes().end()) {
        ee->putAttribute(it->first, it->second->clone());
        ++it;
    }
    return ee;
}

/*  - - - - - - - - - - - - - --ooOoo-- - - - - - - - - - - -  */

Event_t event(const std::string& event)
{
    return Event_t(event);
}

Output_t output(const std::string& output)
{
    return Output_t(output);
}

}} // namespace vle extension