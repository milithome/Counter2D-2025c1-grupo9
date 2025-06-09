#include "mapEditorTemplate.h"

MapEditorTemplate::MapEditorTemplate(const std::string& name, int width, int height)
    : name(name), width(width), height(height), grid(height, std::vector<Block>(width)) {

}

const std::string& MapEditorTemplate::getName() const {
    return name;
}

int MapEditorTemplate::getWidth() const {
    return width;
}

int MapEditorTemplate::getHeight() const {
    return height;
}

Block MapEditorTemplate::getBlock(int x, int y) const {
    return grid[y][x];
}

void MapEditorTemplate::setBlock(int x, int y, BlockType type) {
    grid[y][x].type = type;
}