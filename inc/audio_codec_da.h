#ifndef INC_AUDIO_CODEC_H_
#define INC_AUDIO_CODEC_H_

typedef void (*codec_init)(void);

struct audio_codec
{
	codec_init init;
};

void audio_codec_set_driver(struct audio_codec *codec);
void audio_codec_init(void);

#endif /* INC_AUDIO_CODEC_H_ */
