#pragma once

#include <exception>
#include <functional>
#include <iostream>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

namespace UnitTest
{
	class Scenario
	{
	public:
		void expect(bool condition, const char* expression, const char* file, int line);
		void require(bool condition, const char* expression, const char* file, int line);
		void fail(const std::string& message);
		bool passed() const { return failures == 0; }

	private:
		friend class Environment;
		struct Abort {};
		explicit Scenario(std::ostream& output) : output(output) {}
		std::ostream& output;
		unsigned int failures{0};
	};

	class Fixture
	{
	public:
		virtual ~Fixture() = default;
		virtual void setUp() {}
		virtual void tearDown() {}
	};

	class Environment
	{
	public:
		explicit Environment(std::string name) : name(std::move(name)) {}
		void addScenario(std::string name, std::function<void(Scenario&)> body);

		template<typename FixtureType, typename Body>
		void addScenario(std::string name, Body body)
		{
			static_assert(std::is_base_of_v<Fixture, FixtureType>, "Scenario fixtures must derive from UnitTest::Fixture");
			addScenario(std::move(name), [body = std::move(body)](Scenario& scenario) {
				FixtureType fixture;
				std::exception_ptr failure;
				try
				{
					fixture.setUp();
					body(scenario, fixture);
				}
				catch (...)
				{
					failure = std::current_exception();
				}
				try
				{
					fixture.tearDown();
				}
				catch (const std::exception& error)
				{
					scenario.fail(std::string("Fixture teardown threw: ") + error.what());
				}
				catch (...)
				{
					scenario.fail("Fixture teardown threw an unknown exception");
				}
				if (failure)
					std::rethrow_exception(failure);
			});
		}

		int run(std::ostream& output = std::cout) const;

	private:
		struct Entry
		{
			std::string name;
			std::function<void(Scenario&)> body;
		};
		std::string name;
		std::vector<Entry> scenarios;
	};
}

#define UT_EXPECT(scenario, ...) (scenario).expect(static_cast<bool>((__VA_ARGS__)), #__VA_ARGS__, __FILE__, __LINE__)
#define UT_REQUIRE(scenario, ...) (scenario).require(static_cast<bool>((__VA_ARGS__)), #__VA_ARGS__, __FILE__, __LINE__)
