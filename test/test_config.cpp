#include "app/config/config.h"
#include <vector>
#include <string>
#include <iostream>

struct Person {
    std::string name;
    int age;
};

struct Team {
    std::string name;
    std::vector<Person> members;
    float score;
    Person captain;
};

template <>
const auto json_mappings<Person> = JSONMappings(
        JSONMapping(&Person::age, "age", 18, SKIP_IF_NOT_CHANGED),
        JSONMapping(&Person::name, "name", "Unknown", SKIP_IF_NOT_CHANGED)
);

template <>
const auto json_mappings<Team> = define_mappings(
        JSONMapping(&Team::members, "members"),
        JSONMapping(&Team::name, "name"),
        JSONMapping(&Team::score, "score"),
        JSONMapping(&Team::captain, "captain", {})
);


void test_config() {
    Person dude = {"Dude", 25};
    Person boi = {"Boi"};
    Person mysterious;

    Team team_a;
    team_a.name = "Team A";
    team_a.score = 22.8;
    team_a.members = {dude, boi};
    team_a.captain = mysterious;

    std::vector<Person> dudes = {dude, boi, mysterious};

    json dump = team_a;
    std::cout << dump.dump(4) << std::endl;

    dump["name"] = "Team B";
    auto it = dump["members"][1].find("age");
    dump["members"][1].erase(it);
    it = dump["captain"].find("name");
    dump["captain"].erase(it);

    Team team_b;
    from_json(dump, team_b);
    json dump_2 = team_b;
    std::cout << dump_2.dump(4) << std::endl;
}