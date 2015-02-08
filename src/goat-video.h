#ifndef __GOAT_VIDEO_H__
#define __GOAT_VIDEO_H__

#include <glib-object.h>
#include <gtk/gtk.h>

G_BEGIN_DECLS

#define GOAT_TYPE_VIDEO				(goat_video_get_type ())
#define GOAT_VIDEO(obj)				(G_TYPE_CHECK_INSTANCE_CAST ((obj), GOAT_TYPE_VIDEO, GoatVideo))
#define GOAT_VIDEO_CONST(obj)		(G_TYPE_CHECK_INSTANCE_CAST ((obj), GOAT_TYPE_VIDEO, GoatVideo const))
#define GOAT_VIDEO_CLASS(klass)		(G_TYPE_CHECK_CLASS_CAST ((klass), GOAT_TYPE_VIDEO, GoatVideoClass))
#define GOAT_IS_VIDEO(obj)			(G_TYPE_CHECK_INSTANCE_TYPE ((obj), GOAT_TYPE_VIDEO))
#define GOAT_IS_VIDEO_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE ((klass), GOAT_TYPE_VIDEO))
#define GOAT_VIDEO_GET_CLASS(obj)	(G_TYPE_INSTANCE_GET_CLASS ((obj), GOAT_TYPE_VIDEO, GoatVideoClass))

typedef struct _GoatVideo		GoatVideo;
typedef struct _GoatVideoClass	GoatVideoClass;

struct _GoatVideo
{
	GtkDrawingArea parent;
};

struct _GoatVideoClass
{
	GtkDrawingAreaClass parent_class;
};

GType goat_video_get_type (void) G_GNUC_CONST;
GoatVideo *goat_video_new (void);


G_END_DECLS

#endif /* __GOAT_VIDEO_H__ */
