/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2015 INRA http://www.inra.fr
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

#include <vle/manager/Simulation.hpp>
#include <vle/utils/Tools.hpp>
#include <vle/utils/Path.hpp>
#include <vle/utils/Package.hpp>
#include <vle/utils/i18n.hpp>
#include <vle/version.hpp>
#include <vle/vle.hpp>

#include <boost/algorithm/string.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/program_options.hpp>
#include <boost/mpi/environment.hpp>
#include <boost/mpi/communicator.hpp>
#include <boost/mpi/collectives.hpp>

#include <locale>
#include <limits>
#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>

const int default_block_size_value = 5000;

/**
 * @brief Convert a column name into couple condition, port.
 * @details Split the column name 'condition.name' into 'condition',
 *     'portname'.
 * @param str The column to split.
 * @return Two strings.
 */
static
std::pair <std::string, std::string> split_column_name(const std::string &str)
{
    std::string::size_type dot_position = str.find('.');

    if (dot_position == std::string::npos ||
        dot_position == 0 ||
        dot_position == str.size())
        return std::pair <std::string, std::string>(
                   str, std::string());

    return std::pair <std::string, std::string>(
               str.substr(0, dot_position),
               str.substr(dot_position + 1, std::string::npos));
}

/**
 * @brief Remove quotes from a string.
 * @details Remove quote in start and end position in the string.
 * @example
 * assert(cleanup_token("\"toto\"") == std::string("toto"));
 * assert(cleanup_token("ok") == std::string("ok"));
 * assert(cleanup_token("") == std::string());
 * assert(cleanup_token("\"xxxx") == std::string("\"xxxx"));
 * @endexemple
 *
 * @param str [description]
 * @return [description]
 */
static
std::string cleanup_token(const std::string &str)
{
    if (str.size() >= 2u and str[0] == '"' and str[str.size() - 1] == '"')
        return str.substr(1, str.size() - 2u);

    return str;
}

/**
 * @brief Convert and assign a string to a @e vle::value::Value.
 * @details Convert the string @e str and convert it into the @e
 *     vle::value::Value. Conversion only works for boolean, integer, double
 *     and string. Otherwise, an std::runtime_error is thrown.
 *
 * @param value The value to assign.
 * @param str The string to convert.
 *
 * @return [description]
 */
static
void assign_string_to_value(vle::value::Value *value, const std::string &str)
{
    switch (value->getType()) {
    case vle::value::Value::BOOLEAN:
        value->toBoolean().value() = vle::utils::to <bool>(str);
        break;

    case vle::value::Value::INTEGER:
        value->toInteger().value() = vle::utils::to <vle::int32_t>(str);
        break;

    case vle::value::Value::DOUBLE:
        value->toDouble().value() = vle::utils::to <double>(str);
        break;

    case vle::value::Value::STRING:
        value->toString().value() = str;
        break;

    default:
        throw std::runtime_error((vle::fmt("fail to assign string token "
                                           "`%1%'' to value") % str).str());
    }
}

struct ColumnDefinition {
    ColumnDefinition(const std::string &str)
        : str(str)
        , value(NULL)
    {}

    ColumnDefinition(vle::value::Value *value)
        : str(std::string())
        , value(value)
    {}

    std::string str;
    vle::value::Value *value;
};

struct Columns {
    typedef std::vector <ColumnDefinition> container_type;
    typedef container_type::iterator iterator;
    typedef container_type::const_iterator const_iterator;

    void add(const std::string &str)
    {
        data.push_back(ColumnDefinition(str));
    }

    void add(vle::value::Value *value)
    {
        data.push_back(ColumnDefinition(value));
    }

    std::size_t size() const
    {
        return data.size();
    }

    void update(std::size_t id, const std::string &str)
    {
        if (id >= data.size())
            throw std::runtime_error("Too many column data");

        if (data[id].value == NULL)
            data[id].str = str;
        else
            assign_string_to_value(data[id].value, str);
    }

    container_type data;
};

