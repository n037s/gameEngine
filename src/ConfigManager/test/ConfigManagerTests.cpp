#include "ConfigManager.h"
#include "GlobalConfig.h"
#include "UnitTestEnvironment.h"

#include <chrono>
#include <filesystem>
#include <fstream>
#include <type_traits>

static_assert(std::is_abstract_v<ConfigManager>);
static_assert(std::has_virtual_destructor_v<ConfigManager>);
static_assert(!std::is_copy_constructible_v<GlobalConfig>);

class TestConfig : public ConfigManager
{
public:
    explicit TestConfig(const std::filesystem::path& path) : ConfigManager(path) { loadConfig(); }
    using ConfigManager::getValue;
    using ConfigManager::setValue;

private:
    void parseConfig() override {}
};

class ConfigFixture : public UnitTest::Fixture
{
public:
    std::filesystem::path directory;
    std::filesystem::path path;

    void setUp() override
    {
        directory = std::filesystem::temp_directory_path() / ("gameEngine-config-"
            + std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()));
        std::filesystem::create_directory(directory);
        path = directory / "testConfig.xml";
    }

    void write(const std::string& xml)
    {
        std::ofstream file(path);
        file.exceptions(std::ios::failbit | std::ios::badbit);
        file << xml;
    }

    void tearDown() override
    {
        std::filesystem::remove_all(directory);
    }
};

class ParsedConfig : public TestConfig
{
public:
    explicit ParsedConfig(const std::filesystem::path& path) : TestConfig(path) { parseConfig(); }
    std::string cachedValue;
    int parseCount{0};

private:
    void parseConfig() override
    {
        const auto value = getValue("viewer", "frameRateLimit");
        cachedValue = value;
        ++parseCount;
    }
};

template<typename Exception, typename Action>
bool throws(Action action)
{
    try { action(); }
    catch (const Exception&) { return true; }
    return false;
}

int main()
{
    UnitTest::Environment tests("ConfigManager");

    tests.addScenario("GlobalConfig reads the supplied file and shares one instance", [](UnitTest::Scenario& scenario) {
        auto& config = GlobalConfig::getInstance();
        UT_EXPECT(scenario, &config == &GlobalConfig::getInstance());
        UT_EXPECT(scenario, config.getFrameRateLimit() == 60);
        config.setFrameRateLimit(144);
        for (int read = 0; read < 100; ++read)
            UT_EXPECT(scenario, config.getFrameRateLimit() == 144);
        config.setFrameRateLimit(0);
        UT_EXPECT(scenario, config.getFrameRateLimit() == 0);
        UT_EXPECT(scenario, throws<std::invalid_argument>([&] { config.setFrameRateLimit(-1); }));
        UT_EXPECT(scenario, config.getFrameRateLimit() == 0);
        ConfigManager& base = config;
        base.loadConfig();
        UT_EXPECT(scenario, config.getFrameRateLimit() == 60);
    });

    tests.addScenario<ConfigFixture>("Parses derived values on load and rolls back rejected reloads", [](UnitTest::Scenario& scenario, ConfigFixture& fixture) {
        fixture.write("<testConfig><viewer><frameRateLimit>60</frameRateLimit></viewer></testConfig>");
        ParsedConfig config(fixture.path);
        UT_EXPECT(scenario, config.cachedValue == "60");
        UT_EXPECT(scenario, config.parseCount == 1);
        fixture.write("<testConfig><viewer><frameRateLimit>120</frameRateLimit></viewer></testConfig>");
        UT_EXPECT(scenario, config.cachedValue == "60");
        ConfigManager& base = config;
        base.loadConfig();
        UT_EXPECT(scenario, config.cachedValue == "120");
        UT_EXPECT(scenario, config.parseCount == 2);
        fixture.write("<testConfig><viewer/></testConfig>");
        UT_EXPECT(scenario, throws<std::out_of_range>([&] { base.loadConfig(); }));
        UT_EXPECT(scenario, config.cachedValue == "120");
        UT_EXPECT(scenario, config.getValue("viewer", "frameRateLimit") == "120");
        UT_EXPECT(scenario, config.parseCount == 2);
    });

    tests.addScenario<ConfigFixture>("Parses XML text and preserves other values when saving", [](UnitTest::Scenario& scenario, ConfigFixture& fixture) {
        fixture.write("<?xml version=\"1.0\"?><testConfig><!--comment--><viewer><frameRateLimit>60</frameRateLimit>"
            "<title>A &amp; B</title><empty/></viewer><audio><volume>10</volume></audio></testConfig>");
        TestConfig config(fixture.path);
        UT_EXPECT(scenario, config.getValue("viewer", "frameRateLimit") == "60");
        UT_EXPECT(scenario, config.getValue("viewer", "title") == "A & B");
        UT_EXPECT(scenario, config.getValue("viewer", "empty").empty());
        config.setValue("viewer", "frameRateLimit", "120");
        config.setValue("viewer", "title", "<New & title>");
        config.saveConfig();
        TestConfig reloaded(fixture.path);
        UT_EXPECT(scenario, reloaded.getValue("viewer", "frameRateLimit") == "120");
        UT_EXPECT(scenario, reloaded.getValue("viewer", "title") == "<New & title>");
        UT_EXPECT(scenario, reloaded.getValue("audio", "volume") == "10");
        UT_EXPECT(scenario, throws<std::out_of_range>([&] { config.getValue("missing", "value"); }));
        UT_EXPECT(scenario, throws<std::out_of_range>([&] { config.setValue("viewer", "missing", "1"); }));
    });

    tests.addScenario<ConfigFixture>("Rejects missing, malformed and incorrectly rooted files", [](UnitTest::Scenario& scenario, ConfigFixture& fixture) {
        UT_EXPECT(scenario, throws<std::runtime_error>([&] { TestConfig config(fixture.path); }));
        for (const auto* xml : { "", "<testConfig><viewer></testConfig>", "<wrong/>", "<testConfig/><other/>" })
        {
            fixture.write(xml);
            UT_EXPECT(scenario, throws<std::runtime_error>([&] { TestConfig config(fixture.path); }));
        }
    });

    tests.addScenario<ConfigFixture>("Failed reload retains the last valid configuration", [](UnitTest::Scenario& scenario, ConfigFixture& fixture) {
        fixture.write("<testConfig><viewer><frameRateLimit>60</frameRateLimit></viewer></testConfig>");
        TestConfig config(fixture.path);
        fixture.write("<wrong/>");
        UT_EXPECT(scenario, throws<std::runtime_error>([&] { config.loadConfig(); }));
        UT_EXPECT(scenario, config.getValue("viewer", "frameRateLimit") == "60");
        std::filesystem::remove(fixture.path);
        std::filesystem::remove(fixture.directory);
        UT_EXPECT(scenario, throws<std::runtime_error>([&] { config.saveConfig(); }));
    });

    return tests.run();
}