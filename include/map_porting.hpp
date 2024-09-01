#pragma once

#include <filesystem>
#include <unordered_map>

void map_load_from_file(const std::string &map_name, Map &map) {
  if (!std::filesystem::exists("maps/" + map_name) || map_name == "") {
    std::cout << "Didn't even start\n";
    return;
  }

  std::string assets_tiles_path = "assets/tiles";
  std::string map_tiles_path = "maps/" + map_name + "/tiles/";
  std::vector<std::string> unique_tile_names, tile_names;
  std::vector<uint16_t> actual_id;

  std::ifstream fin("maps/" + map_name + "/map.dat",
                    std::ios::binary | std::ios::in);
  int size_x = 0, size_y = 0;
  uint16_t unique_tile_count = 0;

  fin.read((char *)&size_x, sizeof(size_x));
  fin.read((char *)&size_y, sizeof(size_y));
  fin.read((char *)&unique_tile_count, sizeof(unique_tile_count));

  map.resize(size_x, size_y);

  fin.read((char *)map.grid, size_x * size_y * 2);

  for (uint16_t i = 0; i < unique_tile_count; i++) {
    uint16_t name_size = 0;
    char tile_name[101] = {0};

    fin.read((char *)&name_size, sizeof(name_size));
    fin.read(tile_name, name_size);

    std::string temp(tile_name);
    unique_tile_names.push_back(temp);
  }

  for (const auto &tilename : unique_tile_names)
    std::filesystem::copy_file(
        map_tiles_path + tilename, assets_tiles_path + "/" + tilename,
        std::filesystem::copy_options::overwrite_existing);

  for (const auto &entry :
       std::filesystem::directory_iterator(assets_tiles_path))
    tile_names.push_back(
        entry.path().string().substr(assets_tiles_path.size() + 1));
  sort(tile_names.begin(), tile_names.end());

  uint16_t idx = 0, it = 0;
  for (auto &tilename : tile_names) {
    if (tilename == unique_tile_names.at(it)) {
      actual_id.push_back(idx);

      if (++it == unique_tile_count)
        break;
    }
    idx++;
  }

  for (auto *it = map.grid; it != map.grid_end(); it++) {
    *it = *it & 0b1000000000000000U | actual_id[*it & 0b0111111111111111U];
  }
}

void map_save_to_file(const std::string &new_map_name, Map &map) {
  std::string assets_tiles_path = "assets/tiles";
  std::string new_map_tiles_path = "maps/" + new_map_name + "/tiles/";
  std::vector<uint16_t> unique_tiles_id = get_unique_tiles(map);
  std::vector<std::string> unique_tile_names, tile_names;
  std::unordered_map<uint16_t, uint16_t> mapped_values;

  std::filesystem::remove_all("maps/" + new_map_name);
  std::filesystem::create_directory("maps/");
  std::filesystem::create_directory("maps/" + new_map_name);
  std::filesystem::create_directory(new_map_tiles_path);

  for (const auto &entry :
       std::filesystem::directory_iterator(assets_tiles_path))
    tile_names.push_back(
        entry.path().string().substr(assets_tiles_path.size() + 1));
  sort(tile_names.begin(), tile_names.end());

  uint16_t index = 0, actual_index = 0;
  for (auto &tilename : tile_names) {
    if (index == unique_tiles_id.at(actual_index)) {
      unique_tile_names.push_back(tilename);

      mapped_values[index] = actual_index++;

      std::filesystem::copy_file(
          assets_tiles_path + "/" + tilename, new_map_tiles_path + tilename,
          std::filesystem::copy_options::overwrite_existing);
    }

    if (actual_index == unique_tiles_id.size())
      break;
    index++;
  }

  uint16_t unique_tiles_count = unique_tiles_id.size();
  std::ofstream fout("maps/" + new_map_name + "/map.dat", std::ios::binary);

  fout.write((char *)&map.size.x, sizeof(map.size.x));
  fout.write((char *)&map.size.y, sizeof(map.size.y));
  fout.write((char *)&unique_tiles_count, sizeof(unique_tiles_count));

  for (auto *it = map.grid; it != map.grid_end(); it++) {
    uint16_t value =
        mapped_values[*it & 0b0111111111111111U] | (*it & 0b1000000000000000U);

    fout.write((char *)&value, sizeof(*it));
  }

  for (const auto &tile_name : unique_tile_names) {
    const char *name = tile_name.c_str();
    uint16_t name_size = tile_name.size();

    fout.write((char *)&name_size, sizeof(name_size));
    fout.write(name, name_size);
  }

  fout.close();
}