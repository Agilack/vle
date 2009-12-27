/**
 * @file vle/vpz/Project.hpp
 * @author The VLE Development Team
 * See the AUTHORS or Authors.txt file
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment
 * http://www.vle-project.org
 *
 * Copyright (C) 2003-2007 Gauthier Quesnel quesnel@users.sourceforge.net
 * Copyright (C) 2007-2009 INRA http://www.inra.fr
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


#ifndef VLE_VPZ_PROJECT_HPP
#define VLE_VPZ_PROJECT_HPP

#include <vle/vpz/Base.hpp>
#include <vle/vpz/DllDefines.hpp>
#include <vle/vpz/Model.hpp>
#include <vle/vpz/Dynamics.hpp>
#include <vle/vpz/Experiment.hpp>
#include <vle/vpz/Classes.hpp>

namespace vle { namespace vpz {

    /**
     * @brief The Vpz Project stores all information of the VPZ files, authors,
     * date, version and the hierachy of models, the list of dynamics, the
     * experiment conditions and observables, and the classes availables.
     */
    class VLE_VPZ_EXPORT Project : public Base
    {
    public:
        /**
         * @brief Build an empty project without information.
         */
        Project();

        virtual ~Project() {}

        /**
         * @brief Set a new name for the experiment and a new date.
         *
         * @param name The name of the experiment.
         * @param date The new date.
         */
        void setProject(const std::string& name, const std::string& date);

        /**
         * @brief Write all informations of the VPZ files.
         *
         * @param out Write in XML into an std::ostream.
         */
        virtual void write(std::ostream& out) const;

        /**
         * @brief Return the type of the project Vpz tag.
         *
         * @return The constant PROJECT.
         */
        virtual Base::type getType() const
        { return PROJECT; }

        /**
         * @brief Clear all information from the current project, ie: date,
         * hierarchies of models, list of dynamics, experiments and classes.
         */
        void clear();

        /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
          * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
         * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

        /**
         * @brief Get a constant reference to the vpz::Model.
         *
         * @return Get a constant reference to the vpz::Model.
         */
        const Model& model() const { return m_model; }

        /**
         * @brief Get a reference to the vpz::Model.
         *
         * @return Get a reference to the vpz::Model.
         */
        Model& model() { return m_model; }

        /**
         * @brief Get a constant reference to the vpz::Dynamics.
         *
         * @return Get a constant reference to the vpz::Dyanmics.
         */
        const Dynamics& dynamics() const { return m_dynamics; }

        /**
         * @brief Get a reference to the vpz::Dynamics.
         *
         * @return Get a reference to the vpz::Dynamics.
         */
        Dynamics& dynamics() { return m_dynamics; }

        /**
         * @brief Get a reference to the vpz::Experiment.
         *
         * @return Get a reference to the vpz::Experiment.
         */
        const Experiment& experiment() const { return m_experiment; }

        /**
         * @brief Get a reference to the vpz::Experiment.
         *
         * @return Get a reference to the vpz::Experiment.
         */
        Experiment& experiment() { return m_experiment; }

        /**
         * @brief Get a reference to the vpz::Classes.
         *
         * @return Get a reference to the vpz::Classes.
         */
        const Classes& classes() const { return m_classes; }

        /**
         * @brief Get a reference to the vpz::Classes.
         *
         * @return Get a reference to the vpz::Classes.
         */
        Classes& classes() { return m_classes; }

        /**
         * @brief Set a new name for the author.
         *
         * @param name The new name of the author.
         *
         * @throw utils::ArgError if the name is empty.
         */
        void setAuthor(const std::string& name);

        /**
         * @brief Get a constant reference of the author.
         *
         * @return A constant reference of the author.
         */
        inline const std::string& author() const
        { return m_author; }

        /**
         * @brief Set a new date for the project.
         *
         * @param date The new date to assign. If date is empty then, this
         * function assign the current date.
         */
        void setDate(const std::string& date = std::string());

        /**
         * @brief Get the version of this vpz project.
         *
         * @return The version of this vpz project.
         */
        inline const std::string &version() const
        { return m_version; }

        /**
         * @brief Get the number of the instance aissgned to this project.
         *
         * @return The number of the instance.
         */
        inline int instance() const
        { return m_instance; }

        /**
         * @brief Assign a new number for instance.
         *
         * @param instance
         */
        inline void setInstance(int instance)
        { m_instance = instance; }

        /**
         * @brief Assig a new number of replica.
         *
         * @param replica
         */
        inline void setReplica(int replica)
        { m_replica = replica; }

        /**
         * @brief Get the number of the replicas assigned to this project.
         * @return the replica.
         */
        inline int replica() const
        { return m_replica; }

        /**
         * @brief Assign a new version to this project.
         * @param version the name of the version.
         */
        inline void setVersion(const std::string& version)
        { if (not version.empty()) m_version.assign(version); }

        /**
         * @brief Get a constant reference to the date.
         * @return A constant reference to the date.
         */
        inline const std::string& date() const
        { return m_date; }

    private:
        std::string     m_author;
        std::string     m_date;
        std::string     m_version;
        int             m_instance;
        int             m_replica;

        Model           m_model;
        Dynamics        m_dynamics;
        Experiment      m_experiment;
        Classes         m_classes;
    };

}} // namespace vle vpz

#endif
