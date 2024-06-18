#pragma once

#include <glm/ext/matrix_transform.hpp>

#include <rubus-engine/game/scene.hpp>
#include <rubus-engine/game/resource.hpp>

#include "../game/data.hpp"
#include "../game/components.hpp"

#include <random>

enum struct GameState {
  Ready,
  SkillSelectTarget,

  UsingSkillStart,
  UsingSkill,
  UsingSkillEnd,

  MonsterSkillStart,
  MonsterSkill,
  MonsterSkillEnd,
};

struct GameScene : public rugame::Scene {
  GameState state = GameState::Ready;

  int max_ap = 0;
  int cur_ap = 0;
  int character_count = 4;

  ruecs::EntityId acting_entity_id;
  ruecs::EntityId target_entity_id;
  glm::vec3 target_entity_pos;
  CharacterComponent *target_character = nullptr;
  MonsterComponent *target_monster = nullptr;

  int selected_skill = 0;
  std::array<SkillData, 3> selected_character_skills;

  inline auto reset() -> void {
    state = GameState::Ready;
    max_ap = 0;
    cur_ap = 0;
    target_character = nullptr;
    target_monster = nullptr;
    selected_skill = 0;
  }

  inline auto get_selected_skill_data() -> SkillData {
    return selected_character_skills[selected_skill];
  }

  inline auto update_skill_use_button() {
    auto skill_data = get_selected_skill_data();
    if (skill_data.ap_cost > cur_ap) {
      ui_nodes.at("skill_use_button")->set_color(SkColors::kGray);
    } else {
      ui_nodes.at("skill_use_button")->set_color(SkColors::kLtGray);
    }
  }
};

