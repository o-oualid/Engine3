#include "baseTypes.h"

void Mesh {
    Vector2 vertices[];
    uint16 indicies[];
};

const uint16 indices[6]{
    0,
    1,
    2,
    2,
    3,
    0,
};

void generateTiles(Mesh *mesh, float tileSize, Vector2i mapSize) {

    // the origin of each tile is the top left
    // this means that we will have one more vertex horizontaly and another vertically for the map size
    Vector2i vertexSize = {mapSize.x + 1, mapSize.y + 1};
    for (int32 y = 0; y < vertexSize.y; y++) {
        for (int32 x = 0; x < vertexSize.x; x++) {
            int32 vertexIndex = y * vertexSize.x + 1 + x;
            mesh.vertices[vertexIndex] = {x * tileSize, y * tileSize};
        }
    }
    int32 index = 0;
    for (int32 y = 0; y <= mapSize.y; y++) {
        for (int32 x = 0; x <= mapSize.x; x++) {
            int32 tileIndex = y * vertexSize.x + x;

            mesh.indices[index++] = tileIndex;
            mesh.indices[index++] = tileIndex + 1;
            mesh.indices[index++] = tileIndex + 1 + vertexSize.x;

            mesh.indices[index++] = tileIndex + 1 + vertexSize.x;
            mesh.indices[index++] = tileIndex + vertexSize.x;
            mesh.indices[index++] = tileIndex;
        }
    }
};

struct GameObject {
    Vector2 location;
};

void Innit() {}

void Render(float deltaTime) {}