std::ostream &operator<<(std::ostream &os, const vle::value::Matrix *value)
{
    if  (value->rows() <= 0 or value->columns() <= 0)
        return os;

    for (std::size_t i = 0; i < value->columns(); ++i) {
        if (value->get(i, value->rows() - 1)) {
            switch (value->get(i, value->rows() - 1)->getType()) {
            case vle::value::Value::BOOLEAN:
                os << (value->get(i,
                                  value->rows() - 1)->toBoolean().value() == true ? "true" : "false");
                break;

            case vle::value::Value::INTEGER:
                os << value->get(i, value->rows() - 1)->toInteger().value();
                break;

            case vle::value::Value::DOUBLE:
                os << value->get(i, value->rows() - 1)->toDouble().value();
                break;

            case vle::value::Value::STRING:
                os << '"' << value->get(i, value->rows() - 1)->toString() << '"';
                break;

            default:
                throw std::runtime_error("fail to output a value (only bool, "
                                         "integer, double and string are "
                                         "available");
            }
        }

        if (i != value->columns() - 1)
            os << ',';
    }

    return os;
}

std::ostream &operator<<(std::ostream &os, const Columns &columns)
{
    for (std::size_t i = 0, e = columns.size(); i != e; ++i)
        if (columns.data[i].value == NULL)
            os << columns.data[i].str << ',';

    return os;
}

class Worker
{
private:
    typedef boost::scoped_ptr <vle::value::Map> MapPtr;
    typedef boost::shared_ptr <vle::vpz::Vpz> VpzPtr;

    std::string m_packagename;
    std::string m_vpzfilename;
    vle::manager::Simulation m_simulator;
    vle::utils::ModuleManager m_modules;
    VpzPtr m_vpz;
    Columns m_columns;

    void simulate(std::ostream &os)
    {
        vle::manager::Error error;
        MapPtr result(m_simulator.run(new vle::vpz::Vpz(*m_vpz.get()),
                                      m_modules, &error));

        if (error.code) {
            std::cerr << vle::fmt("Simulation failed. %1% [code: %2%] in %3%")
                      %
                      error.message % error.code % m_columns << '\n';
        } else if (result == NULL) {
            std::cerr << vle::fmt("Simulation without result (try storage as"
                                  " output plug-in) in %1%") %
                      m_columns << '\n';
        } else {
            for (vle::value::Map::const_iterator it = result->begin(),
                 et = result->end(); it != et; ++it) {
                if (it->second && it->second->isMatrix()) {
                    os << vle::value::toMatrixValue(it->second);
                }
            }
        }
    }

public:
    Worker(const std::string &package, const std::string &vpz)
        : m_packagename(package)
        , m_simulator(vle::manager::LOG_NONE, vle::manager::SIMULATION_NONE, NULL)
    {
        vle::utils::Package pack;
        pack.select(m_packagename);
        m_vpz = VpzPtr(
                    new vle::vpz::Vpz(
                        pack.getExpFile(vpz, vle::utils::PKG_BINARY)));
    }

    void init(const std::string &header)
    {
        using namespace boost::algorithm;
        std::vector <std::string> tokens;
        boost::algorithm::split(tokens, header,
                                boost::algorithm::is_any_of(","));

        for (std::size_t i = 0, e = tokens.size(); i != e; ++i) {
            std::string current = cleanup_token(tokens[i]);
            std::pair <std::string, std::string> names =
                split_column_name(current);

            if (names.second.empty()) {
                m_columns.add(names.second);
            } else {
                vle::vpz::Condition &cnd(
                    m_vpz->project().experiment().conditions().get(names.first));
                vle::value::Set &set(cnd.getSetValues(names.second));

                if (set.size() != 1u or set.get(0) == NULL)
                    throw std::runtime_error((vle::fmt("can not determine type"
                                                       " of condition %1% port"
                                                       " %2%")
                                              % names.first % names.second).str());

                m_columns.add(set.get(0));
            }
        }
    }

