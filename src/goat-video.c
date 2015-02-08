#include "goat-video.h"

#include  <cairo.h>




gboolean
draw_callback (GtkWidget *widget, cairo_t *cr, gpointer data);


typedef struct _GoatVideoPrivate
{
	GMutex cond_mutex;
	GCond cond;
	guint timer;
	gpointer ringbuffer;
	cairo_surface_t *next_frame_surf;
	gboolean eof;
	gboolean game_over;
} GoatVideoPrivate;


G_DEFINE_TYPE_WITH_PRIVATE (GoatVideo, goat_video, GTK_TYPE_DRAWING_AREA)

	
static void
goat_video_finalize (GObject *object)
{
	G_OBJECT_CLASS (goat_video_parent_class)->finalize (object);
}


static void
goat_video_class_init (GoatVideoClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	object_class->finalize = goat_video_finalize;
}


static void
goat_video_init (GoatVideo *self)
{
	gtk_widget_set_size_request (GTK_WIDGET (self), 100, 100);
	g_signal_connect (G_OBJECT (self), "draw", G_CALLBACK (draw_callback), NULL);
	
	//FIXME ringbuffer would be nicer
	GoatVideoPrivate *priv = GOAT_VIDEO_GET_PRIVATE (self);
	
	g_mutex_init (&priv->cond_mutex);
	g_cond_init (&priv->cond);
	priv->next_frame_surf = NULL;
	priv->timer = 0;
	priv->ringbuffer = NULL; //TODO
}


GoatVideo *
goat_video_new ()
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
	//FIXME
	return TRUE;
}


gboolean
draw_callback (GtkWidget *widget, cairo_t *cr, gpointer data)
{
	guint width, height;
	GdkRGBA color;

	width = gtk_widget_get_allocated_width (widget);
	height = gtk_widget_get_allocated_height (widget);
	
	if (priv->next_frame_surf!=NULL) {
		cairo_set_source_surface (cr, priv->next_frame_surf, 0, 0);
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
goat_decode_single_frame (GoatVideo *self) {
	g_error ("You need to overwrite the goat_decode_frame virtual function/func ptr!");
	return NULL; //FIXME TODO
}


/**
 * a thread
 */
gpointer
decoder_thread (gpointer user_data)
{
	GoatVideo *self = user_data;
	GoatVideoPrivate *priv = GOAT_VIDEO_GET_PRIVATE (self);

	while (priv->game_over==TRUE || priv->eof==TRUE) {
		while (goat_ring_buffer_is_full (ringbuffer)==TRUE || ring_buffer_length (priv->ringbuffer) > 120) {
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
goat_video_pause ()
{
	if (priv->timer!=0) {
		g_source_remove_by_id (priv->timer);
		priv->timer = 0;
	}
}


gboolean
goat_video_unpause ()
{
	if (priv->timer == 0) {
		guint timeout_s = 1; //FIXME
		priv->timer = g_timeout_add (timeout_s, (GSourceFunc)timer_callback);
	}
}


