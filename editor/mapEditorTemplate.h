#ifndef MAP_TEMPLATE_EDITOR_H
#define MAP_TEMPLATE_EDITOR_H

#include <string>
#include <vector>

enum class BlockType {
    EMPTY,
    FLOOR,
    WALL,
    BOX,
    PLANT
};

struct Block {
    BlockType type;

    Block(BlockType type = BlockType::EMPTY) : type(type) {}
};

class MapEditorTemplate {
public:
    MapEditorTemplate(const std::string& name, int width, int height);
    const std::string& getName() const;
    int getWidth() const;
    int getHeight() const;
    Block getBlock(int x, int y) const;
    void setBlock(int x, int y, BlockType type);

private:
    std::string name;
    int width;
    int height;
    std::vector<std::vector<Block>> grid;
};

#endif // MAP_TEMPLATE_EDITOR_H