    std::string run(const std::string &block)
    {
        std::ostringstream result;
        std::vector <std::string> output;
        std::string::size_type begin = 0u;
        std::string::size_type end = block.find('\n');
        result.imbue(std::locale::classic());
        result << std::setprecision(
                   std::floor(
                       std::numeric_limits<double>::digits * std::log10(2) + 2))
               << std::scientific;

        for (begin = 0, end = block.find('\n');
             begin < block.size();
             begin = end + 1, end = block.find('\n', end + 1)) {
            std::string buffer(block, begin, end - begin);
            boost::algorithm::split(output, buffer,
                                    boost::algorithm::is_any_of(","));

            for (std::size_t i = 0, e = output.size(); i != e; ++i) {
                std::string current = cleanup_token(output[i]);
                m_columns.update(i, current);
            }

            result << m_columns;
            simulate(result);
            result << '\n';
        }

        return result.str();
    }
};

template <typename T>
struct no_deleter {
    void operator()(T *)
    {
    }
};

template <typename T>
boost::shared_ptr <T> open(const std::string &file)
{
    boost::shared_ptr <T> fs(new T(file.c_str()));

    if (!fs->is_open()) {
        throw std::invalid_argument((vle::fmt("fail to open file %1%") % file).str());
    }

    return fs;
}

class Root
{
    boost::shared_ptr <std::istream> m_is;
    boost::shared_ptr <std::ostream> m_os;
    std::ofstream m_ofs;
    int m_blocksize;

public:
    Root(const std::string &input, const std::string &output, int blocksize)
        : m_is(&std::cin, no_deleter <std::istream>())
        , m_os(&std::cout, no_deleter <std::ostream>())
        , m_blocksize(blocksize)
    {
        if (!input.empty())
            m_is = open <std::ifstream>(input);

        if (!output.empty())
            m_os = open <std::ofstream>(output);
    }

    bool header(std::string &header)
    {
        std::getline(*m_is.get(), header);
        return m_is.get()->good();
    }

    bool read(std::string &block)
    {
        int i = 0;
        block.clear();

        while (m_is->good() && i < m_blocksize) {
            std::string tmp;
            std::getline(*m_is.get(), tmp);

            if (!m_is->good())
                return false;

            block += tmp;
            block += '\n';
            ++i;
        }

        return true;
    }

    void write(const std::string &block)
    {
        (*m_os.get()) << block;
        m_os->flush();
    }
};

enum CommunicationTag {
    worker_block_todo_tag,
    worker_block_end_tag,
    worker_end_tag
};

int run_as_master(const std::string &inputfile,
                  const std::string &outputfile,
                  int blocksize)
{
    int ret = EXIT_SUCCESS;
    int blockid = 0;

    try {
        vle::Init app;
        Root r(inputfile, outputfile, blocksize);
        boost::mpi::communicator comm;
        std::vector <bool> workers(comm.size(), false);
        std::string block, header;
        r.header(header);
        boost::mpi::broadcast(comm, header, 0);

        for (int child = 1; child < comm.size(); ++child) {
            if (r.read(block)) {
                std::cout << vle::fmt("master sends block %1% to %2%\n") % blockid++ % child;
                comm.send(child, worker_block_todo_tag, block);
                workers[child] = true;
            } else
                break;
        }

        bool end = false;

        while (end == false) {
            boost::mpi::status msg = comm.probe();
            comm.recv(msg.source(), worker_block_end_tag, block);
            workers[msg.source()] = false;
            r.write(block);
            end = !r.read(block);

            if (!block.empty()) {
                std::cout << vle::fmt("master sends block %1% to %2%\n") % blockid++ %
                          msg.source();
                comm.send(msg.source(), worker_block_todo_tag, block);
                workers[msg.source()] = true;
            }
        }

        while (std::find(workers.begin(), workers.end(), true) != workers.end()) {
            boost::mpi::status msg = comm.probe();
            comm.recv(msg.source(), worker_block_end_tag, block);
            workers[msg.source()] = false;
            r.write(block);
        }

        for (int child = 1; child < comm.size(); ++child)
            comm.send(child, worker_end_tag);
    } catch (const std::exception &e) {
        std::cerr << "master fails: " << e.what() << std::endl;
        ret = EXIT_SUCCESS;
    }

    return ret;
}

