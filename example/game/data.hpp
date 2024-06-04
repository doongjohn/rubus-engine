#pragma once

#include <vector>
#include <unordered_map>
#include <string>

struct CharacterData {
  std::string id;
  std::string name;
  std::string desc;
  std::string texture_id;
  int health = 0;
  int ap = 0; // action point

  inline auto operator==(const CharacterData &other) const -> bool = default;
};

struct GameData {
  std::unordered_map<std::string, CharacterData> character_data;
  std::vector<CharacterData> picked_characters;

  inline auto init() -> void {
    character_data.insert( //
      {"human_warrior",    //
       CharacterData{
         .id = "human_warrior",
         .name = "인간 전사",
         .desc = "많은 전장에서 살아남은 배테랑 전사다. 전장속에서 적을 두려워하지 않는다.\n"
                 "- 체력: 100\n"
                 "- 행동력: 20\n",
         .texture_id = "character.human_warrior",
         .health = 100,
         .ap = 20,
       }});

    character_data.insert( //
      {"human_priest",     //
       CharacterData{
         .id = "human_priest",
         .name = "인간 성직자",
         .desc = "신을 섬기는 고결한 신자. 신의 힘을 빌려 사람들의 상처를 치유해준다.\n"
                 "- 체력: 60\n"
                 "- 행동력: 30\n",
         .texture_id = "character.human_priest",
         .health = 60,
         .ap = 30,
       }});

    character_data.insert( //
      {"elf_archer",       //
       CharacterData{
         .id = "elf_archer",
         .name = "엘프 궁수",
         .desc = "엘프의 민첨함으로 적에게 활을 명중하는 명사수. 화살이 날아오는 소리를 들었다면 이미 늦었다.\n"
                 "- 체력: 60\n"
                 "- 행동력: 40\n",
         .texture_id = "character.elf_archer",
         .health = 60,
         .ap = 40,
       }});

    character_data.insert( //
      {"elf_mage",         //
       CharacterData{
         .id = "elf_mage",
         .name = "엘프 마법사",
         .desc = "엘프에게만 대대로 전해져오는 고대의 마법을 전수받은 신비로운 마법사. "
                 "그 누구도 이 마법사의 마법을 따라하지 못한다.\n"
                 "- 체력: 70\n"
                 "- 행동력: 30\n",
         .texture_id = "character.elf_mage",
         .health = 70,
         .ap = 30,
       }});

    character_data.insert( //
      {"darkelf_assassin", //
       CharacterData{
         .id = "darkelf_assassin",
         .name = "다크엘프 암살자",
         .desc = "어둠의 세계에서 명성이 높은 이 암살자는 돈을 위해서라면 어떠한 잔인한 일도 저지를 수 있다.\n"
                 "- 체력: 50\n"
                 "- 행동력: 50\n",
         .texture_id = "character.darkelf_assassin",
         .health = 50,
         .ap = 50,
       }});
  }

  inline auto reset() -> void {
    picked_characters.clear();
  }
};
