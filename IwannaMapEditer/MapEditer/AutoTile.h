#pragma once
#include <Siv3D.hpp>

class AutoTile
{
public:

	AutoTile() = default;

	/// @brief オートタイルを作成します。
	/// @param image オートタイルの基本画像（1x5 タイル、または 8x6 タイル）
	SIV3D_NODISCARD_CXX20
		explicit AutoTile(const Image& image, const int32 id)
	{
		if ((image.width()) == (image.height()) && image.width() == 32)
		{
			*this = CreateFromOneTile(image);
			this->tileId = id;
			this->isSingleTile = true;
		}
		else
		{
			*this = CreateFromTiles(image);
			this->tileId = id;
			this->isSingleTile = false;
		}

		this->m_tileSize = 32;
	}

	/// @brief タイルのサイズ（ピクセル）を返します。
	/// @return タイルのサイズ（ピクセル）
	[[nodiscard]]
	int32 getTileSize() const noexcept
	{
		return m_tileSize;
	}

	/// @brief オートタイルを返します。
	/// @return オートタイル
	[[nodiscard]]
	TextureRegion getTile(int32 id, int32 n) const
	{
		Point startDrawPos{ 0, 0 };
		return m_tileTexture(startDrawPos, m_tileSize, m_tileSize);
	}

	/// @brief オートタイルの展開図のテクスチャを返します。
	/// @return オートタイルの展開図のテクスチャ
	[[nodiscard]]
	const Texture& getTileTexture() const noexcept
	{
		return m_tileTexture;
	}

	/// @brief タイル番号を返します。
	/// @return タイル番号
	const int32& getTileId() const noexcept
	{
		return this->tileId;
	}

private:

	int32 tileId;
	bool isSingleTile = false;

	struct BaseTileIndex
	{
		int8 topLeft, topRight, bottomLeft, bottomRight;
	};

	[[nodiscard]]
	static Image MakeTileImage(const Image& baseTileImage, const BaseTileIndex& index, int32 tileSize)
	{
		const int32 halfTileSize = (tileSize / 2);
		Image image{ Size{ tileSize, tileSize } };
		baseTileImage(0, (tileSize * index.topLeft), halfTileSize, halfTileSize).overwrite(image, 0, 0);
		baseTileImage(halfTileSize, (tileSize * index.topRight), halfTileSize, halfTileSize).overwrite(image, halfTileSize, 0);
		baseTileImage(0, (tileSize * index.bottomLeft + halfTileSize), halfTileSize, halfTileSize).overwrite(image, 0, halfTileSize);
		baseTileImage(halfTileSize, (tileSize * index.bottomRight + halfTileSize), halfTileSize, halfTileSize).overwrite(image, halfTileSize, halfTileSize);
		return image;
	}

	[[nodiscard]]
	static Image MakeTiles(const Image& baseTileImage, int32 tileSize)
	{
		constexpr std::array<BaseTileIndex, 47> BaseTileIndices =
		{ {
			{ 0, 0, 0, 0 }, { 0, 2, 0, 2 },	{ 2, 2, 2, 2 },	{ 2, 0, 2, 0 }, { 0, 0, 1, 1 },	{ 0, 2, 1, 4 },	{ 2, 2, 4, 4 },	{ 2, 0, 4, 1 },
			{ 0, 2, 1, 3 },	{ 2, 0, 3, 1 },	{ 1, 3, 1, 3 },	{ 2, 2, 3, 3 },	{ 1, 1, 1, 1 },	{ 1, 4, 1, 4 },	{ 4, 4, 4, 4 },	{ 4, 1, 4, 1 },
			{ 1, 3, 0, 2 },	{ 3, 1, 2, 0 },	{ 3, 3, 2, 2 },	{ 3, 1, 3, 1 },	{ 1, 1, 0, 0 },	{ 1, 4, 0, 2 },	{ 4, 4, 2, 2 },	{ 4, 1, 2, 0 },
			{ 1, 4, 1, 3 },	{ 4, 1, 3, 1 },	{ 2, 2, 4, 3 },	{ 2, 2, 3, 4 },	{ 4, 4, 4, 3 },	{ 4, 4, 3, 4 },	{ 4, 3, 3, 3 },	{ 3, 4, 3, 3 },
			{ 1, 3, 1, 4 },	{ 3, 1, 4, 1 }, { 4, 3, 2, 2 },	{ 3, 4, 2, 2 },	{ 4, 3, 4, 4 },	{ 3, 4, 4, 4 },	{ 3, 3, 4, 3 },	{ 3, 3, 3, 4 },
			{ 3, 3, 4, 4 },	{ 4, 4, 3, 3 },	{ 4, 3, 4, 3 },	{ 3, 4, 3, 4 },	{ 4, 3, 3, 4 },	{ 3, 4, 4, 3 },	{ 3, 3, 3, 3 }
		} };

		Image image{ Size{ (tileSize * 8), (tileSize * 6) }, Color{ 255, 0 } };

		for (int32 i = 0; i < 47; ++i)
		{
			const int32 x = ((i % 8) * tileSize);
			const int32 y = ((i / 8) * tileSize);
			MakeTileImage(baseTileImage, BaseTileIndices[i], tileSize).overwrite(image, x, y);
		}

		return image;
	}