int run_as_worker(const std::string &package, const std::string &vpzfile)
{
    int ret = EXIT_SUCCESS;

    try {
        boost::mpi::communicator comm;
        Worker w(package, vpzfile);
        std::string block;
        boost::mpi::broadcast(comm, block, 0);
        w.init(block);

        for (;;) {
            boost::mpi::status msg = comm.probe();

            switch (msg.tag()) {
            case worker_block_todo_tag:
                comm.recv(0, worker_block_todo_tag, block);
                block = w.run(block);
                comm.send(0, worker_block_end_tag, block);
                break;

            case worker_end_tag:
                comm.recv(0, worker_end_tag);
                break;
            }
        }
    } catch (const std::exception &e) {
        std::cerr << "worker fails: " << e.what() << std::endl;
        ret = EXIT_FAILURE;
    }

    return ret;
}

class ProgramOption
{
public:
    ProgramOption(int *blocksize,
                  std::string *package,
                  std::string *vpzfile,
                  std::string *inputfile,
                  std::string *outputfile)
        : blocksize(blocksize), package(package), vpzfile(vpzfile),
          inputfile(inputfile), outputfile(outputfile),
          desc("Allowed options")
    {
        desc.add_options()("help", "produce help message")
        ("package,P",
         boost::program_options::value<std::string>(package),
         "set package name")
        ("vpz,V",
         boost::program_options::value<std::string>(vpzfile),
         "set vpz file")
        ("input-file,i",
         boost::program_options::value<std::string>(inputfile),
         "csv input file")
        ("output-file,o",
         boost::program_options::value<std::string>(outputfile),
         "csv output file")
        ("blocksize,b",
         boost::program_options::value<int>(blocksize)->default_value(
             default_block_size_value),
         "block size to treat");
    }

    bool run(int argc, char *argv[])
    {
        boost::program_options::variables_map vm;
        boost::program_options::store(boost::program_options::command_line_parser(
                                          argc, argv).options(desc).run(), vm);
        boost::program_options::notify(vm);
        return !package->empty() and !vpzfile->empty() and !vm.count("help");
    }

    void show()
    {
        std::cout << desc << '\n'
                  << "Example:\n\tcvle -P sunflo -V sunflo_GEM.vpz"
                  << " -i design_GWA.csv -o result.csv\n\n";
    }

private:
    int *blocksize;
    std::string *package;
    std::string *vpzfile;
    std::string *inputfile;
    std::string *outputfile;
    boost::program_options::options_description desc;
};

int main(int argc, char *argv[])
{
    boost::mpi::environment env(argc, argv);
    boost::mpi::communicator comm;
    int blocksize = default_block_size_value;
    std::string package, vpzfile, inputfile, outputfile;

    if (comm.size() == 1)  {
        std::cerr << "cvle needs two processors.\n";
        return EXIT_FAILURE;
    }

    ProgramOption po(&blocksize, &package, &vpzfile, &inputfile, &outputfile);

    try {
        if (!po.run(argc, argv)) {
            if (comm.rank() == 0)
                po.show();

            return EXIT_SUCCESS;
        }
    } catch (const std::exception &e) {
        if (comm.rank() == 0)
            std::cerr << "Parsing option error: " << e.what() << std::endl;

        return EXIT_FAILURE;
    }

    if (comm.rank() == 0) {
        std::cout << "\nblock size : " << blocksize
                  << "\npackage    : " << package
                  << "\nvpz        : " << vpzfile
                  << "\ninputfile  : "
                  << (inputfile.empty() ? "stdin" : inputfile)
                  << "\noutput file: "
                  << (outputfile.empty() ? "stdout" : outputfile)
                  << '\n';
        return run_as_master(inputfile, outputfile, blocksize);
    } else {
        return run_as_worker(package, vpzfile);
    }
}
