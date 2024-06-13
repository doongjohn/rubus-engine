#pragma once

#include <rubus-engine/game/scene.hpp>
#include <rubus-engine/game/resource.hpp>

#include "../game/data.hpp"

#include <array>

inline auto new_character_selection_menu_scene(GameData *game_data) -> rugame::Scene * {
  auto scene = new rugame::Scene{};

  struct UiState {
    int max_select_count = 4;
    int cur_select_count = 0;
    std::array<bool, 8> selection{};
    std::array<std::string, 8> character_id{};

    inline UiState() {
      character_id[0] = "human_warrior";
      character_id[1] = "human_priest";
      character_id[2] = "elf_archer";
      character_id[3] = "elf_mage";
      character_id[4] = "darkelf_assassin";
    }

    inline auto reset() -> void {
      cur_select_count = 0;
      selection = {};
    }

    inline auto can_select_more() -> bool {
      return cur_select_count < max_select_count;
    }
  };

  auto ui_state = new UiState{}; // leaking

  scene->fn_on_init = [](rugame::Scene *) {
    rugame::SpriteMaterial::init();
  };

  scene->fn_on_deinit = [=](rugame::Scene *) {
    rugame::SpriteMaterial::deinit();
    ui_state->reset();
  };

  scene->fn_on_start = [=](ruapp::Window *, rugame::SceneManager *scene_manager, rugame::Scene *scene) {
    auto skimg_characters = std::unordered_map<std::string, sk_sp<SkImage>>{
      {
        "human_warrior",
        SkImages::DeferredFromEncodedData(SkData::MakeFromFileName("assets/character/human_warrior.png")),
      },
      {
        "human_priest",
        SkImages::DeferredFromEncodedData(SkData::MakeFromFileName("assets/character/human_priest.png")),
      },
      {
        "elf_archer",
        SkImages::DeferredFromEncodedData(SkData::MakeFromFileName("assets/character/elf_archer.png")),
      },
      {
        "elf_mage",
        SkImages::DeferredFromEncodedData(SkData::MakeFromFileName("assets/character/elf_mage.png")),
      },
      {
        "darkelf_assassin",
        SkImages::DeferredFromEncodedData(SkData::MakeFromFileName("assets/character/darkelf_assassin.png")),
      },
    };

    auto node_char_image = (new rugui::Node{"character_image"})
                             ->set_margin(10)
                             ->set_width({rugui::SizeMode::Self, 112})
                             ->set_height({rugui::SizeMode::Self, 112})
                             ->set_image_sampling(SkSamplingOptions{SkFilterMode::kNearest});

    auto node_char_name = (new rugui::Node{"character_name"})
                            ->set_margin(10)
                            ->set_width({rugui::SizeMode::Parent, 0.5f})
                            ->set_height({rugui::SizeMode::FitContent, 0})
                            ->add((new rugui::Node{"text", "Character Name"})->set_font_size(25));

    auto node_char_desc = (new rugui::Node{"character_desc"})
                            ->set_margin(5)
                            ->set_width({rugui::SizeMode::Parent, 1})
                            ->set_height({rugui::SizeMode::Parent, 1})
                            ->add((new rugui::Node{"text", "Character description"})->set_font_size(20));

    auto node_back_button = (new rugui::Node{"back_button"})
                              ->set_color(SkColors::kLtGray)
                              ->set_margin(10)
                              ->set_width({rugui::SizeMode::Parent, 0.5f})
                              ->set_height({rugui::SizeMode::Parent, 1})
                              ->set_flex_align(rugui::FlexAlign::Center)
                              ->set_flex_items_align(rugui::FlexAlign::Center)
                              ->set_on_mouse_click_in([=](rugui::Node *, rugui::MouseButton button, int, int) {
                                if (button == rugui::MouseButton::Left) {
                                  scene_manager->set_active_scene("menu:main");
                                }
                                return false;
                              })
                              ->add((new rugui::Node{"text", "Back to menu"})->set_font_size(20));

    auto node_start_button = (new rugui::Node{"start_button"})
                               ->set_color(SkColors::kLtGray)
                               ->set_margin(10)
                               ->set_width({rugui::SizeMode::Parent, 0.5f})
                               ->set_height({rugui::SizeMode::Parent, 1})
                               ->set_flex_align(rugui::FlexAlign::Center)
                               ->set_flex_items_align(rugui::FlexAlign::Center)
                               ->set_on_mouse_click_in([=](rugui::Node *, rugui::MouseButton button, int, int) {
                                 if (ui_state->cur_select_count != ui_state->max_select_count) {
                                   return false;
                                 }
                                 if (button == rugui::MouseButton::Left) {
                                   scene_manager->set_active_scene("game:game");
                                 }
                                 return false;
                               })
                               ->add((new rugui::Node{"text", "Start game"})->set_font_size(20));

    // initial character info
    auto character_data = game_data->character_data.at(ui_state->character_id[0]);
    node_char_image->set_image(skimg_characters.at(character_data.id));
    node_char_name->children[0]->text = character_data.name;
    node_char_desc->children[0]->text = character_data.desc;

    // initial start button
    if (ui_state->cur_select_count == ui_state->max_select_count) {
      node_start_button->set_color(SkColors::kLtGray);
    } else {
      node_start_button->set_color(SkColors::kGray);
    }

    const auto fn_make_character_button = [=](std::string_view node_name, int index) {
      return (new rugui::Node{node_name})
        ->set_color(SkColors::kTransparent)
        ->set_margin(10)
        ->set_width({rugui::SizeMode::Self, 112})
        ->set_height({rugui::SizeMode::Self, 112})
        ->add((new rugui::Node{"button"})
                ->set_color(SkColors::kLtGray)
                ->set_margin(5)
                ->set_width({rugui::SizeMode::Parent, 1})
                ->set_height({rugui::SizeMode::Parent, 1})
                ->set_image(skimg_characters.at(ui_state->character_id.at(index)))
                ->set_image_sampling(SkSamplingOptions{SkFilterMode::kNearest})
                ->set_on_mouse_click_in([=](rugui::Node *node, rugui::MouseButton button, int, int) {
                  if (button == rugui::MouseButton::Left) {
                    // update character info
                    auto character_data = game_data->character_data.at(ui_state->character_id[index]);
                    node_char_image->set_image(node->style.image);
                    node_char_name->children[0]->text = character_data.name;
                    node_char_desc->children[0]->text = character_data.desc;

                    if (ui_state->can_select_more() or ui_state->selection[index]) {
                      // toggle selection
                      ui_state->selection[index] = not ui_state->selection[index];
                      if (ui_state->selection[index]) {
                        // select
                        ui_state->cur_select_count += 1;
                        game_data->picked_characters.push_back(character_data);
                        node->parent->set_color(SkColor4f::FromColor(0xFF'FCBA03));
                      } else {
                        // deselect
                        ui_state->cur_select_count -= 1;
                        auto e = std::ranges::remove(game_data->picked_characters, character_data);
                        game_data->picked_characters.erase(e.begin(), e.end());
                        node->parent->set_color(SkColors::kTransparent);
                      }
                    }

                    // update start button
                    if (ui_state->cur_select_count == ui_state->max_select_count) {
                      node_start_button->set_color(SkColors::kLtGray);
                    } else {
                      node_start_button->set_color(SkColors::kGray);
                    }
                  }
                  return false;
                }));
    };

    scene->ui_tree.root->add(
      (new rugui::Node{"bg"})
        ->set_margin(10)
        ->set_width({rugui::SizeMode::Parent, 1})
        ->set_height({rugui::SizeMode::Parent, 1})
        ->set_flex_dir(rugui::FlexDir::Row)
        ->add((new rugui::Node{"left_panel"})
                ->set_width({rugui::SizeMode::Parent, 0.5f})
                ->set_height({rugui::SizeMode::Parent, 1})
                ->set_flex_dir(rugui::FlexDir::Row)
                ->set_flex_wrap(rugui::FlexWrap::Wrap)
                ->add((new rugui::Node{"title"})
                        ->set_width({rugui::SizeMode::Parent, 1})
                        ->set_height({rugui::SizeMode::FitContent, 0})
                        ->set_flex_items_align(rugui::FlexAlign::Center)
                        ->add((new rugui::Node{"text", "4명의 캐릭터를 선택하세요."})->set_font_size(25)))
                ->add(fn_make_character_button("0", 0))
                ->add(fn_make_character_button("1", 1))
                ->add(fn_make_character_button("2", 2))
                ->add(fn_make_character_button("3", 3))
                ->add(fn_make_character_button("4", 4)))
        ->add((new rugui::Node{"right_panel"})
                ->set_width({rugui::SizeMode::Parent, 0.5f})
                ->set_height({rugui::SizeMode::Parent, 1})
                ->add((new rugui::Node{"character_info"})
                        ->set_width({rugui::SizeMode::Parent, 1})
                        ->set_height({rugui::SizeMode::Parent, 0.85f})
                        ->add((new rugui::Node{"top_section"})
                                ->set_flex_dir(rugui::FlexDir::Row)
                                ->set_width({rugui::SizeMode::Parent, 1})
                                ->set_height({rugui::SizeMode::Parent, 0.3f})
                                ->add(node_char_image)
                                ->add(node_char_name))
                        ->add((new rugui::Node{"bot_section"})
                                ->set_color(SkColors::kLtGray)
                                ->set_margin(10)
                                ->set_width({rugui::SizeMode::Parent, 1})
                                ->set_height({rugui::SizeMode::Parent, 0.7f})
                                ->add(node_char_desc)))
                ->add((new rugui::Node{"menu_buttons"})
                        ->set_width({rugui::SizeMode::Parent, 1})
                        ->set_height({rugui::SizeMode::Parent, 0.15f})
                        ->set_flex_dir(rugui::FlexDir::Row)
                        ->add(node_back_button)
                        ->add(node_start_button))));
  };

  return scene;
}
