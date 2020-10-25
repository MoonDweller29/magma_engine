#include "vk3d/app/config/json.h"
#include <vector>
#include <string>
#include <iostream>

struct Person {
    std::string name = "Unknown";
    int age = 18;

    JSON_MAPPINGS(
            {age, "age"},
            {name, "name"}
    )
};

struct Team {
    std::string name = "Unnamed";
    std::vector<Person> members;
    float score;
    Person captain;

private:
    JSON_MAPPINGS(
            {members, "members"},
            {name, "name"},
            {score, "score"},
            {captain, "captain"}
    )

};

namespace hidden {
    struct Sneaky {
        int get_x() const {
            return x;
        }

    private:
        int x = 42;

        JSON_MAPPINGS(
                { x, "x" }
        )
    };
}

void test_config() {
    Person dude = {"Dude", 25};
    Person boi = {"Boi"};
    Person mysterious;

    Team team_a {
        "Team A",
        {
            {"Dude", 25},
            {"Boi"},
            {}
        },
        22.8,
        {"Captain", 99}
    };

    json dump = team_a;
    std::cout << dump.dump(4) << std::endl;

    Team team_b;
    team_b = dump.get<Team>();
    json dump_2 = team_b;
    std::cout << dump_2.dump(4) << std::endl;

    hidden::Sneaky sneaky;
    json sneaky_json = sneaky;
    std::cout << sneaky_json.dump(4) << std::endl;
    sneaky_json["x"] = 12;
    hidden::Sneaky another_sneaky = sneaky_json.get<hidden::Sneaky>();
    std::cout << another_sneaky.get_x() << std::endl;

}