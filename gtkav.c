#include "goat-audio-video-frame.h"


typedef struct _GoatAudiovideoframePrivate
{
} GoatAudiovideoframePrivate;

G_DEFINE_TYPE_WITH_PRIVATE (GoatAudiovideoframe, goat_audiovideoframe, G_TYPE_OBJECT)

static void
goat_audiovideoframe_finalize (GObject *object)
{
	G_OBJECT_CLASS (goat_audiovideoframe_parent_class)->finalize (object);
}

static void
goat_audiovideoframe_class_init (GoatAudiovideoframeClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	object_class->finalize = goat_audiovideoframe_finalize;
}

static void
goat_audiovideoframe_init (GoatAudiovideoframe *self)
{
}

GoatAudiovideoframe *
goat_audiovideoframe_new ()
{
	return g_object_new (GOAT_TYPE_AUDIOVIDEOFRAME, NULL);
}
