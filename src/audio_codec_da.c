#include "audio_codec_da.h"

static struct audio_codec *driver;

void audio_codec_set_driver(struct audio_codec *codec)
{
	driver = codec;
}

void audio_codec_init(void)
{
	if (driver->init)
		driver->init();
}
