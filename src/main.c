#include <gtk/gtk.h>
//~ #include <glade/glade.h>
#include <stdio.h>
#include "serialcom.h"

int fd;

#define ON 1
#define OFF 0

//Motor
//Motor speed min max in [rpm]
#define MOTOR_RPM_MIN 0
#define MOTOR_RPM_MAX 4000							//TODO: Fix
#define MOTOR_RPM_STARTUP_DEFAULT_VALUE 30			//TODO: Match with RAMP_TO_DEFAULT_VALUE?

//Motor current min, max values given in mA
#define MOTOR_CURRENT_MIN 0
#define MOTOR_CURRENT_MAX 250						//TODO: Fix
#define MOTOR_STARTUP_CURRENT_DEFAULT_VALUE 10		//TODO: Fix

typedef enum {
	MOTOR_OFF = 0,
	MOTOR_ON,
	MOTOR_IDLE,
} motor_state_t;

volatile motor_state_t motor_state;
GtkWidget * spin_button_motor_startup_current;
GtkWidget * spin_button_motor_startup_until_rpm;

//Ramp
//Ramp time in [s]
#define RAMP_TIME_MIN 1								//One second ramp time is probalby too fast
#define RAMP_TIME_MAX 3600							//Something silly big
#define RAMP_TIME_DEFAULT_VALUE 10
#define RAMP_FROM_DEFAULT_VALUE 30					//TODO: Match with MOTOR_RPM_STARTUP_DEFAULT_VALUE?
#define RAMP_TO_DEFAULT_VALUE 600

typedef enum {
	RAMP_START,
	RAMP_REVERSE,
	RAMP_STOP,
} ramp_state_t;

GtkWidget * spin_button_ramp_from;
GtkWidget * spin_button_ramp_to;
GtkWidget * spin_button_ramp_time;

//Scale, PMSM speed settings
#define SCALE_DEFAULT_VALUE 40

//Status bar
GtkWidget * status_bar;
int status_bar_context_id;


void m_gtk_main_quit(GtkWidget * widget, gpointer data)
{
	g_print("Quit\n");
	gtk_main_quit();
}

void print_statusbar(char * str)
{
	gtk_statusbar_remove_all(GTK_STATUSBAR(status_bar), status_bar_context_id);
	gtk_statusbar_push(GTK_STATUSBAR(status_bar), status_bar_context_id, str);
}

void m_button_clicked(GtkWidget * widget,  gpointer data)
{
	g_print("Motor say: ");
	switch((int)data) {
		case MOTOR_ON:
			g_print("Vrrrom!\n");
			motor_state = MOTOR_ON;
			serialcom_put(&fd, '1');
			break;
		case MOTOR_OFF:
			g_print("Schreech!\n");
			motor_state = MOTOR_OFF;
			serialcom_put(&fd, '3');
			break;
		default:
			g_print("Error!\n");
			break;
	}
}

volatile char option1;
void m_clicked_option1(GtkWidget * widget, gpointer data)
{
	option1 ^= 1;
	switch(option1) {
		case ON:
			g_print("Option1 on\n");
			gtk_statusbar_push(GTK_STATUSBAR(status_bar), status_bar_context_id, "Option1 on");
			break;
		case OFF:
			g_print("Option1 off\n");
			gtk_statusbar_push(GTK_STATUSBAR(status_bar), status_bar_context_id, "Option1 off");
			break;
		default:
			g_print("Strange\n");
	}
}

volatile char option2;
void m_clicked_option2(GtkWidget * widget, gpointer data)
{
	option2 ^= 1;
	switch(option2) {
		case ON:
			g_print("Option2 on\n");
			gtk_statusbar_push(GTK_STATUSBAR(status_bar), status_bar_context_id, "Option2 on");
			break;
		case OFF:
			g_print("Option2 off\n");
			gtk_statusbar_push(GTK_STATUSBAR(status_bar), status_bar_context_id, "Option2 off");
			break;
		default:
			g_print("Strange\n");
	}
}

volatile char option3;
void m_clicked_option3(GtkWidget * widget, gpointer data)
{
	option3 ^= 1;
	switch(option3) {
		case ON:
			g_print("Option3 on\n");
			gtk_statusbar_push(GTK_STATUSBAR(status_bar), status_bar_context_id, "Option3 on");
			break;
		case OFF:
			g_print("Option3 off\n");
			gtk_statusbar_push(GTK_STATUSBAR(status_bar), status_bar_context_id, "Option3 off");
			break;
		default:
			g_print("Strange\n");
	}
}

void m_ramp_clicked(GtkWidget * widget, gpointer data)
{
	gdouble tmp;
	switch((gint)data) {
		case RAMP_START:
			print_statusbar("Ramp started");
			break;
		case RAMP_STOP:
			print_statusbar("Ramp paused");
			break;
		case RAMP_REVERSE:
			tmp = gtk_spin_button_get_value(GTK_SPIN_BUTTON(spin_button_ramp_from));
			gtk_spin_button_set_value(GTK_SPIN_BUTTON(spin_button_ramp_from), gtk_spin_button_get_value(GTK_SPIN_BUTTON(spin_button_ramp_to)));
			gtk_spin_button_set_value(GTK_SPIN_BUTTON(spin_button_ramp_to), tmp);
			print_statusbar("Ramp reversed");
			break;
	}
}

#define STRING_MAX_LEN 0x80
void m_scale_speed_changed(GtkAdjustment * widget,  gpointer data)
{
	char str[STRING_MAX_LEN];
	snprintf(str, STRING_MAX_LEN, "Setting RPM: %i", (gint)widget->value);
	print_statusbar(str);
}

int main(int argc, char * argv[])
{
	char ch;
	serialcom_init_old(&fd);

	GtkBuilder * builder;
	GtkWidget * window;

	gtk_init(&argc, &argv);

	builder = gtk_builder_new();
	gtk_builder_add_from_file(builder, "layout/gui.xml", NULL);
	window = GTK_WIDGET(gtk_builder_get_object (builder, "window1"));
	gtk_builder_connect_signals(builder, NULL);


	gtk_widget_show(window);

	gtk_main();
	g_object_unref(G_OBJECT (builder));

	serialcom_destroy(&fd);
	return 0;
}
//~
//~ int main_glade(int argc, char ** argv)
//~ {
	//~ GladeXML * xml;
	//~ GtkWidget * widget;
//~
	//~ gtk_init(&argc, &argv);
	//~ xml = glade_xml_new("layout/gui.xml", NULL, NULL);
	//~ widget - glade_xml_get_widget(xml, "window1");
//~
	//~ glade_xml_signal_autoconnect(xml);
//~
	//~ gtk_main();
	//~ return 0;
//~ }
