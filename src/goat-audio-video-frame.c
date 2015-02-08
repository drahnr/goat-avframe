#include "goat-audio-video-frame.h"


typedef struct _GoatAudioVideoFramePrivate
{
	GMutex cond_mutex;
	GCond cond;
	guint timer;
	gpointer ringbuffer;
	cairo_surface_t next_frame_surf;
} GoatAudioVideoFramePrivate;


G_DEFINE_TYPE_WITH_PRIVATE (GoatAudioVideoFrame, goat_audio_video_frame, GTK_TYPE_DRAWING_AREA)

	
static void
goat_audio_video_frame_finalize (GObject *object)
{
	G_OBJECT_CLASS (goat_audio_video_frame_parent_class)->finalize (object);
}


static void
goat_audio_video_frame_class_init (GoatAudioVideoFrameClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	object_class->finalize = goat_audio_video_frame_finalize;
}


static void
goat_audio_video_frame_init (GoatAudioVideoFrame *self)
{
	gtk_widget_set_size_request (drawing_area, 100, 100);
	g_signal_connect (G_OBJECT (self), "draw", G_CALLBACK (draw_callback), NULL);
	
	//FIXME ringbuffer would be nicer
	GoatAudioVideoFramePrivate *priv = GOAT_AUDIO_VIDEO_FRAME_GET_PRIVATE (self);
	
	g_mutex_init (&priv->cond_mutex);
	g_cond_init (&priv->cond);
	priv->next_frame_surf = NULL;
	priv->timer = 0;
	priv->ringbuffer = ...; //TODO
}


GoatAudioVideoFrame *
goat_audio_video_frame_new ()
{
	return g_object_new (GOAT_TYPE_AUDIO_VIDEO_FRAME, NULL);
}


/**
 * cyclic drawing
 */
gboolean
timer_callback (GSource *source, gpointer data)
{
	g_mutex_lock (&priv->cond_mutex);
	g_cond_signal (&priv->cond_mutex);
	
	cairo_surface_t *nfs = ring_buffer_pop();
	if (priv->next_frame_surf) {
		cairo_surface_destroy (priv->next_frame_surf);
	}
	priv->next_frame_surf = nfs;
	g_mutex_unlock (&priv->cond_mutex);

	if (nfs) {
		gtk_widget_queue_draw (GTK_WIDGET (data));
	}
	return G_SOURCE_CONTINUE;
}


gboolean
realize_callback (GtkWidget *widget, gpointer data)
{
	
}


gboolean
draw_callback (GtkWidget *widget, cairo_t *cr, gpointer data)
{
	guint width, height;
	GdkRGBA color;

	width = gtk_widget_get_allocated_width (widget);
	height = gtk_widget_get_allocated_height (widget);
	
	if (priv->next_frame_surf!=NULL) {
		cairo_set_source_surface (cr, priv->next_frame_surf);
	} else {
		cairo_arc (cr,
				 width / 2.0, height / 2.0,
				 MIN (width, height) / 2.0,
				 0, 2 * G_PI);

		gtk_style_context_get_color (gtk_widget_get_style_context (widget),
									 0,
									 &color);
		gdk_cairo_set_source_rgba (cr, &color);

	}

	cairo_fill (cr);
	
	return FALSE;
}



/**
 * virtual func
 */
cairo_surface_t *
goat_decode_single_frame (GoatAudioVideoFrame *self) {
	return NULL; //FIXME TODO
}


/**
 * a thread
 */
gpointer
decoder_thread (gpointer user_data)
{
	GoatAudioVideoFrame *self = user_data;
	GoatAudioVideoFramePrivate *priv = GOAT_AUDIO_VIDEO_FRAME_GET_PRIVATE (self);

	while (priv->game_over==TRUE || priv->eof==TRUE) {
		while (goat_ring_buffer_is_full (ringbuffer)==TRUE || ring_buffer_length (priv->ringbuffer) > 120)
			g_mutex_lock (&priv->cond_mutex);
			g_cond_wait (&priv->cond, &priv->cond_mutex);
			g_mutex_unlock (&priv->cond_mutex);
		}
		cairo_surface_t *frame_surf = decode_single_frame ();
		if (frame_surf) {
			goat_ring_buffer_append (priv->ringbuffer, frame_surf);
		}
	}
	return NULL;
}


gboolean
goat_audio_video_frame_pause ()
{
	if (priv->timer!=0) {
		g_source_remove_by_id (priv->timer);
		priv->timer = 0;
	}
}

gboolean
goat_audio_video_frame_unpause ()
{
	if (priv->timer == 0) {
		priv->timer = g_timeout_add ((GSourceFunc)timer_callback);
	}
}


