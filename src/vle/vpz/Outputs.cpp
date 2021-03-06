/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2014 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2014 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2014 INRA http://www.inra.fr
 *
 * See the AUTHORS or Authors.txt file for copyright owners and
 * contributors
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


#include <vle/vpz/Outputs.hpp>
#include <vle/utils/Algo.hpp>
#include <vle/utils/Exception.hpp>
#include <iterator>

namespace vle { namespace vpz {

void Outputs::write(std::ostream& out) const
{
    if (not m_list.empty()) {
        out << "<outputs>\n";
        std::transform(begin(), end(),
                       std::ostream_iterator < Output >(out, "\n"),
                       utils::select2nd < OutputList::value_type >());
        out << "</outputs>\n";
    }
}

Output& Outputs::addLocalStream(const std::string& name,
                                const std::string& location,
                                const std::string& plugin,
                                const std::string& package)
{
    Output o;
    o.setName(name);
    o.setLocalStream(location, plugin, package);
    return add(o);
}

Output& Outputs::addLocalStream(const std::string& name,
                                const std::string& location,
                                const std::string& plugin)
{
    Output o;
    o.setName(name);
    o.setLocalStream(location, plugin);
    return add(o);
}

Output& Outputs::addDistantStream(const std::string& name,
                                  const std::string& location,
                                  const std::string& plugin,
                                  const std::string& package)
{
    Output o;
    o.setName(name);
    o.setDistantStream(location, plugin, package);
    return add(o);
}

Output& Outputs::addDistantStream(const std::string& name,
                                  const std::string& location,
                                  const std::string& plugin)
{
    Output o;
    o.setName(name);
    o.setDistantStream(location, plugin);
    return add(o);
}

void Outputs::del(const std::string& name)
{
    iterator it = m_list.find(name);

    if (it != end()) {
        m_list.erase(it);
    }
}

void Outputs::add(const Outputs& o)
{
    std::for_each(o.begin(), o.end(), AddOutput(*this));
}

Output& Outputs::add(const Output& o)
{
    std::pair < iterator, bool > x;

    x = m_list.insert(value_type( o.name(), o));

    if (not x.second) {
        throw utils::ArgError(fmt(
                _("An output have already this name '%1%'\n")) % o.name());
    }

    return x.first->second;
}

Output& Outputs::get(const std::string& name)
{
    iterator it = m_list.find(name);

    if (it == end()) {
        throw utils::ArgError(fmt(_("Unknow output '%1%'\n")) % name);
    }

    return it->second;
}

const Output& Outputs::get(const std::string& name) const
{
    const_iterator it = m_list.find(name);

    if (it == end()) {
        throw utils::ArgError(fmt(_("Unknow output '%1%'\n")) % name);
    }

    return it->second;
}

void Outputs::rename(const std::string& oldoutputname,
		     const std::string& newoutputname)
{
    Output copy = get(oldoutputname);
    del(oldoutputname);

    switch(copy.format()) {
    case Output::LOCAL:
        addLocalStream(newoutputname, copy.location(), copy.plugin(),
                       copy.package());
	break;
    case Output::DISTANT:
        addDistantStream(newoutputname, copy.location(), copy.plugin(),
                         copy.package());
        break;
    }
}

std::set < std::string > Outputs::depends() const
{
    std::set < std::string > result;

    for (const_iterator it = begin(); it != end(); ++it) {
        if (not it->second.package().empty()) {
            result.insert(it->second.package());
        }
    }

    return result;
}

std::vector < std::string > Outputs::outputsname() const
{
    std::vector < std::string > result(m_list.size());

    std::transform(begin(), end(), result.begin(),
                   utils::select1st < OutputList::value_type >());

    return result;
}

}} // namespace vle vpz