inline auto new_game_scene(GameData *game_data) -> rugame::Scene * {
  auto scene = new GameScene{};

  scene->fn_on_init = [](rugame::Scene *) {
    // texture: backgrounds
    rugame::ResourceManager::load_texture2d_pixel("bg.plains-sheet1", "assets/bg/plains-sheet1.png");
    rugame::ResourceManager::load_texture2d_pixel("bg.plains-sheet2", "assets/bg/plains-sheet2.png");
    rugame::ResourceManager::load_texture2d_pixel("bg.plains-sheet3", "assets/bg/plains-sheet3.png");
    rugame::ResourceManager::load_texture2d_pixel("bg.plains-sheet4", "assets/bg/plains-sheet4.png");

    // texture: characters
    rugame::ResourceManager::load_texture2d_pixel("character.human_warrior", "assets/character/human_warrior.png");
    rugame::ResourceManager::load_texture2d_pixel("character.human_priest", "assets/character/human_priest.png");
    rugame::ResourceManager::load_texture2d_pixel("character.elf_archer", "assets/character/elf_archer.png");
    rugame::ResourceManager::load_texture2d_pixel("character.elf_mage", "assets/character/elf_mage.png");
    rugame::ResourceManager::load_texture2d_pixel("character.darkelf_assassin",
                                                  "assets/character/darkelf_assassin.png");

    // texture: skills
    rugame::ResourceManager::load_texture2d_pixel("attack.sword", "assets/skill/attack_sword.png");
    rugame::ResourceManager::load_texture2d_pixel("attack.magic", "assets/skill/attack_magic.png");
    rugame::ResourceManager::load_texture2d_pixel("attack.arrow", "assets/skill/attack_arrow.png");
    rugame::ResourceManager::load_texture2d_pixel("attack.dagger", "assets/skill/attack_dagger.png");
    rugame::ResourceManager::load_texture2d_pixel("skill.shield_bash", "assets/skill/skill_shield_bash.png");
    rugame::ResourceManager::load_texture2d_pixel("skill.shields_up", "assets/skill/skill_shields_up.png");
    rugame::ResourceManager::load_texture2d_pixel("skill.gods_blessing", "assets/skill/skill_gods_blessing.png");
    rugame::ResourceManager::load_texture2d_pixel("skill.heal", "assets/skill/skill_heal.png");
    rugame::ResourceManager::load_texture2d_pixel("skill.snipe", "assets/skill/skill_snipe.png");
    rugame::ResourceManager::load_texture2d_pixel("skill.rain_of_arrows", "assets/skill/skill_rain_of_arrows.png");
    rugame::ResourceManager::load_texture2d_pixel("skill.meteorite", "assets/skill/skill_meteorite.png");
    rugame::ResourceManager::load_texture2d_pixel("skill.sharp_wind", "assets/skill/skill_sharp_wind.png");
    rugame::ResourceManager::load_texture2d_pixel("skill.poison_strike", "assets/skill/skill_poison_strike.png");
    rugame::ResourceManager::load_texture2d_pixel("skill.vital_strike", "assets/skill/skill_vital_strike.png");

    // texture: monsters
    rugame::ResourceManager::load_texture2d_pixel("monster.green_dragon", "assets/monster/green_dragon.png");
    rugame::ResourceManager::load_texture2d_pixel("monster.red_dragon", "assets/monster/red_dragon.png");

    // material: sprite
    rugame::SpriteMaterial::init();
  };

  scene->fn_on_deinit = [=](rugame::Scene *scene) {
    rugame::ResourceManager::unload_texture2d_all();
    rugame::SpriteMaterial::deinit();
    game_data->reset();

    auto this_scene = dynamic_cast<GameScene *>(scene);
    this_scene->reset();
  };

  scene->fn_on_start = [=](ruapp::Window *, rugame::SceneManager *, rugame::Scene *scene) {
    auto this_scene = dynamic_cast<GameScene *>(scene);

    // spawn backgrounds
    {
      const auto w = 320.f * 3.5f;
      const auto h = 180.f * 3.5f;
      for (auto i = 1; i <= 4; ++i) {
        auto entity = scene->arch_storage.create_entity();
        auto position = glm::vec3{0, 0, 0};
        auto texture_id = std::format("bg.plains-sheet{}", i);
        auto sprite = new rugame::Sprite{{0.5f, 0.5f}, w, h, rugame::SpriteMaterial{texture_id}};
        sprite->zorder = -5 + i;
        entity.add_component<TransformComponent>(position);
        entity.add_component<SpriteComponent>(sprite);
      }
    }

    // spawn characters
    {
      auto spawn_pos = std::array{
        glm::vec3{-150, -130, 0},
        glm::vec3{-210, -130, 0},
        glm::vec3{-270, -130, 0},
        glm::vec3{-330, -130, 0},
      };

      const auto w = 50.f;
      const auto h = 50.f;
      for (auto i = 0; i < 4; ++i) {
        auto character_data = game_data->picked_characters[i];
        auto entity = scene->arch_storage.create_entity();
        auto position = spawn_pos[i];
        auto sprite = new rugame::Sprite{{0.5f, 0.5f}, w, h, rugame::SpriteMaterial{character_data.texture_id}};
        entity.add_component<TransformComponent>(position);
        entity.add_component<SpriteComponent>(sprite);
        entity.add_component<CharacterComponent>(character_data);

        // calculate ap
        this_scene->max_ap += character_data.ap;
      }

      this_scene->cur_ap = this_scene->max_ap;
    }

    // spawn monster
    {
      auto entity = scene->arch_storage.create_entity();
      auto position = glm::vec3{150, -130 + 25, 0};
      auto sprite = new rugame::Sprite{{0.5f, 0.5f}, -90, 90, rugame::SpriteMaterial{"monster.red_dragon"}};
      entity.add_component<TransformComponent>(position);
      entity.add_component<SpriteComponent>(sprite);
      entity.add_component<MonsterComponent>(30, 10, position);
    }

    // player data ui
    auto node_player_ap = (new rugui::Node{"player_ap", std::format("Action point: {}", this_scene->cur_ap)})
                            ->set_flex_self_align(rugui::FlexAlign::Center)
                            ->set_margin(5)
                            ->set_font_size(30);

    scene->ui_nodes.insert({"player_ap", node_player_ap});

    // character data ui
    auto node_character_name = (new rugui::Node{"character_name", "Character name"})->set_font_size(25);
    auto node_character_hp = (new rugui::Node{"character_hp", "HP: 1"})->set_font_size(25);
    auto node_character_data = (new rugui::Node{"character_data"})
                                 ->set_display_mode(rugui::DisplayMode::Collapsed)
                                 ->set_margin(10)
                                 ->set_width(rugui::Size::FitContent())
                                 ->set_height(rugui::Size::FitContent())
                                 ->add(node_character_name)
                                 ->add(node_character_hp);

    scene->ui_nodes.insert({"character_data", node_character_data});
    scene->ui_nodes.insert({"character_name", node_character_name});
    scene->ui_nodes.insert({"character_hp", node_character_hp});

    // skill data ui
    auto node_skill_name = (new rugui::Node{"skill_name", "Skill name"})->set_font_size(25);
    auto node_skill_desc = (new rugui::Node{"skill_desc", "Skill description"})->set_font_size(20);
    auto node_skill_use_button = (new rugui::Node{"use_button"})
                                   ->set_color(SkColors::kLtGray)
                                   ->set_margin_t(5)
                                   ->set_padding(5)
                                   ->set_width(rugui::Size::FitContent())
                                   ->set_height(rugui::Size::FitContent())
                                   ->set_on_mouse_click_in([=](rugui::Node *, rugui::MouseButton button, int, int) {
                                     auto skill_data = this_scene->get_selected_skill_data();
                                     if (button == rugui::MouseButton::Left) {
                                       if (this_scene->state != GameState::Ready) {
                                         return false;
                                       }
                                       if (this_scene->cur_ap >= skill_data.ap_cost) {
                                         this_scene->cur_ap -= skill_data.ap_cost;
                                         this_scene->update_skill_use_button();
                                         node_player_ap->text = std::format("Action point: {}", this_scene->cur_ap);
                                         this_scene->state = GameState::SkillSelectTarget;
                                       }
                                     }
                                     return false;
                                   })
                                   ->add((new rugui::Node{"text", "사용하기"})->set_font_size(20));

    auto node_skill_data = (new rugui::Node{"skill_data"})
                             ->set_display_mode(rugui::DisplayMode::Collapsed)
                             ->set_margin(10)
                             ->set_width(rugui::Size::FitContent())
                             ->set_height(rugui::Size::FitContent())
                             ->add(node_skill_name)
                             ->add(node_skill_desc)
                             ->add(node_skill_use_button);

    scene->ui_nodes.insert({"skill_data", node_skill_data});
    scene->ui_nodes.insert({"skill_use_button", node_skill_use_button});

    // skill bar ui
    auto make_skill_button = [=](std::string node_name, int skill_index) {
      auto skill_data = &this_scene->selected_character_skills[skill_index];
      return (new rugui::Node{node_name})
        ->set_color(SkColors::kLtGray)
        ->set_margin(5)
        ->set_padding(5)
        ->set_width(rugui::Size::Self(32 * 2 + 10))
        ->set_height(rugui::Size::Self(32 * 2 + 10))
        ->set_image(game_data->skimg_skills.at("attack_sword"))
        ->set_image_sampling(SkSamplingOptions{SkFilterMode::kNearest})
        ->set_on_mouse_click_in([=](rugui::Node *, rugui::MouseButton button, int, int) {
          if (button == rugui::MouseButton::Left) {
            this_scene->selected_skill = skill_index;
            node_character_data->set_display_mode(rugui::DisplayMode::Collapsed);
            node_skill_data->set_display_mode(rugui::DisplayMode::Shown);
            node_skill_name->text = std::format("[{}] {}", skill_data->name, skill_data->ap_cost);
            node_skill_desc->text = skill_data->desc;
            this_scene->update_skill_use_button();
          }
          return false;
        });
    };

    auto node_skill_bar = (new rugui::Node{"skill_bar"})
                            ->set_display_mode(rugui::DisplayMode::Collapsed)
                            ->set_flex_dir(rugui::FlexDir::Row)
                            ->set_margin(20)
                            ->set_width(rugui::Size::FitContent())
                            ->set_height(rugui::Size::FitContent())
                            ->add(make_skill_button("skill0", 0))
                            ->add(make_skill_button("skill1", 1))
                            ->add(make_skill_button("skill2", 2));

    scene->ui_nodes.insert({"skill_bar", node_skill_bar});

    // end turn ui
    auto node_end_turn_button = (new rugui::Node{"end_turn_button"})
                                  ->set_flex_self_align(rugui::FlexAlign::Center)
                                  ->set_color(SkColors::kLtGray)
                                  ->set_margin_t(5)
                                  ->set_padding(5)
                                  ->set_width(rugui::Size::FitContent())
                                  ->set_height(rugui::Size::FitContent())
                                  ->set_on_mouse_click_in([=](rugui::Node *node, rugui::MouseButton button, int, int) {
                                    if (button == rugui::MouseButton::Left) {
                                      if (this_scene->state != GameState::Ready) {
                                        return false;
                                      }

                                      node->set_color(SkColors::kGray);

                                      // hide ui
                                      node_character_data->set_display_mode(rugui::DisplayMode::Collapsed);
                                      node_skill_bar->set_display_mode(rugui::DisplayMode::Collapsed);
                                      node_skill_data->set_display_mode(rugui::DisplayMode::Collapsed);

                                      this_scene->state = GameState::MonsterSkillStart;
                                    }
                                    return false;
                                  })
                                  ->add((new rugui::Node{"text", "턴 종료"})->set_font_size(20));

    scene->ui_nodes.insert({"end_turn_button", node_end_turn_button});

    // ui tree
    scene->ui_tree.root
      ->add((new rugui::Node{"data_container"})
              ->set_width(rugui::Size::Parent(1))
              ->set_height(rugui::Size::Parent(0.8f))
              ->add(node_player_ap)
              ->add(node_end_turn_button)
              ->add(node_character_data)
              ->add(node_skill_data))
      ->add((new rugui::Node{"skill_bar_container"})
              ->set_flex_align(rugui::FlexAlign::End)
              ->set_width(rugui::Size::Parent(1))
              ->set_height(rugui::Size::Parent(0.2f))
              ->add(node_skill_bar));
  };

  scene->fn_on_update = [=](ruapp::Window *window, rugame::SceneManager *scene_manager, rugame::Scene *scene,
                            double delta_time) {
    auto this_scene = dynamic_cast<GameScene *>(scene);

    if (window->is_key_just_down(VK_ESCAPE)) {
      scene_manager->set_active_scene("menu:main");
    }

    static auto query_character = ruecs::Query{&scene->arch_storage}.with<TransformComponent, CharacterComponent>();
    static auto query_monster = ruecs::Query{&scene->arch_storage}.with<TransformComponent, MonsterComponent>();
    static auto query_skill_timer = ruecs::Query{&scene->arch_storage}.with<TransformComponent, SkillComponent>();
    static auto query_render = ruecs::Query{&scene->arch_storage}.with<TransformComponent, SpriteComponent>();

    if (this_scene->state == GameState::Ready) {
      // character click
      for_each_entities(&scene->arch_storage, &scene->command, query_character) {
        auto transform = entity.get_component<TransformComponent>();
        auto character = entity.get_component<CharacterComponent>();

        if (window->is_mouse_just_down(rugui::MouseButton::Left)) {
          auto mouse_world_pos = scene->camera.screen_to_world_space({window->mouse_x, window->mouse_y});
          if (glm::distance(transform->position, {mouse_world_pos, 0}) <= 24.f) {
            this_scene->acting_entity_id = entity.id;

            // update skill bar
            scene->ui_nodes.at("skill_bar")->set_display_mode(rugui::DisplayMode::Shown);
            scene->ui_nodes.at("skill_data")->set_display_mode(rugui::DisplayMode::Collapsed);
            this_scene->selected_character_skills = character->skills;
            for (int i = 0; i < 3; ++i) {
              auto skill_data = character->skills[i];
              auto &node_skill_button = scene->ui_nodes.at("skill_bar")->children[i];
              node_skill_button->set_image(game_data->skimg_skills.at(skill_data.id));
            }

            // update character data
            scene->ui_nodes.at("character_data")->set_display_mode(rugui::DisplayMode::Shown);
            scene->ui_nodes.at("character_name")->text = std::format("[{}]", character->name);
            scene->ui_nodes.at("character_hp")->text = std::format("HP: {}", character->health);
          }
        }
      }
    }

    if (this_scene->state == GameState::SkillSelectTarget) {
      for_each_entities(&scene->arch_storage, &scene->command, query_character) {
        auto transform = entity.get_component<TransformComponent>();
        auto character = entity.get_component<CharacterComponent>();

        if (window->is_mouse_just_down(rugui::MouseButton::Left)) {
          auto mouse_world_pos = scene->camera.screen_to_world_space({window->mouse_x, window->mouse_y});
          if (glm::distance(transform->position, {mouse_world_pos, 0}) <= 24.f) {
            this_scene->target_entity_id = entity.id;
            this_scene->target_entity_pos = transform->position;
            this_scene->target_character = character;
            this_scene->target_monster = nullptr;
            this_scene->state = GameState::UsingSkillStart;
            break;
          }
        }
      }
    }
    if (this_scene->state == GameState::SkillSelectTarget) {
      for_each_entities(&scene->arch_storage, &scene->command, query_monster) {
        auto transform = entity.get_component<TransformComponent>();
        auto monster = entity.get_component<MonsterComponent>();

        if (window->is_mouse_just_down(rugui::MouseButton::Left)) {
          auto mouse_world_pos = scene->camera.screen_to_world_space({window->mouse_x, window->mouse_y});
          if (glm::distance(transform->position, {mouse_world_pos, 0}) <= 24.f) {
            this_scene->target_entity_id = entity.id;
            this_scene->target_entity_pos = transform->position;
            this_scene->target_character = nullptr;
            this_scene->target_monster = monster;
            this_scene->state = GameState::UsingSkillStart;
            break;
          }
        }
      }
    }

    if (this_scene->state == GameState::UsingSkillStart) {
      for_each_entities(&scene->arch_storage, &scene->command, query_character) {
        auto transform = entity.get_component<TransformComponent>();

        if (entity.id == this_scene->acting_entity_id) {
          transform->position.y += 50;

          auto skill_position = this_scene->target_entity_pos + glm::vec3{0, 50, 0};
          auto skill_data = this_scene->get_selected_skill_data();
          auto skill_sprite = new rugame::Sprite{{0.5f, 0.5f}, 90, 90, rugame::SpriteMaterial{skill_data.texture_id}};
          skill_sprite->zorder = 10;

          auto skill = scene->command.create_entity();
          skill.add_component<TransformComponent>(skill_position);
          skill.add_component<SpriteComponent>(skill_sprite);
          skill.add_component<SkillComponent>(skill_data);

          this_scene->state = GameState::UsingSkill;
          break;
        }
      }

      scene->command.run();
    }

    if (this_scene->state == GameState::UsingSkill) {
      for_each_entities(&scene->arch_storage, &scene->command, query_skill_timer) {
        auto transform = entity.get_component<TransformComponent>();
        auto skill = entity.get_component<SkillComponent>();

        // move sprite
        transform->position.y -= 200.f * delta_time;

        // tick timer
        skill->time += delta_time;
        if (skill->time >= skill->end_time) {
          scene->command.delete_entity(entity);
          this_scene->state = GameState::UsingSkillEnd;
        }
      }

      scene->command.run();
    }

    if (this_scene->state == GameState::UsingSkillEnd) {
      for_each_entities(&scene->arch_storage, &scene->command, query_monster) {
        auto monster = entity.get_component<MonsterComponent>();

        if (monster->health <= 0) {
          scene->command.delete_entity(entity);
          this_scene->character_count -= 1;
        }
      }

      for_each_entities(&scene->arch_storage, &scene->command, query_character) {
        auto transform = entity.get_component<TransformComponent>();
        auto character = entity.get_component<CharacterComponent>();

        if (character->health <= 0) {
          scene->command.delete_entity(entity);
          this_scene->character_count -= 1;
        }

        if (entity.id == this_scene->acting_entity_id) {
          transform->position.y -= 50;
          this_scene->state = GameState::Ready;
        }
      }

      scene->command.run();
    }

    if (this_scene->state == GameState::MonsterSkillStart) {
      for_each_entities(&scene->arch_storage, &scene->command, query_monster) {
        auto transform = entity.get_component<TransformComponent>();
        auto monster = entity.get_component<MonsterComponent>();

        if (not monster->action_done) {
          auto rd = std::random_device{};
          auto rng = std::mt19937{rd()};
          std::uniform_int_distribution<> distr(0, 3);

          int i = 0;
          int rand_i = distr(rng);
          for_each_entities(&scene->arch_storage, &scene->command, query_character) {
            if (i == rand_i) {
              auto transform = entity.get_component<TransformComponent>();
              this_scene->target_entity_id = entity.id;
              this_scene->target_entity_pos = transform->position;
              break;
            }
            ++i;
          }

          this_scene->acting_entity_id = entity.id;
          transform->position.y += 50;

          auto dist = glm::distance(this_scene->target_entity_pos, transform->position);
          monster->end_time = dist / 600.f;
          monster->time = 0;

          this_scene->state = GameState::MonsterSkill;
          break;
        }
      }
    }

    if (this_scene->state == GameState::MonsterSkill) {
      for_each_entities(&scene->arch_storage, &scene->command, query_monster) {
        auto transform = entity.get_component<TransformComponent>();
        auto monster = entity.get_component<MonsterComponent>();

        if (entity.id == this_scene->acting_entity_id) {
          auto dir = glm::normalize(this_scene->target_entity_pos - transform->position);
          transform->position += dir * (600.f * (float)delta_time);

          // tick timer
          monster->time += delta_time;
          if (monster->time >= monster->end_time) {
            this_scene->state = GameState::MonsterSkillEnd;
          }
          break;
        }
      }
    }

    if (this_scene->state == GameState::MonsterSkillEnd) {
      for_each_entities(&scene->arch_storage, &scene->command, query_monster) {
        auto transform = entity.get_component<TransformComponent>();
        auto monster = entity.get_component<MonsterComponent>();

        if (entity.id == this_scene->acting_entity_id) {
          transform->position = monster->start_pos;
          monster->action_done = true;
          break;
        }
      }

      // after all monster finished attack
      for_each_entities(&scene->arch_storage, &scene->command, query_monster) {
        auto monster = entity.get_component<MonsterComponent>();
        monster->action_done = false;
      }

      scene->ui_nodes.at("end_turn_button")->set_color(SkColors::kLtGray);
      this_scene->cur_ap = this_scene->max_ap;
      scene->ui_nodes.at("player_ap")->text = std::format("Action point: {}", this_scene->cur_ap);
      this_scene->state = GameState::Ready;
    }

    // render
    for_each_entities(&scene->arch_storage, &scene->command, query_render) {
      auto transform = entity.get_component<TransformComponent>();
      auto sprite_component = entity.get_component<SpriteComponent>();
      auto sprite = sprite_component->sprite;
      sprite->transform = glm::translate(glm::mat4{1}, transform->position);
      scene->sprites.push_back(sprite);
    }
  };

  return scene;
}
