#include "m_common.h"

static const char* g_glyph_name_class[MAHJONG_GLYPH_COUNT] =
{
	"Character",
	"Character",
	"Character",
	"Character",
	"Character",
	"Character",
	"Character",
	"Character",
	"Character",
	"Bamboo",
	"Bamboo",
	"Bamboo",
	"Bamboo",
	"Bamboo",
	"Bamboo",
	"Bamboo",
	"Bamboo",
	"Bamboo",
	"Circle",
	"Circle",
	"Circle",
	"Circle",
	"Circle",
	"Circle",
	"Circle",
	"Circle",
	"Circle",
	"Wind",
	"Wind",
	"Wind",
	"Wind",
	"Season",
	"Season",
	"Season",
	"Season",
	"Flower",
	"Flower",
	"Flower",
	"Flower",
	"Dragon",
	"Dragon",
	"Dragon",
};

static const char* g_glyph_name_item[MAHJONG_GLYPH_COUNT] =
{
	"1",
	"2",
	"3",
	"4",
	"5",
	"6",
	"7",
	"8",
	"9",
	"1",
	"2",
	"3",
	"4",
	"5",
	"6",
	"7",
	"8",
	"9",
	"1",
	"2",
	"3",
	"4",
	"5",
	"6",
	"7",
	"8",
	"9",
	"East",
	"South",
	"West",
	"North",
	"Spring",
	"Summer",
	"Autumn",
	"Winter",
	"Plum Blossom",
	"Orchid Blossom",
	"Chrysanthemum Blossom",
	"Bamboo Blossom",
	"Red",
	"Green",
	"White",
};

const char* mah_glyph_get_class_name(int32_t glyph_index)
{
	if (glyph_index < 0 || glyph_index >= MAHJONG_GLYPH_COUNT)
	{
		return "Unknown";
	}
	return g_glyph_name_class[glyph_index];
}

const char* mah_glyph_get_item_name(int32_t glyph_index)
{
	if (glyph_index < 0 || glyph_index >= MAHJONG_GLYPH_COUNT)
	{
		return "Unknown";
	}

	return g_glyph_name_item[glyph_index];
}