	[[nodiscard]]
	static uint8 GetTileIndex(uint8 bits) noexcept
	{
		constexpr std::array<uint8, 256> Indices =
		{
			 0,  0,  4,  4,  0,  0,  4,  4,  1,  1,  8,  5,  1,  1,  8,  5,
			 3,  3,  9,  9,  3,  3,  7,  7,  2,  2, 11, 27,  2,  2, 26,  6,
			 0,  0,  4,  4,  0,  0,  4,  4,  1,  1,  8,  5,  1,  1,  8,  5,
			 3,  3,  9,  9,  3,  3,  7,  7,  2,  2, 11, 27,  2,  2, 26,  6,
			20, 20, 12, 12, 20, 20, 12, 12, 16, 16, 10, 32, 16, 16, 10, 32,
			17, 17, 19, 19, 17, 17, 33, 33, 18, 18, 46, 39, 18, 18, 38, 40,
			20, 20, 12, 12, 20, 20, 12, 12, 21, 21, 24, 13, 21, 21, 24, 13,
			17, 17, 19, 19, 17, 17, 33, 33, 35, 35, 31, 43, 35, 35, 45, 37,
			 0,  0,  4,  4,  0,  0,  4,  4,  1,  1,  8,  5,  1,  1,  8,  5,
			 3,  3,  9,  9,  3,  3,  7,  7,  2,  2, 11, 27,  2,  2, 26,  6,
			 0,  0,  4,  4,  0,  0,  4,  4,  1,  1,  8,  5,  1,  1,  8,  5,
			 3,  3,  9,  9,  3,  3,  7,  7,  2,  2, 11, 27,  2,  2, 26,  6,
			20, 20, 12, 12, 20, 20, 12, 12, 16, 16, 10, 32, 16, 16, 10, 32,
			23, 23, 25, 25, 23, 23, 15, 15, 34, 34, 30, 44, 34, 34, 42, 36,
			20, 20, 12, 12, 20, 20, 12, 12, 21, 21, 24, 13, 21, 21, 24, 13,
			23, 23, 25, 25, 23, 23, 15, 15, 22, 22, 41, 29, 22, 22, 28, 14
		};

		return Indices[bits];
	}

	[[nodiscard]]
	static AutoTile CreateFromBaseImage(const Image& baseTileImage_1x5)
	{
		AutoTile autoTiles;
		autoTiles.m_tileSize = baseTileImage_1x5.width();
		autoTiles.m_tileTexture = Texture{ MakeTiles(baseTileImage_1x5, autoTiles.m_tileSize) };
		return autoTiles;
	}

	[[nodiscard]]
	static AutoTile CreateFromTiles(const Image& tiles)
	{
		AutoTile autoTiles;
		autoTiles.m_tileSize = 32;
		autoTiles.m_tileTexture = Texture{ tiles };
		return autoTiles;
	}

	[[nodiscard]]
	static AutoTile CreateFromOneTile(const Image& tile)
	{
		AutoTile autoTile;
		autoTile.m_tileSize = (tile.width());
		autoTile.m_tileTexture = Texture{ tile };
		return autoTile;
	}

	int32 m_tileSize = 1;

	Texture m_tileTexture;
};
