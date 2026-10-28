#include "UnitTestEnvironment.h"

namespace UnitTest
{
    void Scenario::expect(bool condition, const char* expression, const char* file, int line)
    {
        if (!condition)
            fail(std::string(file) + ":" + std::to_string(line) + ": " + expression);
    }

    void Scenario::require(bool condition, const char* expression, const char* file, int line)
    {
        expect(condition, expression, file, line);
        if (!condition)
            throw Abort{};
    }

    void Scenario::fail(const std::string& message)
    {
        ++failures;
        output << "  " << message << '\n';
    }

    void Environment::addScenario(std::string name, std::function<void(Scenario&)> body)
    {
        if (name.empty() || !body)
            throw std::invalid_argument("A scenario needs a name and a body");
        for (const auto& entry : scenarios)
            if (entry.name == name)
                throw std::invalid_argument("Duplicate scenario: " + name);
        scenarios.push_back({std::move(name), std::move(body)});
    }

    int Environment::run(std::ostream& output) const
    {
        output << "[ Suite    ] " << name << '\n';
        if (scenarios.empty())
        {
            output << "[ FAILED   ] No scenarios registered\n";
            return 1;
        }
        std::size_t failed = 0;
        for (const auto& entry : scenarios)
        {
            output << "[ RUN      ] " << entry.name << '\n';
            Scenario scenario(output);
            try
            {
                entry.body(scenario);
            }
            catch (const Scenario::Abort&)
            {
            }
            catch (const std::exception& error)
            {
                scenario.fail(std::string("Unhandled exception: ") + error.what());
            }
            catch (...)
            {
                scenario.fail("Unhandled unknown exception");
            }
            if (!scenario.passed())
                ++failed;
            output << (scenario.passed() ? "[       OK ] " : "[ FAILED   ] ") << entry.name << '\n';
        }
        output << "[ Summary  ] " << scenarios.size() - failed << " passed, " << failed << " failed\n";
        return failed == 0 ? 0 : 1;
    }
}