#pragma once

#include <include/core/SkData.h>
#include <include/core/SkImage.h>

#include <array>
#include <vector>
#include <unordered_map>
#include <string>

struct CharacterComponent;
struct MonsterComponent;

struct SkillData {
  std::string id;
  std::string name;
  std::string desc;
  std::string texture_id;
  int ap_cost = 0;

  inline auto operator==(const SkillData &other) const -> bool {
    return id == other.id and name == other.name and desc == other.desc and texture_id == other.texture_id;
  }
};

struct CharacterData {
  std::string id;
  std::string name;
  std::string desc;
  std::string texture_id;
  int health = 0;
  int ap = 0;
  std::array<SkillData, 3> skills;

  float damage_buff = 0;

  inline auto operator==(const CharacterData &other) const -> bool = default;
};

struct GameData {
  std::unordered_map<std::string, sk_sp<SkImage>> skimg_skills;
  std::unordered_map<std::string, SkillData> skill_data;
  std::unordered_map<std::string, CharacterData> character_data;
  std::vector<CharacterData> picked_characters;

  auto init() -> void {
    skimg_skills = {
      {
        "attack_sword",
        SkImages::DeferredFromEncodedData(SkData::MakeFromFileName("assets/skill/attack_sword.png")),
      },
      {
        "skill_shield_bash",
        SkImages::DeferredFromEncodedData(SkData::MakeFromFileName("assets/skill/skill_shield_bash.png")),
      },
      {
        "skill_shields_up",
        SkImages::DeferredFromEncodedData(SkData::MakeFromFileName("assets/skill/skill_shields_up.png")),
      },
      {
        "attack_magic",
        SkImages::DeferredFromEncodedData(SkData::MakeFromFileName("assets/skill/attack_magic.png")),
      },
      {
        "skill_gods_blessing",
        SkImages::DeferredFromEncodedData(SkData::MakeFromFileName("assets/skill/skill_gods_blessing.png")),
      },
      {
        "skill_heal",
        SkImages::DeferredFromEncodedData(SkData::MakeFromFileName("assets/skill/skill_heal.png")),
      },
      {
        "attack_arrow",
        SkImages::DeferredFromEncodedData(SkData::MakeFromFileName("assets/skill/attack_arrow.png")),
      },
      {
        "skill_snipe",
        SkImages::DeferredFromEncodedData(SkData::MakeFromFileName("assets/skill/skill_snipe.png")),
      },
      {
        "skill_rain_of_arrows",
        SkImages::DeferredFromEncodedData(SkData::MakeFromFileName("assets/skill/skill_rain_of_arrows.png")),
      },
      {
        "skill_meteorite",
        SkImages::DeferredFromEncodedData(SkData::MakeFromFileName("assets/skill/skill_meteorite.png")),
      },
      {
        "skill_sharp_wind",
        SkImages::DeferredFromEncodedData(SkData::MakeFromFileName("assets/skill/skill_sharp_wind.png")),
      },
      {
        "attack_dagger",
        SkImages::DeferredFromEncodedData(SkData::MakeFromFileName("assets/skill/attack_dagger.png")),
      },
      {
        "skill_poison_strike",
        SkImages::DeferredFromEncodedData(SkData::MakeFromFileName("assets/skill/skill_poison_strike.png")),
      },
      {
        "skill_vital_strike",
        SkImages::DeferredFromEncodedData(SkData::MakeFromFileName("assets/skill/skill_vital_strike.png")),
      },
    };

    // skill data
    skill_data.insert({
      "attack_sword",
      SkillData{
        .id = "attack_sword",
        .name = "공격",
        .desc = "기본공격",
        .texture_id = "attack.sword",
        .ap_cost = 10,
      },
    });
    skill_data.insert({
      "attack_magic",
      SkillData{
        .id = "attack_magic",
        .name = "공격",
        .desc = "기본공격",
        .texture_id = "attack.magic",
        .ap_cost = 10,
      },
    });
    skill_data.insert({
      "attack_arrow",
      SkillData{
        .id = "attack_arrow",
        .name = "공격",
        .desc = "기본공격",
        .texture_id = "attack.arrow",
        .ap_cost = 10,
      },
    });
    skill_data.insert({
      "attack_dagger",
      SkillData{
        .id = "attack_dagger",
        .name = "공격",
        .desc = "기본공격",
        .texture_id = "attack.dagger",
        .ap_cost = 10,
      },
    });
    skill_data.insert({
      "skill_shield_bash",
      SkillData{
        .id = "skill_shield_bash",
        .name = "방패 치기",
        .desc = "방패로 적을 강타한다.",
        .texture_id = "skill.shield_bash",
        .ap_cost = 10,
      },
    });
    skill_data.insert({
      "skill_shields_up",
      SkillData{
        .id = "skill_shields_up",
        .name = "방패 올리기",
        .desc = "다음 공격을 막는다.",
        .texture_id = "skill.shields_up",
        .ap_cost = 10,
      },
    });
    skill_data.insert({
      "skill_gods_blessing",
      SkillData{
        .id = "skill_gods_blessing",
        .name = "신의 축복",
        .desc = "아군의 다음 공격이 강화된다.",
        .texture_id = "skill.gods_blessing",
        .ap_cost = 10,
      },
    });
    skill_data.insert({
      "skill_heal",
      SkillData{
        .id = "skill_heal",
        .name = "치유 마법",
        .desc = "아군의 체력을 회복한다.",
        .texture_id = "skill.heal",
        .ap_cost = 10,
      },
    });
    skill_data.insert({
      "skill_snipe",
      SkillData{
        .id = "skill_snipe",
        .name = "저격",
        .desc = "적에게 데미지를 입힌다.",
        .texture_id = "skill.snipe",
        .ap_cost = 10,
      },
    });
    skill_data.insert({
      "skill_rain_of_arrows",
      SkillData{
        .id = "skill_rain_of_arrows",
        .name = "화살 비",
        .desc = "모든 적에게 데미지를 입힌다.",
        .texture_id = "skill.rain_of_arrows",
        .ap_cost = 10,
      },
    });
    skill_data.insert({
      "skill_meteorite",
      SkillData{
        .id = "skill_meteorite",
        .name = "메테오",
        .desc = "적에게 데미지를 입힌다.",
        .texture_id = "skill.meteorite",
        .ap_cost = 20,
      },
    });
    skill_data.insert({
      "skill_sharp_wind",
      SkillData{
        .id = "skill_sharp_wind",
        .name = "칼바람",
        .desc = "적에게 데미지를 입힌다.",
        .texture_id = "skill.sharp_wind",
        .ap_cost = 15,
      },
    });
    skill_data.insert({
      "skill_poison_strike",
      SkillData{
        .id = "skill_poison_strike",
        .name = "맹독 찌르기",
        .desc = "적에게 데미지를 입히고 독을 부여한다.",
        .texture_id = "skill.poison_strike",
        .ap_cost = 10,
      },
    });
    skill_data.insert({
      "skill_vital_strike",
      SkillData{
        .id = "skill_vital_strike",
        .name = "급소 찌르기",
        .desc = "적에게 데미지를 입힌다.",
        .texture_id = "skill.vital_strike",
        .ap_cost = 10,
      },
    });

    // character data
    character_data.insert({
      "human_warrior",
      CharacterData{
        .id = "human_warrior",
        .name = "인간 전사",
        .desc = "많은 전장에서 살아남은 배테랑 전사다. 전장속에서 적을 두려워하지 않는다.\n"
                "- 체력: 100\n"
                "- 행동력: 20\n",
        .texture_id = "character.human_warrior",
        .health = 100,
        .ap = 6,
        .skills =
          {
            skill_data.at("attack_sword"),
            skill_data.at("skill_shield_bash"),
            skill_data.at("skill_shields_up"),
          },
      },
    });
    character_data.insert({
      "human_priest",
      CharacterData{
        .id = "human_priest",
        .name = "인간 성직자",
        .desc = "신을 섬기는 고결한 신자. 신의 힘을 빌려 사람들의 상처를 치유해준다.\n"
                "- 체력: 60\n"
                "- 행동력: 30\n",
        .texture_id = "character.human_priest",
        .health = 60,
        .ap = 5,
        .skills =
          {
            skill_data.at("attack_magic"),
            skill_data.at("skill_gods_blessing"),
            skill_data.at("skill_heal"),
          },
      },
    });
    character_data.insert({
      "elf_archer",
      CharacterData{
        .id = "elf_archer",
        .name = "엘프 궁수",
        .desc = "엘프의 민첨함으로 적에게 활을 명중하는 명사수. 화살이 날아오는 소리를 들었다면 이미 늦었다.\n"
                "- 체력: 60\n"
                "- 행동력: 40\n",
        .texture_id = "character.elf_archer",
        .health = 60,
        .ap = 8,
        .skills =
          {
            skill_data.at("attack_arrow"),
            skill_data.at("skill_snipe"),
            skill_data.at("skill_rain_of_arrows"),
          },
      },
    });
    character_data.insert({
      "elf_mage",
      CharacterData{
        .id = "elf_mage",
        .name = "엘프 마법사",
        .desc = "엘프에게만 대대로 전해져오는 고대의 마법을 전수받은 신비로운 마법사. "
                "그 누구도 이 마법사의 마법을 따라하지 못한다.\n"
                "- 체력: 70\n"
                "- 행동력: 30\n",
        .texture_id = "character.elf_mage",
        .health = 70,
        .ap = 8,
        .skills =
          {
            skill_data.at("attack_magic"),
            skill_data.at("skill_meteorite"),
            skill_data.at("skill_sharp_wind"),
          },
      },
    });
    character_data.insert({
      "darkelf_assassin",
      CharacterData{
        .id = "darkelf_assassin",
        .name = "다크엘프 암살자",
        .desc = "어둠의 세계에서 명성이 높은 이 암살자는 돈을 위해서라면 어떠한 잔인한 일도 저지를 수 있다.\n"
                "- 체력: 50\n"
                "- 행동력: 50\n",
        .texture_id = "character.darkelf_assassin",
        .health = 50,
        .ap = 7,
        .skills =
          {
            skill_data.at("attack_dagger"),
            skill_data.at("skill_poison_strike"),
            skill_data.at("skill_vital_strike"),
          },
      },
    });
  }

  inline auto reset() -> void {
    picked_characters.clear();
  }
};
