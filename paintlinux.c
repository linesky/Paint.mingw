#include <gtk/gtk.h>
#include <cairo.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FILENAME_ENTRY "new.png"
//sudo apt-get install gtk+3.0
//sudo apt-get install gtk+-3.0
//sudo apt-get install cairo-5c
//sudo apt-get install libcairo-5c-dev
//sudo apt-get install libcairo-5c0
//gcc -o paint_app paint_app.c `pkg-config --cflags --libs gtk+-3.0 cairo`
GtkWidget *drawing_area;
GtkWidget *entry;
cairo_surface_t *surface = NULL;

static void clear_surface() {
    cairo_t *cr = cairo_create(surface);
    cairo_set_source_rgb(cr, 1, 1, 1);
    cairo_paint(cr);
    cairo_destroy(cr);
}

static void load_png(const char *filename) {
    cairo_surface_t *image = cairo_image_surface_create_from_png(filename);
    if (cairo_surface_status(image) != CAIRO_STATUS_SUCCESS) {
        g_printerr("Error loading PNG file '%s'\n", filename);
        cairo_surface_destroy(image);
        return;
    }
    if (surface) {
        cairo_surface_destroy(surface);
    }
    surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32,
                                         cairo_image_surface_get_width(image),
                                         cairo_image_surface_get_height(image));
    cairo_t *cr = cairo_create(surface);
    cairo_set_source_surface(cr, image, 0, 0);
    cairo_paint(cr);
    cairo_destroy(cr);
    cairo_surface_destroy(image);
    gtk_widget_queue_draw(drawing_area);
}

static void save_png(const char *filename) {
    cairo_surface_write_to_png(surface, filename);
}

static gboolean on_draw_event(GtkWidget *widget, cairo_t *cr, gpointer user_data) {
    cairo_set_source_surface(cr, surface, 0, 0);
    cairo_paint(cr);
    return FALSE;
}

static gboolean on_button_press_event(GtkWidget *widget, GdkEventButton *event, gpointer user_data) {
    if (surface == NULL) {
        return FALSE;
    }

    if (event->button == GDK_BUTTON_PRIMARY) {
        cairo_t *cr = cairo_create(surface);
        cairo_set_source_rgb(cr, 0, 0, 0);
        cairo_arc(cr, event->x, event->y, 5, 0, 2 * G_PI);
        cairo_fill(cr);
        cairo_destroy(cr);
        gtk_widget_queue_draw(widget);
    }

    return TRUE;
}

static void on_btn_new_clicked(GtkButton *button, gpointer user_data) {
    clear_surface();
    gtk_widget_queue_draw(drawing_area);
}

static void on_btn_load_clicked(GtkButton *button, gpointer user_data) {
    const char *filename = gtk_entry_get_text(GTK_ENTRY(entry));
    load_png(filename);
}

static void on_btn_save_clicked(GtkButton *button, gpointer user_data) {
    const char *filename = gtk_entry_get_text(GTK_ENTRY(entry));
    save_png(filename);
}

static void on_resize(GtkWidget *widget, GdkRectangle *allocation, gpointer data) {
    if (surface) {
        cairo_surface_destroy(surface);
    }
    surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, allocation->width, allocation->height);
    clear_surface();
}

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);

    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Paint Application");
    gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    GdkRGBA yellow = {1, 1, 0, 1};
    gtk_widget_override_background_color(vbox, GTK_STATE_FLAG_NORMAL, &yellow);

    drawing_area = gtk_drawing_area_new();
    gtk_widget_set_size_request(drawing_area, 800, 550);
    g_signal_connect(G_OBJECT(drawing_area), "draw", G_CALLBACK(on_draw_event), NULL);
    g_signal_connect(G_OBJECT(drawing_area), "button-press-event", G_CALLBACK(on_button_press_event), NULL);
    g_signal_connect(drawing_area, "size-allocate", G_CALLBACK(on_resize), NULL);

    gtk_widget_set_events(drawing_area, gtk_widget_get_events(drawing_area) | GDK_BUTTON_PRESS_MASK);
    gtk_box_pack_start(GTK_BOX(vbox), drawing_area, TRUE, TRUE, 0);

    entry = gtk_entry_new();
    gtk_entry_set_text(GTK_ENTRY(entry), FILENAME_ENTRY);
    gtk_box_pack_start(GTK_BOX(vbox), entry, FALSE, FALSE, 0);

    GtkWidget *hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 0);

    GtkWidget *btn_new = gtk_button_new_with_label("New");
    g_signal_connect(btn_new, "clicked", G_CALLBACK(on_btn_new_clicked), NULL);
    gtk_box_pack_start(GTK_BOX(hbox), btn_new, FALSE, FALSE, 0);

    GtkWidget *btn_load = gtk_button_new_with_label("Load");
    g_signal_connect(btn_load, "clicked", G_CALLBACK(on_btn_load_clicked), NULL);
    gtk_box_pack_start(GTK_BOX(hbox), btn_load, FALSE, FALSE, 0);

    GtkWidget *btn_save = gtk_button_new_with_label("Save");
    g_signal_connect(btn_save, "clicked", G_CALLBACK(on_btn_save_clicked), NULL);
    gtk_box_pack_start(GTK_BOX(hbox), btn_save, FALSE, FALSE, 0);

    gtk_widget_show_all(window);

    gtk_main();

    if (surface) {
        cairo_surface_destroy(surface);
    }

    return 0;
}
