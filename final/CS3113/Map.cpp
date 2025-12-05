#include "Map.h"

Map::Map(int mapColumns, int mapRows, unsigned int *levelData,
         const char *textureFilePath, float tileSize, int textureColumns,
         int textureRows, Vector2 origin) : 
         mMapColumns {mapColumns}, mMapRows {mapRows}, 
         mTextureAtlas { LoadTexture(textureFilePath) },
         mLevelData {levelData }, mTileSize {tileSize}, 
         mTextureColumns {textureColumns}, mTextureRows {textureRows},
         mOrigin {origin} { build(); }

Map::~Map() { UnloadTexture(mTextureAtlas); }

void Map::build()
{
    mLeftBoundary   = mOrigin.x - (mMapColumns * mTileSize) / 2.0f;
    mRightBoundary  = mOrigin.x + (mMapColumns * mTileSize) / 2.0f;
    mTopBoundary    = mOrigin.y - (mMapRows * mTileSize) / 2.0f;
    mBottomBoundary = mOrigin.y + (mMapRows * mTileSize) / 2.0f;

    for (int row = 0; row < mTextureRows; row++)
    {
        for (int col = 0; col < mTextureColumns; col++)
        {
            Rectangle textureArea = {
                (float) col * (mTextureAtlas.width / mTextureColumns),
                (float) row * (mTextureAtlas.height / mTextureRows),
                (float) mTextureAtlas.width / mTextureColumns,
                (float) mTextureAtlas.height / mTextureRows
            };

            mTextureAreas.push_back(textureArea);
        }
    }
}

void Map::render()
{
    for (int row = 0; row < mMapRows; row++)
    {
        for (int col = 0; col < mMapColumns; col++)
        {
            int tile = mLevelData[row * mMapColumns + col];

            if (tile == 0) continue;

            Rectangle destinationArea = {
                mLeftBoundary + col * mTileSize,
                mTopBoundary  + row * mTileSize, 
                mTileSize,
                mTileSize
            };

            DrawTexturePro(
                mTextureAtlas,
                mTextureAreas[tile - 1], 
                destinationArea,
                {0.0f, 0.0f},
                0.0f,         
                WHITE         
            );
        }
    }
}
